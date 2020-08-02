/*
 * SS_igniter_com.h
 *
 *  Created on: 02.08.2020
 *      Author: Maciek
 */

#ifndef SS_IGNITER_COM
#define SS_IGNITER_COM

#include "SS_com.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum {
    COM_IGNITER_IGNITE     = 0x01,
    COM_IGNITER_OFF        = 0x02,
    COM_IGNITER_RESISTANCE = 0x03,
    COM_IGNITER_STATUS     = 0x04,
} ComIgniterID;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

ComStatus SS_igniter_com_service(ComFrame *frame);
ComStatus SS_igniter_com_request(ComFrame *frame);
ComStatus SS_igniter_com_sequence_validate(ComFrame *frame);
ComStatus SS_igniter_sequence(uint8_t id, uint8_t operation, int16_t value);
void SS_igniter_sequence_finish(uint8_t id);

#endif

