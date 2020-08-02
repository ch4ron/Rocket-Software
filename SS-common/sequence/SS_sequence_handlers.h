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
#ifdef SS_USE_IGNITER
#include "SS_igniter_com.h"
#endif

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef ComStatus (*SequenceFunction)(uint8_t id, uint8_t operation, int16_t value);
typedef void (*SequenceFinishFunction)(uint8_t id);

typedef struct {
    ComDeviceID device;
    SequenceFunction run;
    SequenceFinishFunction finish;
} SequenceHandler;

/* ==================================================================== */
/* ============================= Handlers ============================= */
/* ==================================================================== */

static SequenceHandler sequence_handlers[] = {
#ifdef SS_USE_SERVOS
    {COM_SERVO_ID, SS_servos_sequence, SS_servos_sequence_finish},
#endif
#ifdef SS_USE_DYNAMIXEL
    {COM_DYNAMIXEL_ID, SS_dynamixel_sequence, SS_dynamixel_sequence_finish},
#endif
#ifdef SS_USE_RELAYS
    {COM_RELAY_ID, SS_relays_sequence, SS_relays_sequence_finish},
#endif
#ifdef SS_USE_IGNITER
    {COM_IGNITER_ID, SS_igniter_sequence, SS_igniter_sequence_finish},
#endif
};
