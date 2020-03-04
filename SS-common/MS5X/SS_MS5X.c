/* SS_MS5607.c
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
 *  6. Then just use SS_MS56_read_convert or SS_MS56_read_convert_non_polling function
 *     by calling it in main loop of the program.
 *
 *  How to use DMA mode:
 *  1. Configure SPI connection in STM32CubeMX and add DMA requests for chosen SPI. Do not forget about enabling
 *     global interrupts for both DMA streams.
 *  2. Do the same what in points 2,3,4,5.
 *  3. Use SS_MS56_DMA_read_convert with SS_MS56_calculate_values function by calling it in main loop of the program.
 *     Or just use SS_MS56_DMA_read_convert_and_calculate.
 *
 *  WARNING: Do not use both non_DMA and DMA mode at the same time.
 */

#include "SS_MS5X.h"
#ifdef SS_USE_S25FL
#include "SS_S25FL.h"
#endif

struct MS5607 ms5607;
uint8_t MS56_WAIT_READY = 0;
uint8_t data_flash[8];

/**
  * @brief  This is initialization function for MS5607.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  MS56_PRESS_mode: Its corresponding value of convert command. Just use predefined macro from SS_MS5607.h
  * @param  MS56_TEMP_mode: Its corresponding value of convert command. Just use predefined macro from SS_MS5607.h
  * @note   It has to be used at the beginning of main function in init section.
  * @retval None.
  */
void SS_MS56_init(struct MS5607 *ms5607, uint8_t MS56_PRESS_mode, uint8_t MS56_TEMP_mode) {
    SS_MS56_reset();
    ms5607->pressOSR = MS56_PRESS_mode;
    ms5607->tempOSR = MS56_TEMP_mode;
    ms5607->refPress = 101100;
    ms5607->sequence_flag = 0;
    SS_MS56_prom_read(ms5607);
}

/**
  * @brief  Function enable chip select pin on SPI.
  * @retval None
  */
void SS_MS56_CS_ENABLE(void) {
    HAL_GPIO_WritePin(MS56_CS_GPIO_Port, MS56_CS_Pin, RESET);
}

/**
  * @brief  Function disable chip select pin on SPI.
  * @retval None
  */
void SS_MS56_CS_DISABLE(void) {
    HAL_GPIO_WritePin(MS56_CS_GPIO_Port, MS56_CS_Pin, SET);
}

/**
  * @brief  Reset sequence shall be sent once after power-on to make sure that the calibration
  *  		PROM gets loaded into the internal register. It can be also used to reset the device
  *  		ROM from an unknown condition
  * @note   Reset function is included in init function.
  * @retval Condition of SPI communication.
  */
enum RESULT SS_MS56_reset(void) {
    uint8_t buff = MS56_RESET;
    SS_MS56_CS_ENABLE();
    HAL_SPI_TransmitReceive(&HSPI_MS56, &buff, &buff, 1, 200);
    while (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY);
    SS_MS56_CS_DISABLE();
    HAL_Delay(3);
    if (254 == buff)
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
enum RESULT SS_MS56_prom_read(struct MS5607 *ms5607) {
    uint8_t buff[3] = { MS56_PROM_READ_BASE, 0, 0 };
    uint8_t buffRX[2];
    for (uint8_t i = 0; i < 8; i++) {
        SS_MS56_CS_ENABLE();
        HAL_SPI_TransmitReceive(&HSPI_MS56, buff, buffRX, 3, 200);
        if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
            return 1;
        SS_MS56_CS_DISABLE();
        buff[0] += 2;
        ms5607->PROM[i] = buffRX[2] + (uint16_t) (buffRX[1] << 8);
    }
    return 0;
}

/**
  * @brief  This function read from MS5607.
  * @param  data: pointer to read data container (use struct element ms5607.uncomp_press or
  *         ms5607_uncomp_temp).
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS56_adc_read(uint32_t *data) {
    uint8_t buffTX = MS56_ADC_READ;
    uint8_t buffRX[3];
    SS_MS56_CS_ENABLE();
    HAL_SPI_Transmit(&HSPI_MS56, &buffTX, 1, 200);
    while (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

    HAL_SPI_Receive(&HSPI_MS56, buffRX, 3, 200);
    while (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_RX);

    SS_MS56_CS_DISABLE();
    *data = buffRX[2] + ((uint16_t) buffRX[1] << 8) + ((uint32_t) buffRX[0] << 16);
    if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
        return 1;
    else
        return 0;
}

/**
  * @brief  This function starts conversion of pressure.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS56_convertion_press_start(struct MS5607 *ms5607) {
    SS_MS56_CS_ENABLE();
    HAL_SPI_Transmit(&HSPI_MS56, &ms5607->pressOSR, 1, 200);
    while (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

    SS_MS56_CS_DISABLE();
    if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
        return 1;
    else
        return 0;
}

/**
  * @brief  This function starts conversion of temperature.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval Condition of SPI comunication.
  */
enum RESULT SS_MS56_convertion_temp_start(struct MS5607 *ms5607) {
    SS_MS56_CS_ENABLE();
    HAL_SPI_Transmit(&HSPI_MS56, &ms5607->tempOSR, 1, 200);
    while (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_BUSY_TX);

