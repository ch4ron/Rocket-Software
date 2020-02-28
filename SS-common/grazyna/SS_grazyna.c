/*
 * SS_grazyna_com.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "SS_Grazyna.h"
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

typedef struct {
    GrazynaFrame rx_frame, tx_frame;
    GrazynaState grazyna_state;
    ComFrame frame;
    ComFrameContent frame_content;
    UART_HandleTypeDef *huart;
} Grazyna;

static Grazyna grazyna;

void SS_grazyna_receive(uint8_t *data, uint16_t length) {
#ifndef SIMULATE
    HAL_UART_Receive_DMA(grazyna.huart, data, length);
#else
    HAL_UART_Receive_IT(grazyna.huart, data, length);
#endif
}

void SS_grazyna_init(UART_HandleTypeDef *huart) {
    grazyna.grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
    SS_grazyna_receive((uint8_t*) &grazyna.rx_frame, sizeof(grazyna.rx_frame.header));
    grazyna.huart = huart;
}

void SS_grazyna_main() {
    if(SS_fifo_get_data(&grazyna_fifo, &grazyna.frame)) {
        SS_com_parse_frame(&grazyna.frame, &grazyna.frame_content);
//        SS_com_print_message_received(&frame_content);
        SS_grazyna_transmit(&grazyna.frame);
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
    grazyna_frame->crc = SS_grazyna_CRC_calculate(grazyna_frame);
//    printf("\r\ncrc:0x%04x", calculated_crc);
}

/* Add mutex for transmission */
void SS_grazyna_transmit(ComFrame *frame) {
    SS_grazyna_prepare_tx_frame(frame, &grazyna.tx_frame);
        SS_com_parse_frame(frame, &grazyna.frame_content);
    /*
    SS_com_print_message_sent(&frame_content);
    uint8_t *p = (uint8_t*) &grazyna.tx_frame;
    printf("\r\n");
    for(int i = 0; i < sizeof(grazyna.tx_frame);i++) {
        printf("0x%02x, ", p[i]);
    }
    */
#ifndef SIMULATE
    HAL_UART_Transmit_DMA(grazyna.huart, (uint8_t*) &grazyna.tx_frame, sizeof(grazyna.tx_frame));
#else
    HAL_UART_Transmit_IT(grazyna.huart, (uint8_t*) &grazyna.tx_frame, sizeof(grazyna.tx_frame));
#endif
}

static void SS_grazyna_handle_frame() {
    uint32_t calculated_crc = SS_grazyna_CRC_calculate(&grazyna.rx_frame);
/* TODO - implement CRC mock */
#ifndef SIMULATE
    if(grazyna.rx_frame.crc == calculated_crc) {
        SS_fifo_put_data(&grazyna_fifo, &grazyna.rx_frame.com_frame);
    }
#else
    SS_fifo_put_data(&grazyna_fifo, &grazyna.rx_frame.com_frame);
#endif
    grazyna.grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
    SS_grazyna_receive((uint8_t*) &grazyna.rx_frame, sizeof(grazyna.rx_frame.header));
}


static void SS_grazyna_handle_header() {
   if(grazyna.rx_frame.header == GRAZYNA_HEADER) {
       grazyna.grazyna_state = GRAZYNA_RECEIVING_FRAME;
       SS_grazyna_receive((uint8_t*) &grazyna.rx_frame + sizeof(grazyna.rx_frame.header), sizeof(grazyna.rx_frame) - sizeof(grazyna.rx_frame.header));
   } else {
       grazyna.grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
       SS_grazyna_receive((uint8_t*) &grazyna.rx_frame, sizeof(grazyna.rx_frame.header));
   }
}

void SS_grazyna_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart == grazyna.huart) {
       switch(grazyna.grazyna_state) {
           case GRAZYNA_LOOKING_FOR_HEADER:
               SS_grazyna_handle_header();
               break;
           case GRAZYNA_RECEIVING_FRAME:
               SS_grazyna_handle_frame();
               break;
       }
    }
}
