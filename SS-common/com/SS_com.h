/*
 * SS_com.h
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#ifndef SS_COM_H_
#define SS_COM_H_

//#include "SS_can.h"
#include "stm32f4xx_hal.h"

#define INT 0
#define SHORT 1
#define FLOAT 2

typedef struct parameter {
    const int header;
    const void *data;
    const uint8_t type;
} parameter;

extern uint32_t parameters_number[5];

void SS_com_grazyna_main(void);
//void SS_com_handle_parameters_update(uint32_t can_id, can_board com_board, uint32_t board);
int16_t SS_com_handle_onboard_parameters(uint16_t *counter);
uint8_t SS_com_get_board_to(uint32_t header);
//void SS_com_handle_can_received(can_fifo_bufor *fifo_bufor, uint8_t priority);
void SS_com_handle_grazyna_received(void);
void SS_com_handle_kromek_to_pauek(void);


#endif /* SS_COM_H_ */
