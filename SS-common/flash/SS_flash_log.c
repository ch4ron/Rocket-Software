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

static QueueHandle_t var_queue;

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

void SS_flash_log_task(void *pvParameters)
{
    Var var;

    while (1) {
        if (xQueueReceive(var_queue, &var, portMAX_DELAY) == pdTRUE) {
            // FIXME: Do not cast (uint64_t *) to (uint8_t *), make an array instead.
            SS_flash_ctrl_log_var(FLASH_STREAM_VAR, var.id, (uint8_t *)&var.data, 4);
        }
    }
}
