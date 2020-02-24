/*
 * SS_measurements.c
 *
 *  Created on: 05.02.2020
 *      Author: maciek
 */

#include "SS_measurements.h"
#include "string.h"

Measurement measurements[MEASUREMENTS_NUM];
uint8_t last_measurement;
static float vcc;

void SS_measurements_chid_to_channel(uint8_t chid, uint8_t* reg_addr, uint8_t* reg_mask) {
	*reg_addr = chid / 8 + 3;
	*reg_mask = 1 << (chid % 8);
}

void SS_measurements_add(uint8_t chid) {
	if(last_measurement >= MEASUREMENTS_NUM) return;
	measurements[last_measurement].channel = chid;
	uint8_t reg_addr, reg_mask;
	SS_measurements_chid_to_channel(chid, &reg_addr, &reg_mask);
	measurements[last_measurement].reg_address = reg_addr;
	measurements[last_measurement++].reg_mask = reg_mask;
}

void SS_measurements_clear() {
	memset(measurements, 0, sizeof(measurements));
	last_measurement = 0;
}

void SS_measurements_start() {
	uint8_t muxdif = 0, muxsg0 = 0, muxsg1 = 0, sysred = 0;
	for(uint8_t i = 0; i < last_measurement; i++) {
		uint8_t mask = measurements[i].reg_mask;
		switch(measurements[i].reg_address) {
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

void SS_measurements_init() {
    SS_measurements_read_VCC();
	SS_measurements_start();
	SS_ADS1258_startConversions();
}

void SS_measurements_parse(ADS1258_Measurement* meas) {
	for(uint8_t i = 0; i < last_measurement; i++) {
		if(measurements[i].channel == meas->channel) {
			measurements[i].raw = meas->value;
		}
	}
}

float SS_measurements_read_VCC() {
    SS_measurements_clear();
    SS_measurements_add(STATUS_CHID_VCC);
	SS_measurements_start();
	SS_ADS1258_startConversions();
	HAL_Delay(10);
	for(uint8_t i = 0; i < 25; i++) {
        vcc += measurements[0].raw / 786432.0f;
	    HAL_Delay(1);
	}
	vcc /= 25;
    SS_measurements_clear();
    return vcc;
}

//void SS_measurements_read_json(char *json, jsmntok_t **tok) {
//    for(uint8_t i = 0; i < sizeof(servos)/sizeof(servos[0]); i++) {
//        int id, opened_pos, closed_pos;
//        JsonData data[] = {
//            {
//                .name = "id",
//                .type = JSON_INT,
//                .data = &id
//            },
//            {
//                .name = "closedPos",
//                .type = JSON_INT,
//                .data = &closed_pos
//            },
//            {
//                .name = "openedPos",
//                .type = JSON_INT,
//                .data = &opened_pos
//            },
//        };
//        SS_json_parse_data(data, sizeof(data)/sizeof(data[0]), json, tok[i]);
//        servos[id].opened_position = opened_pos;
//        servos[id].closed_position = closed_pos;
//        servos[id].tok = tok[i];
//    }
//}
