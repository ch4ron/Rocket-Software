/*
 * SS_s25fl.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef SS_S25FL_H
#define SS_S25FL_H

#include "main.h"
#include "stdbool.h"

#ifndef S25FL_USE_QUAD
#define S25FL_USE_QUAD
#endif

#define S25FL_MEMORY_SIZE (64*1024*1024)
#define S25FL_SECTOR_SIZE (256*1024)
#define S25FL_SECTOR_COUNT (S25FL_MEMORY_SIZE/S25FL_SECTOR_SIZE)
#define S25FL_PAGE_SIZE 512
#define S25FL_PAGE_COUNT (S25FL_MEMORY_SIZE/S25FL_PAGE_SIZE)

typedef enum
{
	S25FL_STATUS_OK,
	S25FL_STATUS_ERR,
	S25FL_STATUS_BUSY,
	S25FL_STATUS_SUSPENDED,
	S25FL_STATUS_COUNT
}S25flStatus;

S25flStatus SS_s25fl_init(void);

S25flStatus SS_s25fl_read_rems_id(uint16_t *id);

S25flStatus SS_s25fl_erase_all(void);
S25flStatus SS_s25fl_erase_sector(uint32_t sector);

S25flStatus SS_s25fl_write_bytes(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_write_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size);

S25flStatus SS_s25fl_write_page(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_write_page_dma(uint32_t page, uint8_t *data);

S25flStatus SS_s25fl_read_bytes(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_read_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_read_bytes_dma_wait(uint32_t addr, uint8_t *data, uint32_t size);

S25flStatus SS_s25fl_read_page(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_read_page_dma(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_read_page_dma_wait(uint32_t page, uint8_t *data);

S25flStatus SS_s25fl_wait_until_ready(void);
S25flStatus SS_s25fl_get_status(void);

S25flStatus SS_s25fl_qspi_cmdcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw);
S25flStatus SS_s25fl_qspi_txcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw);
S25flStatus SS_s25fl_qspi_rxcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw);

#ifdef DEBUG
S25flStatus SS_s25fl_debug_read_regs(uint8_t *status_reg1, uint8_t *status_reg2, uint8_t *config_reg);
#endif

#endif /* SS_S25FL_H */
