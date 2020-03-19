//
// Created by maciek on 18.03.2020.
//

#ifndef SS_CAN_H
#define SS_CAN_H

#include "stm32f4xx_hal.h"
#include "can.h"
#include "SS_com_ids.h"
#include "SS_com.h"

#define CAN_DEBUG
#define CAN_DEBUG_ERRORS

void SS_can_init(CAN_HandleTypeDef *hcan, ComBoardID board);
void SS_can_start(void);
void SS_can_rx_put_to_fifo_data(CAN_HandleTypeDef *hcan, uint8_t priority);
int8_t SS_can_rx_handle_from_fifo_data(uint8_t priority);
void SS_can_transmit(ComFrame *frame, uint8_t priority);
void SS_can_tx_data_fifo(uint8_t priority);
void SS_can_error(char *error);


#endif // SS_CAN_H
