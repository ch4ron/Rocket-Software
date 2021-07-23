//
// Created by maciek on 02.03.2020.
//

#include "SS_platform.h"
#include "spi.h"
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
#include "tim.h"
#include "SS_console.h"
#include "usart.h"
#include "SS_log.h"
#include "stdbool.h"
#include "quadspi.h"
#ifdef SS_USE_FLASH
#include "SS_s25fl.h"
#include "SS_flash_caching.h"
#include "SS_flash_ctrl.h"
#endif

/*********** LED **********/
/*
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
*/
void SS_platform_toggle_loop_led(void) {
    HAL_GPIO_TogglePin(BLUE_GPIO_Port, BLUE_Pin);
}

/********** ADC *********/

#if defined(SS_USE_ADC)
static void SS_platform_adc_init(void) {
    ADC_HandleTypeDef *adc[] = {
        &hadc1, &hadc2, &hadc3};
    SS_adc_init(adc, sizeof(adc) / sizeof(adc[0]));
}
#endif
/********** MS5X *********/
extern struct MS5607 ms5607;
static void SS_platform_init_MS5X(void) {
    ms5607.hspi=&hspi3;
    ms5607.pause_time=1000;
    SS_MS56_init(&ms5607,MS56_PRESS_256,MS56_TEMP_256);

}

/********** MPU9250 *********/

MPU9250 mpu = {
    .gyro_id = 10,
    .accel_id = 11,
    .mgnt_id = 12,
    .CS_Port = MPU_CS_GPIO_Port,
    .CS_Pin = MPU_CS_Pin,
    .INT_Pin = MPU_INT_Pin,
    .hspi = &hspi1,
    .accel_scale = MPU_ACCEL_SCALE_8,
    .gyro_scale = MPU_GYRO_SCALE_1000,

    .mgnt_bias_x = 38,
    .mgnt_bias_y = 217,
    .mgnt_bias_z = 92,
    .mgnt_scale_x = 1.040606,
    .mgnt_scale_y = 1.015,
    .mgnt_scale_z = 0.95,
    .bias = {0, 0, 0, 0, 0, 0}
};


static void SS_platform_init_MPU(void) {

      MPU_STATUS result = MPU_OK;
    /* HAL_NVIC_DisableIRQ(MPU_INT_EXTI_IRQn); */
    /* HAL_Delay(50); */
    /* result |= SS_AK8963_set_calibration_values(&mpu, 38, 217, 92, 1.040606, 1.018278, 0.946424); */
      result |= SS_MPU_init(&mpu);
    /* result |= SS_MPU_init(&mpu2); */   /* int32_t bias1[] = {-15, -11, 72, 230, 300, 537}; */
    /* result |= SS_MPU_set_calibration(&mpu1, bias1); */
    /* HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQn); */
}

/********** MAIN INIT *********/

void SS_platform_init() {
    SS_log_init(&huart2);
    SS_console_init(&huart2);
    SS_platform_init_MPU();
    SS_platform_init_MS5X();
#ifdef SS_USE_FLASH
    /* assert(SS_s25fl_init() == FLASH_STATUS_OK); */
    assert(SS_flash_init(&hqspi, FLASH_RESET_GPIO_Port, FLASH_RESET_Pin) == FLASH_STATUS_OK);
#endif
}
