/*
 * SS_MS5607.c
 *
 *  Created on: 22.12.2017
 *      Author: Tomasz
 *
 * Modified on: 26.12.2018
 *  	    By: PR
 *
 *  How to use it:
 *  1. Configure SPI connection in STM32CubeMX.
 *  2. Enter 'MS56_CS' as user label for CPU's pin connected with MS5607's chip select pin (in CubeMX).
 *  3. #include "SS_MS5607.h" in main.c file, and add SS_MS5607.c and SS_MS5607.h to your project.
 *  4. Call SS_MS56_init function in init section of main function.
 *  5. Enter proper pointer to a SPI_HandleTypeDef structure that contains the configuration
 *     information for proper SPI module. It is placed at the beginning of SS_MS5607.h file.
 *  6. When want to operate in polling mode - just now you can use SS_MS56_read_convert function.
 *  7. However when non-polling mode should be used, then it is required to call SS_MS5X_SYSTICK_Callback()
 *     function in HAL_SYSTICK_Callback() or in any other interruption handler which calls every 1ms.
 *     Then instead of SS_MS56_read_convert use SS_MS56_read_convert_non_polling function.
 *
 *  How to use DMA mode:
 *  1. Configure SPI connection in STM32CubeMX and add DMA requests for chosen SPI. Do not forget about enabling
 *     global interrupts for both DMA streams.
 *  2. Do the same what in points 2,3,4,5.
 *  3. Call SS_MS5X_SYSTICK_Callback() function in HAL_SYSTICK_Callback() or in any other interruption handler
 *     which calls every 1ms. Then call SS_MS5X_HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
 *     and SS_MS5X_HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) in right HAL_SPI_Callback functions (in SS_it.c file).
 *  4. Use SS_MS56_DMA_read_convert with SS_MS56_calculate_values function by calling it in main loop of the program.
 *     Or just use SS_MS56_DMA_read_convert_and_calculate.
 *
 *  WARNING1: Maximum read frequency depends on chosen resolution. Every measurement needs 8.22, 4.13, 2.08, 1.06 or 0.54ms.
 *            For more information refer to MS5607 datasheet.
 *  WARNING2: Do not use both non_DMA and DMA mode at the same time.
 */

#include "SS_MS5X.h"

struct MS5X ms5607;
struct MS5X ms5803;
uint8_t MS5X_WAIT_READY;
uint8_t data_flash[8];


/**
  * @brief  This function
  * @param  ms5607: pointer to MS5607 struct.
  * @param  MS56_PRESS_mode: Its corresponding value of convert command. Just use predefined macro from SS_MS5607.h
  * @param  MS56_TEMP_mode: Its corresponding value of convert command. Just use predefined macro from SS_MS5607.h
  * @note   It has to be used at the beginning of main function in init section.
  * @retval None.
  */
void SS_barometerTask()
{
    uint8_t txbuff = 0;
    uint8_t rxbuff = 0;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS( 24));
        SS_MS56_DMA_read_convert_and_calculate(&ms5803);
        vTaskDelay(pdMS_TO_TICKS( 24));
        SS_MS56_DMA_read_convert_and_calculate(&ms5607);
//        vTaskDelay(pdMS_TO_TICKS( 2000));
//        HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
//
//        txbuff++;
//        HAL_GPIO_WritePin(MS56_CS_GPIO_Port, MS56_CS_Pin , GPIO_PIN_RESET);
//        HAL_SPI_TransmitReceive(&HSPI_MS56, &txbuff, &rxbuff, 1, 100);
//        HAL_GPIO_WritePin(MS56_CS_GPIO_Port, MS56_CS_Pin, GPIO_PIN_SET);
//        HAL_Delay(10);

    }
}

/**
  * @brief  This is initialization function for MS5607.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  MS56_PRESS_mode: Its corresponding value of convert command. Just use predefined macro from SS_MS5607.h
  * @param  MS56_TEMP_mode: Its corresponding value of convert command. Just use predefined macro from SS_MS5607.h
  * @note   It has to be used at the beginning of main function in init section.
  * @retval None.
  */
