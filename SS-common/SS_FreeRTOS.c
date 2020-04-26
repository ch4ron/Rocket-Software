#include "FreeRTOS.h"
#include "assert.h"
#include "main.h"
#include "stdio.h"
#include "task.h"
#include "tim.h"
#ifdef SS_USE_COM
#include "SS_com.h"
#endif

static void vLEDFlashTask(void *pvParameters) {
    for(;;) {
        vTaskDelay(500);
        HAL_GPIO_TogglePin(COM_RED_GPIO_Port, COM_RED_Pin);
    }
}

#ifdef SS_RUN_TESTS
static void run_tests_task(void *pvParameters) {
    SS_run_all_tests();
    vTaskDelete(NULL);
}
#endif

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

__weak void vApplicationStackOverflowHook(xTaskHandle xTask,
                                          signed char *pcTaskName) {
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
}

static void SS_FreeRTOS_create_tasks(void) {
    BaseType_t res;
    res = xTaskCreate(vLEDFlashTask, "LEDx", 1024, NULL, 2, (TaskHandle_t *) NULL);
    assert(res == pdTRUE);
#ifdef SS_USE_COM
    res = xTaskCreate(SS_com_rx_handler_task, "Com Rx Handler Task", 128, NULL, 5, NULL);
    assert(res == pdTRUE);
    res = xTaskCreate(SS_com_tx_handler_task, "Com Tx Handler Task", 128, NULL, 5, NULL);
    assert(res == pdTRUE);
#endif
}

void SS_FreeRTOS_init(void) {
#ifdef SS_RUN_TESTS
    xTaskCreate(run_tests_task, "Tests task", 2048, NULL, 4, (TaskHandle_t *) NULL);
#endif
    SS_FreeRTOS_create_tasks();
#ifdef SS_FREERTOS_TRACE
    vTraceEnable(TRC_START);
#endif
    vTaskStartScheduler();
}
