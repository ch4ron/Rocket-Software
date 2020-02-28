/*
 * SS_supply.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "SS_supply.h"
#include "SS_adc.h"
#include "SS_platform_init.h"


void SS_supply_init(Supply *supply) {
    SS_adc_add_measurement(&supply->measurement);
}

void SS_enable_supply(Supply *supply) {
    HAL_GPIO_WritePin(supply->ENABLE_Port, supply->ENABLE_Pin, SET);
}

void SS_disable_supply(Supply *supply) {
    HAL_GPIO_WritePin(supply->ENABLE_Port, supply->ENABLE_Pin, RESET);
}

Supply_state_t SS_supply_get_state(Supply *supply) {
   return HAL_GPIO_ReadPin(supply->ENABLE_Port, supply->ENABLE_Pin);
}

void SS_supply_set_timeout(Supply *supply, uint16_t timeout) {
    supply->timeout = timeout;
}

void SS_supply_single_SYSTICK(Supply *supply) {
    if(supply->timeout == 1)
        SS_disable_supply(supply);
    if(supply->timeout > 0)
        supply->timeout--;
}

void SS_supply_SYSTICK() {
    SS_supply_single_SYSTICK(&relay_supply);
    SS_supply_single_SYSTICK(&servos1_supply);
    SS_supply_single_SYSTICK(&servos2_supply);
    SS_supply_single_SYSTICK(&kozackie_servo_supply);
}
