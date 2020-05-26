/*
 * SS_dynamixel_com.c
 *
 *  Created on: 01.05.2020
 *      Author: Maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_dynamixel_com.h"

#include "SS_dynamixel.h"
#include "SS_log.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

ComStatus SS_dynamixel_com_service(ComFrame *frame) {
    ComDynamixelID msgID = frame->operation;
    uint32_t value = frame->payload;

    switch(msgID) {
        case COM_DYNAMIXEL_OPEN:
            if(SS_dynamixel_open(&dynamixel) != DYNAMIXEL_RESULT_OK) {
                return COM_ERROR;
            }
            break;
        case COM_DYNAMIXEL_CLOSE:
            if(SS_dynamixel_close(&dynamixel) != DYNAMIXEL_RESULT_OK) {
                return COM_ERROR;
            }
            break;
        case COM_DYNAMIXEL_OPENED_POSITION:
            if(SS_dynamixel_set_opened_position(&dynamixel, value) != DYNAMIXEL_RESULT_OK) {
                return COM_ERROR;
            }
            break;
        case COM_DYNAMIXEL_CLOSED_POSITION:
            if(SS_dynamixel_set_closed_position(&dynamixel, value) != DYNAMIXEL_RESULT_OK) {
                return COM_ERROR;
            }
            break;
        case COM_DYNAMIXEL_POSITION:
            if(SS_dynamixel_set_goal_position(&dynamixel, value) != DYNAMIXEL_RESULT_OK) {
                return COM_ERROR;
            }
            break;
        default:
            SS_error("Unhandled Grazyna dynamixel service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

ComStatus SS_dynamixel_com_request(ComFrame *frame) {
    ComDynamixelID msgID = frame->operation;
    switch(msgID) {
        case COM_DYNAMIXEL_OPENED_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &dynamixel.opened_position);
            break;
        case COM_DYNAMIXEL_CLOSED_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &dynamixel.closed_position);
            break;
        case COM_DYNAMIXEL_POSITION:
            if(SS_dynamixel_get_position(&dynamixel) != DYNAMIXEL_RESULT_OK) {
                return COM_ERROR;
            }
            SS_com_add_payload_to_frame(frame, UINT16, &dynamixel.present_position);
            break;
        default:
            SS_error("Unhandled Grazyna dynamixel request: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}
