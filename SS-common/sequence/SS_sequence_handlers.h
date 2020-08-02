/**
  * SS_sequence_handlers.h
  *
  *  Created on: Jul 02, 2020
  *      Author: Maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_RELAYS
#include "SS_relays_com.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos_com.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel_com.h"
#endif

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef ComStatus (*SequenceFunction)(uint8_t id, uint8_t operation, int16_t value);

typedef struct {
    ComDeviceID device;
    SequenceFunction func;
} SequenceHandler;

/* ==================================================================== */
/* ============================= Handlers ============================= */
/* ==================================================================== */

static SequenceHandler sequence_handlers[] = {
#ifdef SS_USE_SERVOS
    {COM_SERVO_ID, SS_servos_sequence},
#endif
#ifdef SS_USE_DYNAMIXEL
    {COM_DYNAMIXEL_ID, SS_dynamixel_sequence},
#endif
#ifdef SS_USE_RELAYS
    {COM_RELAY_ID, SS_relays_sequence},
#endif
};
