/**
  * SS_flash_ctrl.h
  *
  *  Created on: Feb 11, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_FLASH_CTRL_H
#define SS_FLASH_CTRL_H

#include "SS_flash.h"

#define FLASH_CTRL_CONFIG_FILE_SIZE (8*1024)

FlashStatus SS_flash_ctrl_init(void);
FlashStatus SS_flash_ctrl_update(void);

FlashStatus SS_flash_ctrl_start_logging(void);
FlashStatus SS_flash_ctrl_stop_logging(void);
bool SS_flash_ctrl_get_is_logging(void);

FlashStatus SS_flash_ctrl_erase_logs(void);
FlashStatus SS_flash_ctrl_log_var(FlashStream stream, uint8_t id, uint8_t *data, uint32_t size);
//FlashStatus SS_flash_ctrl_log_str(FlashStream stream, char *str);
FlashStatus SS_flash_ctrl_log_char(FlashStream stream, char c);

FlashStatus SS_flash_ctrl_write_page_dma(uint32_t page, uint8_t *data);
FlashStatus SS_flash_ctrl_read_page_dma(uint32_t page, uint8_t *data);
FlashStatus SS_flash_ctrl_read_page_dma_wait(uint32_t first_page, uint8_t *data);

FlashStatus SS_flash_ctrl_set_is_emulating(bool is_emulating_);
bool SS_flash_ctrl_get_is_page_emulated(uint32_t page);

void SS_flash_ctrl_time_increment_handler(void);

#endif
