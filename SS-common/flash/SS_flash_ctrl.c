/*
 * SS_log_ctrl.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Mikolaj Wielgus
 */

#include "SS_flash_caching.h"
#include "SS_flash_ctrl.h"
#include "SS_s25fl.h"

#define NULL_PAGE (~0UL)
#define TIMEOUT_ms 100

#define CLUSTER_SIZE 512
#define FAT_ENTRY_SIZE 4

#define PAGES_PER_CLUSTER (CLUSTER_SIZE/S25FL_PAGE_SIZE)
#define FAT_ENTRIES_PER_PAGE (S25FL_PAGE_SIZE/FAT_ENTRY_SIZE)

#define BOOT_SECTOR_FIRST_PAGE 0
#define BOOT_SECTOR_LEN 1

#define FS_INFO_SECTOR_FIRST_PAGE 1
#define FS_INFO_SECTOR_LEN 1

#define BACKUP_BOOT_SECTOR_FIRST_PAGE 6
#define BACKUP_BOOT_SECTOR_LEN 1

#define FAT_REGION_FIRST_PAGE 32
#define FAT_REGION_LEN 1016

// XXX: The file information could perhaps be moved to a structure.

#define DIR_TABLE_FIRST_CLUSTER 2
#define DIR_TABLE_CLUSTER_COUNT 1 // Dir table takes only one cluster.
#define DIR_TABLE_FIRST_PAGE (FAT_REGION_FIRST_PAGE+FAT_REGION_LEN)
#define DIR_TABLE_LEN (DIR_TABLE_CLUSTER_COUNT*PAGES_PER_CLUSTER)

#define DIR_TABLE_CONFIG_FILE_DATE_PAGE DIR_TABLE_FIRST_PAGE
#define DIR_TABLE_CONFIG_FILE_DATE_OFFSET 0x00000016UL
#define DIR_TABLE_CONFIG_FILE_DATE_SIZE 4

#define DIR_TABLE_LOG_FILE_SIZE_PAGE DIR_TABLE_FIRST_PAGE
#define DIR_TABLE_LOG_FILE_SIZE_OFFSET 0x0000005CUL
#define DIR_TABLE_LOG_FILE_SIZE_SIZE 4

#define CONFIG_FILE_FIRST_CLUSTER 3
#define CONFIG_FILE_CLUSTER_COUNT (FLASH_CTRL_CONFIG_FILE_SIZE/CLUSTER_SIZE) // 8 KiB allocated for config file.
#define CONFIG_FILE_FIRST_PAGE (DIR_TABLE_FIRST_PAGE+DIR_TABLE_LEN)
#define CONFIG_FILE_LEN (CONFIG_FILE_CLUSTER_COUNT*PAGES_PER_CLUSTER)
#define CONFIG_FILE_MAPPED_SECTOR 0
#define CONFIG_FILE_MAPPED_FIRST_PAGE 0

#define PAGES_FILE_FIRST_CLUSTER (CONFIG_FILE_FIRST_CLUSTER+CONFIG_FILE_CLUSTER_COUNT)
#define PAGES_FILE_CLUSTER_COUNT (16*1024/CLUSTER_SIZE) // 16 KiB allocated for sectors file.
#define PAGES_FILE_FIRST_PAGE (CONFIG_FILE_FIRST_PAGE+CONFIG_FILE_LEN)
#define PAGES_FILE_LEN (PAGES_FILE_CLUSTER_COUNT*PAGES_PER_CLUSTER)

#define LOG_FILE_FIRST_CLUSTER (PAGES_FILE_FIRST_CLUSTER+PAGES_FILE_CLUSTER_COUNT)
#define LOG_FILE_CLUSTER_COUNT 129975
#define LOG_FILE_FIRST_PAGE (PAGES_FILE_FIRST_PAGE+PAGES_FILE_LEN)
#define LOG_FILE_LEN (LOG_FILE_CLUSTER_COUNT*PAGES_PER_CLUSTER)

#define EMULATED_REGION_FIRST_PAGE BOOT_SECTOR_FIRST_PAGE
#define EMULATED_REGION_LEN (DIR_TABLE_FIRST_PAGE+DIR_TABLE_LEN)