void SS_MS5X_init(struct MS5X *ms5X, uint8_t id, GPIO_TypeDef *Port, uint16_t Pin, uint8_t MS56_PRESS_mode, uint8_t MS56_TEMP_mode)
{
	SS_MS56_reset(ms5X);
        ms5X->id = id;
        ms5X->CS_Port = Port;
        ms5X->CS_Pin = Pin;
	ms5X->pressOSR = MS56_PRESS_mode;
	ms5X->tempOSR = MS56_TEMP_mode;
	ms5X->refPress = 101100;
	ms5X->sequence_flag = 0;
	SS_MS56_prom_read(ms5X);
}

/**
  * @brief  Function enable chip select pin on SPI.
  * @retval None
  */
void SS_MS56_CS_ENABLE(struct MS5X *ms5X)
{
    HAL_GPIO_WritePin(ms5X->CS_Port, ms5X->CS_Pin, RESET);
}

/**
  * @brief  Function disable chip select pin on SPI.
  * @retval None
  */
void SS_MS56_CS_DISABLE(struct MS5X *ms5X)
{
    HAL_GPIO_WritePin(ms5X->CS_Port, ms5X->CS_Pin, SET);
}

/**
  * @brief  Reset sequence shall be sent once after power-on to make sure that the calibration
  *  		PROM gets loaded into the internal register. It can be also used to reset the device
  *  		ROM from an unknown condition
  * @note   Reset function is included in init function.
  * @retval Condition of SPI communication.
  */
enum RESULT SS_MS56_reset(struct MS5X *ms5X)
{
	uint8_t buff = MS56_RESET;
	SS_MS56_CS_ENABLE(ms5X);
	HAL_SPI_TransmitReceive(&HSPI_MS56, &buff, &buff, 1, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY);
	SS_MS56_CS_DISABLE(ms5X);
	HAL_Delay(3);
	if(254 == buff)
		return 0;
	else
		return 1;
}

/**
  * @brief  The read command for PROM shall be executed once after reset by the user to read
  *  		the content of the calibration PROM and to calculate the calibration coefficients.
  * @note   Prom read function is included in init function.
  * @retval Condition of SPI communication.
  */
enum RESULT SS_MS56_prom_read(struct MS5X *ms5X)
{
	uint8_t buff = MS56_PROM_READ_BASE;
	uint8_t buffRX[2];
	for(uint8_t i=0; i<8; i++)
	{
		SS_MS56_CS_ENABLE(ms5X);
		HAL_SPI_Transmit(&HSPI_MS56, &buff, 1, 200);
		while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

		HAL_SPI_Receive(&HSPI_MS56, buffRX, 2, 200);
		while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_RX);
		if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
			return 1;
		SS_MS56_CS_DISABLE(ms5X);
		buff += 2;
		ms5X->PROM[i] = buffRX[1] + (uint16_t)(buffRX[0]<<8);
	}
	return 0;
}

/**
  * @brief  This function read from MS5607.
  * @param  data: pointer to read data container (use struct element ms5607.uncomp_press or
  *         ms5607_uncomp_temp).
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS56_adc_read(struct MS5X *ms5X, uint32_t *data)
{
	uint8_t buffTX = MS56_ADC_READ;
	uint8_t buffRX[3];
	SS_MS56_CS_ENABLE(ms5X);
	HAL_SPI_Transmit(&HSPI_MS56, &buffTX, 1, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

	HAL_SPI_Receive(&HSPI_MS56, buffRX, 3, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_RX);

	SS_MS56_CS_DISABLE(ms5X);
	*data = buffRX[2] + ((uint16_t)buffRX[1]<<8) + ((uint32_t)buffRX[0]<<16);
	if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
		return 1;
	else
		return 0;
}

/**
  * @brief  This function starts conversion of pressure.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS56_conversion_press_start(struct MS5X *ms5X)
{
	SS_MS56_CS_ENABLE(ms5X);
	HAL_SPI_Transmit(&HSPI_MS56, &ms5X->pressOSR, 1, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

	SS_MS56_CS_DISABLE(ms5X);
	if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
		return 1;
	else
		return 0;
}

/**
  * @brief  This function starts conversion of temperature.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS56_conversion_temp_start(struct MS5X *ms5X)
{
	SS_MS56_CS_ENABLE(ms5X);
	HAL_SPI_Transmit(&HSPI_MS56, &ms5X->tempOSR, 1, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

	SS_MS56_CS_DISABLE(ms5X);
	if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
		return 1;
	else
		return 0;
}

// UNUSED ?
///**
//  * @brief  This function checks if conversion is done.
//  * @param  type: .
//  * @retval If conversion is done returns 0, if not 1.
//  */
//uint8_t SS_MS56_check_wait_ready(void)
//{
//	if(MS5X_WAIT_READY == 0)
//		return 1;
//	else return 0;
//}

