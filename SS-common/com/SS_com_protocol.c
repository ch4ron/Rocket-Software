/*
 * SS_com_protocol.c
 *
 *  Created on: Jan 16, 2020
 *      Author: maciek
 */

#include "SS_com_protocol.h"
#include "SS_servos.h"
#include "stdio.h"

ComStatus SS_com_handle_action(ComFrameContent *frame) {
    ComActionID action = frame->action;
    switch(action) {
        case COM_REQUEST:
            return SS_com_handle_request(frame);
        case COM_SERVICE:
            return SS_com_handle_service(frame);
        default:
            printf("Unsupported action: %d\r\n", frame->action);
    }
    return COM_ERROR;
}

ComStatus SS_com_handle_request(ComFrameContent *frame) {
    ComDeviceID device = frame->device;
    switch(device) {
        case COM_SERVO_ID:
            return SS_servos_handle_grazyna_request(frame);
        case COM_RELAY_ID:
            break;
        case COM_PRESSURE_ID:
            break;
        case COM_SUPPLY_ID:
            break;
        case COM_MEMORY_ID:
            break;
        case COM_IGNITER_ID:
            break;
        case COM_TENSOMETER_ID:
            break;
        default:
            printf("Unsupported device: %d\r\n", frame->action);
    }
    return COM_ERROR;
}

ComStatus SS_com_handle_service(ComFrameContent *frame) {
    ComDeviceID device = frame->device;
    switch(device) {
        case COM_SERVO_ID:
            return SS_servos_handle_grazyna_service(frame);
        case COM_RELAY_ID:
            break;
        case COM_PRESSURE_ID:
            break;
        case COM_SUPPLY_ID:
            break;
        case COM_MEMORY_ID:
            break;
        case COM_IGNITER_ID:
            break;
        case COM_TENSOMETER_ID:
            break;
        default:
            printf("Unsupported device: %d\r\n", frame->action);
    }
    return COM_ERROR;
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
