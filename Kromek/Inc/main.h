/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#define IGN_TEST_Pin GPIO_PIN_0
#define IGN_TEST_GPIO_Port GPIOF
#define IGN_Pin GPIO_PIN_1
#define IGN_GPIO_Port GPIOF
#define SERVOS_TRIGGER_Pin GPIO_PIN_2
#define SERVOS_TRIGGER_GPIO_Port GPIOF
#define RES_WIRE_CURRENT_Pin GPIO_PIN_4
#define RES_WIRE_CURRENT_GPIO_Port GPIOF
#define ENABLE3_Pin GPIO_PIN_5
#define ENABLE3_GPIO_Port GPIOF
#define ENABLE4_Pin GPIO_PIN_7
#define ENABLE4_GPIO_Port GPIOF
#define ENABLE2_Pin GPIO_PIN_8
#define ENABLE2_GPIO_Port GPIOF
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOF
#define ENABLE1_Pin GPIO_PIN_10
#define ENABLE1_GPIO_Port GPIOF
#define SERVOS2_VOLTAGE_Pin GPIO_PIN_1
#define SERVOS2_VOLTAGE_GPIO_Port GPIOC
#define RELAYS_VOLTAGE_Pin GPIO_PIN_2
#define RELAYS_VOLTAGE_GPIO_Port GPIOC
#define SERVOS1_VOLTAGE_Pin GPIO_PIN_3
#define SERVOS1_VOLTAGE_GPIO_Port GPIOC
#define SERVO1_CURRENT_Pin GPIO_PIN_0
#define SERVO1_CURRENT_GPIO_Port GPIOA
#define SERVO2_CURRENT_Pin GPIO_PIN_1
#define SERVO2_CURRENT_GPIO_Port GPIOA
#define SERVO3_CURRENT_Pin GPIO_PIN_2
#define SERVO3_CURRENT_GPIO_Port GPIOA
#define SERVO4_CURRENT_Pin GPIO_PIN_3
#define SERVO4_CURRENT_GPIO_Port GPIOA
#define KOZACKIE_CURRENT_Pin GPIO_PIN_4
#define KOZACKIE_CURRENT_GPIO_Port GPIOA
#define KOZACKIE_SERVO_VOLTAGE_Pin GPIO_PIN_5
#define KOZACKIE_SERVO_VOLTAGE_GPIO_Port GPIOA
#define SERVO5_CURRENT_Pin GPIO_PIN_6
#define SERVO5_CURRENT_GPIO_Port GPIOA
#define SERVO7_CURRENT_Pin GPIO_PIN_7
#define SERVO7_CURRENT_GPIO_Port GPIOA
#define SERVO6_CURRENT_Pin GPIO_PIN_4
#define SERVO6_CURRENT_GPIO_Port GPIOC
#define SERVO8_CURRENT_Pin GPIO_PIN_5
#define SERVO8_CURRENT_GPIO_Port GPIOC
#define RELAY9_Pin GPIO_PIN_0
#define RELAY9_GPIO_Port GPIOB
#define RELAY8_Pin GPIO_PIN_1
#define RELAY8_GPIO_Port GPIOB
#define RELAY7_Pin GPIO_PIN_11
#define RELAY7_GPIO_Port GPIOF
#define RELAY6_Pin GPIO_PIN_12
#define RELAY6_GPIO_Port GPIOF
#define RELAY5_Pin GPIO_PIN_13
#define RELAY5_GPIO_Port GPIOF
#define RELAY4_Pin GPIO_PIN_14
#define RELAY4_GPIO_Port GPIOF
#define RELAY3_Pin GPIO_PIN_15
#define RELAY3_GPIO_Port GPIOF
#define RELAY2_Pin GPIO_PIN_0
#define RELAY2_GPIO_Port GPIOG
#define RELAY1_Pin GPIO_PIN_1
#define RELAY1_GPIO_Port GPIOG
#define FLASH_RESET_Pin GPIO_PIN_15
#define FLASH_RESET_GPIO_Port GPIOE
#define ADS_START_Pin GPIO_PIN_11
#define ADS_START_GPIO_Port GPIOB
#define ADS_DRDY_Pin GPIO_PIN_12
#define ADS_DRDY_GPIO_Port GPIOB
#define ADS_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define EXT_UART_TX_Pin GPIO_PIN_8
#define EXT_UART_TX_GPIO_Port GPIOD
#define EXT_UART_RX_Pin GPIO_PIN_9
#define EXT_UART_RX_GPIO_Port GPIOD
#define ADS_RESET_Pin GPIO_PIN_10
#define ADS_RESET_GPIO_Port GPIOD
#define XBEE_TX_LED_Pin GPIO_PIN_11
#define XBEE_TX_LED_GPIO_Port GPIOD
#define XBEE_RX_LED_Pin GPIO_PIN_12
#define XBEE_RX_LED_GPIO_Port GPIOD
#define ADC_RED_Pin GPIO_PIN_13
#define ADC_RED_GPIO_Port GPIOD
#define ADC_GREEN_Pin GPIO_PIN_14
#define ADC_GREEN_GPIO_Port GPIOD
#define ADC_BLUE_Pin GPIO_PIN_15
#define ADC_BLUE_GPIO_Port GPIOD
#define COM_RED_Pin GPIO_PIN_2
#define COM_RED_GPIO_Port GPIOG
#define COM_GREEN_Pin GPIO_PIN_3
#define COM_GREEN_GPIO_Port GPIOG
#define COM_BLUE_Pin GPIO_PIN_4
#define COM_BLUE_GPIO_Port GPIOG
#define MEM_RED_Pin GPIO_PIN_5
#define MEM_RED_GPIO_Port GPIOG
#define MEM_GREEN_Pin GPIO_PIN_6
#define MEM_GREEN_GPIO_Port GPIOG
#define MEM_BLUE_Pin GPIO_PIN_7
#define MEM_BLUE_GPIO_Port GPIOG
#define CHECK_RESISTANCE_Pin GPIO_PIN_8
#define CHECK_RESISTANCE_GPIO_Port GPIOG
#define SERVO8_Pin GPIO_PIN_6
#define SERVO8_GPIO_Port GPIOC
#define SERVO7_Pin GPIO_PIN_7
#define SERVO7_GPIO_Port GPIOC
#define SERVO6_Pin GPIO_PIN_8
#define SERVO6_GPIO_Port GPIOC
#define SERVO5_Pin GPIO_PIN_9
#define SERVO5_GPIO_Port GPIOC
#define SERVO4_Pin GPIO_PIN_8
#define SERVO4_GPIO_Port GPIOA
#define SERVO3_Pin GPIO_PIN_9
#define SERVO3_GPIO_Port GPIOA
#define SERVO2_Pin GPIO_PIN_10
#define SERVO2_GPIO_Port GPIOA
#define FAN_Pin GPIO_PIN_15
#define FAN_GPIO_Port GPIOA
#define STLINK_TX_Pin GPIO_PIN_12
#define STLINK_TX_GPIO_Port GPIOC
#define STLINK_RX_Pin GPIO_PIN_2
#define STLINK_RX_GPIO_Port GPIOD
#define AC_RELAY2_Pin GPIO_PIN_3
#define AC_RELAY2_GPIO_Port GPIOD
#define AC_RELAY1_Pin GPIO_PIN_4
#define AC_RELAY1_GPIO_Port GPIOD
#define XBEE_TX_Pin GPIO_PIN_5
#define XBEE_TX_GPIO_Port GPIOD
#define XBEE_RX_Pin GPIO_PIN_6
#define XBEE_RX_GPIO_Port GPIOD
#define SERVOS_RX_Pin GPIO_PIN_9
#define SERVOS_RX_GPIO_Port GPIOG
#define RS485_DE_Pin GPIO_PIN_13
#define RS485_DE_GPIO_Port GPIOG
#define SERVOS_TX_Pin GPIO_PIN_14
#define SERVOS_TX_GPIO_Port GPIOG
#define SERVO1_Pin GPIO_PIN_5
#define SERVO1_GPIO_Port GPIOB
#define RS485_TX_Pin GPIO_PIN_6
#define RS485_TX_GPIO_Port GPIOB
#define RS485_RX_Pin GPIO_PIN_7
#define RS485_RX_GPIO_Port GPIOB
#define I2C_SDA_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_3
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SCL_Pin GPIO_PIN_4
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
