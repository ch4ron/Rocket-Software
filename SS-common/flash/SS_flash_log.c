/**
  * SS_log_task.c
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_flash.h"
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct
{
    uint8_t id;
    uint64_t data;
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

FlashStatus SS_flash_log_var(FlashStream stream, uint8_t id, uint64_t data)
{
    Var var = {
        .id = id,
        .data = data
    };

    if (!xQueueSend(var_queue, &var, pdMS_TO_TICKS(10))) {
        return FLASH_STATUS_BUSY;
    }

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
            // FIXME: Do not cast (uint64_t *) to (uint8_t *), make an array instead.
            SS_flash_ctrl_log_var(FLASH_STREAM_VAR, var.id, (uint8_t *)&var.data, 4);
        }

        /*if (xQueueReceive(text_queue, &c, pdMS_TO_TICKS(1)) == pdTRUE) {
            SS_flash_ctrl_log_char(FLASH_STREAM_TEXT, c);
        }*/
    }
}