#define DATA_REGION_FIRST_PAGE (EMULATED_REGION_FIRST_PAGE+EMULATED_REGION_LEN)
#define DATA_REGION_LEN ((DIR_TABLE_CLUSTER_COUNT+CONFIG_FILE_CLUSTER_COUNT+PAGES_FILE_CLUSTER_COUNT+LOG_FILE_CLUSTER_COUNT)*PAGES_PER_CLUSTER)

static const uint8_t boot_sector[S25FL_PAGE_SIZE] = {
    0xEB, 0x58, 0x90, 0x6D, 0x6B, 0x66, 0x73, 0x2E, 0x66, 0x61, 0x74, 0x00, 0x02, 0x01, 0x20, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x29, 0x19, 0x7D, 0x80, 0x6D, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 0x20,
    0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x0E, 0x1F, 0xBE, 0x77, 0x7C, 0xAC,
    0x22, 0xC0, 0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0x5E, 0xEB, 0xF0, 0x32,
    0xE4, 0xCD, 0x16, 0xCD, 0x19, 0xEB, 0xFE, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x6E,
    0x6F, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x20, 0x64, 0x69,
    0x73, 0x6B, 0x2E, 0x20, 0x20, 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20, 0x69, 0x6E, 0x73, 0x65,
    0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x20, 0x66, 0x6C,
    0x6F, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6E, 0x64, 0x0D, 0x0A, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20,
    0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74, 0x72, 0x79, 0x20, 0x61,
    0x67, 0x61, 0x69, 0x6E, 0x20, 0x2E, 0x2E, 0x2E, 0x20, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA,

};

static const uint8_t fs_info_sector[S25FL_PAGE_SIZE] = {
    0x52, 0x52, 0x61, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x72, 0x72, 0x41, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA,
};

static const uint8_t backup_boot_sector[S25FL_PAGE_SIZE] = {
    0xEB, 0x58, 0x90, 0x6D, 0x6B, 0x66, 0x73, 0x2E, 0x66, 0x61, 0x74, 0x00, 0x02, 0x01, 0x20, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x29, 0x19, 0x7D, 0x80, 0x6D, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 0x20,
    0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x0E, 0x1F, 0xBE, 0x77, 0x7C, 0xAC,
    0x22, 0xC0, 0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0x5E, 0xEB, 0xF0, 0x32,
    0xE4, 0xCD, 0x16, 0xCD, 0x19, 0xEB, 0xFE, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x6E,
    0x6F, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x20, 0x64, 0x69,
    0x73, 0x6B, 0x2E, 0x20, 0x20, 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20, 0x69, 0x6E, 0x73, 0x65,
    0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x20, 0x66, 0x6C,
    0x6F, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6E, 0x64, 0x0D, 0x0A, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20,
    0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74, 0x72, 0x79, 0x20, 0x61,
    0x67, 0x61, 0x69, 0x6E, 0x20, 0x2E, 0x2E, 0x2E, 0x20, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA,
};

static const uint8_t fat_region_head[LOG_FILE_FIRST_CLUSTER*FAT_ENTRY_SIZE] = {
    0xF8, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0xF8, 0xFF, 0xFF, 0x0F, 0x04, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
    0x0D, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x0F, 0x14, 0x00, 0x00, 0x00,
    0x15, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x19, 0x00, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00,
    0x1D, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
    0x25, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x29, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x2B, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00,
    0x2D, 0x00, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
    0x31, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x0F,
};

