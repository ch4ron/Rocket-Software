/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#define SD_CARD_MOSI_Pin GPIO_PIN_1
#define SD_CARD_MOSI_GPIO_Port GPIOC
#define GRZALKA_Pin GPIO_PIN_3
#define GRZALKA_GPIO_Port GPIOC
#define VIBRATION_SENSOR_Pin GPIO_PIN_0
#define VIBRATION_SENSOR_GPIO_Port GPIOA
#define TEMPERATURE_SENSOR_Pin GPIO_PIN_1
#define TEMPERATURE_SENSOR_GPIO_Port GPIOA
#define OXYGEN_SENSOR_Pin GPIO_PIN_2
#define OXYGEN_SENSOR_GPIO_Port GPIOA
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
#define BAT_VOLTAGE_Pin GPIO_PIN_5
#define BAT_VOLTAGE_GPIO_Port GPIOC
#define COM_RED_Pin GPIO_PIN_0
#define COM_RED_GPIO_Port GPIOB
#define COM_GREEN_Pin GPIO_PIN_1
#define COM_GREEN_GPIO_Port GPIOB
#define COM_BLUE_Pin GPIO_PIN_7
#define COM_BLUE_GPIO_Port GPIOE
#define RED_Pin GPIO_PIN_8
#define RED_GPIO_Port GPIOE
#define GREEN_Pin GPIO_PIN_9
#define GREEN_GPIO_Port GPIOE
#define BLUE_Pin GPIO_PIN_10
#define BLUE_GPIO_Port GPIOE
#define LOOP_LED_Pin GPIO_PIN_11
#define LOOP_LED_GPIO_Port GPIOE
#define IND_LED_Pin GPIO_PIN_12
#define IND_LED_GPIO_Port GPIOE
#define COM_RED2_Pin GPIO_PIN_13
#define COM_RED2_GPIO_Port GPIOE
#define COM_GREEN2_Pin GPIO_PIN_14
#define COM_GREEN2_GPIO_Port GPIOE
#define COM_BLUE2_Pin GPIO_PIN_15
#define COM_BLUE2_GPIO_Port GPIOE
#define SENSOR_D_SCL_Pin GPIO_PIN_10
#define SENSOR_D_SCL_GPIO_Port GPIOB
#define SD_CARD_CS_Pin GPIO_PIN_12
#define SD_CARD_CS_GPIO_Port GPIOB
#define SD_CARD_SCK_Pin GPIO_PIN_13
#define SD_CARD_SCK_GPIO_Port GPIOB
#define SD_CARD_MISO_Pin GPIO_PIN_14
#define SD_CARD_MISO_GPIO_Port GPIOB
#define FLASH_RESET_Pin GPIO_PIN_8
#define FLASH_RESET_GPIO_Port GPIOD
#define SD_CARD_DETECT_Pin GPIO_PIN_9
#define SD_CARD_DETECT_GPIO_Port GPIOD
#define SENSOR_C_DRDY_Pin GPIO_PIN_8
#define SENSOR_C_DRDY_GPIO_Port GPIOC
#define SENSOR_C_SDA_Pin GPIO_PIN_9
#define SENSOR_C_SDA_GPIO_Port GPIOC
#define SENSOR_C_SCL_Pin GPIO_PIN_8
#define SENSOR_C_SCL_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOA
#define SENSOR_D_SDA_Pin GPIO_PIN_12
#define SENSOR_D_SDA_GPIO_Port GPIOC
#define SENSOR_D_DRDY_Pin GPIO_PIN_0
#define SENSOR_D_DRDY_GPIO_Port GPIOD
#define MS56_CS_Pin GPIO_PIN_7
#define MS56_CS_GPIO_Port GPIOD
#define MS56_SCK_Pin GPIO_PIN_3
#define MS56_SCK_GPIO_Port GPIOB
#define MS56_MISO_Pin GPIO_PIN_4
#define MS56_MISO_GPIO_Port GPIOB
#define MS56_MOSI_Pin GPIO_PIN_5
#define MS56_MOSI_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
