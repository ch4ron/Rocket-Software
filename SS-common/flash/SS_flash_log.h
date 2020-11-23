/**
  * SS_log_task.h
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_FLASH_LOG_H
#define SS_FLASH_LOG_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "FreeRTOS.h"
#include "SS_flash.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

#define FLASH_LOG_MAX_VAR_DATA_SIZE 4
#define FLASH_LOG_VARS_FILENAME "vars.bin"
#define FLASH_LOG_TEXT_FILENAME "text.txt"

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_flash_create_tasks(UBaseType_t priority);
FlashStatus SS_flash_stream_start(char *filename);
FlashStatus SS_flash_stream_stop(char *filename);
bool SS_flash_stream_toggle(char *filename);
FlashStatus SS_flash_stream_erase(char *filename);
FlashStatus SS_flash_stream_erase_all(void);

FlashStatus SS_flash_log_var(uint8_t id, uint8_t *data, uint16_t size);
void SS_flash_log_var_fromISR(uint8_t id, uint8_t *data, uint16_t size);
FlashStatus SS_flash_log_text(const char *str);
void SS_flash_print_logs(char *args);

#endif /* SS_FLASH_LOG_H */