static const uint8_t dir_table[S25FL_PAGE_SIZE] = {
    0x43, 0x4F, 0x4E, 0x46, 0x49, 0x47, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x02, 0x50, 0x50, 0x03, 0x00, 0x00, 0x20, 0x00, 0x00,
    0x50, 0x41, 0x47, 0x45, 0x53, 0x20, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x02, 0x50, 0x50, 0x13, 0x00, 0x00, 0x40, 0x00, 0x00,
    0x4C, 0x4F, 0x47, 0x20, 0x20, 0x20, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B, 0x02, 0x50, 0x50, 0x33, 0x00, 0x00, 0x6E, 0xF7, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint32_t stream_first_page[FLASH_CTRL_STREAM_COUNT] = {
    [FLASH_CTRL_STREAM_FRONT] = LOG_FILE_FIRST_PAGE,
    [FLASH_CTRL_STREAM_BACK] = LOG_FILE_FIRST_PAGE+LOG_FILE_LEN-1,
};

static const uint32_t stream_max_len[FLASH_CTRL_STREAM_COUNT] = {
    [FLASH_CTRL_STREAM_FRONT] = LOG_FILE_LEN,
    [FLASH_CTRL_STREAM_BACK] = LOG_FILE_LEN,
};

static const int32_t stream_direction[FLASH_CTRL_STREAM_COUNT] = {
    [FLASH_CTRL_STREAM_FRONT] = 1,
    [FLASH_CTRL_STREAM_BACK] = -1,
};

static FlashCtrlStatus find_last_logged_page(FlashCtrlStream stream, uint32_t *page);
static FlashCtrlStatus select_next_page(FlashCtrlStream stream);

static FlashCtrlStatus log_byte(FlashCtrlStream stream, uint8_t byte);
//static FlashCtrlStatus flush_stream(FlashCtrlStream stream);

static FlashCtrlStatus translate_s25fl_status(S25flStatus s25fl_status);
static FlashCtrlStatus translate_flash_caching_status(FlashCachingStatus flash_caching_status);
static FlashCtrlStatus translate_hal_status(HAL_StatusTypeDef hal_status);

//static uint8_t *get_flushed_buf(FlashCtrlStream stream);
//static int32_t get_first_set_bit_pos(uint8_t byte, int32_t direction);

static uint32_t get_journal_page_pos(uint32_t page);
static uint32_t get_journal_byte_pos(uint32_t page);
static uint32_t get_journal_bit_pos(uint32_t page);
//static int32_t get_last_bit_pos(int32_t direction);

static volatile uint8_t page_buf1[FLASH_CTRL_STREAM_COUNT][S25FL_PAGE_SIZE];
static volatile uint8_t page_buf2[FLASH_CTRL_STREAM_COUNT][S25FL_PAGE_SIZE];
static volatile uint8_t *cur_page_buf[FLASH_CTRL_STREAM_COUNT];
static volatile uint32_t cur_page_buf_pos[FLASH_CTRL_STREAM_COUNT];
static volatile uint32_t cur_page[FLASH_CTRL_STREAM_COUNT];

static volatile uint8_t config_file_buf[CONFIG_FILE_LEN*S25FL_PAGE_SIZE];
static volatile uint64_t time;
static volatile bool is_config_file_open;

static volatile bool is_flush_required[FLASH_CTRL_STREAM_COUNT];
static volatile bool is_logging;
static volatile bool is_emulating;

FlashCtrlStatus SS_flash_ctrl_init(void)
{
    time = 0;
    is_config_file_open = false;
    is_logging = false;
    is_emulating = true;

    for (uint32_t stream = 0; stream < FLASH_CTRL_STREAM_COUNT; ++stream) {
        cur_page_buf[stream] = page_buf1[stream];
        cur_page_buf_pos[stream] = 0;
        cur_page[stream] = stream_first_page[stream];
        is_flush_required[stream] = false;
    }

    S25flStatus s25fl_status = SS_s25fl_init();
    if (s25fl_status != S25FL_STATUS_OK) {
        return translate_s25fl_status(s25fl_status);
    }

    return FLASH_CTRL_STATUS_OK;//SS_flash_ctrl_start_logging();
}

FlashCtrlStatus SS_flash_ctrl_update(void)
{
    /*for (uint32_t i = 0; i < FLASH_CTRL_STREAM_COUNT; ++i) {
        if (is_flush_required[i]) {
            flush_stream(i);
        }
    }*/

    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_start_logging(void)
{
    if (is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }
    is_logging = true;

    FlashCachingStatus flash_caching_status = SS_flash_caching_stop();
    if (flash_caching_status != FLASH_CACHING_STATUS_OK
    && flash_caching_status != FLASH_CACHING_STATUS_DISABLED) {
        return translate_flash_caching_status(flash_caching_status);
    }

    for (FlashCtrlStream stream = 0; stream < FLASH_CTRL_STREAM_COUNT; ++stream) {
        FlashCtrlStatus status = find_last_logged_page(stream, (uint32_t *)&cur_page[stream]);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        status = select_next_page(stream);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        cur_page_buf_pos[stream] = 0;
    }

    time = 0;
    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_stop_logging(void)
{
    if (!is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }
    is_logging = false;

    for (FlashCtrlStream stream = 0; stream < FLASH_CTRL_STREAM_COUNT; ++stream) {
        // Set the remaining bytes to 0xFF.
        for (uint32_t pos = cur_page_buf_pos[stream]; pos < S25FL_PAGE_SIZE; ++pos) {
            cur_page_buf[stream][pos] = 0xFF;
        }

        // Dump the final buffer to Flash.
        FlashCtrlStatus status = SS_s25fl_write_page_dma(cur_page[stream], (uint8_t *)cur_page_buf[stream]);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    FlashCachingStatus flash_caching_status = SS_flash_caching_start();
    if (flash_caching_status != FLASH_CACHING_STATUS_OK
    && flash_caching_status != FLASH_CACHING_STATUS_DISABLED) {
        return translate_flash_caching_status(flash_caching_status);
    }

    // Emulate device disconnection to force the OS to read the files again.
    // XXX: Perhaps move this somewhere else, this does not seem to fit here.
    // XXX: Does this even work at all?
    HAL_StatusTypeDef hal_status = USB_DevDisconnect(USB_OTG_FS);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }
    HAL_Delay(1000);
    hal_status = USB_DevConnect(USB_OTG_FS);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }

    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_erase_logs(void)
{
    if (is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }
    // Erasing is marked as logging,
    // since accessing the memory while erase is in progress may break things.
    is_logging = true;

    for (FlashCtrlStream stream = 0; stream < FLASH_CTRL_STREAM_COUNT; ++stream) {
        uint32_t last_page;
        FlashCtrlStatus status = find_last_logged_page(stream, &last_page);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        for (uint32_t sector = last_page*S25FL_PAGE_SIZE/S25FL_SECTOR_SIZE;
        sector+stream_direction[stream] != stream_first_page[stream]*S25FL_PAGE_SIZE/S25FL_SECTOR_SIZE;
        sector -= stream_direction[stream]) {
            S25flStatus s25fl_status = SS_s25fl_erase_sector(sector);
            if (s25fl_status != S25FL_STATUS_OK) {
                return translate_s25fl_status(s25fl_status);
            }
        }
    }

    // Now erase the journal.
    // XXX: Unnecessarily erases sector 2 again.
    for (uint32_t sector = PAGES_FILE_FIRST_PAGE*S25FL_PAGE_SIZE/S25FL_SECTOR_SIZE;
    sector <= (PAGES_FILE_FIRST_PAGE+PAGES_FILE_LEN-1)*S25FL_PAGE_SIZE/S25FL_SECTOR_SIZE;
    ++sector) {
        S25flStatus s25fl_status = SS_s25fl_erase_sector(sector);
        if (s25fl_status != S25FL_STATUS_OK) {
            return translate_s25fl_status(s25fl_status);
        }
    }

    // Erasing is over, clear the logging flag.
    is_logging = false;

    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_log_var(FlashCtrlStream stream, uint8_t id, uint8_t *data, uint32_t size)
{
    if (!is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }

    FlashCtrlStatus status = log_byte(stream, id);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    // Log only 2 most significant bytes of time.
    status = log_byte(stream, time);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    status = log_byte(stream, time >> 8);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    for (uint32_t i = 0; i < size; ++i) {
        status = log_byte(stream, data[i]);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_log_str(FlashCtrlStream stream, char *str)
{
    for (uint32_t i = 0; str[i] != '\0'; ++i) {
        FlashCtrlStatus status = log_byte(stream, str[i]);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return FLASH_CTRL_STATUS_OK;
}

/*FlashCtrlStatus SS_flash_ctrl_write_pages_dma(uint32_t first_page, uint32_t len, uint8_t *data)
{
    FlashCtrlStatus status = FLASH_CTRL_STATUS_OK;

    for (uint32_t i = 0; i < len; ++i, data += S25FL_PAGE_SIZE) {
        FlashCtrlStatus status = emulated_write_page_dma(first_page, data);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return status;
}

FlashCtrlStatus SS_flash_ctrl_read_pages_dma(uint32_t first_page, uint32_t len, uint8_t *data)
{
    FlashCtrlStatus status = FLASH_CTRL_STATUS_OK;

    for (uint32_t i = 0; i < len; ++i, data += S25FL_PAGE_SIZE) {
        status = emulated_read_page_dma(first_page+i, data);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return status;
}*/

// Currently does not really use DMA.
FlashCtrlStatus SS_flash_ctrl_write_page_dma(uint32_t page, uint8_t *data)
{
    if (!is_emulating) {
        S25flStatus s25fl_status = SS_s25fl_write_page_dma(page, data);
        return translate_s25fl_status(s25fl_status);
    }

    // The config file is written to Flash only after a write to the last modification date has been detected.
    // Before that happens, the file data is held in MCU SRAM.
    if (page >= CONFIG_FILE_FIRST_PAGE && page < CONFIG_FILE_FIRST_PAGE+CONFIG_FILE_LEN) {
        is_config_file_open = true;
        memcpy((uint8_t *)&config_file_buf[(page-CONFIG_FILE_FIRST_PAGE)*S25FL_PAGE_SIZE], data, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= DIR_TABLE_FIRST_PAGE && page < DIR_TABLE_FIRST_PAGE+DIR_TABLE_LEN) {
        // TODO: Fix possible memory initialization problems.
        // This solution relies on the assumption that the modification date will be modified when the file is closed.
        // I was unable to find any guarantees for this in the specifications.
        if (is_config_file_open
        && (data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET] != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET]
        || data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET+1] != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET+1]
        || data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET+2] != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET+2]
        || data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET+3] != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET+3])) {

            S25flStatus s25fl_status = SS_s25fl_erase_sector(CONFIG_FILE_MAPPED_SECTOR);
            if (s25fl_status != S25FL_STATUS_OK) {
                return translate_s25fl_status(s25fl_status);
            }

            for (uint32_t i = 0; i < CONFIG_FILE_LEN; ++i) {
                s25fl_status = SS_s25fl_write_page(CONFIG_FILE_MAPPED_FIRST_PAGE+i, (uint8_t *)&config_file_buf[i*S25FL_PAGE_SIZE]);
                if (s25fl_status != S25FL_STATUS_OK) {
                    return translate_s25fl_status(s25fl_status);
                }
            }

            is_config_file_open = false;
        }
        return FLASH_CTRL_STATUS_OK;
    }

    // The config file is mapped to the first sector first page (sectors are 256 KiB blocks).
    // However, only a part of this sector is used.
    // Note that the second sector is left unused. The data begins on the third sector.

    /*if (page >= CONFIG_FILE_FIRST_PAGE
    && page < CONFIG_FILE_FIRST_PAGE+CONFIG_FILE_LEN) {
        page = page-CONFIG_FILE_FIRST_PAGE;
        //return SS_flash_ctrl_write_config_file(page, data);

        // XXX: Because DMA writing does not work for some reason, blocking writing is used for now.
        s25fl_status = SS_s25fl_write_page(page, data);
        return translate_s25fl_status(s25fl_status);
    }*/

    return FLASH_CTRL_STATUS_WRITE_PROTECTED;
}

