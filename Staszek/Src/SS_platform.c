//
// Created by maciek on 02.03.2020.
//

#include "SS_platform.h"
#include "main.h"

#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif
#ifdef SS_USE_CAN
#include "SS_can.h"
#include "can.h"
#endif
#include "SS_common.h"
#ifdef SS_USE_SERVOS
#include "SS_servos.h"
#endif
#include "tim.h"
#include "SS_console.h"
#include "usart.h"
#include "SS_log.h"
#include "stdbool.h"

/*********** LED **********/

void SS_platform_set_mem_led(bool red, bool green, bool blue) {
    HAL_GPIO_WritePin(MEM_RED_GPIO_Port, MEM_RED_Pin, !red);
    HAL_GPIO_WritePin(MEM_GREEN_GPIO_Port, MEM_GREEN_Pin, !green);
    HAL_GPIO_WritePin(MEM_BLUE_GPIO_Port, MEM_BLUE_Pin, !blue);
}

void SS_platform_set_com_led(bool red, bool green, bool blue) {
    HAL_GPIO_WritePin(COM_RED_GPIO_Port, COM_RED_Pin, !red);
    HAL_GPIO_WritePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin, !green);
    HAL_GPIO_WritePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin, !blue);
}

/* void SS_platform_set_adc_led(bool red, bool green, bool blue) { */
    /* HAL_GPIO_WritePin(ADC_RED_GPIO_Port, ADC_RED_Pin, !red); */
    /* HAL_GPIO_WritePin(ADC_GREEN_GPIO_Port, ADC_GREEN_Pin, !green); */
    /* HAL_GPIO_WritePin(ADC_BLUE_GPIO_Port, ADC_BLUE_Pin, !blue); */
/* } */

void SS_platform_toggle_loop_led() {
    HAL_GPIO_TogglePin(LOOP_LED_GPIO_Port, LOOP_LED_Pin);
}

/********** SERVOS *********/

/* TODO Mock servos, init with actual values */

#ifdef SS_USE_SERVOS
Servo servos[] = {
        {.id = 0, .tim = &htim1, .channel = TIM_CHANNEL_2 },
        {.id = 1, .tim = &htim1, .channel = TIM_CHANNEL_3 },
        {.id = 2, .tim = &htim1, .channel = TIM_CHANNEL_2 },
        {.id = 3, .tim = &htim1, .channel = TIM_CHANNEL_1 },
        {.id = 4, .tim = &htim1, .channel = TIM_CHANNEL_4 },
        {.id = 5, .tim = &htim1, .channel = TIM_CHANNEL_3 },
        {.id = 6, .tim = &htim8, .channel = TIM_CHANNEL_2 },
        {.id = 7, .tim = &htim1, .channel = TIM_CHANNEL_1 }
};

void SS_platform_servos_init() {
    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
}
#endif

/********** ADC *********/

#if defined(SS_USE_ADC)
static void SS_platform_adc_init() {
    ADC_HandleTypeDef *adc[] = {
        &hadc1, &hadc2, &hadc3};
    SS_adc_init(adc, sizeof(adc) / sizeof(adc[0]));
}
#endif

/********** ADS1258 *********/

#ifdef SS_USE_ADS1258
Measurement measurements[] = {
        { .channel_id = STATUS_CHID_DIFF0,
                .a_coefficient = 0.251004016064257028112449799196787148f,
                .b_coefficient = 0.75f },
};

static void SS_platform_ADS1258_init() {
    SS_ADS1258_measurements_init(measurements, sizeof(measurements) / sizeof(measurements[0]));
    SS_ADS1258_init(&hspi1);
}
#endif

/********** MAIN INIT *********/

void SS_platform_init() {
    SS_log_init(&huart4);
    SS_console_init(&huart4);
    //    SS_platform_adc_init();
    /* SS_platform_servos_init(); */
#ifdef SS_USE_ADS1258
    SS_platform_ADS1258_init();
#endif
#ifdef SS_USE_S25FL
    SS_s25fl_init();
#endif
    /* SS_MS56_init(&ms5607, MS56_PRESS_4096, MS56_TEMP_4096); */
    /* SS_can_init(&hcan2, COM_STASZEK_ID); */
}
