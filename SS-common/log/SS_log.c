/**
  * SS_log.c
  *
  *  Created on: May 2, 2020
  *      Author: Maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_log.h"

#include "FreeRTOS.h"
#include "portable.h"
#include "queue.h"
#include "semphr.h"
#include "stdarg.h"
#include "string.h"
#include "task.h"

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct {
    char *content;
    int len;
} LogMessage;

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void _SS_vprint(const char *format, va_list args);

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static QueueHandle_t log_queue;
static SemaphoreHandle_t log_mutex;
static UART_HandleTypeDef *log_huart;

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_log_init(UART_HandleTypeDef *huart) {
    log_queue = xQueueCreate(30, sizeof(LogMessage));
    assert(log_queue != NULL);
    log_mutex = xSemaphoreCreateBinary();
    assert(log_mutex != NULL);
    log_huart = huart;
}

void SS_error(const char *format, ...) {
    va_list arg;
    va_start(arg, format);

    const char *error_format = "\x01b[41mERROR\x01b[0m@ %s\r\n";
    int len = sprintf(NULL, error_format, format);

    char *error = pvPortMalloc(len);
    if(error == NULL) {
        return;
    }
    sprintf(error, error_format, format);

    _SS_vprint(error, arg);

    vPortFree(error);
    va_end(arg);
}

void SS_print_line(const char *format, ...) {
    va_list arg;
    va_start(arg, format);

    const char *line_format = "%s\r\n";
    int len = sprintf(NULL, line_format, format);

    char *error = pvPortMalloc(len);
    if(error == NULL) {
        return;
    }
    sprintf(error, line_format, format);

    _SS_vprint(error, arg);

    vPortFree(error);
    va_end(arg);
}

void SS_print(const char *format, ...) {
    va_list arg;
    va_start(arg, format);

    _SS_vprint(format, arg);

    va_end(arg);
}

void SS_log_task(void *pvParameters) {
    LogMessage msg;
    while(1) {
        if(xQueueReceive(log_queue, &msg, portMAX_DELAY) == pdTRUE) {
            HAL_UART_Transmit_IT(log_huart, (uint8_t *) msg.content, msg.len);
            xSemaphoreTake(log_mutex, 200);
            vPortFree(msg.content);
        }
    }
}

void __assert_func(const char *file, int line, const char *function, const char *assertion) {
    taskDISABLE_INTERRUPTS();
    HAL_UART_Abort_IT(log_huart);
    const char *format = "assertion %s failed: file %s, line %d, function: %s\r\n";
    char msg[256];
    int len = sprintf(msg, format, assertion, file, line, function);
    HAL_UART_Transmit(log_huart, (uint8_t *) msg, len, 1000);
    while(1);
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static void _SS_vprint(const char *format, va_list args) {
    LogMessage msg;
    int len = vsnprintf(NULL, 2048, format, args) + 1;

    msg.content = pvPortMalloc(len);
    memset(msg.content, 0, len);
    msg.len = len;

    if(msg.content != NULL) {
        vsnprintf(msg.content, len, format, args);
        if(xQueueSend(log_queue, &msg, pdMS_TO_TICKS(10)) != pdTRUE) {
            vPortFree(msg.content);
        }
    }
}

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void SS_log_tx_isr(UART_HandleTypeDef *huart) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    if(huart == log_huart) {
        xSemaphoreGiveFromISR(log_mutex, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}
