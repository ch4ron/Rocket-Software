/*
 * SS_adc.h
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#ifndef SS_ADC_H_
#define SS_ADC_H_

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

/* Takes measured voltage as parameter, should return a scaled value */
typedef float (*SS_adc_scale_fun)(float);

struct AdcMeasurement_tag;

typedef struct Adc_tag {
    ADC_HandleTypeDef hadc;

    /* Allocated dynamically */
    struct AdcMeasurement_tag **measurements;
    uint16_t *raw;

    uint8_t max_channel_count;
    bool save_to_flash;
    uint8_t active_channels;
} Adc;

typedef struct AdcMeasurement_tag {
    float value;
    SS_adc_scale_fun scale_fun;
    uint32_t channel;
    Adc *adc;
} AdcMeasurement;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_adc_init(Adc *adc, ADC_TypeDef *adc_type, uint8_t channel_count);
void SS_adc_add_measurement(AdcMeasurement *meas);
void SS_adc_start();
void SS_adc_enable_vref(Adc *adc);

#endif /* SS_ADC_H_ */
