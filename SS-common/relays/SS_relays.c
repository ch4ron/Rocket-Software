/*
 * SS_relays.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_relays.h"

#include "SS_log.h"
#include "SS_platform.h"
#ifdef SS_USE_SUPPLY
#include "SS_supply.h"
#endif

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static int8_t SS_relays_check_id(uint8_t id);
static void SS_relay_init(Relay *relay);

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

Relay *relay_pointers[MAX_RELAY_COUNT];

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_relays_init(Relay *relay_array, uint8_t count) {
    for(uint8_t i = 0; i < count; i++) {
        SS_relay_init(&relay_array[i]);
    }
}

void SS_relay_open(Relay *relay) {
#ifdef SS_USE_SUPLY
    SS_enable_supply(&relay_supply);
#endif
    HAL_GPIO_WritePin(relay->GPIO_Port, relay->Pin, GPIO_PIN_SET);
    relay->state = 1;
}

void SS_relay_close(Relay *relay) {
    HAL_GPIO_WritePin(relay->GPIO_Port, relay->Pin, GPIO_PIN_RESET);
    relay->state = 0;
}

Relay *SS_relay_get(uint8_t id) {
    if(SS_relays_check_id(id) != 0) return NULL;
    return relay_pointers[id];
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

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

static void SS_relay_init(Relay *relay) {
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