    SS_MS56_CS_DISABLE();
    if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
        return 1;
    else
        return 0;
}

/**
  * @brief  This function checks if conversion is done.
  * @param  type: .
  * @retval If conversion is done returns 0, if not 1.
  */
uint8_t SS_MS56_check_wait_ready(void) {
    if (MS56_WAIT_READY == 0)
        return 1;
    else return 0;
}

/**
  * @brief  This function chooses proper conversion time for MS5607.
  * @param  type: Corresponding value of convert command.
  * @retval None
  */
void SS_MS56_set_wait_it(uint8_t type) {
    switch (type) {
        case MS56_PRESS_256:
            MS56_WAIT_READY = 1 + 1;
            break;
        case MS56_PRESS_512:
            MS56_WAIT_READY = 2 + 1;
            break;
        case MS56_PRESS_1024:
            MS56_WAIT_READY = 3 + 1;
            break;
        case MS56_PRESS_2048:
            MS56_WAIT_READY = 5 + 1;
            break;
        case MS56_PRESS_4096:
            MS56_WAIT_READY = 9 + 1;
            break;
        case MS56_TEMP_256:
            MS56_WAIT_READY = 1 + 1;
            break;
        case MS56_TEMP_512:
            MS56_WAIT_READY = 2 + 1;
            break;
        case MS56_TEMP_1024:
            MS56_WAIT_READY = 3 + 1;
            break;
        case MS56_TEMP_2048:
            MS56_WAIT_READY = 5 + 1;
            break;
        case MS56_TEMP_4096:
            MS56_WAIT_READY = 9 + 1;
            break;
    }
}

/**
  * @brief  This function waits for conversion on MS5607.
  * @param  type: Corresponding value of convert command.
  * @retval None
  */
void SS_MS56_wait(uint8_t type) {
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
  * @brief  Decrement of MS56_WAIT_READY counter.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_decrement_wait_ready(struct MS5607 *ms5607) {
    if (MS56_WAIT_READY == 1) {
        ms5607->stage++;
        MS56_WAIT_READY = 0;
    }
    if (MS56_WAIT_READY > 1) {
        MS56_WAIT_READY--;
    }
}

/**
  * @brief  This function read values from MS5607, calculate and put final values
  *         in ms5607 struct.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_read_convert(struct MS5607 *ms5607) {
    int64_t dT;
    int64_t OFF, SENS;

    SS_MS56_convertion_press_start(ms5607);
    SS_MS56_wait(ms5607->pressOSR);
    SS_MS56_adc_read(&ms5607->uncomp_press);

    SS_MS56_convertion_temp_start(ms5607);
    SS_MS56_wait(ms5607->tempOSR);
    SS_MS56_adc_read(&ms5607->uncomp_temp);

    dT = ms5607->uncomp_temp - ((int32_t) ms5607->PROM[5] << 8);
    ms5607->temp = 2000 + (((int64_t) ms5607->PROM[6] * dT) >> 23);
    OFF = ((int64_t) ms5607->PROM[2] << 17) + (((int64_t) ms5607->PROM[4] * dT) >> 6);
    SENS = ((int64_t) ms5607->PROM[1] << 16) + ((int64_t) (ms5607->PROM[3] * dT) >> 7);
    ms5607->press = ((((int64_t) ms5607->uncomp_press * SENS) >> 21) - OFF) >> 15;
}

/**
  * @brief  This function read values from MS5607, calculate and put final values
  *         in ms5607 struct. It is non-polling.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_read_convert_non_polling(struct MS5607 *ms5607) {
    static int64_t dT;
    static int64_t OFF, SENS;
    switch (ms5607->stage) {
        case 0:
            SS_MS56_convertion_press_start(ms5607);
            SS_MS56_set_wait_it(ms5607->pressOSR);
            ms5607->stage++;
            break;
        case 2:
            SS_MS56_adc_read(&ms5607->uncomp_press);
            SS_MS56_convertion_temp_start(ms5607);
            SS_MS56_set_wait_it(ms5607->tempOSR);
            ms5607->stage++;
            break;
        case 4:
            SS_MS56_adc_read(&ms5607->uncomp_temp);
            dT = ms5607->uncomp_temp - ((int32_t) ms5607->PROM[5] << 8);
            ms5607->temp = 2000 + (((int64_t) ms5607->PROM[6] * dT) >> 23);
            OFF = ((int64_t) ms5607->PROM[2] << 17) + (((int64_t) ms5607->PROM[4] * dT) >> 6);
            SENS = ((int64_t) ms5607->PROM[1] << 16) + ((int64_t) (ms5607->PROM[3] * dT) >> 7);
            ms5607->press = ((((int64_t) ms5607->uncomp_press * SENS) >> 21) - OFF) >> 15;
            ms5607->stage = 0;
            break;
        default:
            break;
    }
}


/* ***DMA MODE FUNCTIONS***  */

uint8_t conversion_ongoing = 0;
uint8_t sequence_start = 0;

/**
  * @brief  Function starts conversion in DMA mode and when it finished uncompensated pressure
  * 		and uncompensated temperature are in buffers.
  * @Note1  It is necessary to use SS_MS56_calculate_values	function to calculate values
  * 		and put them into ms5607.press and ms5607temp.
  * @Note2  It takes about 21ms to get data from MS5607.
  * @retval None
  */
void SS_MS56_DMA_read_convert(void) {
    sequence_start = 1;
}

/**
  * @brief  Function starts conversion in DMA mode and calculate values of pressure and
  * 		temperature from actual (from previous measurement) data.
  * @Note   It takes about 21ms to get data from MS5607.
  * @retval None
  */
void SS_MS56_DMA_read_convert_and_calculate(void) {
    sequence_start = 1;
    SS_MS56_calculate_values();
}

/**
  * @brief  This function sends request to MS5607 to start press conversion.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_DMA_convertion_press_start(struct MS5607 *ms5607) {
    ms5607->sequence_flag = 1;
    SS_MS56_CS_ENABLE();
    HAL_SPI_Transmit_DMA(&HSPI_MS56, &ms5607->pressOSR, 1);
}

/**
  * @brief  This function sends request to MS5607 to start temperature conversion.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_DMA_convertion_temp_start(struct MS5607 *ms5607) {
    ms5607->sequence_flag = 1;
    SS_MS56_CS_ENABLE();
    HAL_SPI_Transmit_DMA(&HSPI_MS56, &ms5607->tempOSR, 1);
}

/**
  * @brief  This function sends request to MS5607 to prepare read mode.
  * @retval None
  */
void SS_MS56_DMA_adc_read_TX(void) {

    uint8_t buffTX = MS56_ADC_READ;
    ms5607.sequence_flag = 3;
    SS_MS56_CS_ENABLE();
    HAL_SPI_Transmit_DMA(&HSPI_MS56, &buffTX, 1);
}

/**
  * @brief  This function read uncompensated pressure from MS5607 and save it in press_buff.
  * @retval None
  */
void SS_MS56_DMA_adc_read_RX_press(void) {
    HAL_SPI_Receive_DMA(&HSPI_MS56, ms5607.uncomp_press_buff, 3);
}

/**
  * @brief  This function read uncompensated pressure from MS5607 and save it in press_buff.
  * @retval None
  */
void SS_MS56_DMA_adc_read_RX_temp(void) {
    HAL_SPI_Receive_DMA(&HSPI_MS56, ms5607.uncomp_temp_buff, 3);
}

/**
  * @brief  Calculation of values. Base on uncomp values and factory coefficients.
  * @retval None
  */
void SS_MS56_calculate_values(void) {
    ms5607.uncomp_press = ms5607.uncomp_press_buff[2] + ((uint16_t) ms5607.uncomp_press_buff[1] << 8) +
                          ((uint32_t) ms5607.uncomp_press_buff[0] << 16);
    ms5607.uncomp_temp = ms5607.uncomp_temp_buff[2] + ((uint16_t) ms5607.uncomp_temp_buff[1] << 8) +
                         ((uint32_t) ms5607.uncomp_temp_buff[0] << 16);
    int64_t dT;
    int64_t OFF, SENS;
    dT = ms5607.uncomp_temp - ((int32_t) ms5607.PROM[5] << 8);
    ms5607.temp = 2000 + (((int64_t) ms5607.PROM[6] * dT) >> 23);
    OFF = ((int64_t) ms5607.PROM[2] << 17) + (((int64_t) ms5607.PROM[4] * dT) >> 6);
    SENS = ((int64_t) ms5607.PROM[1] << 16) + ((int64_t) (ms5607.PROM[3] * dT) >> 7);
    ms5607.press = ((((int64_t) ms5607.uncomp_press * SENS) >> 21) - OFF) >> 15;
}

/**
  * @brief  This function chooses proper conversion time for MS5607.
  * @param  press_or_temp_OSR: Corresponding value of convert command.
  * @retval Conversion time
  */
uint8_t SS_MS56_DMA_wait(uint8_t press_or_temp_OSR) {
    switch (press_or_temp_OSR) {
        case MS56_PRESS_256:
            return 1 + 1;
            break;
        case MS56_PRESS_512:
            return 2 + 1;
            break;
        case MS56_PRESS_1024:
            return 3 + 1;
            break;
        case MS56_PRESS_2048:
            return 5 + 1;
            break;
        case MS56_PRESS_4096:
            return 9 + 1;
            break;
    }
    return 0;
}

/**
  * @brief  This function handles SysTick interrupt.
  * @retval None
  */
void SS_MS56_SYSTICK_Callback(void) {
    static uint8_t counter = 0;
    /* Wait for MS5607 conversion.  */
    if (ms5607.sequence_flag == 2) {
        counter++;
        if (counter >= SS_MS56_DMA_wait(ms5607.pressOSR)) {
            if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_READY) {
                counter = 0;
                SS_MS56_DMA_adc_read_TX();
            }
        }
    }
    /* This allows to automatically starts temperature conversion just after press conversion and leave
     * all previous interruptions.  */
    if (ms5607.sequence_flag == 4) {
        counter++;
        if (counter >= 1) {
            if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_READY) {
                counter = 0;
                ms5607.sequence_flag = 0;
                SS_MS56_DMA_convertion_temp_start(&ms5607);
            }
        }
    }
    /* Check if there is sequence_start request and if there is conversion_ongoing flag.
     * Fulfilled condition causes start of DMA conversion sequence.  */
    if (sequence_start == 1 && conversion_ongoing == 0) {

        if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_READY) {
            conversion_ongoing = 1;
            sequence_start = 0;
            SS_MS56_DMA_convertion_press_start(&ms5607);
        }
    }
    /* Wait for conversion. It works only when using SS_MS56_read_convert_non_polling function  */
    if (ms5607.stage == 1 || ms5607.stage == 3) {
        SS_MS56_decrement_wait_ready(&ms5607);
    }
}

