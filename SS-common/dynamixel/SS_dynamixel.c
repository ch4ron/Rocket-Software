/*
 * SS_dynamixel.c
 *
 *  Created on: Dec 29, 2019
 *      Author: maciek
 */

#include "SS_dynamixel.h"
#include "string.h"
#include "SS_fifo.h"

Dynamixel dynamixel = {
        .id = 0x01,
        .opened_position = 4096,
};

/* Needs to be refactored to support multiple servos */
uint8_t tx_packet_buff[MAX_PACKET_LENGTH];
uint8_t rx_packet_buff[MAX_PACKET_LENGTH];
Dynamixel_fifo_bufor tmp_packet_buff;
static uint8_t params[MAX_PACKET_LENGTH];

static uint16_t packet_size;
static void *dest_data;
static bool torque_status;
static uint16_t rec_size;
static uint8_t ping_response[3];

FIFO_INIT(dynamixel, 10, Dynamixel_fifo_bufor)

/* Macros */

#define GET_INSTRUCTION(packet) packet[7]
#define GET_REG(packet) (uint16_t) (packet[8] | packet[9] << 8)
#define GET_LENGTH(packet) (uint16_t) (packet[5] | packet[6] << 8) - 3

/* Init */
Dynamixel_status SS_dynamixel_init(Dynamixel *servo) {
    Dynamixel_status res = 0;
    SS_dynamixel_disable_torque(servo);
    HAL_Delay(UART_TIMEOUT);
    return res;
}

/********** API **********/

void SS_dynamixel_start_communication(Dynamixel *servo) {
    servo->systick_enabled = true;
}

void SS_dynamixel_stop_communication(Dynamixel *servo) {
    servo->systick_enabled = false;
}

void SS_dynamixel_open(Dynamixel *servo) {
    SS_dynamixel_set_goal_position(&dynamixel, servo->opened_position);
}

void SS_dynamixel_close(Dynamixel *servo) {
    SS_dynamixel_set_goal_position(&dynamixel, servo->closed_position);
}

/* Setters */

void SS_dynamixel_enable_torque(Dynamixel *servo) {
    if (servo->torque_enabled)
        return;
    uint8_t data = 0x01;
    servo->torque_enabled = true;
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_TORQUE_ENABLE, &data, 1);
}

void SS_dynamixel_disable_torque(Dynamixel *servo) {
    if (!servo->torque_enabled)
        return;
    uint8_t data = 0x00;
    servo->torque_enabled = false;
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_TORQUE_ENABLE, &data, 1);
}

void SS_dynamixel_enable_led(Dynamixel *servo) {
    uint8_t buf = 0x01;
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_LED, &buf, 1);
}

void SS_dynamixel_disable_led(Dynamixel *servo) {
    uint8_t buf = 0x00;
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_LED, &buf, 1);
}

void SS_dynamixel_set_goal_position(Dynamixel *servo, int32_t position) {
    SS_dynamixel_enable_torque(servo);
    int32_t max_pos = (servo->opened_position > servo->closed_position) ? servo->opened_position : servo->closed_position;
    int32_t min_pos = (servo->opened_position < servo->closed_position) ? servo->opened_position : servo->closed_position;
    if(position > max_pos)
        position = max_pos;
    if(position < min_pos)
        position = min_pos;
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_GOAL_POSITION, (uint8_t*) &position, 4);
}

void SS_dynamixel_set_velocity(Dynamixel *servo, uint32_t velocity) {
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_GOAL_VELOCITY, (uint8_t*) &velocity, 4);
}

void SS_dynamixel_set_velocity_limit(Dynamixel *servo, uint32_t limit) {
    SS_dynamixel_write_DMA(servo, DYNAMIXEL_VELOCITY_LIMIT, (uint8_t*) &limit, 4);
}

void SS_dynamixel_set_opened_position(Dynamixel *servo, uint32_t position) {
    servo->opened_position = position;
    Dynamixel_ID id = (position > servo->closed_position) ? DYNAMIXEL_MAX_POSITION_LIMIT : DYNAMIXEL_MIN_POSITION_LIMIT;
    SS_dynamixel_write_DMA(servo, id, (uint8_t*) &position, 4);
}

void SS_dynamixel_set_closed_position(Dynamixel *servo, uint32_t position) {
    servo->closed_position = position;
    Dynamixel_ID id = (position > servo->opened_position) ? DYNAMIXEL_MAX_POSITION_LIMIT : DYNAMIXEL_MIN_POSITION_LIMIT;
    SS_dynamixel_write_DMA(servo, id, (uint8_t*) &position, 4);
}

/* Getters */

