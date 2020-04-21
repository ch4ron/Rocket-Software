/*
 * SS_measurements.c
 *
 *  Created on: 05.02.2020
 *      Author: maciek
 */

#include <com/SS_com.h>
#include <grazyna/SS_grazyna.h>
#include <com/SS_com_debug.h>
#include "SS_measurements.h"
#include "string.h"

#define ADS1258_PREVIEW_VALUES_PERIOD 100

Measurement *measurement_pointers[MAX_MEASUREMENT_COUNT];
static float vref;
static uint8_t measurements_count;

void SS_ADS1258_measurements_chid_to_channel(uint8_t chid, uint8_t *reg_addr, uint8_t *reg_mask) {
    *reg_addr = chid / 8 + 3;
    *reg_mask = 1 << (chid % 8);
}

void SS_ADS1258_measurement_init(Measurement *measurement) {
    measurement_pointers[measurement->channel_id] = measurement;
    uint8_t reg_addr, reg_mask;
    SS_ADS1258_measurements_chid_to_channel(measurement->channel_id, &reg_addr, &reg_mask);
    measurement->reg_address = reg_addr;
    measurement->reg_mask = reg_mask;
    measurements_count++;
}

void SS_ADS1258_measurements_init(Measurement *measurements, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        SS_ADS1258_measurement_init(&measurements[i]);
    }
}

void SS_ADS1258_measurements_clear() {
    memset(measurement_pointers, 0, sizeof(measurement_pointers));
    measurements_count = 0;
}

void SS_ADS1258_measurements_start() {
    uint8_t muxdif = 0, muxsg0 = 0, muxsg1 = 0, sysred = 0;
    for (uint8_t i = 0; i < MAX_MEASUREMENT_COUNT; i++) {
        if (measurement_pointers[i] != NULL) {
            uint8_t mask = measurement_pointers[i]->reg_mask;
            switch (measurement_pointers[i]->reg_address) {
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
    }
    SS_ADS1258_adcStartupRoutine(muxdif, muxsg0, muxsg1, sysred);
    SS_ADS1258_startMeasurements();
}

void SS_ADS1258_measurements_parse(ADS1258_Measurement *meas) {
    if (measurement_pointers[meas->channel] == NULL) {
        /* TODO fix tests */
//        SS_error("Measurement channel: %u not initialized", meas->channel);
        return;
    }
    measurement_pointers[meas->channel]->raw = meas->value;
}

float SS_ADS1258_measurements_read_VCC() {
    Measurement meas = {
            .channel_id = STATUS_CHID_VCC,
    };
    Measurement *tmp_pointers[MAX_MEASUREMENT_COUNT];
    memcpy(tmp_pointers, measurement_pointers, sizeof(tmp_pointers));
    SS_ADS1258_measurements_clear();
    SS_ADS1258_measurement_init(&meas);
    SS_ADS1258_measurements_start();
    SS_ADS1258_startConversions();
    HAL_Delay(10);
    float vcc = 0.0f;
    for (uint8_t i = 0; i < 25; i++) {
        vcc += meas.raw / 786432.0f;
        HAL_Delay(1);
    }
    vcc /= 25.0f;
    SS_ADS1258_measurements_clear();
    for (uint8_t i = 0; i < MAX_MEASUREMENT_COUNT; i++) {
        if (tmp_pointers[i] != NULL) {
            SS_ADS1258_measurement_init(tmp_pointers[i]);
        }
    }
    return vcc;
}

float SS_ADS1258_measurements_read_VREF() {
    Measurement meas = {
            .channel_id = STATUS_CHID_REF,
    };
    Measurement *tmp_pointers[MAX_MEASUREMENT_COUNT];
    memcpy(tmp_pointers, measurement_pointers, sizeof(tmp_pointers));
    SS_ADS1258_measurements_clear();
    SS_ADS1258_measurement_init(&meas);
    SS_ADS1258_measurements_start();
    SS_ADS1258_startConversions();
    HAL_Delay(10);
    for (uint8_t i = 0; i < 25; i++) {
        vref += meas.raw / 786432.0f;
        HAL_Delay(1);
    }
    vref /= 25.0f;
    SS_ADS1258_measurements_clear();
    for (uint8_t i = 0; i < MAX_MEASUREMENT_COUNT; i++) {
        if (tmp_pointers[i] != NULL) {
            SS_ADS1258_measurement_init(tmp_pointers[i]);
        }
    }
    return vref;
}

static float SS_ADS1258_calculate_voltage(Measurement *measurement) {
    return ((float) measurement->raw) / 0x780000 * vref;

}

static float SS_ADS1258_calculate_scaled(Measurement *meas) {
    float voltage = SS_ADS1258_calculate_voltage(meas);
    meas->scaled = voltage * meas->a_coefficient + meas->b_coefficient;
    return meas->scaled;
}

static void SS_ADS1258_measurement_feed(Measurement *meas, ComFrame *frame) {
    SS_ADS1258_calculate_scaled(meas);
    frame->priority = COM_LOW_PRIORITY;
    frame->action = COM_FEED;
    frame->destination = COM_GRAZYNA_ID;
    frame->device = COM_MEASUREMENT_ID;
    frame->operation = 0;
    frame->id = meas->channel_id;
    frame->data_type = FLOAT;
    frame->payload = *((uint32_t *) &meas->scaled);
}

/* Function for transmitting feed values, retval is the number of remaining values to transmit */
int8_t SS_ADS1258_com_feed(ComFrame *frame) {
    static uint8_t counter, meas_num;
    if (measurements_count == 0) return -1;
    if (meas_num == 0) {
        meas_num = measurements_count;
    }
    Measurement *meas;
    do {
        meas = measurement_pointers[counter++];
        if (counter >= MAX_MEASUREMENT_COUNT) {
            counter = 0;
        }
    } while (meas == NULL);
    meas_num--;
    SS_ADS1258_measurement_feed(meas, frame);
    return meas_num;
}

ComStatus SS_ADS1258_com_request(ComFrame *frame) {
    if (frame->id > sizeof(measurement_pointers) / sizeof(measurement_pointers[0])) return COM_ERROR;
    Measurement *meas = measurement_pointers[frame->id];
    if(meas == NULL) return COM_ERROR;
    SS_ADS1258_calculate_scaled(meas);
    SS_com_add_payload_to_frame(frame, FLOAT, &meas->scaled);
    return COM_OK;
}

/* Unnecessary, feed calculates scaled values before sending */

//static void SS_ADS1258_calculate_preview_values() {
//    static uint32_t timer = 0;
//    static uint8_t counter = 0;
//    if(measurements_count == 0) return;
//    timer++;
//    if(timer >= ADS1258_PREVIEW_VALUES_PERIOD / measurements_count)  {
//        timer = 0;
//        Measurement *meas;
//        do {
//            meas = measurement_pointers[counter++];
//            if(counter >= MAX_MEASUREMENT_COUNT) {
//                counter = 0;
//            }
//        } while(meas == NULL);
//        float voltage = SS_ADS1258_calculate_voltage(meas);
//        meas->scaled = voltage * meas->a_coefficient + meas->b_coefficient;
//    }
//}
//
//void SS_ADS1258_Systick() {
//    SS_ADS1258_calculate_preview_values();
//}