/**
  * @brief  This function handles after SPI transmission interrupt.
  * @note   It is hardware interrupt.
  * @retval None
  */
void SS_MS56_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &HSPI_MS56) {
        if ((HAL_GPIO_ReadPin(MS56_CS_GPIO_Port, MS56_CS_Pin) == GPIO_PIN_RESET) && ms5607.sequence_flag == 1) {
            ms5607.sequence_flag = 2;
            SS_MS56_CS_DISABLE();
        } else if ((HAL_GPIO_ReadPin(MS56_CS_GPIO_Port, MS56_CS_Pin) == GPIO_PIN_RESET) && ms5607.sequence_flag == 3) {
            ms5607.sequence_flag = 0;
            if (ms5607.comp_type == press) {
                ms5607.comp_type = temp;
                SS_MS56_DMA_adc_read_RX_press();
            } else {
                ms5607.comp_type = press;
                SS_MS56_DMA_adc_read_RX_temp();
            }

        }

        /* Error report.  */
        if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
            ms5607.result = 1;
        else
            ms5607.result = 0;
    }

}

/**
  * @brief  This function handles after SPI receive interrupt.
  * @note   It is hardware interrupt.
  * @retval None
  */
void SS_MS56_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &HSPI_MS56) {
        /* Check if its time to temp conversion or to end of conversion.  */
        if (ms5607.comp_type == temp) {
            SS_MS56_CS_DISABLE();
            ms5607.sequence_flag = 4;
        } else {
            SS_MS56_CS_DISABLE();
            conversion_ongoing = 0;
#ifdef SS_USE_S25FL
            SS_S25FL_save_variable_u32(100, ms5607.press);
#endif
        }

        /* Error report.  */
        if (HAL_SPI_GetState(&HSPI_MS56) == HAL_SPI_STATE_ERROR)
            ms5607.result = 1;
        else
            ms5607.result = 0;
    }

}


