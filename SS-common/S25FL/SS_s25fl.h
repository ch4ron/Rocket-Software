/*
 * SS_s25fl.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef INC_SS_S25FL_H_
#define INC_SS_S25FL_H_

#include "main.h"

#define S25FL_USE_QUAD

#define S25FL_MEMORY_SIZE (64UL*1024UL*1024UL)
#define S25FL_SECTOR_SIZE (256UL*1024UL)
#define S25FL_SECTOR_COUNT (S25FL_MEMORY_SIZE/S25FL_SECTOR_SIZE)
#define S25FL_PAGE_SIZE 512UL
#define S25FL_PAGE_COUNT (S25FL_MEMORY_SIZE/S25FL_PAGE_SIZE)

typedef enum
{
	S25FL_STATUS_OK,
	S25FL_STATUS_ERR,
	S25FL_STATUS_BUSY,
	S25FL_STATUS_SUSPENDED,
	S25FL_STATUS_NUM
}S25flStatus;

S25flStatus SS_s25fl_init(void);

S25flStatus SS_s25fl_read_rems_id(uint16_t *id);

S25flStatus SS_s25fl_wait_until_ready(void);
//S25flStatus SS_s25fl_wait_until_ready_dma(void);
S25flStatus SS_s25fl_wait_until_dma_ready(void);

S25flStatus SS_s25fl_erase_all(void);
S25flStatus SS_s25fl_erase_sector(uint32_t sector);

S25flStatus SS_s25fl_write_bytes(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_write_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_write_page(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_write_page_dma(uint32_t page, uint8_t *data);

S25flStatus SS_s25fl_read_bytes(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_read_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_read_page(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_read_page_dma(uint32_t page, uint8_t *data);

S25flStatus SS_s25fl_get_status(void);
//S25flStatus SS_s25fl_get_status_dma(void);
bool SS_s25fl_get_is_dma_ready(void);

void SS_s25fl_txcplt_handler(void);
void SS_s25fl_rxcplt_handler(void);

#ifdef DEBUG
S25flStatus SS_s25fl_debug_read_regs(uint8_t *status_reg1, uint8_t *status_reg2, uint8_t *config_reg);
#endif

void SS_s25fl_test(void);

#endif /* INC_SS_S25FL_H_ */
