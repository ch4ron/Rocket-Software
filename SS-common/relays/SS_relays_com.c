
/*
 *
 * SS_dynamixel_com.c
 *
 *  Created on: 01.05.2020
 *      Author: Maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_COM

#include "SS_relays_com.h"

#include "SS_log.h"
#include "SS_relays.h"
#include "string.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

ComStatus SS_relays_com_service(ComFrame *frame) {
    ComRelayID msgID = frame->operation;
    Relay *relay = SS_relay_get(frame->id);
    if(relay == NULL) return COM_ERROR;
    switch(msgID) {
        case COM_RELAY_OPEN:
            SS_relay_open(relay);
            break;
        case COM_RELAY_CLOSE:
            SS_relay_close(relay);
            break;
        default:
            SS_error("Unhandled Grazyna relay service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

ComStatus SS_relays_com_request(ComFrame *frame) {
    ComRelayID msgID = frame->operation;
    Relay *relay = SS_relay_get(frame->id);
    if(relay == NULL) return COM_ERROR;
    switch(msgID) {
        case COM_RELAY_STATUS:
            SS_com_add_payload_to_frame(frame, UINT8, &relay->state);
            break;
        default:
            SS_error("Unhandled Grazyna relay request: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

#ifdef SS_USE_SEQUENCE

ComStatus SS_relays_com_sequence_validate(ComFrame *frame) {
    Relay *relay = SS_relay_get(frame->id);
    if(relay == NULL) return COM_ERROR;
    ComUInt16Int16 value;
    memcpy(&value, &frame->payload, sizeof(uint32_t));
    switch(frame->operation) {
        case COM_RELAY_OPEN:
        case COM_RELAY_CLOSE:
            return COM_OK;
        default:
            return COM_ERROR;
    }
}

ComStatus SS_relays_sequence(uint8_t id, uint8_t operation, int16_t value) {
    Relay *relay = SS_relay_get(id);
    if(relay == NULL) return COM_ERROR;
    switch(operation) {
        case COM_RELAY_OPEN:
            SS_relay_open(relay);
            break;
        case COM_RELAY_CLOSE:
            SS_relay_close(relay);
            break;
        default:
            return COM_ERROR;
    }
    return COM_OK;
}

void SS_relays_sequence_finish(uint8_t id) {
    Relay *relay = SS_relay_get(id);
    if(relay) {
        SS_relay_close(relay);
    }
}

#endif /* SS_USE_SEQUENCE */

#endif /* SS_USE_COM */