/**
  * @brief  This function chooses proper conversion time for MS5607.
  * @param  type: Corresponding value of convert command.
  * @retval None
  */
void SS_MS56_set_wait_it(uint8_t type)
{
	switch (type) {
		case MS56_PRESS_256:
			MS5X_WAIT_READY = 1+1;
			break;
		case MS56_PRESS_512:
			MS5X_WAIT_READY = 2+1;
			break;
		case MS56_PRESS_1024:
			MS5X_WAIT_READY = 3+1;
			break;
		case MS56_PRESS_2048:
			MS5X_WAIT_READY = 5+1;
			break;
		case MS56_PRESS_4096:
			MS5X_WAIT_READY = 9+1;
			break;
		case MS56_TEMP_256:
			MS5X_WAIT_READY = 1+1;
			break;
		case MS56_TEMP_512:
			MS5X_WAIT_READY = 2+1;
			break;
		case MS56_TEMP_1024:
			MS5X_WAIT_READY = 3+1;
			break;
		case MS56_TEMP_2048:
			MS5X_WAIT_READY = 5+1;
			break;
		case MS56_TEMP_4096:
			MS5X_WAIT_READY = 9+1;
			break;
	}
}

/**
  * @brief  This function waits for conversion on MS5607.
  * @param  type: Corresponding value of convert command.
  * @retval None
  */
void SS_MS56_wait(uint8_t type)
{
	switch (type) {
		case MS56_PRESS_256:
			HAL_Delay(1);
			break;
		case MS56_PRESS_512:
			HAL_Delay(2);
			break;
		case MS56_PRESS_1024:
			HAL_Delay(3);
			break;
		case MS56_PRESS_2048:
			HAL_Delay(5);
			break;
		case MS56_PRESS_4096:
			HAL_Delay(9);
			break;
		case MS56_TEMP_256:
			HAL_Delay(1);
			break;
		case MS56_TEMP_512:
			HAL_Delay(2);
			break;
		case MS56_TEMP_1024:
			HAL_Delay(3);
			break;
		case MS56_TEMP_2048:
			HAL_Delay(5);
			break;
		case MS56_TEMP_4096:
			HAL_Delay(9);
			break;
	}
}

