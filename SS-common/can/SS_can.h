/**
  * SS_can.h
  *
  *  Created on: Mar 03, 2020
  *      Author: maciek
 **/

#ifndef SS_CAN_H
#define SS_CAN_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com.h"
#include "SS_com_ids.h"
#include "stdint.h"
#include "stm32f4xx.h"

/* ==================================================================== */
/* ========================== Public macros =========================== */
/* ==================================================================== */

#define CAN_DEBUG_ERRORS

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_can_init(CAN_HandleTypeDef *hcan, ComBoardID board);
void SS_can_enable_grazyna();
void SS_can_disable_grazyna();
void SS_can_transmit(ComFrame *frame);
void SS_can_tx_handler_task(void *pvParameters);
#ifdef SS_USE_EXT_CAN
void SS_can_ext_init(CAN_HandleTypeDef *hcan);
void SS_can_ext_transmit(ComFrame *frame);
#endif

#endif  // SS_CAN_H