FlashCtrlStatus SS_flash_ctrl_read_page_dma(uint32_t page, uint8_t *data)
{
    // We do not want to run into concurrency problems,
    // hence logging must be disabled before we read.
    if (is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }

    if (!is_emulating) {
        S25flStatus s25fl_status = SS_s25fl_read_page_dma(page, data);
        return translate_s25fl_status(s25fl_status);
    }

    if (page >= BOOT_SECTOR_FIRST_PAGE
    && page < BOOT_SECTOR_FIRST_PAGE+BOOT_SECTOR_LEN) {
        memcpy(data, boot_sector, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= FS_INFO_SECTOR_FIRST_PAGE
    && page < FS_INFO_SECTOR_FIRST_PAGE+FS_INFO_SECTOR_LEN) {
        memcpy(data, fs_info_sector, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= BACKUP_BOOT_SECTOR_FIRST_PAGE
    && page < BACKUP_BOOT_SECTOR_FIRST_PAGE+BACKUP_BOOT_SECTOR_LEN) {
        memcpy(data, backup_boot_sector, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= FAT_REGION_FIRST_PAGE
    && page < FAT_REGION_FIRST_PAGE+FAT_REGION_LEN) {
        uint32_t first_cluster = (page-FAT_REGION_FIRST_PAGE)*FAT_ENTRIES_PER_PAGE;
        uint32_t i = 0;

        if (page == FAT_REGION_FIRST_PAGE) {
            for (; i < LOG_FILE_FIRST_CLUSTER; ++i) {
                data[i*FAT_ENTRY_SIZE] = fat_region_head[i*FAT_ENTRY_SIZE];
                data[i*FAT_ENTRY_SIZE+1] = fat_region_head[i*FAT_ENTRY_SIZE+1];
                data[i*FAT_ENTRY_SIZE+2] = fat_region_head[i*FAT_ENTRY_SIZE+2];
                data[i*FAT_ENTRY_SIZE+3] = fat_region_head[i*FAT_ENTRY_SIZE+3];
            }
        }

        uint32_t last_logged_page;
        // Recurrent call. Be careful._
        FlashCtrlStatus status = find_last_logged_page(FLASH_CTRL_STREAM_FRONT, &last_logged_page);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        uint32_t used_cluster_count = (last_logged_page-LOG_FILE_FIRST_PAGE)/PAGES_PER_CLUSTER + 1;
        for (; i < FAT_ENTRIES_PER_PAGE && first_cluster+i+1-LOG_FILE_FIRST_CLUSTER < used_cluster_count; ++i) {
            data[i*FAT_ENTRY_SIZE] = first_cluster+i+1;
            data[i*FAT_ENTRY_SIZE+1] = (first_cluster+i+1) >> 8;
            data[i*FAT_ENTRY_SIZE+2] = (first_cluster+i+1) >> 16;
            data[i*FAT_ENTRY_SIZE+3] = (first_cluster+i+1) >> 24;
        }
        if (i < FAT_ENTRIES_PER_PAGE && first_cluster+i+1-LOG_FILE_FIRST_CLUSTER == used_cluster_count) {
            data[i*FAT_ENTRY_SIZE] = 0xFF;
            data[i*FAT_ENTRY_SIZE+1] = 0xFF;
            data[i*FAT_ENTRY_SIZE+2] = 0xFF;
            data[i*FAT_ENTRY_SIZE+3] = 0x0F;
            ++i;
        }
        for (; i < FAT_ENTRIES_PER_PAGE; ++i) {
            data[i*FAT_ENTRY_SIZE] = 0x00;
            data[i*FAT_ENTRY_SIZE+1] = 0x00;
            data[i*FAT_ENTRY_SIZE+2] = 0x00;
            data[i*FAT_ENTRY_SIZE+3] = 0x00;
        }

        return FLASH_CTRL_STATUS_OK;
    } else if (page >= DIR_TABLE_FIRST_PAGE
    && page < DIR_TABLE_FIRST_PAGE+DIR_TABLE_LEN) {
        memcpy(data, dir_table, S25FL_PAGE_SIZE);

        // Log file size is variable.
        // The size read from the directory table must correspond to its size stored in FAT.
        if (page == DIR_TABLE_LOG_FILE_SIZE_PAGE) {
            uint32_t last_logged_page;
            FlashCtrlStatus status = find_last_logged_page(FLASH_CTRL_STREAM_FRONT, &last_logged_page);
            if (status != FLASH_CTRL_STATUS_OK) {
                return status;
            }

            uint32_t used_cluster_count = (last_logged_page-LOG_FILE_FIRST_PAGE)/PAGES_PER_CLUSTER + 1;
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET] = (used_cluster_count*S25FL_PAGE_SIZE);
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET+1] = (used_cluster_count*S25FL_PAGE_SIZE) >> 8;
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET+2] = (used_cluster_count*S25FL_PAGE_SIZE) >> 16;
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET+3] = (used_cluster_count*S25FL_PAGE_SIZE) >> 24;
        }

        return FLASH_CTRL_STATUS_OK;
    /*} else if (page >= EMULATED_REGION_FIRST_PAGE
    && page < EMULATED_REGION_FIRST_PAGE+EMULATED_REGION_LEN) {*/
    } else if (SS_flash_ctrl_get_is_page_emulated(page)) {
        // Remaining parts of the emulated region are handled here, e.g. FAT32 reserved sectors.
        memset(data, 0x00, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= DATA_REGION_FIRST_PAGE
    && page < DATA_REGION_FIRST_PAGE+DATA_REGION_LEN) {
        // Map config file to the first sector first page.
        if (page >= CONFIG_FILE_FIRST_PAGE
        && page < CONFIG_FILE_FIRST_PAGE+CONFIG_FILE_LEN) {
            page = page-CONFIG_FILE_FIRST_PAGE;
            //return SS_flash_ctrl_read_config_file(page, data);
        }

        S25flStatus s25fl_status = SS_s25fl_read_page_dma(page, data);
        return translate_s25fl_status(s25fl_status);
    }

    return FLASH_CTRL_STATUS_ERR;
}

