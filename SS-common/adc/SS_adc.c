/*
 * SS_adc.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "SS_adc.h"
#include <cstdint>
#include "string.h"
#include "assert"
#include "portable.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "stdbool.h"
#include "stm32f4xx_hal_adc.h"

#define ADC1_NUMBER_OF_CHANNELS 6
#define ADC2_NUMBER_OF_CHANNELS 6
#define ADC3_NUMBER_OF_CHANNELS 6

#define VREFIN_CAL (uint16_t*) 0x1FFF7A2A

static float vdd = 3.3f;
static uint8_t adc_count;

typedef struct {
    /* Allocated dynamically */
    ADC_HandleTypeDef *hadc;
    AdcMeasurement *measurements;
    uint16_t *raw;

    uint8_t chanel_count;
    bool save_to_flash;
    uint8_t active_channels;
} Adc;

/* STM32F446xx have 3 ADC */
static Adc *adc_list[3];

void SS_adc_init(Adc *adc, ADC_TypeDef* adc_type, uint8_t channel_count) {
    /* TODO Add option for different sampling options if required */
    adc->hadc = pvPortMalloc(sizeof(ADC_HandleTypeDef));
    assert(adc->hadc != NULL);
    adc->measurements = pvPortMalloc(channel_count*sizeof(AdcMeasurement));
    assert(adc->measurements != NULL);
    adc->raw = pvPortMalloc(channel_count*sizeof(uint16_t));
    assert(adc->raw != NULL);

    adc->hadc->Instance = adc_type;
    adc->hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    adc->hadc->Init.Resolution = ADC_RESOLUTION_12B;
    adc->hadc->Init.ScanConvMode = ENABLE;
    adc->hadc->Init.ContinuousConvMode = ENABLE;
    adc->hadc->Init.DiscontinuousConvMode = DISABLE;
    adc->hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc->hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc->hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc->hadc->Init.NbrOfConversion = channel_count;
    adc->hadc->Init.DMAContinuousRequests = ENABLE;
    adc->hadc->Init.EOCSelection = ADC_EOC_SEQ_CONV;

    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        if(adc_list[i] == NULL) {
            adc_list[i] = adc;
            return;
        }
    }
    /* Should never happen */
    assert(false);
}

void SS_adc_start(Adc *adc) {
    assert(adc->active_channels == adc->chanel_count);
    HAL_ADC_Start_DMA(adc->hadc, (uint32_t *) adc->raw, adc->chanel_count);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    /* Vrefint channel configured with rank 1 */
    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        if(adc_list[i] != NULL && adc_list[i]->hadc == hadc) {
            /* TODO save to flash */
            adc_list[i]-> = adc1_pointers[i]->fun(adc1_raw[i], vdd);
        }
    }
/*     for(uint8_t i = 0; i < adc_count; i++) { */
/*         if(hadc == adc_list[i]) { */
/*             vdd = 3.3f * (float) (*VREFIN_CAL) / (float) adc1_raw[0]; */
/*             for(uint8_t i = 0; i < ADC1_NUMBER_OF_CHANNELS; i++) { */
/*                 if(adc1_pointers[i] != NULL) */
/*                     adc1_pointers[i]->value = adc1_pointers[i]->fun(adc1_raw[i], vdd); */
/*             } */
/*         } */
/*     } */
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
