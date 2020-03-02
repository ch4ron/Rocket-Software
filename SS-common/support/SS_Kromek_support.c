/*
 * SS_Kromek_support.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "usart.h"
#include "SS_support.h"

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart5, (uint8_t*) ptr, (uint16_t) len, 1000);
	return len;
}

void SS_support_set_mem_led(bool red, bool green, bool blue) {
//	HAL_GPIO_WritePin(MEM_RED_GPIO_Port, MEM_RED_Pin, !red);
//	HAL_GPIO_WritePin(MEM_GREEN_GPIO_Port, MEM_GREEN_Pin, !green);
//	HAL_GPIO_WritePin(MEM_BLUE_GPIO_Port, MEM_BLUE_Pin, !blue);
}

void SS_support_set_com_led(bool red, bool green, bool blue) {
//	HAL_GPIO_WritePin(COM_RED_GPIO_Port, COM_RED_Pin, !red);
//	HAL_GPIO_WritePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin, !green);
//	HAL_GPIO_WritePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin, !blue);
}

void SS_support_set_adc_led(bool red, bool green, bool blue) {
//	HAL_GPIO_WritePin(ADC_RED_GPIO_Port, ADC_RED_Pin, !red);
//	HAL_GPIO_WritePin(ADC_GREEN_GPIO_Port, ADC_GREEN_Pin, !green);
//	HAL_GPIO_WritePin(ADC_BLUE_GPIO_Port, ADC_BLUE_Pin, !blue);
}