void SS_dynamixel_get_position(Dynamixel *servo) {
    SS_dynamixel_read_DMA(servo, DYNAMIXEL_PRESENT_POSITION, (uint8_t*) &servo->present_position, 4);
}

void SS_dynamixel_get_moving(Dynamixel *servo) {
    SS_dynamixel_read_DMA(servo, DYNAMIXEL_MOVING, (uint8_t*) &servo->moving, 1);
}

void SS_dynamixel_get_current(Dynamixel *servo) {
    SS_dynamixel_read_DMA(servo, DYNAMIXEL_PRESENT_CURRENT, (uint8_t*) &servo->present_current, 2);
}

void SS_dynamixel_get_temperature(Dynamixel *servo) {
    SS_dynamixel_read_DMA(servo, DYNAMIXEL_PRESENT_TEMPERATURE, (uint8_t*) &servo->temperature, 1);
}

/********** Packet manipulation **********/

void SS_dynamixel_prepare_packet(Instruction_packet *packet, uint8_t *params, Dynamixel_fifo_bufor *buff) {
    memcpy(buff->packet, packet, sizeof(Instruction_packet));
    uint16_t params_size = packet->length - 3;
    memcpy(buff->packet + sizeof(Instruction_packet), params, params_size);
    uint16_t crc = SS_dynamixel_update_crc(0, (uint8_t*) buff->packet, sizeof(Instruction_packet) + params_size);
    memcpy(buff->packet + sizeof(Instruction_packet) + params_size, &crc, 2);
    buff->packet_size = sizeof(Instruction_packet) + params_size + 2;
}

void SS_dynamixel_create_packet(Dynamixel *servo, Dynamixel_instruction instruction, uint8_t *params, uint16_t params_len, Dynamixel_fifo_bufor *buff) {
    Instruction_packet packet = {
            .header = 0xFDFFFF,
            .reserved = 0x00,
            .packetID = servo->id,
            .length = params_len + 3,
            .instruction = instruction
    };
    SS_dynamixel_prepare_packet(&packet, params, buff);
}

/********** Polling instructions **********/

Dynamixel_status SS_dynamixel_write(Dynamixel *servo, uint16_t reg, void *data, uint16_t size) {
    bool torque_status = servo->torque_enabled;
    if (reg < DYNAMIXEL_TORQUE_ENABLE) {
        SS_dynamixel_disable_torque(servo);
        HAL_Delay(UART_TIMEOUT);
    }
    memcpy(params, &reg, 2);
    memcpy(params + 2, data, size);
    SS_dynamixel_send_packet(servo, DYNAMIXEL_WRITE, params, size + 2);
    Dynamixel_status res = SS_dynamixel_receive(11);
    if (torque_status && reg < DYNAMIXEL_TORQUE_ENABLE) {
        SS_dynamixel_enable_torque(servo);
    }
    return res;
}

Dynamixel_status SS_dynamixel_read(Dynamixel *servo, uint16_t reg, void *data, uint16_t size) {
    memcpy(params, &reg, 2);
    memcpy(params + 2, &size, 2);
    SS_dynamixel_send_packet(servo, DYNAMIXEL_READ, params, 4);
    Dynamixel_status res = SS_dynamixel_receive(11 + size);
    memcpy(data, rx_packet_buff + 9, size);
    return res;
}

Dynamixel_status SS_dynamixel_factory_reset(Dynamixel *servo) {
    uint8_t data = 0xFF;
    SS_dynamixel_send_packet(servo, DYNAMIXEL_FACTORY_RESET, &data, 1);
    Dynamixel_status res = SS_dynamixel_receive(11);
    /* Prevent connection timeout during reset */
    for(uint8_t i = 0; i < 8; i++) {
        servo->connection_timeout = 0;
        HAL_Delay(50);
    }
    return res;
}

Dynamixel_status SS_dynamixel_ping(Dynamixel *servo) {
    SS_dynamixel_send_packet(servo, DYNAMIXEL_PING, NULL, 0);
    return SS_dynamixel_receive(14);
}
/********** Polling communication **********/

Dynamixel_status SS_dynamixel_transmit(Dynamixel_fifo_bufor *buff) {
    if(!SS_mutex_lock(&dynamixel.mutex)) {
        SS_fifo_put_data(&dynamixel_fifo, buff);
        return DYNAMIXEL_BUSY;
    }
    memcpy(tx_packet_buff, buff->packet, MAX_PACKET_LENGTH);
    HAL_UART_AbortReceive(&DYNAMIXEL_UART);
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, SET);
    HAL_UART_Transmit(&DYNAMIXEL_UART, tx_packet_buff, buff->packet_size, 1000);
    SS_mutex_unlock(&dynamixel.mutex);
    return DYNAMIXEL_RESULT_OK;
}

