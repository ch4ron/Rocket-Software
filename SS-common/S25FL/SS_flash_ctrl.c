/*
 * SS_log_ctrl.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Mikolaj Wielgus
 */

#include "SS_flash_ctrl.h"
#include "SS_s25fl.h"
#include "string.h"

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
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA

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
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
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
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};

static const uint8_t fat_region_head[LOG_FILE_FIRST_CLUSTER * FAT_ENTRY_SIZE] = {
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
        0x31, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x0F
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
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static FlashCtrlStatus read_last_logged_page(uint32_t *page);
static FlashCtrlStatus select_next_page(void);

static FlashCtrlStatus log_u64(uint64_t data);
static FlashCtrlStatus log_u32(uint32_t data);
static FlashCtrlStatus log_u16(uint16_t data);
static FlashCtrlStatus log_u8(uint8_t data);

static FlashCtrlStatus emulated_write_page_dma(uint32_t page, uint8_t *data);
static FlashCtrlStatus emulated_read_page_dma(uint32_t page, uint8_t *data);
static FlashCtrlStatus translate_s25fl_status(S25flStatus s25fl_status);
static FlashCtrlStatus translate_hal_status(HAL_StatusTypeDef hal_status);
static int32_t get_first_set_bit(uint8_t byte);

static volatile uint8_t cache[S25FL_PAGE_SIZE];
static volatile uint32_t cached_page = NULL_PAGE;

static volatile uint8_t page_buf1[S25FL_PAGE_SIZE];
static volatile uint8_t page_buf2[S25FL_PAGE_SIZE];
static volatile uint8_t *cur_page_buf = page_buf1;
static volatile uint32_t cur_page_buf_index = 0;
static volatile uint32_t cur_page = NULL_PAGE;

static volatile uint8_t config_file_buf[CONFIG_FILE_LEN * S25FL_PAGE_SIZE];
static volatile bool is_config_file_open = false;

static volatile bool is_logging = false;
static volatile bool is_emulating = true;
static volatile uint64_t time = 0;

FlashCtrlStatus SS_flash_ctrl_init(void) {
    S25flStatus s25fl_status = SS_s25fl_init();
    if (s25fl_status != S25FL_STATUS_OK) {
        return translate_s25fl_status(s25fl_status);
    }

    return FLASH_CTRL_STATUS_OK;//SS_flash_ctrl_start_logging();
}

FlashCtrlStatus SS_flash_ctrl_start_logging(void) {
    if (is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }

    FlashCtrlStatus status = read_last_logged_page((uint32_t *) &cur_page);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    status = select_next_page();
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    cur_page_buf_index = 0;
    is_logging = true;
    time = 0;
    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_stop_logging(void) {
    if (!is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }

    // Set the remaining bytes to 0xFF.
    for (uint32_t i = cur_page_buf_index; i < S25FL_PAGE_SIZE; ++i) {
        cur_page_buf[i] = 0xFF;
    }

    // Dump the final buffer to Flash.
    // XXX: For some reason, DMA writing does not work.
    FlashCtrlStatus status = SS_s25fl_write_page_dma(cur_page, (uint8_t *) cur_page_buf);
    //FlashCtrlStatus status = SS_s25fl_write_page(cur_page, (uint8_t *)cur_page_buf);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    is_logging = false;

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

FlashCtrlStatus SS_flash_ctrl_erase_log(void) {
    if (is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }

    // Erasing is marked as logging,
    // since accessing the memory while erase is in progress may break things.
    is_logging = true;

    // Erase all sectors from the last logged to the third.
    // In this order the operation is fail-safe
    // because the pages file is located before the log file.

    uint32_t last_page;
    FlashCtrlStatus status = read_last_logged_page(&last_page);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }
    for (uint32_t cur_sector = last_page * S25FL_PAGE_SIZE / S25FL_SECTOR_SIZE; cur_sector >= 2; --cur_sector) {
        S25flStatus s25fl_status = SS_s25fl_erase_sector(cur_sector);
        if (s25fl_status != S25FL_STATUS_OK) {
            return translate_s25fl_status(s25fl_status);
        }
    }

    is_logging = false;
    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_log_var_u32(uint8_t id, uint32_t data) {
    uint64_t timestamp = time;

    if (!is_logging) {
        return FLASH_CTRL_STATUS_DISABLED;
    }

    FlashCtrlStatus status = log_u8(id);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    // We assume that data is logged at least once every 2^16 tics.
    status = log_u16(timestamp);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    status = log_u32(data);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    return FLASH_CTRL_STATUS_OK;
}

FlashCtrlStatus SS_flash_ctrl_write_pages(uint32_t first_page, uint32_t len, uint8_t *data) {
    FlashCtrlStatus status = FLASH_CTRL_STATUS_OK;

    // Invalidate the cache.
    if (cached_page >= first_page && cached_page < first_page + len) {
        cached_page = NULL_PAGE;
    }

    for (uint32_t i = 0; i < len; ++i, data += S25FL_PAGE_SIZE) {
        FlashCtrlStatus status = emulated_write_page_dma(first_page, data);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return status;
}

FlashCtrlStatus SS_flash_ctrl_read_pages(uint32_t first_page, uint32_t len, uint8_t *data) {
    for (uint32_t i = 0; i < len; ++i, data += S25FL_PAGE_SIZE) {
        if (first_page + i == cached_page && cached_page != NULL_PAGE) {
            memcpy((uint8_t *) data, (uint8_t *) cache, S25FL_PAGE_SIZE);
        } else {
            FlashCtrlStatus status = emulated_read_page_dma(first_page + i, data);
            if (status != FLASH_CTRL_STATUS_OK) {
                return status;
            }
        }
    }

    cached_page = first_page + len;
    // No need to explicitly wait for the DMA read operation to finish here,
    // because this final call will do it.
    return emulated_read_page_dma(cached_page, (uint8_t *) cache);
}

bool SS_flash_ctrl_set_emulating(bool is_emulating_) {
    bool prev_is_emulating = is_emulating;
    is_emulating = is_emulating_;
    return prev_is_emulating;
}

void SS_flash_ctrl_time_increment_handler(void) {
    if (is_logging) {
        ++time;
    }
}

static FlashCtrlStatus read_last_logged_page(uint32_t *page) {
    for (uint32_t i = 0; i < PAGES_FILE_LEN; ++i) {
        // Can be optimized by using continuous reading mode.
        // TODO: Error should be reported when going beyond last page in data region.

        uint8_t data[S25FL_PAGE_SIZE];
        //FlashCtrlStatus status = emulated_read_page_dma(PAGES_FILE_FIRST_PAGE+i, data);
        FlashCtrlStatus status = SS_s25fl_read_page_dma(PAGES_FILE_FIRST_PAGE + i, data);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        S25flStatus s25fl_status = SS_s25fl_wait_until_dma_ready();
        if (s25fl_status != S25FL_STATUS_OK) {
            return translate_s25fl_status(s25fl_status);
        }

        for (uint32_t j = 0; j < S25FL_PAGE_SIZE; ++j) {
            int32_t first_set_bit = get_first_set_bit(data[j]);

            if (first_set_bit != -1) {
                *page = LOG_FILE_FIRST_PAGE + i * S25FL_PAGE_SIZE * 8 + j * 8 + first_set_bit - 1;
                return FLASH_CTRL_STATUS_OK;
            }

            uint32_t byte_last_page = LOG_FILE_FIRST_PAGE + i * S25FL_PAGE_SIZE * 8 + j * 8 + 7 - 1;
            if (byte_last_page >= LOG_FILE_FIRST_PAGE + LOG_FILE_LEN) {
                return FLASH_CTRL_STATUS_OVERFLOW;
            }
        }
    }

    return FLASH_CTRL_STATUS_OVERFLOW;
}

static FlashCtrlStatus select_next_page(void) {
    ++cur_page;

    if (cur_page >= LOG_FILE_FIRST_PAGE + LOG_FILE_LEN) {
        return FLASH_CTRL_STATUS_OVERFLOW;
    }

    uint32_t addr = PAGES_FILE_FIRST_PAGE * S25FL_PAGE_SIZE + (cur_page - LOG_FILE_FIRST_PAGE) / 8;
    uint8_t data = ~(1 << ((cur_page - LOG_FILE_FIRST_PAGE) % 8));

    S25flStatus s25fl_status = SS_s25fl_write_bytes(addr, &data, 1);
    if (s25fl_status != S25FL_STATUS_OK) {
        return translate_s25fl_status(s25fl_status);
    }

    cur_page_buf_index = 0;
    return FLASH_CTRL_STATUS_OK;
}

static FlashCtrlStatus log_u64(uint64_t data) {
    FlashCtrlStatus status = log_u32(data);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    return log_u32(data >> 32);
}

static FlashCtrlStatus log_u32(uint32_t data) {
    FlashCtrlStatus status = log_u16(data);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    return log_u16(data >> 16);
}

static FlashCtrlStatus log_u16(uint16_t data) {
    FlashCtrlStatus status = log_u8(data);
    if (status != FLASH_CTRL_STATUS_OK) {
        return status;
    }

    return log_u8(data >> 8);
}

static FlashCtrlStatus log_u8(uint8_t data) {
    if (cur_page >= LOG_FILE_FIRST_PAGE + LOG_FILE_LEN) {
        return FLASH_CTRL_STATUS_OVERFLOW;
    }

    cur_page_buf[cur_page_buf_index] = data;
    ++cur_page_buf_index;

    if (cur_page_buf_index >= S25FL_PAGE_SIZE) {
        if (cur_page == cached_page) {
            cached_page = NULL_PAGE;
        }

        FlashCtrlStatus status = SS_s25fl_write_page_dma(cur_page, (uint8_t *) cur_page_buf);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        cur_page_buf = (cur_page_buf == page_buf1) ? page_buf2 : page_buf1;
        status = select_next_page();
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }
    }

    return FLASH_CTRL_STATUS_OK;
}

// Currently does not use DMA.
static FlashCtrlStatus emulated_write_page_dma(uint32_t page, uint8_t *data) {
    if (!is_emulating) {
        S25flStatus s25fl_status = SS_s25fl_write_page_dma(page, data);
        return translate_s25fl_status(s25fl_status);
    }

    // The config file is written to Flash only after a write to the last modification date has been detected.
    // Before that happens, the file data is held in MCU SRAM.
    if (page >= CONFIG_FILE_FIRST_PAGE && page < CONFIG_FILE_FIRST_PAGE + CONFIG_FILE_LEN) {
        is_config_file_open = true;
        memcpy((uint8_t *) &config_file_buf[(page - CONFIG_FILE_FIRST_PAGE) * S25FL_PAGE_SIZE], data, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= DIR_TABLE_FIRST_PAGE && page < DIR_TABLE_FIRST_PAGE + DIR_TABLE_LEN) {
        // TODO: Fix possible memory initialization problems.
        // This solution relies on the assumption that the modification date will be modified when the file is closed.
        // I was unable to find any guarantees for this in the specifications.
        if (is_config_file_open
                && (data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET] != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET]
                        || data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET + 1]
                                != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET + 1]
                        || data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET + 2]
                                != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET + 2]
                        || data[DIR_TABLE_CONFIG_FILE_DATE_OFFSET + 3]
                                != dir_table[DIR_TABLE_CONFIG_FILE_DATE_OFFSET + 3])) {

            S25flStatus s25fl_status = SS_s25fl_erase_sector(CONFIG_FILE_MAPPED_SECTOR);
            if (s25fl_status != S25FL_STATUS_OK) {
                return translate_s25fl_status(s25fl_status);
            }

            for (uint32_t i = 0; i < CONFIG_FILE_LEN; ++i) {
                s25fl_status = SS_s25fl_write_page(CONFIG_FILE_MAPPED_FIRST_PAGE + i,
                                                   (uint8_t *) &config_file_buf[i * S25FL_PAGE_SIZE]);
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

static FlashCtrlStatus emulated_read_page_dma(uint32_t page, uint8_t *data) {
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
            && page < BOOT_SECTOR_FIRST_PAGE + BOOT_SECTOR_LEN) {
        memcpy(data, boot_sector, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= FS_INFO_SECTOR_FIRST_PAGE
            && page < FS_INFO_SECTOR_FIRST_PAGE + FS_INFO_SECTOR_LEN) {
        memcpy(data, fs_info_sector, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= BACKUP_BOOT_SECTOR_FIRST_PAGE
            && page < BACKUP_BOOT_SECTOR_FIRST_PAGE + BACKUP_BOOT_SECTOR_LEN) {
        memcpy(data, backup_boot_sector, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= FAT_REGION_FIRST_PAGE
            && page < FAT_REGION_FIRST_PAGE + FAT_REGION_LEN) {
        uint32_t first_cluster = (page - FAT_REGION_FIRST_PAGE) * FAT_ENTRIES_PER_PAGE;
        uint32_t i = 0;

        if (page == FAT_REGION_FIRST_PAGE) {
            for (; i < LOG_FILE_FIRST_CLUSTER; ++i) {
                data[i * FAT_ENTRY_SIZE] = fat_region_head[i * FAT_ENTRY_SIZE];
                data[i * FAT_ENTRY_SIZE + 1] = fat_region_head[i * FAT_ENTRY_SIZE + 1];
                data[i * FAT_ENTRY_SIZE + 2] = fat_region_head[i * FAT_ENTRY_SIZE + 2];
                data[i * FAT_ENTRY_SIZE + 3] = fat_region_head[i * FAT_ENTRY_SIZE + 3];
            }
        }

        uint32_t last_logged_page;
        // Recurrent call. Be careful._
        FlashCtrlStatus status = read_last_logged_page(&last_logged_page);
        if (status != FLASH_CTRL_STATUS_OK) {
            return status;
        }

        uint32_t used_cluster_count = (last_logged_page - LOG_FILE_FIRST_PAGE) / PAGES_PER_CLUSTER + 1;
        for (; i < FAT_ENTRIES_PER_PAGE && first_cluster + i + 1 - LOG_FILE_FIRST_CLUSTER < used_cluster_count; ++i) {
            data[i * FAT_ENTRY_SIZE] = first_cluster + i + 1;
            data[i * FAT_ENTRY_SIZE + 1] = (first_cluster + i + 1) >> 8;
            data[i * FAT_ENTRY_SIZE + 2] = (first_cluster + i + 1) >> 16;
            data[i * FAT_ENTRY_SIZE + 3] = (first_cluster + i + 1) >> 24;
        }
        if (i < FAT_ENTRIES_PER_PAGE && first_cluster + i + 1 - LOG_FILE_FIRST_CLUSTER == used_cluster_count) {
            data[i * FAT_ENTRY_SIZE] = 0xFF;
            data[i * FAT_ENTRY_SIZE + 1] = 0xFF;
            data[i * FAT_ENTRY_SIZE + 2] = 0xFF;
            data[i * FAT_ENTRY_SIZE + 3] = 0x0F;
            ++i;
        }
        for (; i < FAT_ENTRIES_PER_PAGE; ++i) {
            data[i * FAT_ENTRY_SIZE] = 0x00;
            data[i * FAT_ENTRY_SIZE + 1] = 0x00;
            data[i * FAT_ENTRY_SIZE + 2] = 0x00;
            data[i * FAT_ENTRY_SIZE + 3] = 0x00;
        }

        return FLASH_CTRL_STATUS_OK;
    } else if (page >= DIR_TABLE_FIRST_PAGE
            && page < DIR_TABLE_FIRST_PAGE + DIR_TABLE_LEN) {
        memcpy(data, dir_table, S25FL_PAGE_SIZE);

        // Log file size is variable.
        // The size read from the directory table must correspond to its size stored in FAT.
        if (page == DIR_TABLE_LOG_FILE_SIZE_PAGE) {
            uint32_t last_logged_page;
            FlashCtrlStatus status = read_last_logged_page(&last_logged_page);
            if (status != FLASH_CTRL_STATUS_OK) {
                return status;
            }

            uint32_t used_cluster_count = (last_logged_page - LOG_FILE_FIRST_PAGE) / PAGES_PER_CLUSTER + 1;
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET] = (used_cluster_count * S25FL_PAGE_SIZE);
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET + 1] = (used_cluster_count * S25FL_PAGE_SIZE) >> 8;
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET + 2] = (used_cluster_count * S25FL_PAGE_SIZE) >> 16;
            data[DIR_TABLE_LOG_FILE_SIZE_OFFSET + 3] = (used_cluster_count * S25FL_PAGE_SIZE) >> 24;
        }

        return FLASH_CTRL_STATUS_OK;
    } else if (page >= EMULATED_REGION_FIRST_PAGE
            && page < EMULATED_REGION_FIRST_PAGE + EMULATED_REGION_LEN) {
        // Remaining parts of the emulated region are handled here, e.g. FAT32 reserved sectors.
        memset(data, 0x00, S25FL_PAGE_SIZE);
        return FLASH_CTRL_STATUS_OK;
    } else if (page >= DATA_REGION_FIRST_PAGE
            && page < DATA_REGION_FIRST_PAGE + DATA_REGION_LEN) {
        // Map config file to the first sector first page.
        if (page >= CONFIG_FILE_FIRST_PAGE
                && page < CONFIG_FILE_FIRST_PAGE + CONFIG_FILE_LEN) {
            page = page - CONFIG_FILE_FIRST_PAGE;
            //return SS_flash_ctrl_read_config_file(page, data);
        }

        S25flStatus s25fl_status = SS_s25fl_read_page_dma(page, data);
        return translate_s25fl_status(s25fl_status);
    }

    return FLASH_CTRL_STATUS_ERR;
}

static FlashCtrlStatus translate_s25fl_status(S25flStatus s25fl_status) {
    switch (s25fl_status) {
        case S25FL_STATUS_OK: return FLASH_CTRL_STATUS_OK;
        case S25FL_STATUS_ERR:
        default: return S25FL_STATUS_ERR;
    }
}

static FlashCtrlStatus translate_hal_status(HAL_StatusTypeDef hal_status) {
    switch (hal_status) {
        case HAL_OK: return FLASH_CTRL_STATUS_OK;
        case HAL_ERROR:
        default: return FLASH_CTRL_STATUS_ERR;
    }
}

static int32_t get_first_set_bit(uint8_t byte) {
    for (uint32_t i = 0; i < 8; ++i) {
        if (byte & (1 << i)) {
            return i;
        }
    }

    return -1;
}