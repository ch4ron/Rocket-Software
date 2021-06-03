
/*
 *
 * SS_igniter_com.c
 *
 *  Created on: 02.08.2020
 *      Author: Maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_COM

#include "SS_igniter_com.h"

#include "SS_log.h"
#include "SS_igniter.h"
#include "string.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

ComStatus SS_igniter_com_service(ComFrame *frame) {
    ComIgniterID msgID = frame->operation;
    switch(msgID) {
        case COM_IGNITER_IGNITE:
            SS_igniter_ignite(frame->payload);
            break;
        case COM_IGNITER_OFF:
            SS_igniter_off();
            break;
        default:
            SS_error("Unhandled Grazyna igniter service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

ComStatus SS_igniter_com_request(ComFrame *frame) {
    ComIgniterID msgID = frame->operation;
    uint8_t status;
    switch(msgID) {
        case COM_IGNITER_STATUS:
            status = SS_igniter_status();
            SS_com_add_payload_to_frame(frame, UINT8, &status);
            break;
        default:
            SS_error("Unhandled Grazyna igniter request: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_ERROR;
}

#ifdef SS_USE_SEQUENCE

ComStatus SS_igniter_com_sequence_validate(ComFrame *frame) {
    ComUInt16Int16 value;
    memcpy(&value, &frame->payload, sizeof(uint32_t));
    switch(frame->operation) {
        case COM_IGNITER_IGNITE:
        case COM_IGNITER_OFF:
            return COM_OK;
        default:
            return COM_ERROR;
    }
}

ComStatus SS_igniter_sequence(uint8_t id, uint8_t operation, int16_t value) {
    switch(operation) {
        case COM_IGNITER_IGNITE:
            SS_igniter_ignite(value);
            return COM_OK;
        case COM_IGNITER_OFF:
            SS_igniter_off();
            break;
        default:
            return COM_ERROR;
    }
    return COM_OK;
}

void SS_igniter_sequence_finish(uint8_t id) {
    (void) id;
    SS_igniter_off();
}

#endif /* SS_USE_SEQUENCE */

#endif /* SS_USE_COM */
