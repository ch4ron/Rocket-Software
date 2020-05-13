/**
  * SS_adc.c
  *
  *  Created on: Dec 24, 2019
  *      Author: maciek
  *
  *  Usage:
  *
  *  In STM32CubeMX
  *    1. Enable ADC module and turn on the channels you're planning to use
  *    2. Add DMA stream for each configured ADC, set its mode to 'circular'
  *    3. In Project Manager/Advanced Settings enable checkbox
  *    'Not generate function call' for MX_ADC_Init()
  *
  *  In board configuration (SS_platform.c):
  *    1. Call SS_adc_init() for each ADC
  *    2. Add the measurements want to use with SS_add_measurement()
  *
  *  The scaled values are automatically updated in measurement structure
  * 
 **/

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

#include "SS_adc.h"

#include "FreeRTOS.h"
#include "assert.h"
#include "portable.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "string.h"

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define VREFIN_CAL (uint16_t *) 0x1FFF7A2A

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void SS_adc_start_single(Adc *adc);
static void SS_adc_register(Adc *adc);
static void SS_adc_register(Adc *adc);
static bool SS_adc_check_initialized(Adc *adc);
static void SS_adc_measure_vref();
static float SS_adc_calculate_scaled(uint16_t raw);

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static Adc *adc_list[3];
static float adc_vref;

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_adc_init(Adc *adc, ADC_TypeDef *adc_type, uint8_t channel_count) {
    if(adc_vref == 0.0f) {
        SS_adc_measure_vref();
    }

    adc->measurements = pvPortMalloc(channel_count * sizeof(AdcMeasurement *));
    assert(adc->measurements != NULL);
    adc->raw = pvPortMalloc(channel_count * sizeof(uint16_t));
    assert(adc->raw != NULL);


    adc->max_channel_count = channel_count;

    /* TODO Add option for different sampling options if required */
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

    SS_adc_register(adc);
}

void SS_adc_add_measurement(AdcMeasurement *meas) {
    ADC_ChannelConfTypeDef sConfig = {0};
    Adc *adc = meas->adc;
    assert(SS_adc_check_initialized(adc));
    assert(adc->active_channels < adc->max_channel_count);
    adc->measurements[adc->active_channels] = meas;
    sConfig.Channel = meas->channel;
    /* Ranks are counted from 1 */
    sConfig.Rank = ++adc->active_channels;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    assert(HAL_ADC_ConfigChannel(&adc->hadc, &sConfig) == HAL_OK);
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

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static void SS_adc_start_single(Adc *adc) {
    assert(adc->active_channels <= adc->max_channel_count);
    HAL_ADC_Start_DMA(&adc->hadc, (uint32_t *) adc->raw, adc->active_channels);
}

static void SS_adc_register(Adc *adc) {
    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        if(adc_list[i] == NULL) {
            adc_list[i] = adc;
            return;
        }
    }
    /* Should never happen */
    assert(false);
}

static bool SS_adc_check_initialized(Adc *adc) {
    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        if(adc_list[i] == adc) {
            return true;
        }
    }
    return false;
}

static void SS_adc_measure_vref() {
    ADC_ChannelConfTypeDef sConfig;
    static ADC_HandleTypeDef adc;

    adc.Instance = ADC1;
    adc.Init.ContinuousConvMode = ENABLE;
    adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc.Init.ScanConvMode = DISABLE;
    adc.Init.NbrOfConversion = 1;
    adc.Init.DiscontinuousConvMode = DISABLE;
    adc.Init.NbrOfDiscConversion = 1;

    HAL_ADC_Init(&adc);

    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    assert(HAL_ADC_ConfigChannel(&adc, &sConfig) == HAL_OK);

    HAL_ADC_Start(&adc);

    if(HAL_ADC_PollForConversion(&adc, 10) == HAL_OK) {
        uint16_t raw = HAL_ADC_GetValue(&adc);
        adc_vref = 3.3f * (float) (*VREFIN_CAL) / raw;
    }

    HAL_ADC_Stop(&adc);
    HAL_ADC_DeInit(&adc);
}

static float SS_adc_calculate_scaled(uint16_t raw) {
    return adc_vref * raw / 4095.0f;
}

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    for(uint8_t i = 0; i < sizeof(adc_list) / sizeof(adc_list[0]); i++) {
        Adc *adc = adc_list[i];
        if(adc != NULL && &adc->hadc == hadc) {
            for(uint8_t j = 0; j < adc->active_channels; j++) {
                AdcMeasurement *meas = adc->measurements[j];
                meas->value = meas->scale_fun(SS_adc_calculate_scaled(adc->raw[j]));
            }
        }
    }
}
