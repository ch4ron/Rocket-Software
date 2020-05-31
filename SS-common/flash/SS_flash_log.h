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
FlashStatus SS_flash_log_var(FlashStream stream, uint8_t id, uint64_t data);
void SS_flash_log_task(void *pvParameters);

#endif // SS_FLASH_LOG_H
