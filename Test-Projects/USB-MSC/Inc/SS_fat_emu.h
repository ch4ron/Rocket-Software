/*
 * SS_fat_emu.h
 *
 *  Created on: Mar 21, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef INC_SS_FAT_EMU_H_
#define INC_SS_FAT_EMU_H_

#include "main.h"

typedef enum
{
	FAT_EMU_STATUS_OK,
	FAT_EMU_STATUS_ERR,
}FatEmuStatus;

FatEmuStatus SS_fat_emu_init(void);
FatEmuStatus SS_fat_emu_write_pages(uint32_t first_page, uint32_t len, uint8_t *data);
FatEmuStatus SS_fat_emu_read_pages(uint32_t first_page, uint32_t len, uint8_t *data);

#endif /* INC_SS_FAT_EMU_H_ */
