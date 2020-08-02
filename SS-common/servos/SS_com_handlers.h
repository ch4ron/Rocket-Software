/**
  * SS_com.c
  *
  *  Created on: Jul 02, 2020
  *      Author: Maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com_ids.h"

#ifdef SS_USE_ADS1258
#include "SS_measurements.h"
#endif
#ifdef SS_USE_RELAYS
#include "SS_relays_com.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos_com.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel_com.h"
#endif
#ifdef SS_USE_SEQUENCE
#include "SS_sequence.h"
#endif

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef ComStatus (*ComFunction)(ComFrame *);

typedef struct {
    ComDeviceID id;
    ComFunction func;
} ComHandler;

/* ==================================================================== */
/* ============================= Handlers ============================= */
/* ==================================================================== */

static ComHandler service_handlers[] = {
#ifdef SS_USE_SERVOS
    {COM_SERVO_ID, SS_servos_com_service},
#endif
#ifdef SS_USE_RELAYS
    {COM_RELAY_ID, SS_relays_com_service},
#endif
#ifdef SS_USE_DYNAMIXEL
    {COM_DYNAMIXEL_ID, SS_dynamixel_com_service},
#endif
#ifdef SS_USE_SEQUENCE
    {COM_SEQUENCE_ID, SS_sequence_com_service},
#endif
};

static ComHandler request_handlers[] = {
#ifdef SS_USE_SERVOS
    {COM_SERVO_ID, SS_servos_com_request},
#endif
#ifdef SS_USE_RELAYS
    {COM_RELAY_ID, SS_relays_com_request},
#endif
#ifdef SS_USE_DYNAMIXEL
    {COM_DYNAMIXEL_ID, SS_dynamixel_com_request},
#endif
#ifdef SS_USE_ADS1258
    {COM_MEASUREMENT_ID, SS_ADS1258_com_request},
#endif
};

#ifdef SS_USE_SEQUENCE
static ComHandler sequence_handlers[] = {
#ifdef SS_USE_SERVOS
    {COM_SERVO_ID, SS_servos_com_sequence_validate},
#endif
#ifdef SS_USE_RELAYS
    {COM_RELAY_ID, SS_relays_com_sequence_validate},
#endif
#ifdef SS_USE_DYNAMIXEL
    {COM_DYNAMIXEL_ID, SS_dynamixel_com_sequence_validate},
#endif
};
#endif