Dynamixel_status SS_dynamixel_receive(uint16_t size) {
    if(!SS_mutex_lock(&dynamixel.mutex)) return DYNAMIXEL_BUSY;
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, RESET);
    HAL_UART_Receive(&huart1, rx_packet_buff, size, 1000);
    SS_mutex_unlock(&dynamixel.mutex);
    if (!SS_dynamixel_check_crc(rx_packet_buff, size)) {
        return DYNAMIXEL_REC_CRC_ERROR;
    }
    return rx_packet_buff[8];
}

void SS_dynamixel_send_packet(Dynamixel *servo, Dynamixel_instruction instruction, uint8_t *params, uint16_t params_len) {
    SS_dynamixel_create_packet(servo, instruction, params, params_len, &tmp_packet_buff);
    SS_dynamixel_transmit(&tmp_packet_buff);
}

/********** DMA instructions **********/

void SS_dynamixel_write_DMA(Dynamixel *servo, uint16_t reg, void *data, uint16_t size) {
    bool torque_enabled = servo->torque_enabled;
    if (reg < DYNAMIXEL_TORQUE_ENABLE) {
        SS_dynamixel_disable_torque(servo);
    }
    memcpy(params, &reg, 2);
    memcpy(params + 2, data, size);
    SS_dynamixel_send_packet_DMA(servo, DYNAMIXEL_WRITE, params, size + 2, 11, torque_enabled, NULL);
}

void SS_dynamixel_read_DMA(Dynamixel *servo, uint16_t reg, void *data, uint16_t size) {
    memcpy(params, &reg, 2);
    memcpy(params + 2, &size, 2);
    SS_dynamixel_send_packet_DMA(servo, DYNAMIXEL_READ, params, 4, size + 11, servo->torque_enabled, data);
}

/********** DMA Communication **********/

void SS_dynamixel_transmit_receive_DMA(Dynamixel_fifo_bufor *buff) {
    if(!SS_mutex_lock(&dynamixel.mutex)) {
        SS_fifo_put_data(&dynamixel_fifo, buff);
        return;
    }
    rec_size = buff->rec_size;
    torque_status = buff->torque_status;
    dest_data = buff->data;
    packet_size = buff->packet_size;
//    printf("rec size: %d\r\n", rec_size);
    memcpy(tx_packet_buff, buff->packet, MAX_PACKET_LENGTH);
    HAL_UART_AbortReceive_IT(&DYNAMIXEL_UART);
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, SET);
#ifndef SIMULATE
    HAL_UART_Transmit_DMA(&DYNAMIXEL_UART, tx_packet_buff, packet_size);
#else
    HAL_UART_Transmit_IT(&DYNAMIXEL_UART, tx_packet_buff, packet_size);
#endif
}

void SS_dynamixel_send_packet_DMA(Dynamixel *servo, Dynamixel_instruction instruction, uint8_t *params, uint16_t params_len, uint16_t rec_len, uint8_t torque_enabled, void *data) {
    SS_dynamixel_create_packet(servo, instruction, params, params_len, &tmp_packet_buff);
    tmp_packet_buff.data = data;
    tmp_packet_buff.rec_size = rec_len;
    tmp_packet_buff.torque_status = torque_enabled;
    SS_dynamixel_transmit_receive_DMA(&tmp_packet_buff);
}

void SS_dynamixel_ping_DMA(Dynamixel *servo) {
    SS_dynamixel_send_packet_DMA(servo, DYNAMIXEL_PING, NULL, 0, 14, servo->torque_enabled, ping_response);
}

/********** FIFO **********/

bool SS_dynamixel_send_from_fifo() {
    bool res = false;
    if((res = SS_fifo_get_data(&dynamixel_fifo, &tmp_packet_buff))) {
        SS_dynamixel_transmit_receive_DMA(&tmp_packet_buff);
    }
    return res;
}

/********** CRC **********/

bool SS_dynamixel_check_crc(uint8_t *data, uint16_t size) {
    uint16_t expected = *((uint16_t*) (data + size - 2));
    uint16_t actual = SS_dynamixel_update_crc(0, data, size - 2);
    return expected == actual;
}
static uint16_t crc_table[256] = { 0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
           0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022, 0x8063, 0x0066, 0x006C,
           0x8069, 0x0078, 0x807D, 0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E,
           0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0,
           0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5, 0x80AF, 0x00AA,
           0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087,
           0x0082, 0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, 0x01B0, 0x81B5,
           0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
           0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
           0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176, 0x017C,
           0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D,
           0x8137, 0x0132, 0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101, 0x8303,
           0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A,
           0x832B, 0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374,
           0x8371, 0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
           0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8,
           0x83ED, 0x83E7, 0x03E2, 0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
           0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381, 0x0280, 0x8285, 0x828F,
           0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD,
           0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0,
           0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243, 0x0246, 0x024C, 0x8249,
           0x0258, 0x825D, 0x8257, 0x0252, 0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
           0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 0x8213, 0x0216,
           0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202 };

