/*
 * SS_measurements.c
 *
 *  Created on: 05.02.2020
 *      Author: maciek
 */

#include "SS_measurements.h"
#include "string.h"
#include "SS_error.h"

Measurement *measurement_pointers[MAX_MEASUREMENT_COUNT];
volatile uint8_t last_measurement;
static float vcc;

void SS_measurements_chid_to_channel(uint8_t chid, uint8_t* reg_addr, uint8_t* reg_mask) {
	*reg_addr = chid / 8 + 3;
	*reg_mask = 1 << (chid % 8);
}

void SS_measurement_init(Measurement *measurement) {
    if(last_measurement >= MAX_MEASUREMENT_COUNT) {
        SS_error("Max %d measurements are supported");
        return;
    }
    measurement_pointers[last_measurement++] = measurement;
    uint8_t reg_addr, reg_mask;
    SS_measurements_chid_to_channel(measurement->channel_id, &reg_addr, &reg_mask);
    measurement->reg_address = reg_addr;
    measurement->reg_mask = reg_mask;
}

void SS_measurements_clear() {
	memset(measurement_pointers, 0, sizeof(measurement_pointers));
	last_measurement = 0;
}

void SS_measurements_start() {
	uint8_t muxdif = 0, muxsg0 = 0, muxsg1 = 0, sysred = 0;
	for(uint8_t i = 0; i < last_measurement; i++) {
		uint8_t mask = measurement_pointers[i]->reg_mask;
		switch(measurement_pointers[i]->reg_address) {
			case REG_ADDR_MUXDIF:
				muxdif |= mask;
				break;
			case REG_ADDR_MUXSG0:
				muxsg0 |= mask;
				break;
			case REG_ADDR_MUXSG1:
				muxsg1 |= mask;
				break;
			case REG_ADDR_SYSRED:
				sysred |= mask;
				break;
		}
	}
	SS_ADS1258_adcStartupRoutine(muxdif, muxsg0, muxsg1, sysred);
	SS_ADS1258_startMeasurements();
}

void SS_measurements_parse(ADS1258_Measurement* meas) {
	for(uint8_t i = 0; i < last_measurement; i++) {
		if(measurement_pointers[i]->channel_id == meas->channel) {
			measurement_pointers[i]->raw = meas->value;
		}
	}
}

float SS_measurements_read_VCC() {
    Measurement meas = {
            .channel_id = STATUS_CHID_VCC,
    };
    Measurement *tmp_pointers[MAX_MEASUREMENT_COUNT];
    memcpy(tmp_pointers, measurement_pointers, sizeof(tmp_pointers));
    SS_measurements_clear();
    SS_measurement_init(&meas);
	SS_measurements_start();
	SS_ADS1258_startConversions();
	HAL_Delay(10);
	for(uint8_t i = 0; i < 25; i++) {
        vcc += meas.raw / 786432.0f;
	    HAL_Delay(1);
	}
	vcc /= 25.0f;
    SS_measurements_clear();
    for(uint8_t i = 0; i < MAX_MEASUREMENT_COUNT; i++) {
        if(tmp_pointers[i] != NULL) {
            SS_measurement_init(tmp_pointers[i]);
        }
    }
    return vcc;
}

