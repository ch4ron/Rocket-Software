/*
 * SS_com_protocol.c
 *
 *  Created on: Jan 16, 2020
 *      Author: maciek
 */

#ifdef SS_USE_GRAZYNA
#include "SS_Grazyna.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_measurements.h"
#endif
#ifdef SS_USE_RELAYS
#include "SS_relays.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos.h"
#endif

#include "SS_com_debug.h"
#include "SS_error.h"
#include "SS_com.h"
#include "stdio.h"
#include "string.h"


static ComBoardID board_id;

/* Functions in this module modify the received frame */

void SS_com_init(ComBoardID board) {
    board_id = board;
}

void SS_com_transmit(ComFrame *frame) {
    frame->source = board_id;
    SS_grazyna_transmit(frame);
}

ComStatus SS_com_handle_frame(ComFrame *frame) {
    SS_com_print_message_received(frame);
    ComStatus res = SS_com_handle_action(frame);
    frame->destination = frame->source;
    SS_com_transmit(frame);
    return res;
}

ComStatus SS_com_handle_action(ComFrame *frame) {
    ComActionID action = frame->action;
    switch(action) {
        case COM_REQUEST:
            return SS_com_handle_request(frame);
        case COM_SERVICE:
            return SS_com_handle_service(frame);
        default:
            SS_error("Unsupported action: %d\r\n", frame->action);
    }
    return COM_ERROR;
}

ComStatus SS_com_handle_request(ComFrame *frame) {
    ComDeviceID device = frame->device;
    ComStatus res = COM_OK;
    switch(device) {
#ifdef SS_USE_SERVOS
        case COM_SERVO_ID:
            res = SS_servos_com_request(frame);
            break;
#endif
#ifdef SS_USE_RELAYS
        case COM_RELAY_ID:
            res = SS_relays_com_request(frame);
            break;
#endif
#ifdef SS_USE_ADS1258
        case COM_MEASUREMENT_ID:
            res = SS_ADS1258_com_request(frame);
            break;
#endif
        case COM_SUPPLY_ID:
            break;
        case COM_MEMORY_ID:
            break;
        case COM_IGNITER_ID:
            break;
        case COM_TENSOMETER_ID:
            break;
        default:
            res = COM_ERROR;
            printf("Unsupported device: %d\r\n", frame->device);
    }
    frame->action = COM_RESPONSE;
    return res;
}

ComStatus SS_com_handle_service(ComFrame *frame) {
    ComDeviceID device = frame->device;
    ComStatus res = COM_OK;
    switch(device) {
#ifdef SS_USE_SERVOS
        case COM_SERVO_ID:
            res = SS_servos_com_service(frame);
            break;
#endif
#ifdef SS_USE_RELAYS
        case COM_RELAY_ID:
            res = SS_relay_com_service(frame);
            break;
#endif
        case COM_SUPPLY_ID:
            break;
        case COM_MEMORY_ID:
            break;
        case COM_IGNITER_ID:
            break;
        case COM_TENSOMETER_ID:
            break;
        default:
            res = COM_ERROR;
            SS_error("Unsupported device: %d\r\n", frame->action);
    }
    frame->action = COM_ACK;
    return res;
}

void SS_com_add_payload_to_frame(ComFrame *frame, ComDataType type, void *payload) {
    frame->data_type = type;
    switch(type) {
        case UINT32:
        case INT32:
        case FLOAT:
            memcpy(&frame->payload, payload, 4);
            break;
        case UINT16:
        case INT16:
            memcpy(&frame->payload, payload, 2);
            break;
        case UINT8:
        case INT8:
            memcpy(&frame->payload, payload, 1);
            break;
        default:
            break;
    }
}
