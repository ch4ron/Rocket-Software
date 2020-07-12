/**
  * SS_log_task.c
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_flash.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>

// XXX.
#include "SS_MPU9250.h"

typedef struct
{
    uint8_t id;
    uint8_t data[SS_FLASH_MAX_VAR_DATA_SIZE];
    uint32_t size;
}Var;

// TODO: Support var and text queues for more than 2 streams.
static QueueHandle_t var_queue;
static QueueHandle_t text_queue;

FlashStatus SS_flash_log_init(void)
{
    var_queue = xQueueCreate(64, sizeof(Var));
    if (var_queue == NULL) {
        return FLASH_STATUS_ERR;
    }

    text_queue = xQueueCreate(256, sizeof(char));
    if (text_queue == NULL) {
        return FLASH_STATUS_ERR;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_var(FlashStream stream, uint8_t id, uint8_t *data, uint32_t size)
{
    /*Var var = {
        .id = id,
        .data = data
    };*/
    Var var = {
        .id = id,
        .size = size,
    };

    memcpy(&var.data, data, size);

    if (!xQueueSend(var_queue, &var, pdMS_TO_TICKS(10))) {
        return FLASH_STATUS_BUSY;
    }

    return FLASH_STATUS_OK;
}


FlashStatus SS_flash_log_var_from_isr(FlashStream stream, uint8_t id, uint8_t *data, uint32_t size, bool *hptw)
{
    Var var = {
        .id = id,
        .size = size,
    };

    memcpy(&var.data, data, size);

    BaseType_t higher_priority_task_woken;
    if (!xQueueSendFromISR(var_queue, &var, &higher_priority_task_woken)) {
        return FLASH_STATUS_BUSY;
    }
    *hptw = higher_priority_task_woken;

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_str(FlashStream stream, char *str)
{
    for (uint32_t i = 0; str[i] != '\0'; ++i) {
        if (!xQueueSend(text_queue, &str[i], pdMS_TO_TICKS(10))) {
            return FLASH_STATUS_BUSY;
        }
    }

    return FLASH_STATUS_OK;
}

// TODO: Add start and stop logging functions for this module. Stopping logging at this layer of abstraction shall wait until all variables and chars are logged.

void SS_flash_log_task(void *pvParameters)
{
    Var var;
    char c;

    while (true) {
        if (xQueueReceive(var_queue, &var, pdMS_TO_TICKS(1)) == pdTRUE) {
            SS_flash_ctrl_log_var(FLASH_STREAM_VAR, var.id, (uint8_t *)&var.data, var.size);
        }

        /*if (xQueueReceive(text_queue, &c, pdMS_TO_TICKS(1)) == pdTRUE) {
            SS_flash_ctrl_log_char(FLASH_STREAM_TEXT, c);
        }*/
        if (xTaskGetTickCount() >= 20000) {
            SS_MPU_set_is_logging(false);
            SS_print("mem dump from 0x00085200\r\n");

            for (uint32_t i = 0; i < 10; ++i) {
                static uint8_t data[S25FL_PAGE_SIZE];
                SS_s25fl_read_page(0x00089200UL/S25FL_PAGE_SIZE + i, data);
                for (uint32_t ii = 0; ii < S25FL_PAGE_SIZE; ++ii) {
                    SS_print("%x ", data[ii]);
                    vTaskDelay(pdMS_TO_TICKS(5));
                }

                SS_print("\r\n");
            }
            
            while (true) {
            }
        }
    }
}
