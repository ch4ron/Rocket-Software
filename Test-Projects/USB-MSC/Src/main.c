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
#include "quadspi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SS_s25fl.h"
#include "SS_flash_ctrl.h"
#include "test.h"
//#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CMD_SIZE 160
#define CMD_TOKEN_MAX 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_QUADSPI_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  SS_s25fl_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//#ifdef TEST
  //test_s25fl();
//#endif

  uint8_t cmd[CMD_SIZE];
  uint32_t cmd_len = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);

	uint8_t c;
	if (HAL_UART_Receive(&huart2, &c, 1, 1) == HAL_OK) {
		HAL_UART_Transmit(&huart2, &c, 1, 1);
		if (c == '\r') {
			c = '\n';
			HAL_UART_Transmit(&huart2, &c, 1, 1);

			cmd[cmd_len] = '\0';

			uint32_t addr;
			uint16_t rems, var;
			uint8_t msg[CMD_SIZE];
			S25flStatus status;
			if (sscanf((char *)cmd, "%s", (char *)msg) == 1 && strncmp((char *)msg, "debug", CMD_SIZE) == 0) {
				uint8_t status_reg1, status_reg2, config_reg;
				status = SS_s25fl_debug_read_regs(&status_reg1, &status_reg2, &config_reg);

				snprintf((char *)msg, CMD_SIZE, "Read regs: status reg1 == 0x%X, status reg2 == 0x%X, config reg = 0x%X; with result %d.\r\n", status_reg1, status_reg2, config_reg, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "%s", (char *)msg) == 1 && strncmp((char *)msg, "read_rems_id", CMD_SIZE) == 0) {
				status = SS_s25fl_read_rems_id(&rems);

				snprintf((char *)msg, CMD_SIZE, "Read REMS ID 0x%X with result %d.\r\n", rems, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "%s", (char *)msg) == 1 && strncmp((char *)msg, "erase_all", CMD_SIZE) == 0) {
				status = SS_s25fl_erase_all();

				snprintf((char *)msg, CMD_SIZE, "Erase all with result %d.\r\n", status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
		    } else if (sscanf((char *)cmd, "erase 0x%x", (unsigned int *)&addr) == 1) {
				status = SS_s25fl_erase_sector(addr);

				snprintf((char *)msg, CMD_SIZE, "Erased sector on address 0x%X with result %d.\r\n", (unsigned int)addr, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "write 0x%x %c", (unsigned int *)&addr, (char *)&c) == 2) {
				status = SS_s25fl_write_bytes(addr, &c, 1);

				snprintf((char *)msg, CMD_SIZE, "Write 0x%X to address 0x%X with result %d.\r\n", (char)c, (unsigned int)addr, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "read 0x%x", (unsigned int *)&addr) == 1) {
				status = SS_s25fl_read_bytes(addr, &c, 1);

				snprintf((char *)msg, CMD_SIZE, "Read 0x%X from address 0x%X with result %d.\r\n", (char)c, (unsigned int)addr, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "write_dma 0x%x %c", (unsigned int *)&addr, (char *)&c) == 2) {
				status = SS_s25fl_write_bytes_dma(addr, &c, 1);
				HAL_Delay(1);

				snprintf((char *)msg, CMD_SIZE, "DMA write 0x%X to address 0x%X with result %d.\r\n", (char)c, (unsigned int)addr, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "read_dma 0x%x", (unsigned int *)&addr) == 1) {
				status = SS_s25fl_read_bytes_dma(addr, &c, 1);
				HAL_Delay(1);

				snprintf((char *)msg, CMD_SIZE, "DMA read 0x%X from address 0x%X with result %d.\r\n", (char)c, (unsigned int)addr, status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "%s", (char *)msg) == 1 && strncmp((char *)msg, "start_log", CMD_SIZE) == 0) {
				status = SS_flash_ctrl_start_logging();

				snprintf((char *)msg, CMD_SIZE, "Start logging with result %d.\r\n", status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "%s", (char *)msg) == 1 && strncmp((char *)msg, "stop_log", CMD_SIZE) == 0) {
				status = SS_flash_ctrl_stop_logging();

				snprintf((char *)msg, CMD_SIZE, "Stop logging with result %d.\r\n", status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "log 0x%X", (unsigned int *)&var) == 1) {
				status = SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 3, &var, 4);
				//status = SS_flash_ctrl_log_var_u32(3, var);

				snprintf((char *)msg, CMD_SIZE, "Log with result %d.\r\n", status);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			} else if (sscanf((char *)cmd, "%s", (char *)msg) == 1 && strncmp((char *)msg, "test", CMD_SIZE) == 0) {
//#ifdef TEST
				test();
//#endif
			} else {
				strncpy((char *)msg, "Unknown command.\r\n", CMD_SIZE);
				HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 10);
			}

			cmd_len = 0;
		} else if (cmd_len+1 < CMD_SIZE) {
			cmd[cmd_len] = c;
			++cmd_len;
		} else {
			uint8_t msg[] = "The command is too long.\r\n";
			HAL_UART_Transmit(&huart2, msg, strlen((char *)msg), 100);
			cmd_len = 0;
		}
	}
  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIM = 8;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
	SS_s25fl_qspi_txcplt_handler(hqspi);
}

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
	SS_s25fl_qspi_rxcplt_handler(hqspi);
	SS_flash_caching_qspi_rxcplt_handler(hqspi);
}

int __io_putchar(int ch)
{
	if (ch == '\n') {
		uint8_t cr = '\r';
		if (HAL_UART_Transmit(&huart2, (uint8_t *)&cr, 1, 1000) != HAL_OK) {
			return EOF;
		}
	}

	if (HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000) != HAL_OK) {
		return EOF;
	}
	return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1) {
  }
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
