/*
 * SS_igniter.h
 *
 *  Created on: Aug 02, 2020
 *      Author: Maciek
 */

#ifndef SS_IGNITER_H
#define SS_IGNITER_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "gpio.h"
#include "stdbool.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef struct {
    GPIO_TypeDef *GPIO_Port;
    uint16_t Pin;
} Igniter;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_igniter_init(GPIO_TypeDef *GPIO_Port, uint16_t Pin);
int8_t SS_igniter_ignite(uint16_t time);
void SS_igniter_get(void);
void SS_igniter_off(void);
bool SS_igniter_status(void);

#endif /* SS_IGNITER_H */
