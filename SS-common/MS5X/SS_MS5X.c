/*
 * SS_MS5X07.c
 *
 *  Created on: 22.12.2017
 *      Author: Tomasz
 *
 * Modified on: 26.12.2018
 *  	    By: PR
 *
 * Modified - slimmed down, adjusted to FreeRTOS and adapted for multi baro handling.
 *          on: 15.09.2020
 *          By: PR
 *
 *      //TODO: chceck how system works in real different temperatures
 *
 *  How to use it:
 *  1. Configure SPI connection in STM32CubeMX.
 *  2. Enter 'MS56_CS' and 'MS58_CS' as user label for CPU's pins connected with MS5607's and MS5803's chip select pins (in CubeMX).
 *  3. #include "SS_MS5X.h" where it be use, and add SS_MS5X.c and SS_MS5X.h to your project.
 *  4. Call SS_MS5X_init function in init section of main function.
 *  5. Enter proper pointer to a SPI_HandleTypeDef structure that contains the configuration
 *     information for proper SPI module. It is placed at the beginning of SS_MS5X07.h file.
 *  6. Create two FreeRTOS tasks, one with SS_MS5X_readBarometersTask function, and second one with
 *     SS_MS5X_calculateBarometersTask function. Thats it.
 *
 *  WARNING: Maximum read frequency depends on chosen resolution. Every measurement needs 8.22, 4.13, 2.08, 1.06 or 0.54ms.
 *           For more information refer to MS5x datasheet. Adjust your choosen frequency by
 */

#include "SS_MS5X.h"

struct MS5X ms5607;
struct MS5X ms5803;

/**
  * @brief It is function for FreeRTOS task handling barometers readings.
  * @retval None.
  */
void SS_MS5X_readBarometersTask(void *pvParameters)
{
    while(1)
    {
        SS_MS5X_conversion_temp_start(&ms5607);
        SS_MS5X_conversion_temp_start(&ms5803);
        vTaskDelay(pdMS_TO_TICKS(9));
        SS_MS5X_adc_read(&ms5607, &ms5607.uncomp_temp);
        SS_MS5X_adc_read(&ms5803, &ms5803.uncomp_temp);
        SS_MS5X_conversion_press_start(&ms5607);
        SS_MS5X_conversion_press_start(&ms5803);
        vTaskDelay(pdMS_TO_TICKS(9));
        SS_MS5X_adc_read(&ms5607, &ms5607.uncomp_press);
        SS_MS5X_adc_read(&ms5803, &ms5803.uncomp_press);
    }
    vTaskDelete( NULL );
}

/**
  * @brief It is function for FreeRTOS task handling barometers calculations.
  * @param
  * @retval None.
  */
void SS_MS5X_calculateBarometersTask(void *pvParameters)
{
    while(1)
    {
        SS_MS5X_calculate(&ms5607);
        SS_MS5X_calculate(&ms5803);
        vTaskDelay(pdMS_TO_TICKS(9));
    }
    vTaskDelete( NULL );
}

/**
  * @brief  This is initialization function for MS5x.
  * @param  *ms5X: Pointer to MS5X struct.
  * @param  baroType: Barometer type 0 for MS5607, 1 for MS5803.
  * @param  *Port: GPIO_TypeDef of port within CS pin is placed.
  * @param  Pin: Specifies the port bit of CS pin.
  * @param  MS56_PRESS_mode: Its OSR. Its corresponding value of convert command. Just use predefined macro from SS_MS5X07.h
  * @param  MS56_TEMP_mode: Its OSR. Its corresponding value of convert command. Just use predefined macro from SS_MS5X07.h
  * @note   It has to be used at the beginning of main function in init section for each baro.
  * @retval None.
  */
void SS_MS5X_init(struct MS5X *ms5X, uint8_t baroType, GPIO_TypeDef *Port, uint16_t Pin, uint8_t MS56_PRESS_mode, uint8_t MS56_TEMP_mode)
{
	ms5X->result = SS_MS5X_reset(ms5X);
        ms5X->baroType = baroType;
        ms5X->CS_Port = Port;
        ms5X->CS_Pin = Pin;
	ms5X->pressOSR = MS56_PRESS_mode;
	ms5X->tempOSR = MS56_TEMP_mode;
	ms5X->refPress = 101100;
        HAL_Delay(10);
        if(ms5X->result != 1)
        {
            ms5X->result = SS_MS5X_prom_read(ms5X);
        }
}

