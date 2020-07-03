/*
 * SS_com.h
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#ifndef SS_COM_H_
#define SS_COM_H_

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t com_feed_task;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_com_feed_task();
void SS_com_feed_enable();
void SS_com_feed_disable();

#endif /* SS_COM_H_ */
