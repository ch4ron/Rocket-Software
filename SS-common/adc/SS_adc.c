/*
 * SS_adc.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "SS_adc.h"
#include "stm32f4xx_hal.h"

#define ADC1_NUMBER_OF_CHANNELS 6
#define ADC2_NUMBER_OF_CHANNELS 6
#define ADC3_NUMBER_OF_CHANNELS 6

#define VREFIN_CAL (uint16_t*) 0x1FFF7A2A

static uint16_t adc1_raw[ADC1_NUMBER_OF_CHANNELS];
static uint16_t adc2_raw[ADC2_NUMBER_OF_CHANNELS];
static uint16_t adc3_raw[ADC3_NUMBER_OF_CHANNELS];
static AdcMeasurement *adc1_pointers[ADC1_NUMBER_OF_CHANNELS];
static AdcMeasurement *adc2_pointers[ADC2_NUMBER_OF_CHANNELS];
static AdcMeasurement *adc3_pointers[ADC3_NUMBER_OF_CHANNELS];
static float vdd = 3.3f;

void SS_adc_init(ADC_HandleTypeDef *adc[], uint8_t count) {
    for(uint8_t i = 0; i < count; i++) {
        HAL_ADC_Start_DMA(adc[i], (uint32_t *) adc1_raw, ADC1_NUMBER_OF_CHANNELS);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    /* Vrefint channel configured with rank 1 */
    if(hadc == &hadc1) {
        vdd = 3.3f * (float) (*VREFIN_CAL) / (float) adc1_raw[0];
        for(uint8_t i = 0; i < ADC1_NUMBER_OF_CHANNELS; i++) {
            if(adc1_pointers[i] != NULL)
                adc1_pointers[i]->value = adc1_pointers[i]->fun(adc1_raw[i], vdd);
        }
    }
    if(hadc == &hadc2) {
        for(uint8_t i = 0; i < ADC2_NUMBER_OF_CHANNELS; i++) {
            if(adc2_pointers[i] != NULL)
                adc2_pointers[i]->value = adc2_pointers[i]->fun(adc2_raw[i], vdd);
        }
    }
    if(hadc == &hadc3) {
        for(uint8_t i = 0; i < ADC3_NUMBER_OF_CHANNELS; i++) {
            if(adc3_pointers[i] != NULL)
                adc3_pointers[i]->value = adc3_pointers[i]->fun(adc3_raw[i], vdd);
        }
    }
}

void SS_adc_add_measurement(AdcMeasurement *meas) {
    AdcMeasurement **meas_array;
    switch(meas->adc) {
        case 1:
            meas_array = adc1_pointers;
            break;
        case 2:
            meas_array = adc2_pointers;
            break;
        default:
            meas_array = adc3_pointers;
            break;
    }
    meas_array[meas->rankId - 1] = meas;
}

