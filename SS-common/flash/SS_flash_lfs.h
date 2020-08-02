#include "SS_flash.h"
#include "lfs.h"

FlashStatus SS_flash_lfs_init(void);
FlashStatus SS_flash_lfs_format_and_remount(void);
lfs_t *SS_flash_lfs_get(void);
