/*
 * SS_com.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */


#include "SS_com.h"

//uint8_t COMMUNICATION = 0;

//can_board communication_board;

/* Board specific stuff BEGIN */
//parameter parameters[] = {
//        { PAUEK_CELL_1, &cell_scaled_main[0], SHORT },
//        { PAUEK_CELL_2, &cell_scaled_main[1], SHORT },
//        { PAUEK_CELL_3, &cell_scaled_main[2], SHORT },
//        { PAUEK_CELL_4, &cell_scaled_main[3], SHORT },
//        { PAUEK_BATTERY_MAIN, &battery_scaled[0], SHORT },
//        { PAUEK_BATTERY_BACKUP, &battery_scaled[1], SHORT },
//        { PAUEK_BATTERY_EXTERNAL, &battery_scaled[2], SHORT },
//};

void SS_com_calculate_scaled_values(void) {
}
/* Board specific stuff END */

//void SS_com_can_main(void) {
//    SS_can_rx_handle_from_fifo_data(CAN_HIGH_PRIORITY);
//    SS_can_rx_handle_from_fifo_data(CAN_LOW_PRIORITY);
//    SS_can_tx_data_fifo(CAN_HIGH_PRIORITY);
//    SS_can_tx_data_fifo(CAN_LOW_PRIORITY);
//}

//void SS_com_send_parameter(uint32_t *current_parameter) {
//    int dat;
//    float fl;
//    switch(parameters[*current_parameter].type) {
//        case INT:
//            memcpy(&dat, parameters[*current_parameter].data, 4);
//            dat *= 100;
//            break;
//        case SHORT: {
//            //Only battery values are 16 bits long and they are multiplied by 100 during conversion
//            uint16_t tmp;
//            memcpy(&tmp, parameters[*current_parameter].data, 2);
//            dat = tmp;
//            break;
//        }
//        case FLOAT:
//            memcpy(&fl, parameters[*current_parameter].data, 4);
//            dat = (int) (fl * 100);
//            break;
//        default:
//            return;
//    }
////    if(communication_board == CAN_BOARD_ID){
////        SS_grazyna_put_to_fifo(0x15,  parameters[(*current_parameter)++].header, dat);
////    }
////    else
//        SS_can_send_to_grazyna(parameters[(*current_parameter)++].header, (uint8_t*) &dat, 4, communication_board, CAN_LOW_PRIORITY);
//}

//void SS_com_report_parameters(uint32_t board, can_board com_board, uint32_t *current_parameter) {
//    communication_board = com_board;
//    SS_com_calculate_scaled_values();
//    uint32_t messages = sizeof(parameters) / sizeof(parameter);
//    SS_can_put_to_fifo_data(CAN_PARAMETERS_NUMBER, (uint8_t*) &messages, 4, board, CAN_LOW_PRIORITY);
//    *current_parameter = 0;
//}

//void SS_com_handle_parameters_update(uint32_t can_id, can_board com_board, uint32_t board) {
//    static uint32_t current_parameter = 0;
//    switch (can_id) {
//        case CAN_PARAMETERS_NUMBER:
//            SS_com_report_parameters(board, com_board, &current_parameter);
//            break;
//        case CAN_PARAMETER:
//            SS_com_send_parameter(&current_parameter);
//            break;
//    }
//}

//void SS_com_forward_to_grazyna(can_fifo_bufor *fifo_bufor, uint32_t can_id, uint32_t bufor) {
//    if(fifo_bufor->header.data.grazyna)
//        SS_grazyna_put_to_fifo(0x15, can_id - CAN_OFFSET, bufor);
//}
//
//void SS_com_handle_can_received(can_fifo_bufor *fifo_bufor, uint8_t priority) {
//    uint8_t data[fifo_bufor->length];
//    uint32_t bufor;
//    uint32_t board = fifo_bufor->header.data.source;
//    uint32_t can_id = fifo_bufor->header.data.id;
//    memcpy(data, fifo_bufor->data, fifo_bufor->length);
//    memcpy(&bufor, fifo_bufor->data, fifo_bufor->length);
//    SS_com_forward_to_grazyna(fifo_bufor, can_id, bufor);
//    SS_com_handle_parameters_update(can_id, bufor, board);
//    SS_com_can_received_handler(can_id, board, data, fifo_bufor->length, priority);
//}

//void SS_com_grazyna_main(void) {
//    SS_com_handle_grazyna_received();
//    SS_grazyna_send_from_fifo();
//}

void SS_com_can_main_kromek(void) {
    //    SS_can_tx_data_fifo_kromek();
    //    SS_can_tx_data_fifo_kromek_priority();
}