/**
  * @brief  Decrement of MS5X_WAIT_READY counter.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_decrement_wait_ready(struct MS5X *ms5X)
{
	if(MS5X_WAIT_READY == 1)
	{
		ms5X->stage++;
		MS5X_WAIT_READY = 0;
	}
	if(MS5X_WAIT_READY>1)
	{
		MS5X_WAIT_READY--;
	}
}

/**
  * @brief  This function read values from MS5607, calculate and put final values
  *         in ms5607 struct.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_read_convert(struct MS5X *ms5X)
{
	int64_t dT;
	int64_t OFF, SENS;

	SS_MS56_conversion_press_start(ms5X);
	SS_MS56_wait(ms5X->pressOSR);
	SS_MS56_adc_read(ms5X, ms5X->uncomp_press);

	SS_MS56_conversion_temp_start(ms5X);
	SS_MS56_wait(ms5X->tempOSR);
	SS_MS56_adc_read(ms5X, ms5X->uncomp_temp);

	dT = ms5X->uncomp_temp - ((int32_t)ms5X->PROM[5]<<8);
	ms5X->temp = 2000 + (((int64_t)ms5X->PROM[6]*dT)>>23);
	OFF = ((int64_t)ms5X->PROM[2]<<17) + (((int64_t)ms5X->PROM[4]*dT)>>6);
	SENS = ((int64_t)ms5X->PROM[1]<<16) + ((int64_t)(ms5X->PROM[3]*dT)>>7);
	ms5X->press = ((((int64_t)ms5X->uncomp_press * SENS)>>21) - OFF)>>15;
}

/**
  * @brief  This function read values from MS5607, calculate and put final values
  *         in ms5607 struct. It is non-polling.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_read_convert_non_polling(struct MS5X *ms5X)
{
	static int64_t dT;
	static int64_t OFF, SENS;
	switch (ms5X->stage) {
		case 0:
			SS_MS56_conversion_press_start(ms5X);
			SS_MS56_set_wait_it(ms5X->pressOSR);
			ms5X->stage++;
			break;
		case 2:
			SS_MS56_adc_read(ms5X, ms5X->uncomp_press);
			SS_MS56_conversion_temp_start(ms5X);
			SS_MS56_set_wait_it(ms5X->tempOSR);
			ms5X->stage++;
			break;
		case 4:
			SS_MS56_adc_read(ms5X, ms5X->uncomp_temp);
			dT = ms5X->uncomp_temp - ((int32_t)ms5X->PROM[5]<<8);
			ms5X->temp = 2000 + (((int64_t)ms5X->PROM[6]*dT)>>23);
			OFF = ((int64_t)ms5X->PROM[2]<<17) + (((int64_t)ms5X->PROM[4]*dT)>>6);
			SENS = ((int64_t)ms5X->PROM[1]<<16) + ((int64_t)(ms5X->PROM[3]*dT)>>7);
			ms5X->press = ((((int64_t)ms5X->uncomp_press * SENS)>>21) - OFF)>>15;
			ms5X->stage = 0;
			break;
		default:
			break;
	}
}




/* ***DMA MODE FUNCTIONS***  */

uint8_t conversion_ongoing = 0;
uint8_t sequence_start = 0;
int8_t which_barometer = 0; // 0 - MS5607, 1 - MS5803


struct MS5X *SS_MS56_DMA_which_barometer()
{
    struct MS5X *ms5X;
    if(which_barometer == 0)
    {
        ms5X = &ms5607;
        return ms5X;
    }
    else if(which_barometer == 1)
    {
        ms5X = &ms5803;
        return ms5X;
    }
}

/**
  * @brief  Function starts conversion in DMA mode and when it finished uncompensated pressure
  * 		and uncompensated temperature are in buffers.
  * @Note1  It is necessary to use SS_MS56_calculate_values	function to calculate values
  * 		and put them into ms5607.press and ms5607temp.
  * @Note2  It takes about 21ms to get data from MS5607.
  * @retval None
  */
void SS_MS56_DMA_read_convert(struct MS5X *ms5X)
{
        if(ms5X->id == 0)
        {
            which_barometer = 0;
        }
        else if(ms5X->id == 1)
        {
            which_barometer = 1;
        }
	sequence_start = 1;
}

/**
  * @brief  Function starts conversion in DMA mode and calculate values of pressure and
  * 		temperature from actual (from previous measurement) data.
  * @Note   It takes about 21ms to get data from MS5607.
  * @retval None
  */
void SS_MS56_DMA_read_convert_and_calculate(struct MS5X *ms5X)
{
        SS_MS56_DMA_read_convert(ms5X);
	SS_MS56_calculate_values(ms5X);
}

/**
  * @brief  This function sends request to MS5607 to start press conversion.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_DMA_conversion_press_start(struct MS5X *ms5X)
{
	ms5X->sequence_flag = 1;
	SS_MS56_CS_ENABLE(ms5X);
	HAL_SPI_Transmit_DMA(&HSPI_MS56, &ms5X->pressOSR, 1);
}

/**
  * @brief  This function sends request to MS5607 to start temperature conversion.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_DMA_conversion_temp_start(struct MS5X *ms5X)
{
	ms5X->sequence_flag = 1;
	SS_MS56_CS_ENABLE(ms5X);
	HAL_SPI_Transmit_DMA(&HSPI_MS56, &ms5X->tempOSR, 1);
}

/**
  * @brief  This function sends request to MS5607 to prepare read mode.
  * @retval None
  */
