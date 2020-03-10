/*
 * SS_com.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */


#include "SS_measurements.h"
#include "SS_com_feed.h"
#include "SS_com.h"

#define COM_FEED_PERIOD 500

/* An array of functions that send feed data
 * The functions should return a number of remaining values to transmit
 * Follow the example from SS_ADS1258_com_feed when adding new modules */
static ComFrameContent feed_frame;
static volatile bool enabled;

int8_t (*modules[])(ComFrameContent*) = {
#ifdef SS_USE_ADS1258
        SS_ADS1258_com_feed
#endif
};

void SS_com_feed_enable() {
   enabled = true;
}

void SS_com_feed_disable() {
   enabled = false;
}

void SS_com_main() {
    static uint32_t counter = 0;
    static uint8_t module = 0;

    if(!enabled) return;
    /* Send feed data with defined period */
    if(HAL_GetTick() - counter >= COM_FEED_PERIOD) {
        counter = HAL_GetTick();
        /* Feed functions should return 0 when they transmitted all values */
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
    }
}

