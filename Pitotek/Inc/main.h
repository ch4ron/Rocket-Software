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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define USR_BTN_Pin GPIO_PIN_0
#define USR_BTN_GPIO_Port GPIOC
#define UART4_TX_DBG_Pin GPIO_PIN_0
#define UART4_TX_DBG_GPIO_Port GPIOA
#define UART4_RX_DBG_Pin GPIO_PIN_1
#define UART4_RX_DBG_GPIO_Port GPIOA
#define BAT_MEAS_Pin GPIO_PIN_2
#define BAT_MEAS_GPIO_Port GPIOA
#define MS56_CS_Pin GPIO_PIN_4
#define MS56_CS_GPIO_Port GPIOA
#define PRESS_SCK_Pin GPIO_PIN_5
#define PRESS_SCK_GPIO_Port GPIOA
#define PRESS_MISO_Pin GPIO_PIN_6
#define PRESS_MISO_GPIO_Port GPIOA
#define PRESS_MOSI_Pin GPIO_PIN_7
#define PRESS_MOSI_GPIO_Port GPIOA
#define SD_DET_Pin GPIO_PIN_5
#define SD_DET_GPIO_Port GPIOC
#define FLASH_RST_Pin GPIO_PIN_10
#define FLASH_RST_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_12
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_14
#define LED1_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_15
#define LED0_GPIO_Port GPIOB
#define DIFF_BARO_I2C1_SDA_Pin GPIO_PIN_7
#define DIFF_BARO_I2C1_SDA_GPIO_Port GPIOB
#define DIFF_BARO_I2C1_SCL_Pin GPIO_PIN_8
#define DIFF_BARO_I2C1_SCL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