uint16_t SS_dynamixel_update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size) {
    uint16_t i, j;
    for (j = 0; j < data_blk_size; j++) {
        i = ((uint16_t) (crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }
    return crc_accum;
}

Dynamixel_status SS_dynamixel_get_status() {
    if (!SS_dynamixel_check_crc(rx_packet_buff, rec_size)) {
        return DYNAMIXEL_REC_CRC_ERROR;
    }
    return rx_packet_buff[8];
}

/********** Callbacks **********/

void SS_dynamixel_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &DYNAMIXEL_UART) {
        dynamixel.connection_timeout = 0;
        dynamixel.connected = true;
        dynamixel.last_status = SS_dynamixel_get_status();
        if (dynamixel.last_status != DYNAMIXEL_RESULT_OK) {
            SS_mutex_unlock(&dynamixel.mutex);
            SS_dynamixel_send_from_fifo();
            return;
        }
        switch (GET_INSTRUCTION(tx_packet_buff)) {
            case DYNAMIXEL_READ:
                if(GET_REG(tx_packet_buff) == DYNAMIXEL_MOVING) {
                    /* Test if needed */
//                    if(!dynamixel.moving) {
//                        SS_dynamixel_disable_torque(&dynamixel);
//                    }
                }
//                if(GET_REG(tx_packet_buff) == DYNAMIXEL_PRESENT_POSITION) {
//                }
                if(GET_REG(tx_packet_buff) == DYNAMIXEL_PRESENT_CURRENT) {
//                    float current = dynamixel.present_current * 0.00269;
//                    printf("curr int: %d\r\n", dynamixel.present_current);
//                    printf("curr: %f\r\n", current);
                }
                memcpy(dest_data, rx_packet_buff + 9, GET_LENGTH(tx_packet_buff));
                break;
            case DYNAMIXEL_WRITE:
                if (torque_status && GET_REG(tx_packet_buff) < DYNAMIXEL_TORQUE_ENABLE) {
                    SS_dynamixel_enable_torque(&dynamixel);
                }
                break;
            case DYNAMIXEL_PING:
//                printf("Ping\r\n");
                break;
            default:
                break;
        }
        SS_mutex_unlock(&dynamixel.mutex);
        SS_dynamixel_send_from_fifo();
    }
}

void SS_dynamixel_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &DYNAMIXEL_UART) {
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, RESET);
#ifndef SIMULATE
        HAL_UART_Receive_DMA(&DYNAMIXEL_UART, rx_packet_buff, rec_size);
#else
        HAL_UART_Receive_IT(&DYNAMIXEL_UART, rx_packet_buff, rec_size);
#endif
    }
}

static void (*systick_get_functions[])(Dynamixel*) = {
        SS_dynamixel_get_moving,
        SS_dynamixel_get_current,
        SS_dynamixel_get_position,
        SS_dynamixel_get_temperature
};

/* Note that with measurements_callback disabled every 100ms a timeout will occur*/
static inline void SS_dynamixel_connection_timeout_callback() {
    /* TODO Check why 10 pings are sent */
    dynamixel.connection_timeout++;
    if(dynamixel.connection_timeout >= 100) {
        dynamixel.connected = false;
        SS_mutex_unlock(&dynamixel.mutex);
        Dynamixel_fifo_bufor ignore;
        /* Flush queue */
        for(uint8_t i = 0; i < dynamixel_fifo.buff_size; i++) {
            if(!SS_fifo_get_data(&dynamixel_fifo, &ignore)) {
                break;
            }
        }
        SS_dynamixel_ping_DMA(&dynamixel);
        dynamixel.connection_timeout = 0;
    }
}

static inline void SS_dynamixel_measurements_callback() {
    static uint16_t counter = 0;
    if(dynamixel.connected && dynamixel.systick_enabled) {
        counter++;
        if(counter >= 10*sizeof(systick_get_functions) / sizeof(systick_get_functions[0])) {
            counter = 0;
        }
        if(counter % 10 == 0) {
            systick_get_functions[counter/10](&dynamixel);
        }
    }

}

void SS_dynamixel_SYSTICK_Callback() {
    SS_dynamixel_connection_timeout_callback();
    SS_dynamixel_measurements_callback();
}
