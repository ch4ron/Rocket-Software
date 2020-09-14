#include "SS_fatmap.h"
#include "SS_fatmap_defs.h"
#include "SS_flash_log.h"
#include "SS_flash_lfs.h"


static FatmapStatus read_reserved_pages(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size);
static FatmapStatus read_fat_region(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size);
static FatmapStatus read_data_region(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size);

//static bool safe_read_incr(uint8_t val, uint32_t *data, uint32_t *pos, uint32_t start, uint32_t size);

static uint32_t get_boot_page(Fatmap *fm);
static uint32_t get_fs_info_page(Fatmap *fm);
static uint32_t get_backup_boot_page(Fatmap *fm);
static uint32_t get_dir_table_page(Fatmap *fm);

static uint32_t get_dir_table_page(Fatmap *fm);
static uint32_t get_fat_region_first_page(Fatmap *fm);
static uint32_t get_root_dir_region_first_page(Fatmap *fm);
static uint32_t get_data_region_first_page(Fatmap *fm);

static lfs_file_t vars_file;
static struct lfs_file_config vars_cfg;
static uint8_t vars_buf[FLASH_PAGE_BUF_SIZE];

static lfs_file_t text_file;
static struct lfs_file_config text_cfg;
static uint8_t text_buf[FLASH_PAGE_BUF_SIZE];

bool is_enabled = false;

