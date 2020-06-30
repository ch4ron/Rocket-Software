/*
 * SS_flash_caching.c
 *
 *  Created on: Apr 18, 2020
 *      Author: Mikolaj Wielgus
 */

#include "SS_flash_caching.h"
#include "SS_flash_ctrl.h"
#include "SS_s25fl.h"

static FlashStatus cached_write_page(uint32_t page, uint8_t *data);
static FlashStatus cached_read_page(uint32_t page, uint8_t *data);

static volatile uint8_t cache[S25FL_PAGE_SIZE];
static volatile uint32_t cached_page;
static volatile bool is_caching_enabled = false;
static volatile bool is_cached = false;
static volatile bool is_cache_ready = false;

FlashStatus SS_flash_caching_start(void)
{
    if (is_caching_enabled) {
        return FLASH_STATUS_DISABLED;
    }

    is_caching_enabled = true;
    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_caching_stop(void)
{
    if (!is_caching_enabled) {
        return FLASH_STATUS_DISABLED;
    }

    is_caching_enabled = false;
    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_caching_write_pages(uint32_t first_page, uint32_t len, uint8_t *data)
{
    if (cached_page >= first_page && cached_page < first_page+len) {
        // Invalidate cache.
        is_cached = false;
    }

    for (uint32_t i = 0; i < len; ++i) {
        FlashStatus status = cached_write_page(first_page+i, &data[i*S25FL_PAGE_SIZE]);
        if (status != FLASH_STATUS_OK) {
            return status;
        }
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_caching_read_pages(uint32_t first_page, uint32_t len, uint8_t *data)
{
    for (uint32_t i = 0; i < len; ++i) {
        FlashStatus status = cached_read_page(first_page+i, &data[i*S25FL_PAGE_SIZE]);
        if (status != FLASH_STATUS_OK) {
            return status;
        }
    }

    is_cache_ready = false;
    is_cached = true;
    cached_page = first_page+len;

    FlashStatus status = SS_flash_ctrl_read_page_dma(cached_page, (uint8_t *)&cache[0]);
    if (status != FLASH_STATUS_OK) {
        return status;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_caching_qspi_rxcplt_handler(QSPI_HandleTypeDef *hqspi_, bool *hptw)
{
    if (is_cached) {
        is_cache_ready = true;
    }

    *hptw = false;
    return FLASH_STATUS_OK;
}

static FlashStatus cached_write_page(uint32_t page, uint8_t *data)
{
    FlashStatus status = SS_flash_ctrl_write_page_dma(page, data);
    return status;
}

static FlashStatus cached_read_page(uint32_t page, uint8_t *data)
{
    if (is_cached && is_cache_ready && page == cached_page) {
        memcpy(data, (uint8_t *)cache, S25FL_PAGE_SIZE);
    } else {
        FlashStatus status = SS_flash_ctrl_read_page_dma_wait(page, data);
        if (status != FLASH_STATUS_OK) {
            return status;
        }
    }

    return FLASH_STATUS_OK;
}

#ifdef DEBUG
bool SS_flash_caching_debug_get_is_cache_ready(void)
{
    return is_cache_ready;
}

uint32_t SS_flash_caching_debug_get_cached_page(void)
{
    return cached_page;
}
#endif
