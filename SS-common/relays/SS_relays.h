/*
 * SS_relays.h
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#ifndef SS_RELAYS_H_
#define SS_RELAYS_H_

#include "gpio.h"

typedef struct {
	GPIO_TypeDef* GPIO_Port;
	uint16_t Pin;
} Relay;

extern Relay relays[9];

void SS_open_relay(Relay *relay);
void SS_close_relay(Relay *relay);

#endif /* SS_RELAYS_H_ */
