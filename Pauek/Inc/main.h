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
#define COM_GREEN_Pin GPIO_PIN_13
#define COM_GREEN_GPIO_Port GPIOC
#define COM_BLUE_Pin GPIO_PIN_14
#define COM_BLUE_GPIO_Port GPIOC
#define IND_LED_Pin GPIO_PIN_15
#define IND_LED_GPIO_Port GPIOC
#define LOOP_LED_Pin GPIO_PIN_0
#define LOOP_LED_GPIO_Port GPIOH
#define MAIN_SUPPLY_DIS_Pin GPIO_PIN_1
#define MAIN_SUPPLY_DIS_GPIO_Port GPIOH
#define BACKUP_VALID_Pin GPIO_PIN_1
#define BACKUP_VALID_GPIO_Port GPIOC
#define MAIN_VALID_Pin GPIO_PIN_2
#define MAIN_VALID_GPIO_Port GPIOC
#define EXTERNAL_VALID_Pin GPIO_PIN_3
#define EXTERNAL_VALID_GPIO_Port GPIOC
#define MAIN_PROTECTED_Pin GPIO_PIN_0
#define MAIN_PROTECTED_GPIO_Port GPIOA
#define BACKUP_PROTECTED_Pin GPIO_PIN_1
#define BACKUP_PROTECTED_GPIO_Port GPIOA
#define EXTERNAL_PROTECTED_Pin GPIO_PIN_2
#define EXTERNAL_PROTECTED_GPIO_Port GPIOA
#define MR_6V6_Pin GPIO_PIN_3
#define MR_6V6_GPIO_Port GPIOA
#define CELL4_Pin GPIO_PIN_4
#define CELL4_GPIO_Port GPIOA
#define CELL3_Pin GPIO_PIN_5
#define CELL3_GPIO_Port GPIOA
#define CELL2_Pin GPIO_PIN_6
#define CELL2_GPIO_Port GPIOA
#define CELL1_Pin GPIO_PIN_7
#define CELL1_GPIO_Port GPIOA
#define CELL4_BALANCE_Pin GPIO_PIN_4
#define CELL4_BALANCE_GPIO_Port GPIOC
#define CELL3_BALANCE_Pin GPIO_PIN_5
#define CELL3_BALANCE_GPIO_Port GPIOC
#define CELL2_BALANCE_Pin GPIO_PIN_0
#define CELL2_BALANCE_GPIO_Port GPIOB
#define CELL1_BALANCE_Pin GPIO_PIN_1
#define CELL1_BALANCE_GPIO_Port GPIOB
#define MEM_GREEN_Pin GPIO_PIN_2
#define MEM_GREEN_GPIO_Port GPIOB
#define MEM_BLUE_Pin GPIO_PIN_10
#define MEM_BLUE_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOB
#define FLASH_SCK_Pin GPIO_PIN_13
#define FLASH_SCK_GPIO_Port GPIOB
#define FLASH_MISO_Pin GPIO_PIN_14
#define FLASH_MISO_GPIO_Port GPIOB
#define FLASH_MOSI_Pin GPIO_PIN_15
#define FLASH_MOSI_GPIO_Port GPIOB
#define STLINK_TX_Pin GPIO_PIN_6
#define STLINK_TX_GPIO_Port GPIOC
#define STLINK_RX_Pin GPIO_PIN_7
#define STLINK_RX_GPIO_Port GPIOC
#define MEM_RED_Pin GPIO_PIN_8
#define MEM_RED_GPIO_Port GPIOC
#define MR_7V4_Pin GPIO_PIN_9
#define MR_7V4_GPIO_Port GPIOC
#define MR_3V8_Pin GPIO_PIN_8
#define MR_3V8_GPIO_Port GPIOA
#define XBEE_TX_Pin GPIO_PIN_9
#define XBEE_TX_GPIO_Port GPIOA
#define XBEE_RX_Pin GPIO_PIN_10
#define XBEE_RX_GPIO_Port GPIOA
#define MR_13V_Pin GPIO_PIN_15
#define MR_13V_GPIO_Port GPIOA
#define WDI_7V4_Pin GPIO_PIN_10
#define WDI_7V4_GPIO_Port GPIOC
#define WDI_3V8_Pin GPIO_PIN_11
#define WDI_3V8_GPIO_Port GPIOC
#define WDI_5V5_Pin GPIO_PIN_12
#define WDI_5V5_GPIO_Port GPIOC
#define WDI_6V6_Pin GPIO_PIN_2
#define WDI_6V6_GPIO_Port GPIOD
#define COM_RED_Pin GPIO_PIN_3
#define COM_RED_GPIO_Port GPIOB
#define WDI_13V_Pin GPIO_PIN_4
#define WDI_13V_GPIO_Port GPIOB
#define MR_5V5_Pin GPIO_PIN_7
#define MR_5V5_GPIO_Port GPIOB
#define CHARGER_SCL_Pin GPIO_PIN_8
#define CHARGER_SCL_GPIO_Port GPIOB
#define CHARGER_SDA_Pin GPIO_PIN_9
#define CHARGER_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
