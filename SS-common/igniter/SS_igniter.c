/*
 * SS_igniter.c
 *
 *  Created on: Dec 26, 2019
 *      Author: maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */


#include "SS_platform.h"
#include "SS_supply.h"
#include "SS_igniter.h"
#include "FreeRTOS.h"
#include "timers.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

#define DEFAULT_IGNITION_DURATION pdMS_TO_TICKS(1000)

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void SS_igniter_callback(TimerHandle_t xTimer);

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static Igniter igniter;
static TimerHandle_t igniter_timer;

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_igniter_init(GPIO_TypeDef *GPIO_Port, uint16_t Pin) {
    HAL_GPIO_WritePin(GPIO_Port, Pin, GPIO_PIN_RESET);
    igniter.GPIO_Port = GPIO_Port;
    igniter.Pin = Pin;
    igniter_timer = xTimerCreate("igniter timer", DEFAULT_IGNITION_DURATION, pdFALSE, 0, SS_igniter_callback);
    assert(igniter_timer != NULL);
}

int8_t SS_igniter_ignite(uint16_t time) {
    /* If time equals 0, don't turn igniter off */
    if(time != 0) {
        if(xTimerChangePeriod( igniter_timer, pdMS_TO_TICKS(time), pdMS_TO_TICKS(10)) != pdTRUE) return -1;
        if(xTimerStart(igniter_timer, pdMS_TO_TICKS(10)) != pdTRUE) return -1;
    }
    HAL_GPIO_WritePin(igniter.GPIO_Port, igniter.Pin, GPIO_PIN_SET);
    return 0;
}

void SS_igniter_off(void) {
    HAL_GPIO_WritePin(igniter.GPIO_Port, igniter.Pin, GPIO_PIN_RESET);
}

bool SS_igniter_status(void) {
    return (bool) HAL_GPIO_ReadPin(igniter.GPIO_Port, igniter.Pin);
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static void SS_igniter_callback(TimerHandle_t xTimer) {
    SS_igniter_off();
}