void SS_MS56_DMA_adc_read_TX(struct MS5X *ms5X)
{
        uint8_t buffTX = MS56_ADC_READ;
        ms5X->sequence_flag = 3;
        SS_MS56_CS_ENABLE(ms5X);
        HAL_SPI_Transmit_DMA(&HSPI_MS56, &buffTX, 1);
}

/**
  * @brief  This function read uncompensated pressure from MS5607 and save it in press_buff.
  * @retval None
  */
void SS_MS56_DMA_adc_read_RX_press(struct MS5X *ms5X)
{
	HAL_SPI_Receive_DMA(&HSPI_MS56, ms5X->uncomp_press_buff , 3);
}

/**
  * @brief  This function read uncompensated pressure from MS5607 and save it in press_buff.
  * @retval None
  */
void SS_MS56_DMA_adc_read_RX_temp(struct MS5X *ms5X)
{
	HAL_SPI_Receive_DMA(&HSPI_MS56, ms5X->uncomp_temp_buff , 3);
}

/**
  * @brief  Calculation of values. Base on uncomp values and factory coefficients.
  * @retval None
  */
void SS_MS56_calculate_values(struct MS5X *ms5X)
{
	ms5X->uncomp_press = ms5X->uncomp_press_buff[2] + ((uint16_t)ms5X->uncomp_press_buff[1]<<8) + ((uint32_t)ms5X->uncomp_press_buff[0]<<16);
	ms5X->uncomp_temp = ms5X->uncomp_temp_buff[2] + ((uint16_t)ms5X->uncomp_temp_buff[1]<<8) + ((uint32_t)ms5X->uncomp_temp_buff[0]<<16);
	int64_t dT;
	int64_t OFF, SENS;
	dT = ms5X->uncomp_temp - ((int32_t)ms5X->PROM[5]<<8);
	ms5X->temp = 2000 + (((int64_t)ms5X->PROM[6]*dT)>>23);
	OFF = ((int64_t)ms5X->PROM[2]<<17) + (((int64_t)ms5X->PROM[4]*dT)>>6);
	SENS = ((int64_t)ms5X->PROM[1]<<16) + ((int64_t)(ms5X->PROM[3]*dT)>>7);
	ms5X->press = ((((int64_t)ms5X->uncomp_press * SENS)>>21) - OFF)>>15;
}

/**
  * @brief  This function chooses proper conversion time for MS5607.
  * @param  press_or_temp_OSR: Corresponding value of convert command.
  * @retval Conversion time
  */
uint8_t SS_MS56_DMA_wait(uint8_t press_or_temp_OSR)
{
	switch (press_or_temp_OSR) {
		case MS56_PRESS_256:
			return 1+1;
			break;
		case MS56_PRESS_512:
			return 2+1;
			break;
		case MS56_PRESS_1024:
			return 3+1;
			break;
		case MS56_PRESS_2048:
			return 5+1;
			break;
		case MS56_PRESS_4096:
			return 9+1;
			break;
	}
	return 0;
}

/**
  * @brief  This function handles actions when using non-polling or DMA mode.
  * @note   It has to be call every 1ms.
  * @retval None
  */
void SS_MS5X_SYSTICK_Callback(void)
{
    static uint8_t counter = 0;
    struct MS5X *ms5X;
    ms5X = SS_MS56_DMA_which_barometer();
    /* Wait for MS5X conversion.  */
    if(ms5X->sequence_flag == 2)
    {
            counter++;
            if(counter == SS_MS56_DMA_wait(ms5X->pressOSR))
            {
                    counter = 0;
                    SS_MS56_DMA_adc_read_TX(ms5X);
            }
    }
    /* This allows to automatically starts temperature conversion just after press conversion and leave
     * all previous interruptions.  */
    if(ms5X->sequence_flag == 4)
    {
            counter++;
            if(counter == 1)
            {
                    counter = 0;
                    ms5X->sequence_flag = 0;
                    SS_MS56_DMA_conversion_temp_start(ms5X);
            }
    }
    /* Check if there is sequence_start request and if there is conversion_ongoing flag.
     * Fulfilled condition causes start of DMA conversion sequence.  */
    if(sequence_start == 1 && conversion_ongoing == 0)
    {
        conversion_ongoing = 1;
        sequence_start = 0;
        SS_MS56_DMA_conversion_press_start(ms5X);
    }
    /* Wait for conversion. It works only when using SS_MS56_read_convert_non_polling function  */
    if(ms5X->stage == 1 || ms5X->stage == 3)
    {
            SS_MS56_decrement_wait_ready(ms5X);
    }
}

