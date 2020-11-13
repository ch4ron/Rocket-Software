/*
 * SS_s25fl.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef SS_S25FL_H
#define SS_S25FL_H

#include "SS_common.h"
#include "stm32f4xx_hal.h"

typedef enum
{
	S25FL_STATUS_OK,
	S25FL_STATUS_ERR,
	S25FL_STATUS_BUSY,
	S25FL_STATUS_SUSPENDED,
	S25FL_STATUS_COUNT
}S25flStatus;

S25flStatus SS_s25fl_init(GPIO_TypeDef *nrst_gpio, uint16_t nrst_pin,
    uint32_t memory_size_, uint32_t sector_size_, uint32_t page_size_,
    bool use_quad_, uint32_t quad_read_dummy_cycles_, uint32_t config_reg_count);

S25flStatus SS_s25fl_read_id(uint16_t *id);
S25flStatus SS_s25fl_read_rems_id(uint16_t *id);

S25flStatus SS_s25fl_erase_all(void);
S25flStatus SS_s25fl_erase_sector(uint32_t sector);

S25flStatus SS_s25fl_write_bytes(uint32_t addr, const uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_write_bytes_dma(uint32_t addr, const uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_write_bytes_dma_wait(uint32_t addr, const uint8_t *data, uint32_t size);

S25flStatus SS_s25fl_write_page(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_write_page_dma(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_write_page_dma_wait(uint32_t page, uint8_t *data);

S25flStatus SS_s25fl_read_bytes(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_read_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size);
S25flStatus SS_s25fl_read_bytes_dma_wait(uint32_t addr, uint8_t *data, uint32_t size);

S25flStatus SS_s25fl_read_page(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_read_page_dma(uint32_t page, uint8_t *data);
S25flStatus SS_s25fl_read_page_dma_wait(uint32_t page, uint8_t *data);

S25flStatus SS_s25fl_wait_until_ready(void);
S25flStatus SS_s25fl_get_status(void);

uint32_t SS_s25fl_get_memory_size(void);
uint32_t SS_s25fl_get_sector_size(void);
uint32_t SS_s25fl_get_page_size(void);

S25flStatus SS_s25fl_qspi_cmdcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw);
S25flStatus SS_s25fl_qspi_txcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw);
S25flStatus SS_s25fl_qspi_rxcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw);

#ifdef DEBUG
S25flStatus SS_s25fl_debug_read_status_reg1(uint8_t *status_reg1);
S25flStatus SS_s25fl_debug_read_status_reg2(uint8_t *status_reg2);
S25flStatus SS_s25fl_debug_read_config_reg1(uint8_t *config_reg1);
S25flStatus SS_s25fl_debug_read_config_reg2(uint8_t *config_reg2);
S25flStatus SS_s25fl_debug_read_config_reg3(uint8_t *config_reg3);
#endif

#endif /* SS_S25FL_H */
