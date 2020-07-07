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
#define MPU_CS_Pin GPIO_PIN_4
#define MPU_CS_GPIO_Port GPIOA
#define MPU_SCK_Pin GPIO_PIN_5
#define MPU_SCK_GPIO_Port GPIOA
#define MPU_MISO_Pin GPIO_PIN_6
#define MPU_MISO_GPIO_Port GPIOA
#define MPU_MOSI_Pin GPIO_PIN_7
#define MPU_MOSI_GPIO_Port GPIOA
#define MPU_INT_Pin GPIO_PIN_4
#define MPU_INT_GPIO_Port GPIOC
#define MPU_INT_EXTI_IRQn EXTI4_IRQn
#define LED_GREEN_1_Pin GPIO_PIN_8
#define LED_GREEN_1_GPIO_Port GPIOE
#define LED_ORANGE_Pin GPIO_PIN_9
#define LED_ORANGE_GPIO_Port GPIOE
#define LED_BLUE_1_Pin GPIO_PIN_10
#define LED_BLUE_1_GPIO_Port GPIOE
#define LED_GREEN_2_Pin GPIO_PIN_11
#define LED_GREEN_2_GPIO_Port GPIOE
#define LED_BLUE_2_Pin GPIO_PIN_12
#define LED_BLUE_2_GPIO_Port GPIOE
#define LED_RGB_GREEN_LOWonly_DO_NOT_USE_Pin GPIO_PIN_13
#define LED_RGB_GREEN_LOWonly_DO_NOT_USE_GPIO_Port GPIOE
#define LED_RGB_DO_NOT_WORK_DO_NOT_USE_Pin GPIO_PIN_14
#define LED_RGB_DO_NOT_WORK_DO_NOT_USE_GPIO_Port GPIOE
#define LED_RGB_HIGH_only_DO_NOT_USE_Pin GPIO_PIN_15
#define LED_RGB_HIGH_only_DO_NOT_USE_GPIO_Port GPIOE
#define LED_RGB_GREEN_DO_NOT_USE_Pin GPIO_PIN_12
#define LED_RGB_GREEN_DO_NOT_USE_GPIO_Port GPIOB
#define LED_RGB_HIGHONLY_DO_NOT_USE_Pin GPIO_PIN_13
#define LED_RGB_HIGHONLY_DO_NOT_USE_GPIO_Port GPIOB
#define LED_RGB_BLUE_DO_NOT_USE_Pin GPIO_PIN_14
#define LED_RGB_BLUE_DO_NOT_USE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
