/*
 * SS_supply.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "SS_supply.h"
#include "SS_Kromek_adc.h"

Supply relay_supply = {
        .ENABLE_Port = ENABLE1_GPIO_Port,
        .ENABLE_Pin= ENABLE1_Pin
};

Supply servos1_supply = {
        .ENABLE_Port = ENABLE2_GPIO_Port,
        .ENABLE_Pin= ENABLE2_Pin
};

Supply servos2_supply = {
        .ENABLE_Port = ENABLE3_GPIO_Port,
        .ENABLE_Pin= ENABLE3_Pin
};

Supply kozackie_servo_supply = {
        .ENABLE_Port = ENABLE4_GPIO_Port,
        .ENABLE_Pin= ENABLE4_Pin
};

static float supply_12v_voltage_scaled(uint16_t raw, float vdd) {
    return vdd * (float) raw / 4095.0f * (4990.0f + 20000.0f) / 4990.0f;
}

static float supply_servo_voltage_scaled(uint16_t raw, float vdd) {
    return vdd * (float) raw / 4095.0f * (8450.0f + 20000.0f) / 8450.0f;
}

void SS_supply_init() {
//    SS_enable_supply(&servos1_supply);
//    SS_enable_supply(&servos2_supply);
//    SS_enable_supply(&kozackie_servo_supply);
//    SS_enable_supply(&relay_supply);
    SS_adc_add_measurement(&relay_supply.voltage, supply_12v_voltage_scaled, 4, 1);
    SS_adc_add_measurement(&kozackie_servo_supply.voltage, supply_12v_voltage_scaled, 2, 2);
    SS_adc_add_measurement(&servos1_supply.voltage, supply_servo_voltage_scaled, 5, 3);
    SS_adc_add_measurement(&servos2_supply.voltage, supply_servo_voltage_scaled, 3, 1);
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
