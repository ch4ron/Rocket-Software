/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "SS_FreeRTOS.h"
#include "SS_platform.h"
#include "SS_MPU9250.h"
#include "SS_common.h"
#include "SS_init.h"
#include "scd30.h"
#include "sensirion_common.h"
#include "task.h"
#include "SS_log.h"
#include "timers.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int32_t Dane=100;
float32_t co2_ppm, temperature, relative_humidity;
int16_t err;
uint16_t interval_in_seconds = 2;
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t*) ptr, (uint16_t) len, 1000);
    return len;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Scd30_task(void *pvParameters);
void led_loop(void *pvParameters);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
static MPU9250 mpu = {
    .gyro_id = 10,
    .accel_id = 11,
    .mgnt_id = 12,
    .CS_Port = MPU_CS_GPIO_Port,
    .CS_Pin = MPU_CS_Pin,
    .INT_Pin = MPU_INT_Pin,
    .hspi = &hspi1,
    .accel_scale = MPU_ACCEL_SCALE_2,
    .gyro_scale = MPU_GYRO_SCALE_250,

    .mgnt_bias_x = 38,
    .mgnt_bias_y = 217,
    .mgnt_bias_z = 92,
    .mgnt_scale_x = 1.040606,
    .mgnt_scale_y = 1.015,
    .mgnt_scale_z = 0.95,
    .bias = {-15, -11, 72, 230, 300, 537}
};
 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM14_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_QUADSPI_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  SS_platform_init();
  xTaskCreate(&Scd30_task, "Scd 30", 2048, NULL, 20, NULL);
  xTaskCreate(&led_loop, " loop led ", 64, NULL, 1, NULL);
   while (scd30_probe() != STATUS_OK) {
        SS_print("SCD30 sensor probing failed\n");
        sensirion_sleep_usec(1000000u);
    }
    SS_print("SCD30 sensor probing successful\n");

    scd30_set_measurement_interval(interval_in_seconds);
    sensirion_sleep_usec(20000u);
    scd30_start_periodic_measurement(0);
    sensirion_sleep_usec(interval_in_seconds * 1000000u);

  /* SS_MPU_get_accel_data(&mpu); */
  /* SS_MPU_get_gyro_data(&mpu); */
  /* Dane=SS_MPU_who_am_i(&mpu); */

    vTaskStartScheduler();
    SS_init();





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

    return 0;
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Scd30_task(void *pvParameters){
    while(1){
        //scd30_start_periodic_measurement(0);
       // sensirion_sleep_usec(interval_in_seconds * 1000000u);
        err = scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity);
        if (err != STATUS_OK) {
            SS_print("error reading measurement\n");

        } else {
            SS_print("%0.2f %0.2f %0.2f\n", co2_ppm, temperature, relative_humidity);
        }
       // sensirion_sleep_usec(interval_in_seconds * 1000000u);
      //  scd30_stop_periodic_measurement();
        vTaskDelay( 300 / portTICK_RATE_MS );
    }
}

void led_loop(void *pVParameters)
{
    while(1)
    {
        HAL_GPIO_TogglePin(LED_BLUE_1_GPIO_Port, LED_BLUE_1_Pin);
        vTaskDelay( 300 / portTICK_RATE_MS );
    }
}



extern void SS_FreeRTOS_25khz_timer_callback(TIM_HandleTypeDef *htim);
/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM13 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  SS_FreeRTOS_25khz_timer_callback(htim);
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM13) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
