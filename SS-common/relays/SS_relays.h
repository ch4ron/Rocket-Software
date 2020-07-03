/*
 * SS_relays.h
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

#ifndef SS_RELAYS_H
#define SS_RELAYS_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com.h"
#include "gpio.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

#define MAX_RELAY_COUNT 9

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef struct {
    uint8_t id;
    GPIO_TypeDef *GPIO_Port;
    uint16_t Pin;
    uint8_t state;
} Relay;

typedef enum {
    COM_RELAY_OPEN = 0x01,
    COM_RELAY_CLOSE = 0x02,
    COM_RELAY_STATUS = 0x03,
} ComRelayID;

/* ==================================================================== */
/* ========================= Extern variables ========================= */
/* ==================================================================== */

extern Relay relays[9];

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_relay_open(Relay *relay);
void SS_relay_close(Relay *relay);
void SS_relay_init(Relay *relay);
void SS_relays_init(Relay *relay_array, uint8_t count);
ComStatus SS_relay_com_service(ComFrame *frame);
ComStatus SS_relays_com_request(ComFrame *frame);

#endif /* SS_RELAYS_H_ */