/**
  * @brief  This function handles actions after SPI transmission interrupt.
  * @note   It must depend on hardware interruption, so call it in
  *         HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) function
  *         (in SS_it.c file).
  * @retval None
  */
void SS_MS5X_HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &HSPI_MS56)
	{
            struct MS5X *ms5X;
            ms5X = SS_MS56_DMA_which_barometer();
		if((HAL_GPIO_ReadPin(ms5X->CS_Port, ms5X->CS_Pin) == GPIO_PIN_RESET) && ms5X->sequence_flag == 1)
		{
                    ms5X->sequence_flag = 2;
			SS_MS56_CS_DISABLE(ms5X);
		}
		else if((HAL_GPIO_ReadPin(ms5X->CS_Port, ms5X->CS_Pin) == GPIO_PIN_RESET) && ms5X->sequence_flag == 3)
		{
                        ms5X->sequence_flag = 0;
			if(ms5X->comp_type == press)
			{
                                ms5X->comp_type = temp;
				SS_MS56_DMA_adc_read_RX_press(ms5X);
			}
			else
			{
                                ms5X->comp_type = press;
				SS_MS56_DMA_adc_read_RX_temp(ms5X);
			}
		}

		/* Error report.  */
		if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
                    ms5X->result = 1;
		else
                    ms5X->result = 0;
	}

}

/**
  * @brief  This function handles actions after SPI transmission interrupt.
  * @note   It must depend on hardware interruption, so call it in
  *         HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) function
  *         (in SS_it.c file).
  * @retval None
  */
void SS_MS5X_HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &HSPI_MS56)
	{
                struct MS5X *ms5X;
                ms5X = SS_MS56_DMA_which_barometer();
		/* Check if its time to temp conversion or to end of conversion.  */
		if(ms5X->comp_type == temp)
		{
			SS_MS56_CS_DISABLE(ms5X);
                        ms5X->sequence_flag = 4;
		}
		else
		{
			SS_MS56_CS_DISABLE(ms5X);
			conversion_ongoing = 0;
		}

		/* Error report.  */
		if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
                    ms5X->result = 1;
		else
                    ms5X->result = 0;
	}
}


/* ***ADDITIONAL FUNCTIONS***  */

