/*
 * SS_adc.h
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#ifndef SS_ADC_H_
#define SS_ADC_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

/* Note that channelId is equal to channel's rank, counting from 1 (dependent on CubeMX configuration) not ADC Channel */
typedef struct {
    float value;
    float (*fun)(uint16_t, float);
    uint8_t rankId;
    ADC_HandleTypeDef* adc;
} AdcMeasurement;

void SS_adc_init(ADC_HandleTypeDef *adc[], uint8_t count);
void SS_adc_add_measurement(AdcMeasurement *meas);

#endif /* SS_KROMEK_ADC_H_ */
