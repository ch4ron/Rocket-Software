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
#define MPU_INT_Pin GPIO_PIN_3
#define MPU_INT_GPIO_Port GPIOE
#define MPU_INT_EXTI_IRQn EXTI3_IRQn
#define MPU_CS_Pin GPIO_PIN_4
#define MPU_CS_GPIO_Port GPIOE
#define MEMS_MISO_Pin GPIO_PIN_5
#define MEMS_MISO_GPIO_Port GPIOE
#define MEMS_MOSI_Pin GPIO_PIN_6
#define MEMS_MOSI_GPIO_Port GPIOE
#define ADC_CS_Pin GPIO_PIN_0
#define ADC_CS_GPIO_Port GPIOC
#define MEM_RED_Pin GPIO_PIN_0
#define MEM_RED_GPIO_Port GPIOA
#define MEM_BLUE_Pin GPIO_PIN_1
#define MEM_BLUE_GPIO_Port GPIOA
#define MEM_GREEN_Pin GPIO_PIN_2
#define MEM_GREEN_GPIO_Port GPIOA
#define ISL_COMMON_Pin GPIO_PIN_3
#define ISL_COMMON_GPIO_Port GPIOA
#define ADC_RES1_Pin GPIO_PIN_4
#define ADC_RES1_GPIO_Port GPIOA
#define ADC_RES4_Pin GPIO_PIN_5
#define ADC_RES4_GPIO_Port GPIOA
#define ADC_CURRENT4_Pin GPIO_PIN_6
#define ADC_CURRENT4_GPIO_Port GPIOA
#define ADC_CURRENT1_Pin GPIO_PIN_7
#define ADC_CURRENT1_GPIO_Port GPIOA
#define ADC_CURRENT2_Pin GPIO_PIN_4
#define ADC_CURRENT2_GPIO_Port GPIOC
#define ADC_CURRENT3_Pin GPIO_PIN_5
#define ADC_CURRENT3_GPIO_Port GPIOC
#define ADC_RES3_Pin GPIO_PIN_0
#define ADC_RES3_GPIO_Port GPIOB
#define ADC_RES2_Pin GPIO_PIN_1
#define ADC_RES2_GPIO_Port GPIOB
#define SERVO_RX_Pin GPIO_PIN_7
#define SERVO_RX_GPIO_Port GPIOE
#define SERVO_TX_Pin GPIO_PIN_8
#define SERVO_TX_GPIO_Port GPIOE
#define ADS_START_Pin GPIO_PIN_9
#define ADS_START_GPIO_Port GPIOE
#define ADS_DRDY_Pin GPIO_PIN_10
#define ADS_DRDY_GPIO_Port GPIOE
#define ADS_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define ADS_RESET_Pin GPIO_PIN_11
#define ADS_RESET_GPIO_Port GPIOE
#define MEMS_SCK_Pin GPIO_PIN_12
#define MEMS_SCK_GPIO_Port GPIOE
#define MS56_CS_Pin GPIO_PIN_13
#define MS56_CS_GPIO_Port GPIOE
#define FLASH_CS_Pin GPIO_PIN_15
#define FLASH_CS_GPIO_Port GPIOE
#define FLASH_SCK_Pin GPIO_PIN_10
#define FLASH_SCK_GPIO_Port GPIOB
#define FLASH_MISO_Pin GPIO_PIN_14
#define FLASH_MISO_GPIO_Port GPIOB
#define FLASH_MOSI_Pin GPIO_PIN_15
#define FLASH_MOSI_GPIO_Port GPIOB
#define ISL_G0_Pin GPIO_PIN_8
#define ISL_G0_GPIO_Port GPIOD
#define ISL_G0_Z_Pin GPIO_PIN_9
#define ISL_G0_Z_GPIO_Port GPIOD
#define ISL_G1_Pin GPIO_PIN_10
#define ISL_G1_GPIO_Port GPIOD
#define ISL_G1_Z_Pin GPIO_PIN_11
#define ISL_G1_Z_GPIO_Port GPIOD
#define POWER_INPUT4_Pin GPIO_PIN_12
#define POWER_INPUT4_GPIO_Port GPIOD
#define POWER_INPUT1_Pin GPIO_PIN_13
#define POWER_INPUT1_GPIO_Port GPIOD
#define POWER_INPUT2_Pin GPIO_PIN_14
#define POWER_INPUT2_GPIO_Port GPIOD
#define POWER_INPUT3_Pin GPIO_PIN_15
#define POWER_INPUT3_GPIO_Port GPIOD
#define COM_GREEN_Pin GPIO_PIN_6
#define COM_GREEN_GPIO_Port GPIOC
#define COM_BLUE_Pin GPIO_PIN_7
#define COM_BLUE_GPIO_Port GPIOC
#define COM_RED_Pin GPIO_PIN_8
#define COM_RED_GPIO_Port GPIOC
#define STASZEK_RECO_PWM_Pin GPIO_PIN_9
#define STASZEK_RECO_PWM_GPIO_Port GPIOC
#define CHECK_RESISTANCE_Pin GPIO_PIN_15
#define CHECK_RESISTANCE_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOC
#define MEAS_RED_Pin GPIO_PIN_5
#define MEAS_RED_GPIO_Port GPIOD
#define MEAS_BLUE_Pin GPIO_PIN_6
#define MEAS_BLUE_GPIO_Port GPIOD
#define MEAS_GREEN_Pin GPIO_PIN_7
#define MEAS_GREEN_GPIO_Port GPIOD
#define ADC_SCK_Pin GPIO_PIN_3
#define ADC_SCK_GPIO_Port GPIOB
#define ADC_DOUT_Pin GPIO_PIN_4
#define ADC_DOUT_GPIO_Port GPIOB
#define ADC_MOSI_Pin GPIO_PIN_5
#define ADC_MOSI_GPIO_Port GPIOB
#define MAGNETO_SCL_Pin GPIO_PIN_6
#define MAGNETO_SCL_GPIO_Port GPIOB
#define MAGNETO_SDA_Pin GPIO_PIN_7
#define MAGNETO_SDA_GPIO_Port GPIOB
#define IND_LED_Pin GPIO_PIN_0
#define IND_LED_GPIO_Port GPIOE
#define LOOP_LED_Pin GPIO_PIN_1
#define LOOP_LED_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
