/**
  * SS_log_task.h
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_FLASH_LOG_H
#define SS_FLASH_LOG_H

#include "SS_flash.h"

FlashStatus SS_flash_log_init(void);
FlashStatus SS_flash_log_var(FlashStream stream, uint8_t id, uint8_t *data, uint32_t size);
FlashStatus SS_flash_log_var_from_isr(FlashStream stream, uint8_t id, uint8_t *data, uint32_t size, bool *hptw);
FlashStatus SS_flash_log_str(FlashStream stream, char *str);
void SS_flash_log_task(void *pvParameters);
void SS_flash_print_logs(char *args);

#endif // SS_FLASH_LOG_H
