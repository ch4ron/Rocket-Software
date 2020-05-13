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
#include "stdbool.h"

typedef float (*SS_adc_scale_fun)(uint16_t, float);

struct AdcMeasurement_tag;

typedef struct Adc_tag {
    /* Allocated dynamically */
    ADC_HandleTypeDef hadc;
    struct AdcMeasurement_tag **measurements;
    uint16_t *raw;

    uint8_t channel_count;
    bool save_to_flash;
    uint8_t active_channels;
} Adc;

typedef struct AdcMeasurement_tag {
    float value;
    SS_adc_scale_fun scale_fun;
    uint32_t channel;
    Adc *adc;
} AdcMeasurement;

void SS_adc_init(Adc *adc, ADC_TypeDef *adc_type, uint8_t channel_count);
void SS_adc_start();
void SS_adc_enable_vref(Adc *adc);
void SS_adc_add_measurement(AdcMeasurement *meas);

#endif /* SS_KROMEK_ADC_H_ */
