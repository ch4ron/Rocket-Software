/*
 * SS_dynamixel_com.c
 *
 *  Created on: 01.05.2020
 *      Author: Maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_COM

#include "SS_servos_com.h"
#include "SS_servos.h"
#include "string.h"
#include "SS_log.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

ComStatus SS_servos_com_service(ComFrame *frame) {
    Servo *servo = SS_servo_get(frame->id);
    if(servo == NULL) return COM_ERROR;
    ComServoID msgID = frame->operation;
    uint32_t value = frame->payload;
    switch(msgID) {
        case COM_SERVO_OPEN:
            SS_servo_open(servo);
            break;
        case COM_SERVO_CLOSE:
            SS_servo_close(servo);
            break;
        case COM_SERVO_OPENED_POSITION:
            if(SS_servo_set_opened_position(servo, value) != 0) {
                return COM_ERROR;
            }
            break;
        case COM_SERVO_CLOSED_POSITION:
            if(SS_servo_set_closed_position(servo, value) != 0) {
                return COM_ERROR;
            }
            break;
        case COM_SERVO_POSITION:
            if(SS_servo_set_position(servo, value) != 0) {
                return COM_ERROR;
            }
            break;
        case COM_SERVO_DISABLE:
            SS_servo_disable(servo);
            break;
        case COM_SERVOS_RANGE:
            SS_servos_set_range(value);
            break;
        default:
            SS_error("Unhandled Grazyna servo service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

#ifdef SS_USE_SEQUENCE

ComStatus SS_servos_com_request(ComFrame *frame) {
    Servo *servo = SS_servo_get(frame->id);
    if(servo == NULL) return COM_ERROR;
    ComServoID msgID = frame->operation;
    uint32_t range;
    switch(msgID) {
        case COM_SERVO_OPENED_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &servo->opened_position);
            break;
        case COM_SERVO_CLOSED_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &servo->closed_position);
            break;
        case COM_SERVO_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &servo->position);
            break;
        case COM_SERVOS_RANGE:
            range = SS_servos_get_range();
            SS_com_add_payload_to_frame(frame, UINT16, &range);
            break;
        default:
            SS_error("Unhandled Grazyna servo request: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

ComStatus SS_servos_com_sequence_validate(ComFrame *frame) {
    Servo *servo = SS_servo_get(frame->id);
    if(servo == NULL) return COM_ERROR;
    Com2xInt16 value;
    memcpy(&value, &frame->payload, sizeof(uint32_t));
    switch(frame->operation) {
        case COM_SERVO_OPEN:
        case COM_SERVO_CLOSE:
        case COM_SERVO_DISABLE:
            return COM_OK;
        case COM_SERVO_POSITION:
            return SS_servo_check_position(servo, value.val) == 0 ? COM_OK : COM_ERROR;
        default:
            return COM_ERROR;
    }
}

ComStatus SS_servos_sequence(uint8_t id, uint8_t operation, int16_t value) {
    Servo *servo = SS_servo_get(id);
    if(servo == NULL) return COM_ERROR;
    switch(operation) {
        case COM_SERVO_OPEN:
            SS_servo_open(servo);
            break;
        case COM_SERVO_CLOSE:
            SS_servo_close(servo);
            break;
        case COM_SERVO_POSITION:
            SS_servo_set_position(servo, value);
            break;
        case COM_SERVO_DISABLE:
            SS_servo_disable(servo);
            break;
        default:
            return COM_ERROR;
    }
    return COM_OK;
}

#endif /* SS_USE_SEQUENCE */

#endif /* SS_USE_COM */
