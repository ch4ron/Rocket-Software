#include "FreeRTOS.h"
#include "main.h"
#include "stdio.h"
#include "task.h"
#include "tim.h"

static void vLEDFlashTask(void *pvParameters) {
    for(;;) {
        vTaskDelay(500);
        HAL_GPIO_TogglePin(COM_RED_GPIO_Port, COM_RED_Pin);
    }
}

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

__weak void vApplicationStackOverflowHook(xTaskHandle xTask,
                                          signed char *pcTaskName) {
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
}

void SS_FreeRTOS_init(void) {
    /* HAL_TIM_Base_Start_IT(&htim14); */
    xTaskCreate(vLEDFlashTask, "LEDx", 1024, NULL, 2, (TaskHandle_t *)NULL);
    vTraceEnable(TRC_START);
    vTaskStartScheduler();
}
