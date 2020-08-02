/*
 * SS_servos_com.h
 *
 *  Created on: 02.08.2020
 *      Author: Maciek
 */

#ifndef SS_RELAYS_COM
#define SS_RELAYS_COM

#include "SS_com.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum {
    COM_RELAY_OPEN = 0x01,
    COM_RELAY_CLOSE = 0x02,
    COM_RELAY_STATUS = 0x03,
} ComRelayID;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

ComStatus SS_relays_com_service(ComFrame *frame);
ComStatus SS_relays_com_request(ComFrame *frame);
ComStatus SS_relays_com_sequence_validate(ComFrame *frame);
ComStatus SS_relays_sequence(uint8_t id, uint8_t operation, int16_t value);

#endif

