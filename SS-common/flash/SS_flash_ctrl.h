/*
 * SS_flash_ctrl.h
 *
 *  Created on: Feb 11, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef INC_SS_FLASH_CTRL_H_
#define INC_SS_FLASH_CTRL_H_

#include "main.h"

#define FLASH_CTRL_CONFIG_FILE_SIZE (8*1024)

typedef enum
{
	FLASH_CTRL_STATUS_OK,
	FLASH_CTRL_STATUS_ERR,
	FLASH_CTRL_STATUS_DISABLED,
	//FLASH_CTRL_STATUS_BUSY,
	FLASH_CTRL_STATUS_WRITE_PROTECTED,
	FLASH_CTRL_STATUS_STREAM_OVERFLOW,
	FLASH_CTRL_STATUS_BUF_OVERFLOW,
	FLASH_CTRL_STATUS_COUNT
}FlashCtrlStatus;

typedef enum
{
	FLASH_CTRL_STREAM_FRONT,
	FLASH_CTRL_STREAM_BACK,
	FLASH_CTRL_STREAM_COUNT
}FlashCtrlStream;

FlashCtrlStatus SS_flash_ctrl_init(void);
FlashCtrlStatus SS_flash_ctrl_update(void);

FlashCtrlStatus SS_flash_ctrl_start_logging(void);
FlashCtrlStatus SS_flash_ctrl_stop_logging(void);

FlashCtrlStatus SS_flash_ctrl_erase_logs(void);
FlashCtrlStatus SS_flash_ctrl_log_var(FlashCtrlStream stream, uint8_t id, uint8_t *data, uint32_t size);
FlashCtrlStatus SS_flash_ctrl_log_str(FlashCtrlStream stream, char *str);

FlashCtrlStatus SS_flash_ctrl_write_page_dma(uint32_t page, uint8_t *data);
FlashCtrlStatus SS_flash_ctrl_read_page_dma(uint32_t page, uint8_t *data);
FlashCtrlStatus SS_flash_ctrl_read_page_dma_wait(uint32_t first_page, uint8_t *data);

FlashCtrlStatus SS_flash_ctrl_set_is_emulating(bool is_emulating_);
bool SS_flash_ctrl_get_is_page_emulated(uint32_t page);

void SS_flash_ctrl_time_increment_handler(void);

#endif /* INC_SS_FLASH_CTRL_H_ */
