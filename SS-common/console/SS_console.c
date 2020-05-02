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
#include "SS_log.h"
#include "SS_misc.h"
#include "assert.h"
#include "portmacro.h"
#include "semphr.h"
#include "string.h"
#include "task.h"

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
static void SS_handle_console_input(char *buf);

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static UART_HandleTypeDef *console_huart;
static char console_buf[30];
static SemaphoreHandle_t rx_sem;
static volatile uint8_t idx;

ConsoleCommand commands[] = {
    {"tasks", "Print task info", SS_print_tasks_info},
    {"help", "Print help", SS_console_print_help},
};

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_console_init(UART_HandleTypeDef *huart) {
    console_huart = huart;
    rx_sem = xSemaphoreCreateBinary();
    SS_led_toggle_green_com();
    HAL_UART_Receive_IT(huart, (uint8_t *) console_buf, 1);
}

void SS_console_task(void *pvParameters) {
    while(1) {
        if(xSemaphoreTake(rx_sem, portMAX_DELAY) == pdTRUE) {
            if(idx >= sizeof(console_buf)) {
                SS_error("Line too long");
                idx = 0;
                HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf, 1);
            } else if(console_buf[idx] == '\n' || console_buf[idx] == '\r') {
                SS_led_toggle_green_mem();
                idx = 0;
                SS_handle_console_input(console_buf);
                memset(console_buf, 0, sizeof(console_buf));
                HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf, 1);
                SS_led_toggle_green_adc();
            } else {
                HAL_UART_Receive_IT(console_huart, (uint8_t *) console_buf + (++idx), 1);
                SS_led_toggle_green_mem();
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
        if(memcmp(command->command, buf, sizeof(command->command) - 1) == 0) {
            assert(command->fun != NULL);
            command->fun(buf + sizeof(command->command) - 1);
            break;
        }
        SS_print_line("Invalid command, type 'help' for usage");
    }
}

static void SS_print_tasks_info(char *args) {
    SS_print_line("Running tasks:");
    char *task_info = pvPortMalloc(1024);
    assert(task_info != NULL);
    vTaskGetRunTimeStats(task_info);
    SS_print("%s", task_info);
    vPortFree(task_info);
}

static void SS_console_print_help(char *args) {
    SS_print_line("Available commands:");
    for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        ConsoleCommand *command = commands + i;
        SS_print_line("\t- %s\t\t%s", command->command, command->help);
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
