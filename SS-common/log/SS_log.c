//
// Created by maciek on 29.02.2020.
//

#include "stdarg.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "SS_log.h"
#include "string.h"


static QueueHandle_t log_queue;
static SemaphoreHandle_t log_mutex;

static UART_HandleTypeDef *log_huart;

typedef struct {
    char *content;
    int len;
} LogMessage;

void SS_log_init(UART_HandleTypeDef *huart) {
    log_queue = xQueueCreate(10, sizeof(LogMessage));
    assert(log_queue != NULL);
    log_mutex = xSemaphoreCreateBinary();
    assert(log_mutex != NULL);
    log_huart = huart;
}


void SS_error(const char *format, ...) {
    va_list arg;
    va_start(arg, format);

    const char* error_format = "\x01b[41mERROR\x01b[0m@ %s\r\n";
    int len = sprintf(NULL, error_format, format);

    char* error = pvPortMalloc(len);
    if(error == NULL) {
        return;
    }
    sprintf(error, error_format, format);
    SS_print(error, arg);
    vPortFree(error);
    va_end(arg);
}


void SS_print(const char *format, ...) {
    LogMessage msg;
    va_list arg;
    va_start(arg, format);

    int len = sprintf(NULL, format, arg);
    msg.content = pvPortMalloc(len);
    msg.len = len;

    if(msg.content != NULL) {
        sprintf(msg.content, format, arg);
        if(xQueueSend(log_queue, &msg, pdMS_TO_TICKS(10)) != pdTRUE) {
            vPortFree(msg.content);
        }
    }
    va_end(arg);
}



void SS_log_task(void *pvParameters) {
    LogMessage msg;
    while(1) {
        if(xQueueReceive(log_queue, &msg, portMAX_DELAY) == pdTRUE) {
            HAL_UART_Transmit_IT(log_huart, (uint8_t*) msg.content, msg.len);
            xSemaphoreTake(log_mutex, 200);
            vPortFree(msg.content);
        }
    }
}

void SS_log_tx_isr(UART_HandleTypeDef *huart) {
    if(huart == log_huart) {
        xSemaphoreGive(log_mutex);
    }
}


void __assert_func(const char *file, int line, const char *function, const char *assertion) {
    taskDISABLE_INTERRUPTS();
    HAL_UART_Abort_IT(log_huart);
    const char *format = "assertion %s failed: file %s, line %d, function: %s\r\n";
    int len = sprintf(NULL, format, assertion, file, line, function);
    char *msg = pvPortMalloc(len);
    if(msg != NULL) {
        sprintf(msg, format, assertion, file, line, function);
        HAL_UART_Transmit(log_huart, (uint8_t*) msg, len, 1000);
        vPortFree(msg);
    }
    while(1);

}