/**
  * @brief  Function to enable chip select pin on SPI.
  * @retval None
  */
void SS_MS5X_CS_ENABLE(struct MS5X *ms5X)
{
    HAL_GPIO_WritePin(ms5X->CS_Port, ms5X->CS_Pin, RESET);
}

/**
  * @brief  Function to disable chip select pin on SPI.
  * @retval None
  */
void SS_MS5X_CS_DISABLE(struct MS5X *ms5X)
{
    HAL_GPIO_WritePin(ms5X->CS_Port, ms5X->CS_Pin, SET);
}

/**
  * @brief  Reset sequence shall be sent once after power-on to make sure that the calibration
  *  		PROM gets loaded into the internal register. It can be also used to reset the device
  *  		ROM from an unknown condition
  * @param  *ms5X: Pointer to MS5X struct.
  * @note   Reset function is included in init function.
  * @retval Condition of SPI communication.
  */
enum RESULT SS_MS5X_reset(struct MS5X *ms5X)
{
	uint8_t buff = MS56_RESET;
	SS_MS5X_CS_ENABLE(ms5X);
	HAL_SPI_TransmitReceive(&HSPI_MS56, &buff, &buff, 1, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY);
	SS_MS5X_CS_DISABLE(ms5X);
	HAL_Delay(3);
        return 0;
}

/**
  * @brief  The read command for PROM shall be executed once after reset by the user to read
  *  		the content of the calibration PROM and to calculate the calibration coefficients.
  * @param  *ms5X: Pointer to MS5X struct.
  * @note   Prom read function is included in init function.
  * @retval Condition of SPI communication.
  */
enum RESULT SS_MS5X_prom_read(struct MS5X *ms5X)
{
	uint8_t buff = MS56_PROM_READ_BASE;
	uint8_t buffRX[2];
	for(uint8_t i=0; i<8; i++)
	{
            SS_MS5X_CS_ENABLE(ms5X);
            HAL_SPI_Transmit(&HSPI_MS56, &buff, 1, 200);
            while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);
            HAL_Delay(3);
            HAL_SPI_Receive(&HSPI_MS56, buffRX, 2, 200);
            while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_RX);
            if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
                    return 1;
            SS_MS5X_CS_DISABLE(ms5X);
            buff += 2;
	    ms5X->PROM[i] = buffRX[1] + (uint16_t)(buffRX[0]<<8);
	}
	return 0;
}

/**
  * @brief  This function starts conversion of temperature.
  * @param  *ms5X: Pointer to MS5X struct.
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS5X_conversion_temp_start(struct MS5X *ms5X)
{
    SS_MS5X_CS_ENABLE(ms5X);
    HAL_SPI_Transmit(&HSPI_MS56, &ms5X->tempOSR, 1, 200);
    while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

    SS_MS5X_CS_DISABLE(ms5X);
    if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
        return 1;
    else
        return 0;
}

/**
  * @brief  This function starts conversion of pressure.
  * @param  *ms5X: Pointer to MS5X struct.
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS5X_conversion_press_start(struct MS5X *ms5X)
{
    SS_MS5X_CS_ENABLE(ms5X);
    HAL_SPI_Transmit(&HSPI_MS56, &ms5X->pressOSR, 1, 200);
    while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

    SS_MS5X_CS_DISABLE(ms5X);
    if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
        return 1;
    else
        return 0;
}

/**
  * @brief  This function read from MS5x.
  * @param  *ms5X: Pointer to MS5X struct.
  * @param  data: pointer to read data container (use struct element ms5X.uncomp_press or
  *         ms5607_uncomp_temp).
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS5X_adc_read(struct MS5X *ms5X, uint32_t *data)
{
	uint8_t buffTX = MS56_ADC_READ;
	uint8_t buffRX[3];
	SS_MS5X_CS_ENABLE(ms5X);
	HAL_SPI_Transmit(&HSPI_MS56, &buffTX, 1, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

	HAL_SPI_Receive(&HSPI_MS56, buffRX, 3, 200);
	while(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_RX);

	SS_MS5X_CS_DISABLE(ms5X);
	*data = buffRX[2] + ((uint16_t)buffRX[1]<<8) + ((uint32_t)buffRX[0]<<16);
	if(HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
		return 1;
	else
		return 0;
}

/**
  * @brief  This function calculate final measured values.
  * @param  *ms5X: Pointer to MS5X struct.
  * @retval None
  */
