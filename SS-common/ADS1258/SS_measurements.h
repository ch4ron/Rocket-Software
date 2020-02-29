/*
 * SS_measurements.h
 *
 *  Created on: 05.02.2020
 *      Author: maciek
 */

#ifndef SS_MEASUREMENTS_H_
#define SS_MEASUREMENTS_H_

#include "SS_ADS1258.h"
#include "stm32f4xx_hal.h"

#define MAX_MEASUREMENT_COUNT 9

typedef struct {
    uint8_t channel_id;
	uint8_t reg_address;
	uint8_t reg_mask;
	int32_t raw;
	float scaled;
} Measurement;

void SS_measurement_init(Measurement *measurement);
void SS_measurements_clear();
void SS_measurements_start();
void SS_measurements_parse(ADS1258_Measurement* meas);
float SS_measurements_read_VCC();

#endif /* SS_MEASUREMENTS_H_ */
