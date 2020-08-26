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
#ifdef SS_USE_MPU9250
#include "SS_MPU9250.h"
#endif
#ifdef SS_USE_FLASH
#include "SS_s25fl.h"
#include "SS_flash_caching.h"
#include "SS_flash_ctrl.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_ADS1258.h"
#include "SS_measurements.h"
#endif
#include "quadspi.h"
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

void SS_platform_set_adc_led(bool red, bool green, bool blue) {
    HAL_GPIO_WritePin(MEAS_RED_GPIO_Port, MEAS_RED_Pin, !red);
    HAL_GPIO_WritePin(MEAS_GREEN_GPIO_Port, MEAS_GREEN_Pin, !green);
    HAL_GPIO_WritePin(MEAS_BLUE_GPIO_Port, MEAS_BLUE_Pin, !blue);
}

void SS_platform_toggle_loop_led(void) {
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

void SS_platform_servos_init(void) {
    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
}
#endif

/********** ADC *********/

#if defined(SS_USE_ADC)
static void SS_platform_adc_init(void) {
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

static void SS_platform_ADS1258_init(void) {
    SS_ADS1258_measurements_init(measurements, sizeof(measurements) / sizeof(measurements[0]));
    SS_ADS1258_init(&hspi1);
}
#endif


/********** MPU9250 *********/

#ifdef SS_USE_MPU9250
static MPU9250 mpu = {
    .gyro_id = 10,
    .accel_id = 11,
    .mgnt_id = 12,
    .CS_Port = MPU_CS_GPIO_Port,
    .CS_Pin = MPU_CS_Pin,
    .INT_Pin = MPU_INT_Pin,
    .hspi = &hspi4,
    .accel_scale = MPU_ACCEL_SCALE_2,
    .gyro_scale = MPU_GYRO_SCALE_250,

    .mgnt_bias_x = 38,
    .mgnt_bias_y = 217,
    .mgnt_bias_z = 92,
    .mgnt_scale_x = 1.040606,
    .mgnt_scale_y = 1.015,
    .mgnt_scale_z = 0.95,
    .bias = {-15, -11, 72, 230, 300, 537}
};

static void SS_platform_init_MPU(void) {
    MPU_STATUS result = MPU_OK;
    /* HAL_NVIC_DisableIRQ(MPU_INT_EXTI_IRQn); */
    /* HAL_Delay(50); */
    /* result |= SS_AK8963_set_calibration_values(&mpu, 38, 217, 92, 1.040606, 1.018278, 0.946424); */
    result |= SS_MPU_init(&mpu);
    /* result |= SS_MPU_init(&mpu2); */
    /* int32_t bias1[] = {-15, -11, 72, 230, 300, 537}; */
    /* result |= SS_MPU_set_calibration(&mpu1, bias1); */
    /* HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQn); */
}
#endif

/********** MAIN INIT *********/

void SS_platform_init() {
    SS_log_init(&huart4);
    SS_console_init(&huart4);
    //    SS_platform_adc_init();
#ifdef SS_USE_SERVOS
    SS_platform_servos_init();
#endif
#ifdef SS_USE_ADS1258
    SS_platform_ADS1258_init();
#endif
    /* SS_MS56_init(&ms5607, MS56_PRESS_4096, MS56_TEMP_4096); */
#ifdef SS_USE_CAN
    SS_can_init(&hcan2, COM_STASZEK_ID);
#endif
#ifdef SS_USE_MPU9250
    SS_platform_init_MPU();
#endif
#ifdef SS_USE_FLASH
    assert(SS_s25fl_init(FLASH_RESET_GPIO_Port, FLASH_RESET_Pin, 64*1024*1024, 256*1024, 512, true, 4, 1) == S25FL_STATUS_OK);
    assert(SS_flash_init(&hqspi, FLASH_RESET_GPIO_Port, FLASH_RESET_Pin) == FLASH_STATUS_OK);
#endif
}