/* ***ADDITIONAL FUNCTIONS***  */

/**
  * @brief  This function sets actual pressure as reference pressure.
  * @param  ms5607: pointer to MS5607 struct.
  * @retval None
  */
void SS_MS56_set_ref_press(struct MS5607 *ms5607) {
    ms5607->refPress = ms5607->press;
}

/**
  * @brief  This function calculate altitude. Altitude equal to 0 is at the
  *         reference pressure level.
  * @param  ms5607: pointer to MS5607 struct.
  * @note   It needs actual and reference pressure.
  * @retval Actual altitude
  */
int32_t SS_MS56_get_altitude(struct MS5607 *ms5607) {
    ms5607->altitude = 44330 * (1 - pow(((double) ms5607->press / (double) ms5607->refPress), (1 / 5.255)));
    return ms5607->altitude;
}

/**
  * @brief  This function calculate average press.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  average_cnt: Amount of measurement to averaging.
  * @Note   It is adapted to use with SS_MS56_read_convert_non_polling function
  * @retval None
  */
void SS_MS56_calculate_average_press(struct MS5607 *ms5607, uint8_t average_cnt) {
    static int32_t sum = 0;
    static uint8_t counter = 0;
    if (!ms5607->stage)    //stage = 0 means new record accured
    {
        sum += ms5607->press;
        counter++;
    }
    if (counter == average_cnt) {
        counter = 0;
        ms5607->average_press = sum / average_cnt;
        sum = 0;
    }
}

