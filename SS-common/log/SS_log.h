//
// Created by maciek on 29.02.2020.
//

#ifndef SS_LOG_H
#define SS_LOG_H

#include "printf.h"
#include "assert.h"
#include "stm32f4xx_hal.h"

void SS_log_init(UART_HandleTypeDef *huart);
void SS_error(const char *format, ...);
void SS_print(const char *format, ...);
void SS_log_task(void *pvParameters);
void SS_log_tx_isr(UART_HandleTypeDef *huart);

#endif //SS_LOG_H
