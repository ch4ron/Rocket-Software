/*
 * SS_flash_com.h
 *
 *  Created on: 03.09.2020
 *      Author: Maciek
 */

#ifndef SS_FLASH_COM_H
#define SS_FLASH_COM_H

#ifdef SS_USE_COM

#include "SS_com.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum {
    COM_FLASH_ERASE = 0x01,
    COM_FLASH_PURGE,
    COM_FLASH_START_LOGGING,
    COM_FLASH_STOP_LOGGING,
} ComFlashID;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

ComStatus SS_flash_com_service(ComFrame *frame);

#endif

#endif
