/*
 * SS_flash_ctrl.h
 *
 *  Created on: Feb 11, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef INC_SS_FLASH_CTRL_H_
#define INC_SS_FLASH_CTRL_H_

#include "main.h"
#include "stdbool.h"

#define FLASH_CTRL_CONFIG_FILE_SIZE (8UL*1024UL)

typedef enum
{
	FLASH_CTRL_STATUS_OK,
	FLASH_CTRL_STATUS_ERR,
	FLASH_CTRL_STATUS_DISABLED,
	//FLASH_CTRL_STATUS_BUSY,
	FLASH_CTRL_STATUS_WRITE_PROTECTED,
	FLASH_CTRL_STATUS_OVERFLOW,
	FLASH_CTRL_STATUS_NUM
}FlashCtrlStatus;

FlashCtrlStatus SS_flash_ctrl_init(void);
FlashCtrlStatus SS_flash_ctrl_start_logging(void);
FlashCtrlStatus SS_flash_ctrl_stop_logging(void);

FlashCtrlStatus SS_flash_ctrl_erase_log(void);
FlashCtrlStatus SS_flash_ctrl_log_var_u32(uint8_t id, uint32_t data);
FlashCtrlStatus SS_flash_ctrl_write_pages(uint32_t first_page, uint32_t len, uint8_t *data);
FlashCtrlStatus SS_flash_ctrl_read_pages(uint32_t first_page, uint32_t len, uint8_t *data);

bool SS_flash_ctrl_set_emulating(bool is_emulating_);

void SS_flash_ctrl_time_increment_handler(void);

#endif /* INC_SS_FLASH_CTRL_H_ */
