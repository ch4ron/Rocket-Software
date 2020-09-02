/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define UI_SW_Pin GPIO_PIN_13
#define UI_SW_GPIO_Port GPIOC
#define P_RADIO_GPIO1_Pin GPIO_PIN_1
#define P_RADIO_GPIO1_GPIO_Port GPIOA
#define DEBUG_TX_Pin GPIO_PIN_2
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_3
#define DEBUG_RX_GPIO_Port GPIOA
#define RADIO_SCK_Pin GPIO_PIN_5
#define RADIO_SCK_GPIO_Port GPIOA
#define RADIO_MISO_Pin GPIO_PIN_6
#define RADIO_MISO_GPIO_Port GPIOA
#define RADIO_MOSI_Pin GPIO_PIN_7
#define RADIO_MOSI_GPIO_Port GPIOA
#define P_RADIO_CS_Pin GPIO_PIN_4
#define P_RADIO_CS_GPIO_Port GPIOC
#define LED_IND_Pin GPIO_PIN_5
#define LED_IND_GPIO_Port GPIOC
#define LED_LOOP_Pin GPIO_PIN_0
#define LED_LOOP_GPIO_Port GPIOB
#define P_RADIO_PA_PWR_Pin GPIO_PIN_14
#define P_RADIO_PA_PWR_GPIO_Port GPIOB
#define P_RADIO_TCXO_PWR_Pin GPIO_PIN_15
#define P_RADIO_TCXO_PWR_GPIO_Port GPIOB
#define P_RADIO_IRQ_Pin GPIO_PIN_6
#define P_RADIO_IRQ_GPIO_Port GPIOC
#define P_RADIO_SDN_Pin GPIO_PIN_7
#define P_RADIO_SDN_GPIO_Port GPIOC
#define GPS_PWR_Pin GPIO_PIN_2
#define GPS_PWR_GPIO_Port GPIOD
#define GPS_PULSE_Pin GPIO_PIN_4
#define GPS_PULSE_GPIO_Port GPIOB
#define GPS_RST_Pin GPIO_PIN_5
#define GPS_RST_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