/**
  * @brief  This function calculate average temperature.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  average_cnt: Amount of measurement to averaging.
  * @Note   It is adapted to use with SS_MS56_read_convert_non_polling function
  * @retval None
  */
void SS_MS56_calculate_average_temp(struct MS5607 *ms5607, uint8_t average_cnt) {
    static int32_t sum = 0;
    static uint8_t counter = 0;
    if (!ms5607->stage)    //stage = 0 means new record accured
    {
        sum += ms5607->temp;
        counter++;
    }
    if (counter == average_cnt) {
        counter = 0;
        ms5607->average_temp = sum / average_cnt;
        sum = 0;
    }
}

/**
  * @brief  This function calculate average temperature and prepared it to be save on flash memory.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  average_cnt: Amount of measurement to averaging.
  * @Note1  It is adapted to use with SS_MS56_read_convert_non_polling function
  * @Note2  You have to implement your own flash functions.
  * @retval None
  */
void SS_MS56_calculate_average_temp_flash(struct MS5607 *ms5607, uint8_t average_cnt) {
    static int32_t sum = 0;
    static uint8_t counter = 0;
    if (!ms5607->stage)    //stage = 0 means new record accured
    {
        sum += ms5607->temp;
        counter++;
    }
    if (counter == average_cnt) {
        counter = 0;
        ms5607->average_temp = sum / average_cnt;
//		SS_s25fl_prepare_time_ms(data_flash, HAL_GetTick()-tick_start);
        data_flash[4] = (uint8_t) (ms5607->average_temp >> 24);
        data_flash[5] = (uint8_t) (ms5607->average_temp >> 16);
        data_flash[6] = (uint8_t) (ms5607->average_temp >> 8);
        data_flash[7] = (uint8_t) (ms5607->average_temp);
//		SS_s25fl_write_data8(data_flash, MS56_TEMP);
        sum = 0;
    }
}

