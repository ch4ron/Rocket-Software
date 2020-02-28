/*
 * SS_Grazyna_com.h
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#ifndef SS_GRAZYNA_H_
#define SS_GRAZYNA_H_

#include "SS_com_protocol.h"
#include "usart.h"

#define GRAZYNA_HEADER 0x05

typedef struct __attribute__((packed)) {
    uint8_t header;
    ComFrame com_frame;
    uint32_t crc;
} GrazynaFrame;

void SS_grazyna_init(UART_HandleTypeDef *huart);
void SS_grazyna_main();
void SS_grazyna_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void SS_grazyna_transmit(ComFrame *frame);

#endif /* SS_GRAZYNA_COM_H_ */
