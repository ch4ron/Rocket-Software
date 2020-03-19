//
// Created by maciek on 02.03.2020.
//

#include "SS_can.h"
#include "SS_common.h"
#include "SS_platform.h"
#include "usart.h"
#include "SS_MS5X.h"


/********** PRINTF *********/

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart4, (uint8_t*) ptr, (uint16_t) len, 1000);
	return len;
}


/********** SERVOS *********/

//Servo servos[] = {
//        {.id = 0, .tim = &htim3, .channel = TIM_CHANNEL_2, .supply = &servos1_supply},
//        {.id = 1, .tim = &htim1, .channel = TIM_CHANNEL_3, .supply = &servos1_supply},
//        {.id = 2, .tim = &htim1, .channel = TIM_CHANNEL_2, .supply = &servos1_supply},
//        {.id = 3, .tim = &htim1, .channel = TIM_CHANNEL_1, .supply = &servos1_supply},
//        {.id = 4, .tim = &htim3, .channel = TIM_CHANNEL_4, .supply = &servos2_supply},
//        {.id = 5, .tim = &htim3, .channel = TIM_CHANNEL_3, .supply = &servos2_supply},
//        {.id = 6, .tim = &htim8, .channel = TIM_CHANNEL_2, .supply = &servos2_supply},
//        {.id = 7, .tim = &htim3, .channel = TIM_CHANNEL_1, .supply = &servos2_supply},
//};
//
//void SS_platform_servos_init() {
//    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
//}

/********** ADC *********/

static void SS_platform_adc_init() {
#if defined(SS_USE_ADC) && !defined(SIMULATE)
    ADC_HandleTypeDef *adc[] = {
            &hadc1, &hadc2, &hadc3
    };
    SS_adc_init(adc, sizeof(adc)/sizeof(adc[0]));
#endif
}

/********** ADS1258 *********/

Measurement measurements[] = {
        { .channel_id = STATUS_CHID_DIFF0,
                .a_coefficient = 0.251004016064257028112449799196787148f,
                .b_coefficient = 0.75f },
};

static void SS_platform_ADS1258_init() {
    SS_ADS1258_measurements_init(measurements, sizeof(measurements) / sizeof(measurements[0]));
    SS_ADS1258_init(&hspi2);
}

/********** MAIN INIT *********/

void SS_platform_init() {
//    SS_platform_adc_init();
//    SS_platform_servos_init();
#ifndef SIMULATE
    SS_platform_ADS1258_init();
#endif
    SS_MS56_init(&ms5607, MS56_PRESS_4096, MS56_TEMP_4096);
    SS_can_init(&hcan1, COM_STASZEK_ID);
}
