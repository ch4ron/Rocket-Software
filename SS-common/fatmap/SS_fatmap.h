#ifndef SS_FATMAP_H
#define SS_FATMAP_H

#include "SS_common.h"

#define FATMAP_PAGE_SIZE 512

typedef enum
{
    FATMAP_STATUS_OK,
    FATMAP_STATUS_ERR,
    FATMAP_STATUS_DISABLED,
}FatmapStatus;

typedef enum
{
    FATMAP_TYPE_FAT8, // Reserved.
    FATMAP_TYPE_FAT12, // Reserved.
    FATMAP_TYPE_FAT16, // Reserved.
    FATMAP_TYPE_FAT32,
    FATMAP_TYPE_EXFAT, // Reserved.
}FatmapType;

typedef struct FatmapFile
{
    struct FatmapFile *prev, *next;
    uint32_t size;
    const char *path;
}FatmapFile;

typedef struct FatmapDir
{
    struct FatmapDir *prev, *next;
    struct FatmapDir *subdirs;
    FatmapFile *files;
}FatmapDir;

typedef struct
{
    FatmapType type;
    uint32_t page_size; // Unused at the moment.
    uint32_t page_count;
    uint32_t pages_per_cluster;
    FatmapStatus (*write)(FatmapFile *file, uint32_t off, const void *buf, uint32_t size);
    FatmapStatus (*read)(FatmapFile *file, uint32_t off, void *buf, uint32_t size);
    FatmapStatus (*get_size)(FatmapFile *file, uint32_t *size);
}FatmapConfig;

typedef struct
{
    /*const*/ FatmapConfig *cfg;
    FatmapDir root;
}Fatmap;

FatmapStatus SS_fatmap_init(Fatmap *fm, FatmapConfig *cfg);
FatmapStatus SS_fatmap_start(Fatmap *fm);
FatmapStatus SS_fatmap_stop(Fatmap *fm);

FatmapStatus SS_fatmap_mkfile(Fatmap *fm, FatmapFile *file, const char *path);
FatmapStatus SS_fatmap_rmfile(Fatmap *fm, FatmapFile *file);

FatmapStatus SS_fatmap_mkdir(Fatmap *fm, FatmapDir *dir, const char *path);
FatmapStatus SS_fatmap_rmdir(Fatmap *fm, FatmapDir *dir);

FatmapStatus SS_fatmap_write(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size);
FatmapStatus SS_fatmap_read(Fatmap *fm, uint32_t page, uint32_t off, uint8_t *data, uint32_t size);

#endif /* SS_FATMAP_H */
