/*
 * SS_adc.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */


/*
 * SS_VNQ5027.c
 *
 *  Created on: Mar 25, 2019
 *      Author: charon
 */
#include "adc.h"
#include "stm32f4xx_hal.h"
#include "SS_adc.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfor-loop-analysis"
#define ADC1_NUMBER_OF_CHANNELS 6
#define ADC2_NUMBER_OF_CHANNELS 6
#define ADC3_NUMBER_OF_CHANNELS 6

#define VREFIN_CAL (uint16_t*) 0x1FFF7A2A

typedef struct {
        float *value;
        float (*fun)(uint16_t, float);
} kromek_adc_measurement;

static uint16_t adc1_raw[ADC1_NUMBER_OF_CHANNELS];
static uint16_t adc2_raw[ADC2_NUMBER_OF_CHANNELS];
static uint16_t adc3_raw[ADC3_NUMBER_OF_CHANNELS];
static kromek_adc_measurement adc1_scaled[ADC1_NUMBER_OF_CHANNELS];
static kromek_adc_measurement adc2_scaled[ADC2_NUMBER_OF_CHANNELS];
static kromek_adc_measurement adc3_scaled[ADC3_NUMBER_OF_CHANNELS];
static float vdd = 3.3f;

void SS_adc_init() {
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc1_raw, ADC1_NUMBER_OF_CHANNELS);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*) adc2_raw, ADC2_NUMBER_OF_CHANNELS);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t*) adc3_raw, ADC3_NUMBER_OF_CHANNELS);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    /* Vrefint channel configured with rank 1 */
    if(hadc == &hadc1) {
        vdd = 3.3f * (float) (*VREFIN_CAL) / (float) adc1_raw[0];
        for(uint8_t i = 0; i < ADC1_NUMBER_OF_CHANNELS; i++) {
            if(adc1_scaled[i].fun != NULL)
                *adc1_scaled[i].value = adc1_scaled[i].fun(adc1_raw[i], vdd);
        }
    }
    if(hadc == &hadc2) {
        for(uint8_t i = 0; i < ADC2_NUMBER_OF_CHANNELS; i++) {
            if(adc2_scaled[i].fun != NULL)
                *adc2_scaled[i].value = adc2_scaled[i].fun(adc2_raw[i], vdd);
        }
    }
    if(hadc == &hadc3) {
        for(uint8_t i = 0; i < ADC3_NUMBER_OF_CHANNELS; i++) {
            if(adc3_scaled[i].fun != NULL)
                *adc3_scaled[i].value = adc3_scaled[i].fun(adc3_raw[i], vdd);
        }
    }
}

void SS_adc_add_measurement(float *value, float (*fun)(uint16_t, float), int rankId, int adc) {
    kromek_adc_measurement *meas;
    switch(adc) {
        case 1:
            meas = adc1_scaled;
            break;
        case 2:
            meas = adc2_scaled;
            break;
        default:
            meas = adc3_scaled;
            break;
    }
    meas[rankId - 1].fun = fun;
    meas[rankId - 1].value = value;
}

#pragma clang diagnostic pop