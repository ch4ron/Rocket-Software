/*
 * SS_adc.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "SS_adc.h"

#include "assert.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"
#include "string.h"


#define VREFIN_CAL (uint16_t *) 0x1FFF7A2A


/* STM32F446xx have 3 ADC */
static Adc *adc_list[3];

/* Note that adc needs to be initialized before calling SS_add_measurement
 * Do not call MX_ADC_Init functions when using this module */
void SS_adc_init(Adc *adc, ADC_TypeDef *adc_type, uint8_t channel_count) {
    /* TODO Add option for different sampling options if required */
    adc->measurements = pvPortMalloc(channel_count * sizeof(AdcMeasurement *));
    assert(adc->measurements != NULL);
    adc->raw = pvPortMalloc(channel_count * sizeof(uint16_t));
    assert(adc->raw != NULL);

    adc->channel_count = channel_count;

    adc->hadc.Instance = adc_type;
    adc->hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    adc->hadc.Init.Resolution = ADC_RESOLUTION_12B;
    adc->hadc.Init.ScanConvMode = ENABLE;
    adc->hadc.Init.ContinuousConvMode = ENABLE;
    adc->hadc.Init.DiscontinuousConvMode = DISABLE;
    adc->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc->hadc.Init.NbrOfConversion = channel_count;
    adc->hadc.Init.DMAContinuousRequests = ENABLE;
    adc->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;

    assert(HAL_ADC_Init(&adc->hadc) == HAL_OK);

    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        if(adc_list[i] == NULL) {
            adc_list[i] = adc;
            return;
        }
    }
    /* Should never happen */
    assert(false);
}

static void SS_adc_start_single(Adc *adc) {
    assert(adc->active_channels == adc->channel_count);
    HAL_ADC_Start_DMA(&adc->hadc, (uint32_t *) adc->raw, adc->channel_count);
}

/* Called in SS_init() */
void SS_adc_start() {
    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        Adc *adc = adc_list[i];
        if(adc != NULL) {
            SS_adc_start_single(adc);
        }
    }
}

static float SS_adc_vref_scale_fun(uint16_t raw, float vdd) {
    return 3.3f * (float) (*VREFIN_CAL) / raw;
}

static AdcMeasurement adc_vref = {
    .value = 3.3f,
    .scale_fun = SS_adc_vref_scale_fun,
    .channel = ADC_CHANNEL_VREFINT
};

/* Required to increase measurement accuracy */
void SS_adc_enable_vref(Adc *adc) {
    adc_vref.adc = adc;
    SS_adc_add_measurement(&adc_vref);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        Adc *adc = adc_list[i];
        if(adc != NULL && &adc->hadc == hadc) {
            for(uint8_t j = 0; j < adc->channel_count; j++) {
                adc->measurements[j]->value = adc->measurements[j]->scale_fun(adc->raw[j], adc_vref.value);
            }
        }
    }
}

void SS_adc_add_measurement(AdcMeasurement *meas) {
    ADC_ChannelConfTypeDef sConfig = {0};
    Adc *adc = meas->adc;
    assert(adc->active_channels != adc->channel_count);
    adc->measurements[adc->active_channels] = meas;
    sConfig.Channel = meas->channel;
    /* Ranks are counted from 1 */
    sConfig.Rank = ++adc->active_channels;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    assert(HAL_ADC_ConfigChannel(&adc->hadc, &sConfig) == HAL_OK);
}