FlashCtrlStatus SS_flash_ctrl_read_page_dma_wait(uint32_t page, uint8_t *data)
{
    FlashCtrlStatus status = SS_flash_ctrl_read_page_dma(page, data);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    S25flStatus s25fl_status = SS_s25fl_wait_until_ready();
    return translate_s25fl_status(s25fl_status);
}

FlashCtrlStatus SS_flash_ctrl_set_is_emulating(bool is_emulating_)
{
    is_emulating = is_emulating_;
    return FLASH_CTRL_STATUS_OK;
}

bool SS_flash_ctrl_get_is_page_emulated(uint32_t page)
{
    return page >= EMULATED_REGION_FIRST_PAGE && page < EMULATED_REGION_FIRST_PAGE+EMULATED_REGION_LEN;
}

void SS_flash_ctrl_time_increment_handler(void)
{
    if (is_logging) {
        ++time;
    }
}

static FlashCtrlStatus find_last_logged_page(FlashCtrlStream stream, uint32_t *page)
{
    uint32_t page_pos = get_journal_page_pos(stream_first_page[stream]);
    uint8_t data[S25FL_PAGE_SIZE];

    S25flStatus s25fl_status = SS_s25fl_read_page_dma_wait(PAGES_FILE_FIRST_PAGE+page_pos, data);
    if (s25fl_status != S25FL_STATUS_OK) {
        return translate_s25fl_status(s25fl_status);
    }

    for (uint32_t cur_page = stream_first_page[stream];
    cur_page != stream_first_page[stream]+stream_max_len[stream]*stream_direction[stream];
    cur_page += stream_direction[stream]) {
        uint32_t new_page_pos = get_journal_page_pos(cur_page);

        if (new_page_pos != page_pos) {
            s25fl_status = SS_s25fl_read_page_dma_wait(PAGES_FILE_FIRST_PAGE+new_page_pos, data);
            if (s25fl_status != S25FL_STATUS_OK) {
                return translate_s25fl_status(s25fl_status);
            }

            page_pos = new_page_pos;
        }

        uint32_t byte_pos = get_journal_byte_pos(cur_page);
        uint32_t bit_pos = get_journal_bit_pos(cur_page);

        if (data[byte_pos] & (1 << bit_pos)) {
            *page = cur_page-1;
            return S25FL_STATUS_OK;
        }
    }

    return FLASH_CTRL_STATUS_STREAM_OVERFLOW;
}

