/*
 * SS_servos_com.h
 *
 *  Created on: 02.08.2020
 *      Author: Maciek
 */

#ifndef SS_SERVOS_COM
#define SS_SERVOS_COM

#include "SS_com.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum {
    COM_SERVO_OPEN = 0x01,
    COM_SERVO_CLOSE,
    COM_SERVO_OPENED_POSITION,
    COM_SERVO_CLOSED_POSITION,
    COM_SERVO_POSITION,
    COM_SERVO_DISABLE,
    COM_SERVOS_RANGE,
} ComServoID;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

ComStatus SS_servos_com_service(ComFrame *frame);
ComStatus SS_servos_com_request(ComFrame *frame);
ComStatus SS_servos_com_sequence_validate(ComFrame *frame);
ComStatus SS_servos_sequence(uint8_t id, uint8_t operation, int16_t value);

#endif
