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
#include "SS_flash.h"
#include "SS_flash_log.h"
#include "assert.h"
#include "portmacro.h"
#include "semphr.h"
#include "string.h"
#include "task.h"
#ifdef SS_USE_FLASH
#include "SS_flash_ctrl.h"
#endif

#include "SS_common.h"
#include "SS_log.h"
#include "SS_FreeRTOS.h"
#include "SS_MPU9250.h"

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

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static UART_HandleTypeDef *console_huart;
static char console_buf[30];
static SemaphoreHandle_t rx_sem;
static volatile uint8_t idx;

extern MPU9250 mpu;

static void SS_print_MPU_params(char *params) {
    SS_println("%f %f", mpu.gyro_resolution, mpu.accel_resolution);
}

static void SS_erase_flash(char *params) {
    SS_println("Erasing flash...");
    SS_flash_ctrl_stop_logging();
    SS_MPU_set_is_logging(false);
    FlashStatus status = SS_flash_ctrl_erase_logs();
    if(status == FLASH_STATUS_OK) {
        SS_println("Erase finished successfully!");
    } else {
        SS_println("Erase finished with error code: %#x!", status);
    }
}

static void SS_flash_start(char *params) {
    SS_flash_ctrl_start_logging();
}

static void SS_flash_stop(char *params) {
    SS_flash_ctrl_stop_logging();
    SS_println("Logging disabled");
}

static void SS_mpu_calibrate(char *params) {
    SS_MPU_calibrate(&mpu);
    SS_println("calibrate used successfully");

}

ConsoleCommand commands[] = {
#if defined(SS_RUN_TESTS) && defined(SS_RUN_TESTS_FROM_CONSOLE)
    {"test", "Run tests", SS_console_run_all_tests},
#endif /* defined(SS_RUN_TESTS) && defined(SS_RUN_TESTS_FROM_CONSOLE) */
    {"tasks", "Print task info", SS_print_tasks_info},
    {"stats", "Print task info", SS_print_runtime_stats},
#ifdef SS_USE_FLASH
    {"erase", "Erase flash", SS_erase_flash},
    {"stop", "Stop logging", SS_flash_stop},
    {"start", "Start logging", SS_flash_start},
    {"print", "Pring logs", SS_flash_print_logs},
    {"mpu", "Prints mpu config", SS_print_MPU_params},
    {"c", "Calibrate", SS_mpu_calibrate},
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
    /* TODO Add print bytes function & check asser with flash */
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
