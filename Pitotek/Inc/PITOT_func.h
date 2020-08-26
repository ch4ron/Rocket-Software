/*
 * PITOT_FUNC_H_.h
 *
 *  Created on: 18.04.2019
 *      Author: Andrzej
 */
#ifndef PITOT_FUNC_H_
#define PITOT_FUNC_H_

#include <stdio.h>
#include <stdint.h>
#include "usart.h"
#include "adc.h"

//------------------------------------------ADC>
#define ADC_V_MEAN_SAMPLES 10

void ADC_init_measurement(void);
float ADC_get_VBAT_mean(void);
void ADC_check_bat_voltage(void);
//------------------------------------------ADC<

#define BAT_LOW_LED_ON HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin,GPIO_PIN_SET)
#define BAT_LOW_LED_OFF HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin,GPIO_PIN_RESET)

volatile uint8_t Log_trig_flag;

#define MAX_TIMEOUT 200

void UART_send_debug_string (char * string);

void PITOT_pull_I2C_data(void);
float PITOT_get_pressure_diff_pa(void);
float PITOT_get_pressure_diff_psi(void);
float PITOT_get_temp(void);

void ADC_save_result_2_buff(void);

#endif /* GEIGER_FUNC_H_ */
