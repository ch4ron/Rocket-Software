/* USER CODE BEGIN Header */
// LED0 - BAT_LOW
// LED1 - SD_ERROR
// LED2 - SD_WRITE
// LED3 - LOG_LOOP

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "quadspi.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SS_MS5607.h"
#include "SD_card_func.h"
#include "PITOT_func.h"
#include <string.h>
#include <stdio.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SD_CARD_WRITE_LED_ON  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin,GPIO_PIN_SET)
#define SD_CARD_WRITE_LED_OFF  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin,GPIO_PIN_RESET)

#define LOGS_BUFF_SIZE 20 // save to SD card once every second

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

char Data_log_line_single [250];
uint16_t Data_log_line_length_single;

char Data_log_line [LOGS_BUFF_SIZE][250];
uint16_t Data_log_line_length [LOGS_BUFF_SIZE];
uint8_t Data_log_line_cnt=0;

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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_QUADSPI_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_UART4_Init();
  MX_USB_DEVICE_Init();
  MX_FATFS_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(100);

  SD_CARD_init ();
  f_mount(0,SDPath,1);

  FRESULT file_op_res;
  UINT BytesWritten;

  ADC_init_measurement();

  HAL_TIM_Base_Start_IT(&htim2); // 50 ms for logs

  SS_MS56_init(&ms5607, MS56_PRESS_4096, MS56_TEMP_4096);
  SS_MS56_read_convert(&ms5607);
  SS_MS56_set_ref_press(&ms5607);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			if(Log_trig_flag == 1)
			{
				HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);  // main loop led

				SS_MS56_read_convert(&ms5607);
				SS_MS56_get_altitude(&ms5607);
				ADC_save_result_2_buff();
				ADC_check_bat_voltage(); // Check if bat is lower than 3.2V
				PITOT_pull_I2C_data();

				Data_log_line_length_single = sprintf(Data_log_line_single, "%ld,%.2f,%ld,%ld,%ld,%f,%.2f\r\n",HAL_GetTick(),ADC_get_VBAT_mean(),ms5607.press,ms5607.altitude,ms5607.temp,PITOT_get_pressure_diff_psi(),PITOT_get_temp());
				HAL_UART_Transmit(&huart4,(uint8_t *)Data_log_line_single,Data_log_line_length_single,MAX_TIMEOUT);

				//------------->

				strcpy(&Data_log_line [Data_log_line_cnt][0],Data_log_line_single);
				Data_log_line_length [Data_log_line_cnt] = Data_log_line_length_single;

				Log_trig_flag = 0;

				if ( Data_log_line_cnt >= LOGS_BUFF_SIZE-1)
				{
					SD_CARD_WRITE_LED_ON;

					file_op_res = f_mount(&SDFatFS,SDPath, 1);
					SD_CARD_send_file_debug(file_op_res,MOUNT);

					file_op_res = f_open(&SDFile,SD_current_file_path, FA_WRITE |FA_OPEN_APPEND);
					SD_CARD_send_file_debug(file_op_res,OPEN);


					for(uint8_t line_num=0; line_num <= LOGS_BUFF_SIZE-1; line_num++)
					{
						file_op_res = f_write(&SDFile,&Data_log_line[line_num][0],Data_log_line_length[line_num],&BytesWritten);
						SD_CARD_send_file_debug(file_op_res,WRITE);
					}

					file_op_res = f_close(&SDFile);
					SD_CARD_send_file_debug(file_op_res,CLOSE);

					file_op_res = f_mount(0,SDPath,1);
					SD_CARD_send_file_debug(file_op_res,UNMOUNT);

					UART_send_debug_string ("\r\n");

					Data_log_line_cnt = 0;

					SD_CARD_WRITE_LED_OFF;
				}
				else
				{
					Data_log_line_cnt++;
				}
				//-------------<

			}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
