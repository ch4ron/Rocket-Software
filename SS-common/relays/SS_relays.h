/*
 * SS_relays.h
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#ifndef SS_RELAYS_H_
#define SS_RELAYS_H_

#include "gpio.h"
#include "SS_com_protocol.h"

#define MAX_RELAY_COUNT 9

typedef struct {
    uint8_t id;
	GPIO_TypeDef* GPIO_Port;
	uint16_t Pin;
	uint8_t state;
} Relay;

typedef enum {
    COM_RELAY_OPEN,
    COM_RELAY_CLOSE,
    COM_RELAY_STATUS,
} ComRelayID;

extern Relay relays[9];

void SS_relay_open(Relay *relay);
void SS_relay_close(Relay *relay);
void SS_relay_init(Relay *relay);
void SS_relays_init(Relay *relay_array, uint8_t count);
ComStatus SS_relay_com_service(ComFrameContent *frame);
ComStatus SS_relays_com_request(ComFrameContent *frame);

#endif /* SS_RELAYS_H_ */
