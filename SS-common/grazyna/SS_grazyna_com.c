/*
 * SS_grazyna_com.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "SS_Grazyna_com.h"
#include "SS_com_debug.h"
#include "crc.h"
#include "stdio.h"
#include "SS_fifo.h"
#include "string.h"

typedef enum {
    GRAZYNA_LOOKING_FOR_HEADER,
    GRAZYNA_RECEIVING_FRAME
} GrazynaState;

FIFO_INIT(grazyna, 10, ComFrame)

static GrazynaFrame rx_frame, tx_frame;
static GrazynaState grazyna_state;
static ComFrame frame;
static ComFrameContent frame_content;
static uint32_t frame_crc, calculated_crc;

void SS_grazyna_receive(uint8_t *data, uint16_t length) {
#ifndef SIMULATE
    HAL_UART_Receive_DMA(&GRAZYNA_UART, data, length);
#else
    HAL_UART_Receive_IT(&GRAZYNA_UART, data, length);
#endif
}

void SS_grazyna_init() {
    grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
    SS_grazyna_receive((uint8_t*) &rx_frame, sizeof(rx_frame.header));
}

void SS_grazyna_main() {
    if(SS_fifo_get_data(&grazyna_fifo, &frame)) {
        SS_com_parse_frame(&frame, &frame_content);
//        SS_com_print_message_received(&frame_content);
        SS_grazyna_transmit(&frame);
    }
}

uint32_t SS_grazyna_CRC_calculate(GrazynaFrame *grazyna_frame) {
    static uint32_t buff[3];
    memcpy(buff, grazyna_frame, sizeof(GrazynaFrame) - sizeof(grazyna_frame->crc));
    return HAL_CRC_Calculate(&hcrc, buff, 3);
}

void SS_grazyna_prepare_tx_frame(ComFrame *com_frame, GrazynaFrame *grazyna_frame) {
    grazyna_frame->header = GRAZYNA_HEADER;
    memcpy(&grazyna_frame->com_frame, com_frame, sizeof(ComFrame));
    calculated_crc = SS_grazyna_CRC_calculate(grazyna_frame);
    grazyna_frame->crc = calculated_crc;
//    printf("\r\ncrc:0x%04x", calculated_crc);
}

/* Add mutex for transmission */
void SS_grazyna_transmit(ComFrame *frame) {
    SS_grazyna_prepare_tx_frame(frame, &tx_frame);
        SS_com_parse_frame(frame, &frame_content);
    /*
    SS_com_print_message_sent(&frame_content);
    uint8_t *p = (uint8_t*) &tx_frame;
    printf("\r\n");
    for(int i = 0; i < sizeof(tx_frame);i++) {
        printf("0x%02x, ", p[i]);
    }
    */
#ifndef SIMULATE
    HAL_UART_Transmit_DMA(&GRAZYNA_UART, (uint8_t*) &tx_frame, sizeof(tx_frame));
#else
    HAL_UART_Transmit_IT(&GRAZYNA_UART, (uint8_t*) &tx_frame, sizeof(tx_frame));
#endif
}

static void SS_grazyna_handle_frame() {
    calculated_crc = SS_grazyna_CRC_calculate(&rx_frame);
/* TODO - implement CRC mock */
#ifndef SIMULATE
    if(frame_crc == calculated_crc) {
        SS_fifo_put_data(&grazyna_fifo, &rx_frame.com_frame);
    }
#else
    SS_fifo_put_data(&grazyna_fifo, &rx_frame.com_frame);
#endif
    grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
    SS_grazyna_receive((uint8_t*) &rx_frame, sizeof(rx_frame.header));
}


static void SS_grazyna_handle_header() {
   if(rx_frame.header == GRAZYNA_HEADER) {
       grazyna_state = GRAZYNA_RECEIVING_FRAME;
       SS_grazyna_receive((uint8_t*) &rx_frame + sizeof(rx_frame.header), sizeof(rx_frame) - sizeof(rx_frame.header));
   } else {
       grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
       SS_grazyna_receive((uint8_t*) &rx_frame, sizeof(rx_frame.header));
   }
}

void SS_grazyna_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart == &GRAZYNA_UART) {
       switch(grazyna_state) {
           case GRAZYNA_LOOKING_FOR_HEADER:
               SS_grazyna_handle_header();
               break;
           case GRAZYNA_RECEIVING_FRAME:
               SS_grazyna_handle_frame();
               break;
       }
    }
}
