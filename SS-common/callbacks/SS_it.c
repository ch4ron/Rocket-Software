/*
 * SS_it.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "FreeRTOS.h"
#include "stm32f4xx_hal.h"
#include "SS_log.h"
#include "SS_console.h"

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif
#ifdef SS_USE_FLASH
#include "SS_s25fl.h"
#include "SS_flash_ctrl.h"
#include "SS_flash_caching.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_ADS1258.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel.h"
#endif
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#include "SS_grazyna_hal.h"
#endif
#ifdef SS_USE_SEQUENCE
#include "SS_sequence.h"
#endif
#ifdef SS_USE_MPU9250
#include "SS_MPU9250.h"
#endif
#ifdef SS_USE_LORA
#include "SS_rfm23.h"
#endif
#include "stm32f4xx_hal.h"
#include "SS_log.h"
#include "SS_console.h"

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

#if defined(SS_RUN_TESTS) && defined(SS_USE_ADS1258)
#include "SS_ADS1258_unit_tests.h"
extern void SS_dynamixel_test_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
#ifdef SS_USE_ADS1258
    SS_ADS1258_EXTI_Callback(GPIO_Pin);
#endif
#ifdef SS_USE_MPU9250
    SS_MPU_GPIO_EXTI_Callback(GPIO_Pin);
#endif
#ifdef SS_USE_LORA
    SS_LoRa_EXTI_Callback(GPIO_Pin);
#endif
}

#ifdef HAL_SPI_MODULE_ENABLED
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
#ifdef SS_USE_ADS1258
    SS_ADS1258_SPI_TxRxCpltCallback(hspi);
#endif
#ifdef SS_USE_MPU9250
    SS_MPU_SPI_TxRxCpltCallback(hspi);
#endif
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
#ifdef SS_USE_MS5X
    SS_MS56_TxCpltCallback(hspi);
#endif
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
#ifdef SS_USE_MS5X
    SS_MS56_RxCpltCallback(hspi);
#endif
}
#endif

#ifdef HAL_UART_MODULE_ENABLED
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
#ifdef SS_USE_DYNAMIXEL
    SS_dynamixel_UART_RxCpltCallback(huart);
#endif
#ifdef SS_USE_GRAZYNA
    SS_grazyna_UART_RxCpltCallback(huart);
#endif
    SS_console_rx_isr(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
#ifdef SS_USE_DYNAMIXEL
    SS_dynamixel_UART_TxCpltCallback(huart);
#endif
    SS_log_tx_isr(huart);
#ifdef SS_USE_GRAZYNA
    SS_grazyna_UART_TxCpltCallback(huart);
#endif
}
#endif


/* void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) { */
/* printf("uart error callback: %d\r\n", huart->ErrorCode); */
/* } */

#ifdef HAL_QSPI_MODULE_ENABLED
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi) {
#ifdef SS_USE_FLASH
    /*bool higher_priority_task_woken = false;

    SS_s25fl_qspi_cmdcplt_handler(hqspi, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);*/
#endif
}

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi) {
#ifdef SS_USE_FLASH
    bool higher_priority_task_woken = false;

    SS_s25fl_qspi_txcplt_handler(hqspi, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
#endif
}

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi) {
#ifdef SS_USE_FLASH
    bool higher_priority_task_woken = false;

    SS_s25fl_qspi_rxcplt_handler(hqspi, &higher_priority_task_woken);
    //SS_flash_caching_qspi_rxcplt_handler(hqspi, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
#endif
}
#endif

void HAL_SYSTICK_Callback() {
/* TODO Remove Systick Callback and change it to a task */
#ifdef SS_USE_SERVOS
    /* SS_servos_SYSTICK(); */
#endif
#ifdef SS_USE_SUPPLY
    /* SS_supply_SYSTICK(); */
#endif
#ifdef SS_USE_SEQUENCE
    /* SS_sequence_SYSTICK(); */
#endif
#ifdef SS_USE_MS5X
    /* SS_MS56_SYSTICK_Callback(); */
#endif
#ifdef SS_USE_FLASH
    //SS_flash_ctrl_time_increment_handler();
#endif
}

void SS_25khz_timer_callback(void) {
#ifdef SS_USE_FLASH
    /* SS_flash_ctrl_time_increment_handler(); */
#endif
}
