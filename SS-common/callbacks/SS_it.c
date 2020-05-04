/*
 * SS_it.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#ifdef SS_USE_ADS1258

#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif
#ifdef SS_USE_S25FL
#include "SS_flash_ctrl.h"
#include "SS_s25fl.h"
#endif
#include "FreeRTOS.h"
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
#include "stm32f4xx_hal.h"
#include "SS_log.h"
#include "SS_console.h"

#ifdef SS_RUN_TESTS
#include "SS_ADS1258_unit_tests.h"
extern void SS_dynamixel_test_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
#ifdef SS_USE_ADS1258
    SS_ADS1258_EXTI_Callback(GPIO_Pin);
#endif
}

#ifdef HAL_SPI_MODULE_ENABLED
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
#ifdef SS_USE_ADS1258
    SS_ADS1258_SPI_TxRxCpltCallback(hspi);
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
// void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) { printf("spi
// error\r\n"); }
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
}
#endif


/* void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) { */
/* printf("uart error callback: %d\r\n", huart->ErrorCode); */
/* } */

#ifdef HAL_QSPI_ENABLED
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi) {
#ifdef SS_USE_S25FL
    SS_s25fl_txcplt_handler();
#endif
}

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi) {
#ifdef SS_USE_S25FL
    SS_s25fl_rxcplt_handler();
#endif
}
#endif

void HAL_SYSTICK_Callback() {
#ifdef SS_USE_SERVOS
    SS_servos_SYSTICK();
#endif
#ifdef SS_USE_SUPPLY
    SS_supply_SYSTICK();
#endif
#ifdef SS_USE_SEQUENCE
    SS_sequence_SYSTICK();
#endif
#ifdef SS_USE_MS5X
    SS_MS56_SYSTICK_Callback();
#endif
#ifdef SS_USE_S25FL
    SS_flash_ctrl_time_increment_handler();
#endif
}
