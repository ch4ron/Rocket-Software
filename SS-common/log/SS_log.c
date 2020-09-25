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
#include "SS_misc.h"
#include "printf.h"
#include "semphr.h"
#include "stdarg.h"
#include "string.h"

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct {
    char *content;
    int len;
} LogMessage;

typedef struct {
    uint8_t *data;
    uint16_t capacity;
    uint16_t head;
    uint16_t tail;
    uint16_t transmit_tail;
} CircularBuffer;

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define LOG_BUF_CAPACITY 2048

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static uint16_t SS_log_buf_get_size(CircularBuffer *buf);
static void SS_log_buf_put_wrapper(char character, void *buf);
static void SS_log_buf_flush_internal(CircularBuffer *buf);

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static SemaphoreHandle_t log_mutex;
static SemaphoreHandle_t log_buf_mutex;
static UART_HandleTypeDef *log_huart;

static uint8_t log_data[LOG_BUF_CAPACITY];
static CircularBuffer log_buf = {
    .data = log_data,
    .capacity = LOG_BUF_CAPACITY
};

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_log_init(UART_HandleTypeDef *huart) {
    assert(log_buf.data);
    log_mutex = xSemaphoreCreateBinary();
    assert(log_mutex != NULL);
    log_buf_mutex = xSemaphoreCreateMutex();
    assert(log_buf_mutex != NULL);
    xSemaphoreGive(log_mutex);
    log_huart = huart;
}

void SS_log_buf_flush(void) {
    if(xSemaphoreTake(log_mutex, 0) == pdTRUE) {
        SS_log_buf_flush_internal(&log_buf);
    }
}

void SS_log_buf_flush_fromISR(BaseType_t *higherPriorityTaskWoken) {
    if(xSemaphoreTakeFromISR(log_mutex, higherPriorityTaskWoken) == pdTRUE) {
        SS_log_buf_flush_internal(&log_buf);
    }
}

void _SS_print(const char *format, ...) {
    if(log_huart == NULL) {
        return;
    }
    va_list arg;
    va_start(arg, format);

    if(xSemaphoreTake(log_buf_mutex, pdMS_TO_TICKS(1)) == pdTRUE) {
        vfctprintf(SS_log_buf_put_wrapper, &log_buf, format, arg);
        SS_log_buf_flush();
        xSemaphoreGive(log_buf_mutex);
    }
    va_end(arg);
}

void _SS_print_fromISR(const char *format, ...) {
    if(log_huart == NULL) {
        return;
    }
    va_list arg;
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    va_start(arg, format);

    if(xSemaphoreTakeFromISR(log_buf_mutex, &higherPriorityTaskWoken) == pdTRUE) {
        vfctprintf(SS_log_buf_put_wrapper, &log_buf, format, arg);
        SS_log_buf_flush_fromISR(&higherPriorityTaskWoken);
        xSemaphoreGiveFromISR(log_buf_mutex, &higherPriorityTaskWoken);
    }
    va_end(arg);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

bool SS_print_no_flush_start(void) {
    return xSemaphoreTake(log_buf_mutex, pdMS_TO_TICKS(1));
}

void SS_print_no_flush_end(void) {
    xSemaphoreGive(log_buf_mutex);
}

void SS_print_no_flush(const char *format, ...) {
    if(log_huart == NULL) {
        return;
    }
    va_list arg;
    va_start(arg, format);
    vfctprintf(SS_log_buf_put_wrapper, &log_buf, format, arg);
    va_end(arg);
}

void SS_print_bytes(char *data, uint16_t size) {
    if(log_huart == NULL) {
        return;
    }
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if(xSemaphoreTakeFromISR(log_buf_mutex, &higherPriorityTaskWoken) == pdTRUE) {
        for(uint16_t i = 0; i < size; i++) {
            /* TODO memcpy */
            SS_log_buf_put(data[i]);
        }
        SS_log_buf_flush_fromISR(&higherPriorityTaskWoken);
        xSemaphoreGiveFromISR(log_buf_mutex, &higherPriorityTaskWoken);
    }
    portYIELD_FROM_ISR(higherPriorityTaskWoken);

}

void SS_log_buf_put(char data) {
    /* TODO if memory starts being an issue, flush output automatically */
    if(SS_log_buf_get_size(&log_buf) - log_buf.capacity == 0) {
        return;
    }
    log_buf.data[log_buf.tail] = data;
    log_buf.tail = (log_buf.tail + 1) % log_buf.capacity;
}

void __assert_func(const char *file, int line, const char *function, const char *assertion) {
    taskDISABLE_INTERRUPTS();
    SS_led_set_all(true, false, false);
    if(log_huart != NULL) {
        HAL_UART_Abort_IT(log_huart);
        const char *format = "assertion %s failed: file %s, line %d, function: %s\r\n";
        char msg[256];
        int len = sprintf(msg, format, assertion, file, line, function);
        HAL_UART_Transmit(log_huart, (uint8_t *) msg, len, 1000);
    }
    uint32_t i = 0;
    while(1) {
        i++;
        if(i > 3000000) {
            SS_led_toggle_all(true, false, false);
            i = 0;
        }
    }
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static uint16_t SS_log_buf_get_size(CircularBuffer *buf) {
    return (buf->tail - buf->head) % buf->capacity;
}

static void SS_log_buf_put_wrapper(char character, void *buf) {
    SS_log_buf_put(character);
}

static void SS_log_buf_flush_internal(CircularBuffer *buf) {
    uint16_t tail = buf->tail > buf->head ? buf->tail : buf->capacity;
    buf->transmit_tail = tail;
    HAL_StatusTypeDef res = HAL_UART_Transmit_IT(log_huart, buf->data + buf->head, tail - buf->head);
    assert(res == HAL_OK);
}

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void SS_log_tx_isr(UART_HandleTypeDef *huart) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    if(huart == log_huart) {
        log_buf.head = log_buf.transmit_tail % log_buf.capacity;
        if(log_buf.head != log_buf.tail) {
            SS_log_buf_flush_internal(&log_buf);
        } else {
            xSemaphoreGiveFromISR(log_mutex, &higherPriorityTaskWoken);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
    }
}