/**
  * @brief  This function calculate average altitude and prepared it to be save on flash memory.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  average_cnt: Amount of measurement to averaging.
  * @Note1  It is adapted to use with SS_MS56_read_convert_non_polling function
  * @Note2  You have to implement your own flash functions.
  * @retval None
  */
void SS_MS56_calculate_altitude_flash(struct MS5607 *ms5607) {
    if (!ms5607->stage)    //stage = 0 means new record accured
    {
//		SS_s25fl_prepare_time_ms(data_flash, HAL_GetTick()-tick_start);
        data_flash[4] = (uint8_t) (ms5607->altitude >> 24);
        data_flash[5] = (uint8_t) (ms5607->altitude >> 16);
        data_flash[6] = (uint8_t) (ms5607->altitude >> 8);
        data_flash[7] = (uint8_t) (ms5607->altitude);
//		SS_s25fl_write_data8(data_flash, MS56_ALTITUDE);
    }

}

/**
  * @brief  This function calculate average pressure and prepared it to be save on flash memory.
  * @param  ms5607: pointer to MS5607 struct.
  * @param  average_cnt: Amount of measurement to averaging.
  * @Note1  It is adapted to use with SS_MS56_read_convert_non_polling function
  * @Note2  You have to implement your own flash functions.
  * @retval None
  */
void SS_MS56_calculate_average_press_flash(struct MS5607 *ms5607, uint8_t average_cnt) {
    static int32_t sum = 0;
    static uint8_t counter = 0;
    if (!ms5607->stage)    //stage = 0 means new record accured
    {
        sum += ms5607->press;
        counter++;
    }
    if (counter == average_cnt) {
        counter = 0;
        ms5607->average_press = sum / average_cnt;
//		SS_s25fl_prepare_time_ms(data_flash, HAL_GetTick()-tick_start);
        data_flash[4] = (uint8_t) (ms5607->average_press >> 24);
        data_flash[5] = (uint8_t) (ms5607->average_press >> 16);
        data_flash[6] = (uint8_t) (ms5607->average_press >> 8);
        data_flash[7] = (uint8_t) (ms5607->average_press);
//		SS_s25fl_write_data8(data_flash, MS56_BARO);
        sum = 0;
    }
}