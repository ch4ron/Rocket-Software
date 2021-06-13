/*
 * SS_MS5X07.h
 *
  *  Created on: 22.12.2017
 *      Author: Tomasz
 *
 *  Modified on: 26.12.2018
 *  	    By: PR
 *
 *  Modified - slimmed down, adjusted to FreeRTOS and adapted for multi baro handling.
 *          on: 15.09.2020
 *          By: PR
 *
 *  Information how to use this library is in SS_MS5X07.c file.
 */

#ifndef SS_MS5X07_H_
#define SS_MS5X07_H_

#include "spi.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"


/*	Check if it coincides with pointer to a SPI_HandleTypeDef structure that
 * 	contains the configuration information for proper SPI module. */
#define HSPI_MS56 hspi3

/* Press OSR (Oversampling Ratio) */
#define MS56_PRESS_256 0x40
#define MS56_PRESS_512 0x42
#define MS56_PRESS_1024 0x44
#define MS56_PRESS_2048 0x46
#define MS56_PRESS_4096 0x48

/* Temperature OSR (Oversampling Ratio) */
#define MS56_TEMP_256 0x50
#define MS56_TEMP_512 0x52
#define MS56_TEMP_1024 0x54
#define MS56_TEMP_2048 0x56
#define MS56_TEMP_4096 0x58

#define MS56_RESET 0x1E
#define MS56_ADC_READ 0x00
#define MS56_PROM_READ_BASE 0xA0

/* MAX and MIN values for calculate coefficients */
#define MS5607_dT_MAX       16777216
#define MS5607_dT_MIN      -16776960
#define MS5607_OFF_MAX      25769410560
#define MS5607_OFF_MIN     -17179344900
#define MS5607_SENS_MAX     12884705280
#define MS5607_SENS_MIN    -8589672450
#define MS5803_dT_MAX       16777216
#define MS5803_dT_MIN      -16776960
#define MS5803_OFF_MAX      12884705280
#define MS5803_OFF_MIN     -8589672450
#define MS5803_SENS_MAX     6442352640
#define MS5803_SENS_MIN    -4294836225

extern struct MS5X ms5607;
extern struct MS5X ms5803;

enum RESULT
{
	MS56_OK = 0,
	MS56_NO_COMMUNICATION = 1
};

/**
 * This struct encapsulate variables needed for each barometer.
 */
struct MS5X
{
    uint8_t baroType;       // 0 for MS5607, 1 for MS5803..
    uint16_t PROM[8];
    uint8_t tempOSR;
    uint8_t pressOSR;
    int32_t press;          // here is final press value
    int32_t temp;           // here is final temp value
    uint32_t uncomp_press;
    int32_t uncomp_temp;
    int32_t refPress;
    int32_t altitude;
    GPIO_TypeDef* CS_Port;
    uint16_t CS_Pin;
    enum RESULT result;
};

void SS_MS5X_readBarometersTask(void *pvParameters);
void SS_MS5X_calculateBarometersTask(void *pvParameters);
void SS_MS5X_init(struct MS5X *ms5X, uint8_t baroType, GPIO_TypeDef *Port, uint16_t Pin, uint8_t MS56_PRESS_mode, uint8_t MS56_TEMP_mode);
void SS_MS5X_CS_ENABLE(struct MS5X *ms5X);
void SS_MS5X_CS_DISABLE(struct MS5X *ms5X);
enum RESULT SS_MS5X_reset(struct MS5X *ms5X);
enum RESULT SS_MS5X_prom_read(struct MS5X *ms5X);
enum RESULT SS_MS5X_conversion_temp_start(struct MS5X *ms5X);
enum RESULT SS_MS5X_conversion_press_start(struct MS5X *ms5X);
enum RESULT SS_MS5X_adc_read(struct MS5X *ms5X, uint32_t *data);
void SS_MS5X_calculate(struct MS5X *ms5X);
void SS_MS5X_ms5607Calculate(struct MS5X *ms5X);
void SS_MS5X_ms5803Calculate(struct MS5X *ms5X);

#endif /* SS_MS5X07_H_ */
