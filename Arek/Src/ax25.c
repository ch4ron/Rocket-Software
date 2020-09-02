/***************************************
|-- AUTHOR: ANDRZEJ LACZEWSKI
|-- WEBSITE: WWW.BYTECHLAB.COM
|-- PROJECT: MAGELLAN-1 NANO SATELITE
|-- MODULE: AX.25 func
|-- DESCRIPTION:
.................
***************************************/
#include "ax25.h"

// Module globals
static uint16_t AX25_crc;
static uint8_t AX25_ones_in_a_row;
static uint8_t AX25_packet[MAX_PACKET_LEN];
static uint32_t AX25_packet_size;

// Module functions
void AX25_update_crc(uint8_t a_bit) {
    AX25_crc ^= a_bit;
    if(AX25_crc & 1)
        AX25_crc = (AX25_crc >> 1) ^ 0x8408;  // X-modem CRC poly
    else
        AX25_crc = AX25_crc >> 1;
}

void AX25_send_byte(uint8_t a_byte) {
    uint8_t i = 0;
    while(i++ < 8) {
        uint8_t a_bit = a_byte & 1;
        a_byte >>= 1;
        AX25_update_crc(a_bit);
        if(a_bit) {
            // Next bit is a '1'
            if(AX25_packet_size >= MAX_PACKET_LEN * 8)  // Prevent buffer overrun
                return;
            AX25_packet[AX25_packet_size >> 3] |= (1 << (AX25_packet_size & 7));
            AX25_packet_size++;
            if(++AX25_ones_in_a_row < 5) continue;
        }
        // Next bit is a '0' or a zero padding after 5 ones in a row
        if(AX25_packet_size >= MAX_PACKET_LEN * 8)  // Prevent buffer overrun
            return;
        AX25_packet[AX25_packet_size >> 3] &= ~(1 << (AX25_packet_size & 7));
        AX25_packet_size++;
        AX25_ones_in_a_row = 0;
    }
}

void AX25_send_flag() {
    uint8_t flag = 0x7e;
    int i;
    for(i = 0; i < 8; i++, AX25_packet_size++) {
        if(AX25_packet_size >= MAX_PACKET_LEN * 8)  // Prevent buffer overrun
            return;
        if((flag >> i) & 1)
            AX25_packet[AX25_packet_size >> 3] |= (1 << (AX25_packet_size & 7));
        else
            AX25_packet[AX25_packet_size >> 3] &= ~(1 << (AX25_packet_size & 7));
    }
}

void AX25_send_string(const char *string) {
    int i;
    for(i = 0; string[i]; i++) {
        AX25_send_byte(string[i]);
    }
}

void AX25_send_header(const struct s_address *addresses, uint16_t num_addresses) {
    int i, j;
    AX25_packet_size = 0;
    AX25_ones_in_a_row = 0;
    AX25_crc = 0xffff;

    // Send flags during TX_DELAY milliseconds (8 bit-flag = 8000/1200 ms)
    for(i = 0; i < TX_DELAY * 3 / 20; i++) {
        AX25_send_flag();
    }

    for(i = 0; i < num_addresses; i++) {
        // Transmit callsign
        for(j = 0; addresses[i].callsign[j]; j++)
            AX25_send_byte(addresses[i].callsign[j] << 1);
        // Transmit pad
        for(; j < 6; j++)
            AX25_send_byte(' ' << 1);
        // Transmit SSID. Termination signaled with last bit = 1
        if(i == num_addresses - 1)
            AX25_send_byte(('0' + addresses[i].ssid) << 1 | 1);
        else
            AX25_send_byte(('0' + addresses[i].ssid) << 1);
    }

    // Control field: 3 = APRS-UI frame
    AX25_send_byte(0x03);

    // Protocol ID: 0xf0 = no layer 3 data
    AX25_send_byte(0xf0);
}

void AX25_send_footer() {
    // Save the crc so that it can be treated it atomically
    uint16_t final_crc = AX25_crc;

    // Send the CRC
    AX25_send_byte(~(final_crc & 0xff));
    final_crc >>= 8;
    AX25_send_byte(~(final_crc & 0xff));

    // Signal the end of frame
    AX25_send_flag();
}

void AX25_flush_frame() {
    // Key the transmitter and send the frame
    AFSK_send_frame_dat(AX25_packet, AX25_packet_size);
    AFSK_start_transmission();
}
