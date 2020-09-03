/*
 * SS_MS5607.h
 *
  *  Created on: 22.12.2017
 *      Author: Tomasz
 *
 *  Modified on: 26.12.2018
 *  	    By: PR
 *
 *  Information how to use this library is in SS_MS5607.c file.
 */

#ifndef SS_MS5607_H_
#define SS_MS5607_H_

#include "spi.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"


/*	Check if it coincides with pointer to a SPI_HandleTypeDef structure that
 * 	contains the configuration information for proper SPI module. */
#define HSPI_MS56 hspi3

#define MS56_PRESS_256 0x40
#define MS56_PRESS_512 0x42
#define MS56_PRESS_1024 0x44
#define MS56_PRESS_2048 0x46
#define MS56_PRESS_4096 0x48

#define MS56_TEMP_256 0x50
#define MS56_TEMP_512 0x52
#define MS56_TEMP_1024 0x54
#define MS56_TEMP_2048 0x56
#define MS56_TEMP_4096 0x58

#define MS56_RESET 0x1E
#define MS56_ADC_READ 0x00
#define MS56_PROM_READ_BASE 0xA0

extern struct MS5X ms5607;
extern struct MS5X ms5803;

enum RESULT
{
	MS56_OK = 0,
	MS56_NO_COMMUNICATION = 1
};

enum COMPRESSION_TYPE
{
	press = 0,
	temp = 1
};

struct MS5X
{
    uint8_t id;
    uint16_t PROM[8];
    uint8_t tempOSR;
    uint8_t pressOSR;
    uint8_t tempSens;
    uint8_t pressSens;
    int32_t press;
    int32_t average_press;
    int32_t average_temp;
    int32_t temp;
    uint32_t uncomp_press;
    int32_t uncomp_temp;
    int32_t refPress;
    int32_t altitude;
    enum RESULT result;
    uint8_t stage;
    GPIO_TypeDef* CS_Port;
    uint16_t CS_Pin;

/*Used in DMA mode*/
    uint8_t sequence_flag;
    enum COMPRESSION_TYPE comp_type;
    uint8_t uncomp_press_buff[3];
    uint8_t uncomp_temp_buff[3];
};


void SS_barometerTask();


void SS_MS5X_init(struct MS5X *ms5X, uint8_t id, GPIO_TypeDef *Port, uint16_t Pin, uint8_t MS56_PRESS_mode, uint8_t MS56_TEMP_mode);
void SS_MS56_CS_ENABLE(struct MS5X *ms5X);
void SS_MS56_CS_DISABLE(struct MS5X *ms5X);
enum RESULT SS_MS56_reset(struct MS5X *ms5X);
enum RESULT SS_MS56_prom_read(struct MS5X *ms5X);
enum RESULT SS_MS56_adc_read(struct MS5X *ms5X, uint32_t *data);
enum RESULT SS_MS56_conversion_press_start(struct MS5X *ms5X);
enum RESULT SS_MS56_conversion_temp_start(struct MS5X *ms5X);
// UNUSED ?
//uint8_t SS_MS56_check_wait_ready(void);
void SS_MS56_set_wait_it(uint8_t type);
void SS_MS56_wait(uint8_t type);
void SS_MS56_decrement_wait_ready(struct MS5X *ms5X);
void SS_MS56_read_convert(struct MS5X *ms5X);
void SS_MS56_read_convert_non_polling(struct MS5X *ms5X);

/* ***DMA MODE FUNCTIONS***  */
struct MS5X *SS_MS56_DMA_which_barometer();
void SS_MS56_DMA_read_convert(struct MS5X *ms5X);
void SS_MS56_DMA_read_convert_and_calculate(struct MS5X *ms5X);
void SS_MS56_DMA_conversion_press_start(struct MS5X *ms5X);
void SS_MS56_DMA_conversion_temp_start(struct MS5X *ms5X);
void SS_MS56_DMA_adc_read_TX(struct MS5X *ms5X);
void SS_MS56_DMA_adc_read_RX_press(struct MS5X *ms5X);
void SS_MS56_DMA_adc_read_RX_temp(struct MS5X *ms5X);
void SS_MS56_calculate_values(struct MS5X *ms5X);
uint8_t SS_MS56_DMA_wait(uint8_t press_or_temp_OSR);

/* ***CALLBACK FUNCTIONS***
 * call each of them in right HAL_Callback function (in SS_it.c file) */
void SS_MS5X_SYSTICK_Callback(void);
void SS_MS5X_HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void SS_MS5X_HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);

/* ***ADDITIONAL FUNCTIONS***  */
void SS_MS56_set_ref_press(struct MS5607 *ms5607);
int32_t SS_MS56_get_altitude(struct MS5607 *ms5607);
void SS_MS56_calculate_average_press(struct MS5607 * ms5607, uint8_t average_cnt);
void SS_MS56_calculate_average_temp(struct MS5607 * ms5607, uint8_t average_cnt);
void SS_MS56_calculate_average_temp_flash(struct MS5607 *ms5607, uint8_t average_cnt);
void SS_MS56_calculate_altitude_flash(struct MS5607 *ms5607);
void SS_MS56_calculate_average_press_flash(struct MS5607 *ms5607, uint8_t average_cnt);


#endif /* SS_MS5607_H_ */
