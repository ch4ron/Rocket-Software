/*
 * SS_com.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_ADS1258
#include "SS_measurements.h"
#endif
#include "SS_com.h"
#include "SS_com_feed.h"
#include "stdbool.h"

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define COM_FEED_PERIOD 500

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static ComFrame feed_frame;
TaskHandle_t com_feed_task;

/* An array of functions that send feed data
 * The functions should return a number of remaining values to transmit
 * Follow the example from SS_ADS1258_com_feed when adding new modules */
int8_t (*modules[])(ComFrame*) = {
#ifdef SS_USE_ADS1258
    SS_ADS1258_com_feed
#endif
};

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_com_feed_enable(void) {
    vTaskResume(com_feed_task);
}

void SS_com_feed_disable(void) {
    vTaskSuspend(com_feed_task);
}

void SS_com_feed_task(void *pvParameters) {
    static uint8_t module = 0;
    if(sizeof(modules) == 0) {
        while(true) {
            vTaskDelay(portMAX_DELAY);
        }
    }
    /* Feed functions should return 0 when they transmitted all values */
    while(true) {
        int8_t res = modules[module](&feed_frame);
        if(res <= 0) {
            module++;
        }
        if(module == sizeof(modules) / sizeof(modules[0])) {
            module = 0;
        }
        if(res >= 0) {
            SS_com_transmit(&feed_frame);
        }
        vTaskDelay(pdMS_TO_TICKS(COM_FEED_PERIOD));
    }
}