///**
//  * @brief  This function sets actual pressure as reference pressure.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @retval None
//  */
//void SS_MS56_set_ref_press(struct MS5607 *ms5607)
//{
//	ms5607->refPress= ms5607->press;
//}
//
///**
//  * @brief  This function calculate altitude. Altitude equal to 0 is at the
//  *         reference pressure level.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @note   It needs actual and reference pressure.
//  * @retval Actual altitude
//  */
//int32_t SS_MS56_get_altitude(struct MS5607 *ms5607)
//{
//	ms5607->altitude=44330*(1-pow(((double)ms5607->press/(double)ms5607->refPress),(1/5.255)));
//	return ms5607->altitude;
//}
//
///**
//  * @brief  This function calculate average press.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @param  average_cnt: Amount of measurement to averaging.
//  * @Note   It is adapted to use with SS_MS56_read_convert_non_polling function
//  * @retval None
//  */
//void SS_MS56_calculate_average_press(struct MS5607 * ms5607, uint8_t average_cnt)
//{
//	static int32_t sum = 0;
//	static uint8_t counter = 0;
//	if(!ms5607->stage)    //stage = 0 means new record accured
//	{
//		sum += ms5607->press;
//		counter++;
//	}
//	if(counter == average_cnt)
//	{
//		counter = 0;
//		ms5607->average_press = sum / average_cnt;
//		sum = 0;
//	}
//}
//
///**
//  * @brief  This function calculate average temperature.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @param  average_cnt: Amount of measurement to averaging.
//  * @Note   It is adapted to use with SS_MS56_read_convert_non_polling function
//  * @retval None
//  */
//void SS_MS56_calculate_average_temp(struct MS5607 * ms5607, uint8_t average_cnt)
//{
//	static int32_t sum = 0;
//	static uint8_t counter = 0;
//	if(!ms5607->stage)    //stage = 0 means new record accured
//	{
//		sum += ms5607->temp;
//		counter++;
//	}
//	if(counter == average_cnt)
//	{
//		counter = 0;
//		ms5607->average_temp = sum / average_cnt;
//		sum = 0;
//	}
//}
//
///**
//  * @brief  This function calculate average temperature and prepared it to be save on flash memory.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @param  average_cnt: Amount of measurement to averaging.
//  * @Note1  It is adapted to use with SS_MS56_read_convert_non_polling function
//  * @Note2  You have to implement your own flash functions.
//  * @retval None
//  */
//void SS_MS56_calculate_average_temp_flash(struct MS5607 *ms5607, uint8_t average_cnt)
//{
//	static int32_t sum = 0;
//	static uint8_t counter = 0;
//	if(!ms5607->stage)    //stage = 0 means new record accured
//	{
//		sum += ms5607->temp;
//		counter++;
//	}
//	if(counter == average_cnt)
//	{
//		counter = 0;
//		ms5607->average_temp = sum / average_cnt;
////		SS_s25fl_prepare_time_ms(data_flash, HAL_GetTick()-tick_start);
//		data_flash[4] = (uint8_t)(ms5607->average_temp>>24);
//		data_flash[5] = (uint8_t)(ms5607->average_temp>>16);
//		data_flash[6] = (uint8_t)(ms5607->average_temp>>8);
//		data_flash[7] = (uint8_t)(ms5607->average_temp);
////		SS_s25fl_write_data8(data_flash, MS56_TEMP);
//		sum = 0;
//	}
//}
//
///**
//  * @brief  This function calculate average altitude and prepared it to be save on flash memory.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @param  average_cnt: Amount of measurement to averaging.
//  * @Note1  It is adapted to use with SS_MS56_read_convert_non_polling function
//  * @Note2  You have to implement your own flash functions.
//  * @retval None
//  */
//void SS_MS56_calculate_altitude_flash(struct MS5607 *ms5607)
//{
//	if(!ms5607->stage)    //stage = 0 means new record accured
//	{
////		SS_s25fl_prepare_time_ms(data_flash, HAL_GetTick()-tick_start);
//		data_flash[4] = (uint8_t)(ms5607->altitude>>24);
//		data_flash[5] = (uint8_t)(ms5607->altitude>>16);
//		data_flash[6] = (uint8_t)(ms5607->altitude>>8);
//		data_flash[7] = (uint8_t)(ms5607->altitude);
////		SS_s25fl_write_data8(data_flash, MS56_ALTITUDE);
//	}
//
//}
//
///**
//  * @brief  This function calculate average pressure and prepared it to be save on flash memory.
//  * @param  ms5607: pointer to MS5607 struct.
//  * @param  average_cnt: Amount of measurement to averaging.
//  * @Note1  It is adapted to use with SS_MS56_read_convert_non_polling function
//  * @Note2  You have to implement your own flash functions.
//  * @retval None
//  */
//void SS_MS56_calculate_average_press_flash(struct MS5607 *ms5607, uint8_t average_cnt)
//{
//	static int32_t sum = 0;
//	static uint8_t counter = 0;
//	if(!ms5607->stage)    //stage = 0 means new record accured
//	{
//		sum += ms5607->press;
//		counter++;
//	}
//	if(counter == average_cnt)
//	{
//		counter = 0;
//		ms5607->average_press = sum / average_cnt;
////		SS_s25fl_prepare_time_ms(data_flash, HAL_GetTick()-tick_start);
//		data_flash[4] = (uint8_t)(ms5607->average_press>>24);
//		data_flash[5] = (uint8_t)(ms5607->average_press>>16);
//		data_flash[6] = (uint8_t)(ms5607->average_press>>8);
//		data_flash[7] = (uint8_t)(ms5607->average_press);
////		SS_s25fl_write_data8(data_flash, MS56_BARO);
//		sum = 0;
//	}
//}