void SS_MS5X_calculate(struct MS5X *ms5X)
{
    if(ms5X->baroType == 0)         // MS5607 type
    {
        SS_MS5X_ms5607Calculate(ms5X);
    }
    else if(ms5X->baroType == 1)    // MS5803 type
    {
        SS_MS5X_ms5803Calculate(ms5X);
    }
}

/**
  * @brief  This function calculate final measured values of MS5607 baro.
  * @param  *ms5X: Pointer to MS5X struct.
  * @retval None
  */
void SS_MS5X_ms5607Calculate(struct MS5X *ms5X)
{
    int64_t dT, TEMP, T2, OFF, OFF2, SENS, SENS2;
    dT = ms5X->uncomp_temp - ((int32_t)ms5X->PROM[5]<<8);
    if(dT > MS5607_dT_MAX)
        dT = MS5607_dT_MAX;
    else if(dT < MS5607_dT_MIN)
        dT = -MS5607_dT_MIN;
    TEMP = 2000 + (((int64_t)ms5X->PROM[6]*dT)>>23);
    OFF = ((int64_t)ms5X->PROM[2]<<17) + (((int64_t)ms5X->PROM[4]*dT)>>6);
    if(OFF > MS5607_OFF_MAX)
        OFF = MS5607_OFF_MAX;
    else if(OFF < MS5607_OFF_MIN)
        OFF = MS5607_OFF_MIN;
    SENS = ((int64_t)ms5X->PROM[1]<<16) + ((int64_t)(ms5X->PROM[3]*dT)>>7);
    if(SENS > MS5607_SENS_MAX)
        SENS = MS5607_SENS_MAX;
    else if(SENS < MS5607_SENS_MIN)
        SENS = MS5607_SENS_MIN;

    if(TEMP < 2000)
    {
        T2 = (dT*dT)>>31;
        OFF2 = (61 * (TEMP-2000) * (TEMP-2000))>>4;
        SENS2 = 2 * (TEMP-2000) * (TEMP-2000);
        if(TEMP < -1500)
        {
            OFF2 = OFF2 + 15 * (TEMP+1500) * (TEMP+1500);
            SENS2 = SENS2 + 8 * (TEMP+1500) * (TEMP+1500);
        }
    }
    else
    {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }
    TEMP = TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;
    ms5X->temp = TEMP;
    ms5X->press = ((((int64_t)ms5X->uncomp_press * SENS)>>21) - OFF)>>15;
}

/**
  * @brief  This function calculate final measured values of MS5803 baro.
  * @param  *ms5X: Pointer to MS5X struct.
  * @retval None
  */
void SS_MS5X_ms5803Calculate(struct MS5X *ms5X)
{
    int64_t dT, TEMP, T2, OFF, OFF2, SENS, SENS2;
    dT = ms5X->uncomp_temp - ((int32_t)ms5X->PROM[5]<<8);
    if(dT > MS5803_dT_MAX)
        dT = MS5803_dT_MAX;
    else if(dT < MS5803_dT_MIN)
        dT = -MS5803_dT_MIN;
    TEMP = 2000 + (((int64_t)ms5X->PROM[6]*dT)>>23);
    OFF = ((int64_t)ms5X->PROM[2]<<16) + (((int64_t)ms5X->PROM[4]*dT)>>7);
    if(OFF > MS5803_OFF_MAX)
        OFF = MS5803_OFF_MAX;
    else if(OFF < MS5803_OFF_MIN)
        OFF = MS5803_OFF_MIN;
    SENS = ((int64_t)ms5X->PROM[1]<<15) + ((int64_t)(ms5X->PROM[3]*dT)>>8);
    if(SENS > MS5803_SENS_MAX)
        SENS = MS5803_SENS_MAX;
    else if(SENS < MS5803_SENS_MIN)
        SENS = MS5803_SENS_MIN;

    if(TEMP < 2000)
    {
        T2 = (dT*dT)>>31;
        OFF2 = 3 * (TEMP-2000) * (TEMP-2000);
        SENS2 = (7 * (TEMP-2000) * (TEMP-2000))>>3;
        if(TEMP < -1500)
        {
            SENS2 = SENS2 + 2 * (TEMP+1500) * (TEMP+1500);
        }
    }
    else
    {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
        if(TEMP > 4500)
        {
            SENS2 = SENS2 - ((TEMP-4500) * (TEMP-4500))>>3;
        }
    }
    TEMP = TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;
    ms5X->temp = TEMP;
    ms5X->press = ((((int64_t)ms5X->uncomp_press * SENS)>>21) - OFF)>>15;
}