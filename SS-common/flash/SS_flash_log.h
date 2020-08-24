/**
  * SS_log_task.h
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_FLASH_LOG_H
#define SS_FLASH_LOG_H

#include "SS_flash.h"

#define FLASH_LOG_MAX_VAR_DATA_SIZE 8

FlashStatus SS_flash_log_init(void);

FlashStatus SS_flash_log_erase(void);
FlashStatus SS_flash_log_start(void);
FlashStatus SS_flash_log_stop(void);

FlashStatus SS_flash_log_var(uint8_t id, uint8_t *data, uint32_t size);
FlashStatus SS_flash_log_text(const char *str);

void SS_flash_log_task(void *pvParameters);

#ifdef DEBUG
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t SS_flash_debug_get_mutex(void);
#endif /* DEBUG */

#endif /* SS_FLASH_LOG_H */
