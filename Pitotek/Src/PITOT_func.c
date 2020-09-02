/*
 * PITOT_FUNC.c
 *
 *  Created on: 18.04.2019
 *      Author: Andrzej
 */
#include "PITOT_func.h"

#include "i2c.h"

//-------------- ADC--------
const float SupplyVoltage = 3.3;  // [Volts]
const float ADCResolution = 4095.0;
const float Bat_voltage_div_ratio = 2.02;

volatile uint16_t ADC_VBAT_avg_buf[ADC_V_MEAN_SAMPLES];
volatile uint16_t ADC_avg_buf_cnt = 0;
//-------------------------
uint8_t PITOT_sensor_data_arr[4];
//----------------------- ADC>
float ADC_get_VBAT_mean(void) {
    float mean_value = 0;
    for(uint8_t i = 0; i <= ADC_V_MEAN_SAMPLES - 1; i++) {
        mean_value = mean_value + (Bat_voltage_div_ratio * ((SupplyVoltage * ADC_VBAT_avg_buf[i]) / ADCResolution));
    }
    return mean_value / ADC_V_MEAN_SAMPLES;
}

void ADC_check_bat_voltage(void) {
    if(ADC_get_VBAT_mean() < 3.5)  // batterry discharged
    {
        BAT_LOW_LED_ON;
    } else {
        BAT_LOW_LED_OFF;
    }
}

void ADC_save_result_2_buff(void) {
    HAL_ADC_Start(&hadc1);
    ADC_VBAT_avg_buf[ADC_avg_buf_cnt] = HAL_ADC_GetValue(&hadc1);
    ADC_avg_buf_cnt++;

    if(ADC_avg_buf_cnt >= ADC_V_MEAN_SAMPLES) {
        ADC_avg_buf_cnt = 0;
    }
}

//----------------------- ADC<

void UART_send_debug_string(char *string_to_send) {
    uint16_t debug_tx_length;
    char debug_tx_char_buff[500];

    debug_tx_length = sprintf(debug_tx_char_buff, "%s", string_to_send);
    HAL_UART_Transmit(&huart4, (uint8_t *) debug_tx_char_buff, debug_tx_length, MAX_TIMEOUT);
}

//----------------------- PITOT_DIFFERENTIAL_PRESSURE_SENSOR-->

void PITOT_pull_I2C_data(void) {
    HAL_I2C_Master_Receive(&hi2c1, 0x28 << 1, &PITOT_sensor_data_arr, 4, 100);
}

float PITOT_get_pressure_diff_pa(void) {
    uint16_t pressure_raw;
    float pressure_psi = 0;
    float pressure_pa = 0;

    pressure_raw = (PITOT_sensor_data_arr[0] << 8) | (PITOT_sensor_data_arr[1]);

    pressure_psi = -15 + ((((float) pressure_raw - 1638.0) / 13107.0) * 30.0);
    pressure_pa = pressure_psi * 6894.75729;

    return pressure_pa;
}

float PITOT_get_pressure_diff_psi(void) {
    uint16_t pressure_raw;
    float pressure_psi = 0;

    pressure_raw = (PITOT_sensor_data_arr[0] << 8) | (PITOT_sensor_data_arr[1]);

    pressure_psi = -15 + ((((float) pressure_raw - 1638.0) / 13107.0) * 30.0);

    return pressure_psi;
}

float PITOT_get_temp(void) {
    uint16_t temperature_raw;
    float temperature_deg_c = 0;

    temperature_raw = (PITOT_sensor_data_arr[2] << 8) | (PITOT_sensor_data_arr[3]);

    temperature_deg_c = (((float) temperature_raw * 200.0) / (2048.0)) - 50.0;

    return temperature_deg_c / 100.0;
}