/*
 * SS_relays.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#include <com/SS_com_protocol.h>
#include "SS_relays.h"
#include "SS_supply.h"
#include "SS_platform.h"
#include "SS_error.h"

Relay *relay_pointers[MAX_RELAY_COUNT];

static int8_t SS_relays_check_id(uint8_t id) {
	if(id >= MAX_RELAY_COUNT) {
		SS_error("Relay id: %d too high, max supported id: %d", id, MAX_RELAY_COUNT);
		return -1;
	}
	if(relay_pointers[id] == NULL) {
		SS_error("Relay id: %d not initialized");
		return -1;
	}
	return 0;
}

void SS_relay_init(Relay *relay) {
	if(relay_pointers[relay->id] != relay && relay_pointers[relay->id] != NULL) {
		SS_error("Duplicate servo id, %d", relay->id);
		return;
	}
	if(relay->id >= MAX_RELAY_COUNT) {
		SS_error("Servo id: %d too high, max supported id: %d", relay->id, MAX_RELAY_COUNT);
		return;
	}
	relay_pointers[relay->id] = relay;
}

void SS_relays_init(Relay *relay_array, uint8_t count) {
	for(uint8_t i = 0; i < count; i++) {
		SS_relay_init(&relay_array[i]);
	}
}

void SS_relay_open(Relay *relay) {
	SS_enable_supply(&relay_supply);
	HAL_GPIO_WritePin(relay->GPIO_Port, relay->Pin, GPIO_PIN_SET);
	relay->state = 1;
}

void SS_relay_close(Relay *relay) {
	HAL_GPIO_WritePin(relay->GPIO_Port, relay->Pin, GPIO_PIN_RESET);
    relay->state = 0;
}

ComStatus SS_relay_com_service(ComFrameContent *frame) {
    if(SS_relays_check_id(frame->id) != 0) return COM_ERROR;
    ComRelayID msgID = frame->message_type;
    Relay *relay = relay_pointers[frame->id];
    switch(msgID) {
        case COM_RELAY_OPEN :
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

ComStatus SS_relays_com_request(ComFrameContent *frame) {
    if(SS_relays_check_id(frame->id) != 0) return COM_ERROR;
    ComRelayID msgID = frame->message_type;
    Relay *relay = relay_pointers[frame->id];
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
