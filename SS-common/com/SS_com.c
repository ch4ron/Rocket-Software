/*
 * SS_com_protocol.c
 *
 *  Created on: Jan 16, 2020
 *      Author: maciek
 */

#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif
#ifdef SS_USE_ADS1258

#include "SS_fifo.h"
#include "SS_measurements.h"
#endif
#ifdef SS_USE_RELAYS
#include "SS_relays.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos.h"
#endif
#ifdef SS_USE_CAN
#include "SS_can.h"
#endif

#include "SS_com_debug.h"
#include "SS_error.h"
#include "SS_com.h"
#include "stdio.h"
#include "string.h"

static ComBoardID board_id;

ComFifoManager fifo_manager[10];

/* Functions in this module modify the received frame */

void SS_com_init(ComBoardID board) {
    board_id = board;
}

void SS_com_transmit(ComFrame *frame) {
#ifdef SS_USE_GRAZYNA
    if(frame->destination == COM_GRAZYNA_ID && SS_grazyna_is_enabled()) {
        SS_grazyna_transmit(frame);
    } else {
#ifdef SS_USE_CAN
        SS_can_transmit(frame);
#endif
    }
#else
    SS_can_transmit(frame);
#endif
}

ComStatus SS_com_handle_frame(ComFrame *frame) {
#ifdef SS_USE_GRAZYNA
    if(SS_grazyna_is_enabled() && frame->destination != board_id) {
        SS_com_transmit(frame);
        return COM_OK;
    }
#else
    if(frame->destination != board_id) {
        SS_error("Received message with invalid destination: %d", frame->destination);
    }
#endif
    bool response_required = frame->action == COM_REQUEST || frame->source == COM_GRAZYNA_ID ? true : false;
    ComStatus res = SS_com_handle_action(frame);
    if(response_required) {
        frame->destination = frame->source;
        frame->source = board_id;
        SS_com_transmit(frame);
    }
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
    if(res == 0) {
        frame->action = COM_ACK;
    } else {
        frame->action = COM_NACK;
    }
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

void SS_com_add_fifo(volatile Fifo *fifo, void (*fun)(ComFrame*), ComGroup group_id, ComPriority priority) {
    for(uint8_t i = 0; i < sizeof(fifo_manager) / sizeof(fifo_manager[0]); i++) {
        if(fifo_manager[i].fifo == NULL) {
            fifo_manager[i] = (ComFifoManager) { fifo, fun, group_id, priority};
            return;
        }
    }
    SS_error("Com Fifo manager is full");
}

void SS_com_handle_fifo_manager() {
    /* TODO Handle priorities */
    static ComFrame frame;
    for(uint8_t i = 0; i < sizeof(fifo_manager) / sizeof(fifo_manager[0]); i++) {
        if(fifo_manager[i].fifo == NULL) {
            return;
        }
        else if(SS_fifo_get_data(fifo_manager[i].fifo, &frame)) {
            if(fifo_manager[i].group_id == COM_GROUP_RECEIVE) {
                SS_can_print_message_received(&frame);
                SS_com_handle_frame(&frame);
            } else if(fifo_manager[i].fun != NULL) {
                fifo_manager[i].fun(&frame);
            } else {
                SS_error("Com Fifo Manager: Uninitialized function for non-receive fifo");
            }
        }
    }
}

void SS_com_main() {
    SS_com_handle_fifo_manager();
}