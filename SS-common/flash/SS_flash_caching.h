/*
 * SS_flash_caching.h
 *
 *  Created on: Mar 24, 2020
 *      Author: Mikolaj Wielgus
 */

#ifndef SS_FLASH_CACHING_H
#define SS_FLASH_CACHING_H

#include "SS_flash.h"

#ifndef FLASH_CACHING_MAX_CACHE_LEN
#define FLASH_CACHING_MAX_CACHE_LEN 2
#endif /* FLASH_CACHING_MAX_CACHE_LEN */

FlashStatus SS_flash_caching_start(void);
FlashStatus SS_flash_caching_stop(void);
FlashStatus SS_flash_caching_write_pages(uint32_t first_page, uint32_t len, uint8_t *data);
FlashStatus SS_flash_caching_read_pages(uint32_t first_page, uint32_t len, uint8_t *data);
FlashStatus SS_flash_caching_qspi_rxcplt_handler(QSPI_HandleTypeDef *hqspi_);

#ifdef DEBUG
bool SS_flash_caching_debug_get_is_cache_ready(void);
uint32_t SS_flash_caching_debug_get_cached_page(void);
#endif

#endif /* SS_FLASH_CACHING_H */
