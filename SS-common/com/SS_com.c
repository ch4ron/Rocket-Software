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
#include "SS_com_feed.h"
#include "SS_error.h"
#include "SS_com.h"
#include "stdio.h"


static ComFrameContent frame_content;
static ComBoardID board_id;
static ComFrame tx_frame;

/* The the functions in this module modify the received frame */

void SS_com_init(ComBoardID board) {
    board_id = board;
#ifndef SIMULATE
    SS_com_feed_enable();
#endif
}

void SS_com_transmit(ComFrameContent *frame_content) {
    frame_content->source = board_id;
    SS_com_create_frame(&tx_frame, frame_content);
    SS_grazyna_transmit(&tx_frame);
}

ComStatus SS_com_handle_frame(ComFrame *frame) {
    SS_com_parse_frame(frame, &frame_content);
    SS_com_print_message_received(&frame_content);
    ComStatus res = SS_com_handle_action(&frame_content);
    frame_content.destination = frame_content.source;
    SS_com_transmit(&frame_content);
    return res;
}

ComStatus SS_com_handle_action(ComFrameContent *frame) {
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

ComStatus SS_com_handle_request(ComFrameContent *frame) {
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
            printf("Unsupported device: %d\r\n", frame->action);
    }
    frame->action = COM_RESPONSE;
    return res;
}

ComStatus SS_com_handle_service(ComFrameContent *frame) {
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

void SS_com_add_payload_to_frame(ComFrameContent *frame, ComDataType type, void *payload) {
    frame->data_type = type;
    switch(type) {
        case UINT32:
            *((uint32_t*) &frame->payload) = *((uint32_t*) payload);
            break;
        case UINT16:
            *((uint16_t*) &frame->payload) = *((uint16_t*) payload);
            break;
        case UINT8:
            *((uint8_t*) &frame->payload) = *((uint8_t*) payload);
            break;
        case INT32:
            *((int32_t*) &frame->payload) = *((int32_t*) payload);
            break;
        case INT16:
            *((int16_t*) &frame->payload) = *((int16_t*) payload);
            break;
        case INT8:
            *((int8_t*) &frame->payload) = *((int8_t*) payload);
            break;
        case FLOAT:
            *((float*) &frame->payload) = *((float*) payload);
            break;
        default:
            break;
    }
}

void SS_com_parse_frame(ComFrame *frame, ComFrameContent *content) {
    content->priority = (frame->header >> 5) & 0b00000111;
    content->destination = frame->header & 0b00011111;
    content->source = (frame->header >> 11) & 0b00011111;
    content->action = (frame->header >> 8) & 0b00000111;
    content->device = ((frame->header >> 18) & 0b00111111) ;
    content->id = (((frame->header >> 16) & 0b00000011) << 4) | ((frame->header >> 28) & 0b00001111);
    content->grazyna_ind = (frame->header >> 27) & 0b00000001;
    content->data_type = (frame->header >> 24) & 0b00000111;
    content->message_type = frame->message_type;
    content->payload = frame->payload;
}

void SS_com_create_frame(ComFrame *frame, ComFrameContent *content) {
    frame->header = 0;
    frame->header |= content->priority << 5;
    frame->header |= content->destination;
    frame->header |= content->source << 11;
    frame->header |= content->action << 8;
    frame->header |= content->device << 18;
    frame->header |= (((content->id & 0b00110000) >> 4) << 16) | ((content->id & 0b00001111) << 28);
    frame->header |= content->grazyna_ind << 27;
    frame->header |= content->data_type << 24;
    frame->message_type = content->message_type;
    frame->payload = content->payload;
}
