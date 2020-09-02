#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include "stm32f4xx_hal.h"

#define UART_DEBUG_STRUCT huart2

extern UART_HandleTypeDef UART_DEBUG_STRUCT;

extern char DEBUG_tx_buff [500];
extern uint16_t DEBUG_tx_length;

#define __print_debug_to_UART(x) DEBUG_tx_length = sprintf(DEBUG_tx_buff,x);\
HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)DEBUG_tx_buff,DEBUG_tx_length, 500);


#endif /* UART_DEBUG_H_ */
