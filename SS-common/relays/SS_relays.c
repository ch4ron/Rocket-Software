/*
 * SS_relays.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#include "SS_relays.h"
#include "SS_supply_control.h"

Relay relays[9] = {
    { .GPIO_Port = RELAY1_GPIO_Port, .Pin = RELAY1_Pin },
    { .GPIO_Port = RELAY2_GPIO_Port, .Pin = RELAY2_Pin },
    { .GPIO_Port = RELAY3_GPIO_Port, .Pin = RELAY3_Pin },
    { .GPIO_Port = RELAY4_GPIO_Port, .Pin = RELAY4_Pin },
    { .GPIO_Port = RELAY5_GPIO_Port, .Pin = RELAY5_Pin },
    { .GPIO_Port = RELAY6_GPIO_Port, .Pin = RELAY6_Pin },
    { .GPIO_Port = RELAY7_GPIO_Port, .Pin = RELAY7_Pin },
    { .GPIO_Port = RELAY8_GPIO_Port, .Pin = RELAY8_Pin },
    { .GPIO_Port = RELAY9_GPIO_Port, .Pin = RELAY9_Pin },
};

void SS_open_relay(Relay *relay) {
    SS_enable_supply(&relay_supply);
	HAL_GPIO_WritePin(relay->GPIO_Port, relay->Pin, SET);
}

void SS_close_relay(Relay *relay) {
	HAL_GPIO_WritePin(relay->GPIO_Port, relay->Pin, RESET);
}

