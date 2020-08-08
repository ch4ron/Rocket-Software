#include "SS_flash_lfs.h"
#include "SS_s25fl.h"

static int read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
static int prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
static int erase(const struct lfs_config *c, lfs_block_t block);
static int sync(const struct lfs_config *c);

static lfs_t lfs;
static struct lfs_config cfg;

static uint8_t read_buffer[FLASH_PAGE_BUF_SIZE];
static uint8_t prog_buffer[FLASH_PAGE_BUF_SIZE];
static uint8_t lookahead_buffer[FLASH_PAGE_BUF_SIZE];

FlashStatus SS_flash_lfs_init(void)
{
    uint32_t memory_size = SS_s25fl_get_memory_size();
    uint32_t sector_size = SS_s25fl_get_sector_size();
    uint32_t page_size = SS_s25fl_get_page_size();

    cfg.read = read,
    cfg.prog = prog,
    cfg.erase = erase,
    cfg.sync = sync,
    
    cfg.read_size = page_size;
    cfg.prog_size = page_size;
    cfg.block_size = sector_size;
    cfg.block_count = memory_size/sector_size;
    cfg.cache_size = page_size;
    cfg.lookahead_size = page_size;
    cfg.block_cycles = 500;

    cfg.read_buffer = read_buffer;
    cfg.prog_buffer = prog_buffer;
    cfg.lookahead_buffer = lookahead_buffer;

    if (lfs_mount(&lfs, &cfg)) {
        return SS_flash_lfs_format_and_remount();
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_lfs_format_and_remount(void)
{
    if (lfs_unmount(&lfs)) {
        return FLASH_STATUS_ERR;
    }

    if (lfs_format(&lfs, &cfg)) {
        return FLASH_STATUS_ERR;
    }

    if (lfs_mount(&lfs, &cfg)) {
        return FLASH_STATUS_ERR;
    }

    return FLASH_STATUS_OK;
}

lfs_t *SS_flash_lfs_get(void)
{
    return &lfs;
}

static int read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    SS_s25fl_read_bytes(block*SS_s25fl_get_sector_size()+off, buffer, size);
    //SS_s25fl_read_bytes_dma_wait(block*SS_s25fl_get_sector_size()+off, buffer, size);
    return 0;
}

static int prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    SS_s25fl_write_bytes(block*SS_s25fl_get_sector_size()+off, buffer, size);
    //SS_s25fl_write_bytes_dma_wait(block*SS_s25fl_get_sector_size()+off, buffer, size);
    return 0;
}

static int erase(const struct lfs_config *c, lfs_block_t block)
{
    SS_s25fl_erase_sector(block);
    return 0;
}

static int sync(const struct lfs_config *c)
{
    return 0;
}
