/**
  * SS_console.h
  *
  *  Created on: May 2, 2020
  *      Author: Maciek
 **/

#ifndef SS_CONSOLE_H
#define SS_CONSOLE_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "stm32f4xx_hal.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_console_init(UART_HandleTypeDef *huart);
void SS_console_task(void *pvParameters);
void SS_console_rx_isr(UART_HandleTypeDef *huart);

#endif
