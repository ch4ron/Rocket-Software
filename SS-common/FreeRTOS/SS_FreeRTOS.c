/**
  * SS_FreeRTOS.c
  *
  *  Created on: Mar 29, 2020
  *      Author: Maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "FreeRTOS.h"
#include "assert.h"
#include "main.h"
#include "task.h"
#include "tim.h"
#include "SS_misc.h"
#ifdef SS_USE_COM
#include "SS_com.h"
#include "SS_com_feed.h"
#endif
#include "SS_FreeRTOS.h"
#include "SS_log.h"
#include "SS_console.h"
#ifdef SS_USE_CAN
#include "SS_can.h"
#endif
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void vLEDFlashTask(void *pvParameters);
static void SS_FreeRTOS_create_tasks(void);
#ifdef SS_RUN_TESTS
static void run_tests_task(void *pvParameters);
#endif

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */


void SS_FreeRTOS_init(void) {
    SS_FreeRTOS_create_tasks();
#ifdef SS_FREERTOS_TRACE
    vTraceEnable(TRC_START);
#endif
    SS_print("Scheduler started\r\n");
    vTaskStartScheduler();
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

#ifdef SS_RUN_TESTS
#include "SS_common.h"
static void run_tests_task(void *pvParameters) {
    SS_run_all_tests();
    vTaskDelete(NULL);
}
#endif

static void vLEDFlashTask(void *pvParameters) {
    while(1) {
        vTaskDelay(500);
        SS_platform_toggle_loop_led();
    }
}

static void SS_FreeRTOS_create_tasks(void) {
    BaseType_t res;
    res = xTaskCreate(SS_log_task, "Log Task", 64, NULL, 4, (TaskHandle_t *) NULL);
    assert(res == pdTRUE);
#ifdef SS_RUN_TESTS
    res = xTaskCreate(run_tests_task, "Tests Task", 512, NULL, 4, (TaskHandle_t *) NULL);
    assert(res == pdTRUE);
#endif
    res = xTaskCreate(vLEDFlashTask, "LED Task", 64, NULL, 2, (TaskHandle_t *) NULL);
    assert(res == pdTRUE);
#ifdef SS_USE_COM
    res = xTaskCreate(SS_com_rx_handler_task, "Com Rx Task", 256, NULL, 5, NULL);
    assert(res == pdTRUE);
#ifdef SS_USE_GRAZYNA
    res = xTaskCreate(SS_grazyna_tx_handler_task, "Grazyna Tx Task", 64, NULL, 5, NULL);
    assert(res == pdTRUE);
    /* res = xTaskCreate(SS_com_feed_task, "Feed task", 64, NULL, 5, (TaskHandle_t *) &com_feed_task); */
    /* assert(res == pdTRUE); */
#endif
#ifdef SS_USE_CAN
    res = xTaskCreate(SS_can_tx_handler_task, "Can Tx Task", 64, NULL, 5, NULL);
    assert(res == pdTRUE);
#ifdef SS_USE_EXT_CAN
    res = xTaskCreate(SS_can_tx_handler_task, "Ext Can Tx Task", 64, NULL, 5, NULL);
    assert(res == pdTRUE);
#endif
#endif
#endif
    res = xTaskCreate(SS_console_task, "Console Task", 256, NULL, 5, (TaskHandle_t *) NULL);
    assert(res == pdTRUE);
}

/* ==================================================================== */
/* ============================== Hooks =============================== */
/* ==================================================================== */

void vApplicationStackOverflowHook(xTaskHandle xTask,
                                          signed char *pcTaskName) {
    assert(false);
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
}