FatmapStatus SS_fatmap_init(Fatmap *fm, FatmapConfig *cfg)
{
    // XXX.
    fm->cfg = cfg;
    fm->cfg->type = FATMAP_TYPE_FAT32;
    fm->cfg->page_size = FATMAP_PAGE_SIZE;
    fm->cfg->page_count = 256*1024;
    fm->cfg->pages_per_cluster = 1;

    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_start(Fatmap *fm)
{
    if (is_enabled) {
        return FATMAP_STATUS_DISABLED;
    }

    if (SS_flash_lfs_start() != FLASH_STATUS_OK) {
        return FATMAP_STATUS_ERR;
    }

    lfs_t *lfs = SS_flash_lfs_get();

    vars_cfg.buffer = vars_buf;
    vars_cfg.attr_count = 0;

    if (lfs_file_opencfg(lfs, &vars_file, FLASH_LOG_VARS_FILENAME, LFS_O_RDONLY, &vars_cfg) != LFS_ERR_OK) {
        return FATMAP_STATUS_ERR;
    }

    text_cfg.buffer = text_buf;
    text_cfg.attr_count = 0;

    if (lfs_file_opencfg(lfs, &text_file, FLASH_LOG_TEXT_FILENAME, LFS_O_RDONLY | LFS_O_CREAT, &text_cfg) != LFS_ERR_OK) {
        return FATMAP_STATUS_ERR;
    }

    is_enabled = true;
    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_stop(Fatmap *fm)
{
    if (!is_enabled) {
        return FATMAP_STATUS_DISABLED;
    }

    lfs_t *lfs = SS_flash_lfs_get();

    if (lfs_file_close(lfs, &vars_file) != LFS_ERR_OK) {
        return FATMAP_STATUS_ERR;
    }

    if (lfs_file_close(lfs, &text_file) != LFS_ERR_OK) {
        return FATMAP_STATUS_ERR;
    }

    is_enabled = false;
    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_mkfile(Fatmap *fm, FatmapFile *file, const char *path)
{
    /*fm->root.files->prev->next = file;
    file->prev = fm->root.files->prev;

    fm->root.files->prev = file;
    file->next = fm->root.files;

    file->path = path;*/
    // TODO.
    
    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_rmfile(Fatmap *fm, FatmapFile *file)
{
    /*file->next->prev = file->prev;
    file->prev->next = file->next;*/
    // TODO.

    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_mkdir(Fatmap *fm, FatmapDir *dir, const char *path)
{
    // TODO.

    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_rmdir(Fatmap *fm, FatmapDir *dir)
{
    // TODO.

    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_write(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size)
{
    //assert(off < fm->cfg->page_size);

    if (page >= 0 && page < get_fat_region_first_page(fm)) {

    } else if (page >= get_fat_region_first_page(fm)
    && page < get_root_dir_region_first_page(fm)) {

    } else if (page >= get_root_dir_region_first_page(fm)
    && page < get_data_region_first_page(fm)) {

    } else if (page >= get_data_region_first_page(fm)
    && page < fm->cfg->page_count) {
        
    } else {
        assert(0);
    }

    return FATMAP_STATUS_OK;
}

FatmapStatus SS_fatmap_read(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size)
{
    assert(off < fm->cfg->page_size);

    if (page >= 0
    && page < get_fat_region_first_page(fm)) {
        return read_reserved_pages(fm, page, off, data, size);
    } else if (page >= get_fat_region_first_page(fm)
    && page < get_root_dir_region_first_page(fm)) {
        return read_fat_region(fm, page, off, data, size);
    } else if (page >= get_root_dir_region_first_page(fm)
    && page < get_data_region_first_page(fm)) {
        // Unused.
        assert(false);
    } else if (page >= get_data_region_first_page(fm)
    && page < fm->cfg->page_count) {
        return read_data_region(fm, page, off, data, size);       
    } else {
        assert(false);
    }

    return FATMAP_STATUS_OK;
}

static FatmapStatus read_reserved_pages(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size)
{
    if (page == get_boot_page(fm)) {
        memcpy(data, &boot_page_data[off], size);
    } else if (page == get_fs_info_page(fm)) {
        memcpy(data, &fs_info_page_data[off], size);
    } else if (page == get_backup_boot_page(fm)) {
        memcpy(data, &backup_boot_page_data[off], size);
    } else {
        memset(data, 0, size);
    }

    return FATMAP_STATUS_OK;
}

static FatmapStatus read_fat_region(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size)
{
    //if (page == get_fat_region_first_page(fm)) {
        //memset(data, 0xAA, size);
        /*for (; i < LOG_FILE_first_page; ++i) {
            data[i*FAT32_ENTRY_SIZE] = fat_region_head_data[i*FAT32_ENTRY_SIZE];
            data[i*FAT32_ENTRY_SIZE+1] = fat_region_head_data[i*FAT32_ENTRY_SIZE+1];
            data[i*FAT32_ENTRY_SIZE+2] = fat_region_head_data[i*FAT32_ENTRY_SIZE+2];
            data[i*FAT32_ENTRY_SIZE+3] = fat_region_head_data[i*FAT32_ENTRY_SIZE+3];
        }*/
    //}
    
    uint32_t first_entry = (page-get_fat_region_first_page(fm))*FATMAP_PAGE_SIZE/FAT32_ENTRY_SIZE;

    // Files always span at least one cluster.

    lfs_t *lfs = SS_flash_lfs_get();
    uint32_t vars_file_len = (lfs_file_size(lfs, &vars_file)-1)/FATMAP_PAGE_SIZE + 1;
    uint32_t text_file_len = (lfs_file_size(lfs, &text_file)-1)/FATMAP_PAGE_SIZE + 1;

    //uint32_t vars_file_last_cluster = first_entry+vars_file_len;
    uint32_t i = 0;

    if (page == get_fat_region_first_page(fm)) {
        for (; i < sizeof(fat_region_head_data)/FAT32_ENTRY_SIZE; ++i) {
            data[i*FAT32_ENTRY_SIZE] = fat_region_head_data[i*FAT32_ENTRY_SIZE];
            data[i*FAT32_ENTRY_SIZE+1] = fat_region_head_data[i*FAT32_ENTRY_SIZE+1];
            data[i*FAT32_ENTRY_SIZE+2] = fat_region_head_data[i*FAT32_ENTRY_SIZE+2];
            data[i*FAT32_ENTRY_SIZE+3] = fat_region_head_data[i*FAT32_ENTRY_SIZE+3];
        }
    }

    for (; i < FATMAP_PAGE_SIZE/FAT32_ENTRY_SIZE; ++i) {
        uint32_t entry_off = first_entry + i - sizeof(fat_region_head_data)/FAT32_ENTRY_SIZE;

        if (entry_off == vars_file_len-1
        || entry_off == vars_file_len+text_file_len-1) {
            data[i*FAT32_ENTRY_SIZE] = 0xFF;
            data[i*FAT32_ENTRY_SIZE+1] = 0xFF;
            data[i*FAT32_ENTRY_SIZE+2] = 0xFF;
            data[i*FAT32_ENTRY_SIZE+3] = 0x0F;
        } else if (entry_off < vars_file_len+text_file_len) {
            data[i*FAT32_ENTRY_SIZE] = first_entry+i+1;
            data[i*FAT32_ENTRY_SIZE+1] = (first_entry+i+1) >> 8;
            data[i*FAT32_ENTRY_SIZE+2] = (first_entry+i+1) >> 16;
            data[i*FAT32_ENTRY_SIZE+3] = (first_entry+i+1) >> 24;
        } else {
            data[i*FAT32_ENTRY_SIZE] = 0x00;
            data[i*FAT32_ENTRY_SIZE+1] = 0x00;
            data[i*FAT32_ENTRY_SIZE+2] = 0x00;
            data[i*FAT32_ENTRY_SIZE+3] = 0x00;
            /*data[i*FAT32_ENTRY_SIZE] = entry_off;
            data[i*FAT32_ENTRY_SIZE+1] = entry_off >> 8;
            data[i*FAT32_ENTRY_SIZE+2] = entry_off >> 16;
            data[i*FAT32_ENTRY_SIZE+3] = entry_off >> 24;*/
        }
    }

    return FATMAP_STATUS_OK;
}

static FatmapStatus read_data_region(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size)
{
    lfs_t *lfs = SS_flash_lfs_get();
    uint32_t vars_file_len = (lfs_file_size(lfs, &vars_file)-1) / FATMAP_PAGE_SIZE + 1;
    uint32_t text_file_len = (lfs_file_size(lfs, &text_file)-1) / FATMAP_PAGE_SIZE + 1;

    if (page == get_dir_table_page(fm)) {
        memcpy(data, &dir_table_data[off], size);

        uint32_t vars_file_size = lfs_file_size(lfs, &vars_file);

        data[DIR_TABLE_VARS_FILE_SIZE_OFF] = vars_file_size;
        data[DIR_TABLE_VARS_FILE_SIZE_OFF+1] = vars_file_size >> 8;
        data[DIR_TABLE_VARS_FILE_SIZE_OFF+2] = vars_file_size >> 16;
        data[DIR_TABLE_VARS_FILE_SIZE_OFF+3] = vars_file_size >> 24;

        data[DIR_TABLE_VARS_FILE_CLUSTER_START_OFF] = 0x03;
        data[DIR_TABLE_VARS_FILE_CLUSTER_START_OFF+1] = 0x03 >> 8;
        //data[DIR_TABLE_VARS_FILE_CLUSTER_START_OFF+2] = 0x04 >> 16;
        //data[DIR_TABLE_VARS_FILE_CLUSTER_START_OFF+3] = 0x04 >> 24;

        uint32_t text_file_size = lfs_file_size(lfs, &text_file);

        data[DIR_TABLE_TEXT_FILE_SIZE_OFF] = text_file_size;
        data[DIR_TABLE_TEXT_FILE_SIZE_OFF+1] = text_file_size >> 8;
        data[DIR_TABLE_TEXT_FILE_SIZE_OFF+2] = text_file_size >> 16;
        data[DIR_TABLE_TEXT_FILE_SIZE_OFF+3] = text_file_size >> 24;

        data[DIR_TABLE_TEXT_FILE_CLUSTER_START_OFF] = (0x03 + vars_file_len);
        data[DIR_TABLE_TEXT_FILE_CLUSTER_START_OFF+1] = (0x03 + vars_file_len) >> 8;
        //data[DIR_TABLE_TEXT_FILE_CLUSTER_START_OFF+2] = (0x04 + text_file_len) >> 16;
        //data[DIR_TABLE_TEXT_FILE_CLUSTER_START_OFF+3] = (0x04 + text_file_len) >> 24;
    } else if (page >= get_dir_table_page(fm)+1
    && page < get_dir_table_page(fm)+1 + vars_file_len) {
        uint32_t off = (page-(get_dir_table_page(fm)+1)) * FATMAP_PAGE_SIZE;

        if (lfs_file_seek(lfs, &vars_file, off, LFS_SEEK_SET) < 0) {
            return FATMAP_STATUS_ERR;
        }

        uint32_t read_size = lfs_file_read(lfs, &vars_file, data, size);
        if (read_size < 0) {
            return FATMAP_STATUS_ERR;
        }

        memset(&data[read_size], 0x11, size-read_size);
    } else if (page >= get_dir_table_page(fm)+1 + vars_file_len
    && page < get_dir_table_page(fm)+1 + vars_file_len + text_file_len) {
        uint32_t off = (page-(get_dir_table_page(fm)+1 + vars_file_len)) * FATMAP_PAGE_SIZE;

        if (lfs_file_seek(lfs, &text_file, off, LFS_SEEK_SET) < 0) {
            return FATMAP_STATUS_ERR;
        }

        uint32_t read_size = lfs_file_read(lfs, &text_file, data, size);
        if (read_size < 0) {
            return FATMAP_STATUS_ERR;
        }

        memset(&data[read_size], 0x22, size-read_size);
    } else {
        memset(data, 0x33, size);
    }

    return FATMAP_STATUS_OK;
}

// XXX: Unused for now.
/*static bool safe_read_incr(uint8_t val, uint32_t *data, uint32_t *pos, uint32_t start, uint32_t size)
{
    if (*pos < start+size) {
        if (*pos >= start) {
            data[*pos-start] = val;
        }

        ++*pos;
        return true;
    }

    return false;
}*/

static uint32_t get_boot_page(Fatmap *fm)
{
    return 0;
}

static uint32_t get_fs_info_page(Fatmap *fm)
{
    return 1;
}

static uint32_t get_backup_boot_page(Fatmap *fm)
{
    return 6;
}

static uint32_t get_dir_table_page(Fatmap *fm)
{
    return get_data_region_first_page(fm);
}

static uint32_t get_fat_region_first_page(Fatmap *fm)
{
    return 32;
}

static uint32_t get_root_dir_region_first_page(Fatmap *fm)
{
    return get_data_region_first_page(fm);
}

static uint32_t get_data_region_first_page(Fatmap *fm)
{
    return 1048;
}