static FlashCtrlStatus select_next_page(FlashCtrlStream stream)
{
    if (cur_page[FLASH_CTRL_STREAM_BACK] - cur_page[FLASH_CTRL_STREAM_FRONT] <= 1) {
        return FLASH_CTRL_STATUS_STREAM_OVERFLOW;
    }

    cur_page[stream] += stream_direction[stream];
    cur_page_buf_pos[stream] = 0;

    uint32_t addr = PAGES_FILE_FIRST_PAGE*S25FL_PAGE_SIZE
        + get_journal_page_pos(cur_page[stream])*S25FL_PAGE_SIZE
        + get_journal_byte_pos(cur_page[stream]);
    uint8_t data = ~(1 << get_journal_bit_pos(cur_page[stream]));

    S25flStatus s25fl_status = SS_s25fl_write_bytes(addr, &data, 1);
    if (s25fl_status != S25FL_STATUS_OK) {
        return translate_s25fl_status(s25fl_status);
    }

    return FLASH_CTRL_STATUS_OK;
}

static FlashCtrlStatus log_byte(FlashCtrlStream stream, uint8_t byte)
{
    if (cur_page[stream] >= LOG_FILE_FIRST_PAGE+LOG_FILE_LEN) {
        return FLASH_CTRL_STATUS_STREAM_OVERFLOW;
    }

    cur_page_buf[stream][cur_page_buf_pos[stream]] = byte;
    ++cur_page_buf_pos[stream];

    if (cur_page_buf_pos[stream] >= S25FL_PAGE_SIZE) {
        FlashCtrlStatus status = SS_s25fl_write_page(cur_page[stream], (uint8_t *)cur_page_buf[stream]);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        cur_page_buf[stream] = (cur_page_buf[stream] == page_buf1[stream])? page_buf2[stream] : page_buf1[stream];
        status = select_next_page(stream);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return FLASH_CTRL_STATUS_OK;
}

/*static FlashCtrlStatus flush_stream(FlashCtrlStream stream)
{
    if (!is_flush_required[stream]) {
        return FLASH_CTRL_STATUS_DISABLED;
    }
    is_flush_required[stream] = false;

    FlashCtrlStatus status = SS_s25fl_write_page_dma(cur_page[stream], (uint8_t *)cur_page_buf[stream]);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    status = select_next_page(stream);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    return FLASH_CTRL_STATUS_OK;
}*/

static FlashCtrlStatus translate_s25fl_status(S25flStatus s25fl_status)
{
    switch (s25fl_status) {
    case S25FL_STATUS_OK:
        return FLASH_CTRL_STATUS_OK;
    case S25FL_STATUS_ERR:
    default:
        return S25FL_STATUS_ERR;
    }
}

static FlashCtrlStatus translate_flash_caching_status(FlashCachingStatus flash_caching_status)
{
    switch (flash_caching_status) {
    case FLASH_CACHING_STATUS_OK:
        return FLASH_CTRL_STATUS_OK;
    case FLASH_CACHING_STATUS_DISABLED:
        return FLASH_CTRL_STATUS_DISABLED;
    case FLASH_CACHING_STATUS_ERR:
    default:
        return FLASH_CTRL_STATUS_ERR;
    }
}

static FlashCtrlStatus translate_hal_status(HAL_StatusTypeDef hal_status)
{
    switch (hal_status) {
    case HAL_OK:
        return FLASH_CTRL_STATUS_OK;
    case HAL_ERROR:
    default:
        return FLASH_CTRL_STATUS_ERR;
    }
}

/*static uint8_t *get_flushed_buf(FlashCtrlStream stream)
{
    if (cur_page_buf[stream] == page_buf1[stream]) {
        return (uint8_t *)page_buf2[stream];
    } else {
        return (uint8_t *)page_buf1[stream];
    }
}*/

static uint32_t get_journal_page_pos(uint32_t page)
{
    return (page-LOG_FILE_FIRST_PAGE)/8/S25FL_PAGE_SIZE;
}

static uint32_t get_journal_byte_pos(uint32_t page)
{
    return ((page-LOG_FILE_FIRST_PAGE)/8) % S25FL_PAGE_SIZE;
}

static uint32_t get_journal_bit_pos(uint32_t page)
{
    return (page-LOG_FILE_FIRST_PAGE) % 8;
}

/*static int32_t get_last_bit_pos(int32_t direction)
{
    if (direction == 1) {
        return 7;
    } else if (direction == -1) {
        return 0;
    }
    return -1;
}*/