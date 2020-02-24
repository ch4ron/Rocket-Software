/*
 * SS_it.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "SS_ADS1258.h"
#include "SS_config.h"
#include "SS_servos.h"
#include "SS_dynamixel.h"
#include "SS_Grazyna_com.h"
#include "SS_sequence.h"

#ifdef RUN_TESTS
#include "SS_ADS1258_unit_tests.h"
extern void SS_dynamixel_test_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    SS_ADS1258_EXTI_Callback(GPIO_Pin);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    SS_ADS1258_SPI_TxRxCpltCallback(hspi);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
	printf("spi error\r\n");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    SS_dynamixel_UART_RxCpltCallback(huart);
    SS_grazyna_UART_RxCpltCallback(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    SS_dynamixel_UART_TxCpltCallback(huart);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
#ifdef RUN_TESTS
    SS_dynamixel_test_TIM_PeriodElapsedCallback(htim);
#endif
}
//
//void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
//    if(huart == &huart2)
//    printf("rxcptl half callback\r\n");
//}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
//    if(huart == &huart2)
//    printf("uart error callback\r\n");

}

void HAL_SYSTICK_Callback() {
    SS_servos_SYSTICK();
    SS_supply_SYSTICK();
    SS_dynamixel_SYSTICK_Callback();
    SS_sequence_SYSTICK();
}
