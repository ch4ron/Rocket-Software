/**
  * SS_console.c
  *
  *  Created on: May 2, 2020
  *      Author: Maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_console.h"

#include "FreeRTOS.h"
#include "assert.h"
#include "portmacro.h"
#include "semphr.h"
#include "string.h"
#include "task.h"

#include "SS_log.h"
#include "SS_FreeRTOS.h"

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct {
    const char *command;
    const char *help;
    void (*fun)(char *);
} ConsoleCommand;

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void SS_console_print_help(char *args);
static void SS_print_tasks_info(char *args);
static void SS_print_runtime_stats(char *args);
static void SS_console_run_all_tests(char *args);
static void SS_handle_console_input(char *buf);
static void _SS_flash_log_toggle(char *buf);

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static UART_HandleTypeDef *console_huart;
static char console_buf[30];
static SemaphoreHandle_t rx_sem;
static volatile uint8_t idx;

ConsoleCommand commands[] = {
#if defined(SS_RUN_TESTS) && defined(SS_RUN_TESTS_FROM_CONSOLE)
    {"test", "Run tests", SS_console_run_all_tests},
#endif /* defined(SS_RUN_TESTS) && defined(SS_RUN_TESTS_FROM_CONSOLE) */
    {"tasks", "Print task info", SS_print_tasks_info},
    {"stats", "Print task runtime stats", SS_print_runtime_stats},
#ifdef SS_USE_FLASH
    {"log", "Start / Stop logging to flash", _SS_flash_log_toggle},
#endif
    {"help", "Print help", SS_console_print_help},
};

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_console_init(UART_HandleTypeDef *huart) {
    console_huart = huart;
    rx_sem = xSemaphoreCreateBinary();
    assert(rx_sem != NULL);
}

void SS_console_task(void *pvParameters) {
    HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf, 1);
    while(1) {
        if(xSemaphoreTake(rx_sem, portMAX_DELAY) == pdTRUE) {
            if(idx >= sizeof(console_buf)) {
                SS_error("Line too long");
                idx = 0;
                HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf, 1);
            } else if(console_buf[idx] == '\n' || console_buf[idx] == '\r') {
                if(idx > 0) {
                    SS_handle_console_input(console_buf);
                    idx = 0;
                    memset(console_buf, 0, sizeof(console_buf));
                }
                HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf, 1);
                /* Backspace */
            } else if(console_buf[idx] == 127) {
                if(idx > 0) {
                    HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf + (--idx), 1);
                } else {
                    HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf, 1);
                }
            } else {
                HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf + (++idx), 1);
            }
        }
    }
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static void SS_handle_console_input(char *buf) {
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        ConsoleCommand *command = commands + i;
        int len = strlen(command->command);
        if(memcmp(command->command, buf, len) == 0 &&
           (buf[len] == ' ' || buf[len] == '\r' || buf[len] == '\n')) {
            assert(command->fun != NULL);
            command->fun(buf + sizeof(command->command));
            return;
        }
    }
    SS_println("Invalid command, type 'help' for usage");
}

static void SS_print_tasks_info(char *args) {
    char *task_info = pvPortMalloc(1024);
    assert(task_info != NULL);
    vTaskList(task_info);
    SS_print("%s", task_info);
    vPortFree(task_info);
}

static void SS_print_runtime_stats(char *args) {
    char *task_info = pvPortMalloc(1024);
    assert(task_info != NULL);
    vTaskGetRunTimeStats(task_info);
    SS_println("Runtime stats:");
    SS_print("%s", task_info);
    vPortFree(task_info);
}

static void SS_console_run_all_tests(char *args) {
    BaseType_t res = xTaskCreate(SS_run_tests_task, "Tests task", 512, NULL, 4, (TaskHandle_t *) NULL);
    assert(res == pdTRUE);
}

#ifdef SS_USE_FLASH
#include "SS_flash_log.h"

static void _SS_flash_log_toggle(char *args) {
    bool is_logging;
    SS_flash_log_toggle(&is_logging);
    if(is_logging) {
        SS_println("Logging started");
    } else {
        SS_println("Logging stopped");
    }
}

#endif

static void SS_console_print_help(char *args) {
    SS_println("Available commands:");
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        ConsoleCommand *command = commands + i;
        SS_println("    - %s\t\t%s", command->command, command->help);
    }
}

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void SS_console_rx_isr(UART_HandleTypeDef *huart) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    if(huart == console_huart) {
        xSemaphoreGiveFromISR(rx_sem, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}
