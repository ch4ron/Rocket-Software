/*
 * SS_supply.h
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#ifndef SS_SUPPLY_H_
#define SS_SUPPLY_H_

#include "gpio.h"
#include "SS_adc.h"

typedef struct {
    GPIO_TypeDef *ENABLE_Port;
    uint16_t ENABLE_Pin;
    uint32_t timeout;
    AdcMeasurement measurement;
} Supply;

typedef enum {
    SUPPLY_OFF = 0,
    SUPPLY_ON = 1
} Supply_state_t;

void SS_supply_init(Supply *supply);
void SS_enable_supply(Supply *supply);
void SS_disable_supply(Supply *supply);
Supply_state_t SS_supply_get_state(Supply *supply);
void SS_supply_SYSTICK(void);
void SS_supply_set_timeout(Supply *supply, uint16_t timeout);

#endif /* SS_SUPPLY_CONTROL_H_ */
