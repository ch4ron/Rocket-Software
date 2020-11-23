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
#define SS_MS56070_H_

#include "spi.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "math.h"

/*	Check if it coincides with pointer to a SPI_HandleTypeDef structure that
 * 	contains the configuration information for proper SPI module. */

//#define HSPI_MS56 hspi3


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

extern struct MS5607 ms5607;

enum RESULT {
    MS56_OK = 0,
    MS56_NO_COMMUNICATION = 1
};

enum COMPRESSION_TYPE {
    press = 0,
    temp = 1
};

struct MS5607 {
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
    SPI_HandleTypeDef *hspi;

/*Used in DMA mode*/
    uint8_t sequence_flag;
    enum COMPRESSION_TYPE comp_type;
    uint8_t uncomp_press_buff[3];
    uint8_t uncomp_temp_buff[3];
};

void SS_MS56_init(struct MS5607 *ms5607, uint8_t MS56_PRESS_mode, uint8_t MS56_TEMP_mode);
void SS_MS56_CS_ENABLE(void);
void SS_MS56_CS_DISABLE(void);
enum RESULT SS_MS56_reset(void);
enum RESULT SS_MS56_prom_read(struct MS5607 *ms5607);
enum RESULT SS_MS56_adc_read(uint32_t *data);
enum RESULT SS_MS56_convertion_press_start(struct MS5607 *ms5607);
enum RESULT SS_MS56_convertion_temp_start(struct MS5607 *ms5607);
uint8_t SS_MS56_check_wait_ready(void);
void SS_MS56_set_wait_it(uint8_t type);
void SS_MS56_wait(uint8_t type);
void SS_MS56_decrement_wait_ready(struct MS5607 *ms5607);
void SS_MS56_read_convert(struct MS5607 *ms5607);
void SS_MS56_read_convert_non_polling(struct MS5607 *ms5607);

/* ***DMA MODE FUNCTIONS***  */
void SS_MS56_DMA_read_convert(void);
void SS_MS56_DMA_read_convert_and_calculate(void);
void SS_MS56_DMA_convertion_press_start(struct MS5607 *ms5607);
void SS_MS56_DMA_convertion_temp_start(struct MS5607 *ms5607);
void SS_MS56_DMA_adc_read_TX(void);
void SS_MS56_DMA_adc_read_RX_press(void);
void SS_MS56_DMA_adc_read_RX_temp(void);
void SS_MS56_calculate_values(void);
uint8_t SS_MS56_DMA_wait(uint8_t press_or_temp_OSR);

/* ***ADDITIONAL FUNCTIONS***  */
void SS_MS56_set_ref_press(struct MS5607 *ms5607);
int32_t SS_MS56_get_altitude(struct MS5607 *ms5607);
void SS_MS56_calculate_average_press(struct MS5607 *ms5607, uint8_t average_cnt);
void SS_MS56_calculate_average_temp(struct MS5607 *ms5607, uint8_t average_cnt);
void SS_MS56_calculate_average_temp_flash(struct MS5607 *ms5607, uint8_t average_cnt);
void SS_MS56_calculate_altitude_flash(struct MS5607 *ms5607);
void SS_MS56_calculate_average_press_flash(struct MS5607 *ms5607, uint8_t average_cnt);
void SS_MS56_RxCpltCallback(SPI_HandleTypeDef *hspi);
void SS_MS56_TxCpltCallback(SPI_HandleTypeDef *hspi);
void SS_MS56_SYSTICK_Callback(void);

#endif /* SS_MS5607_H_ */