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

#ifdef SS_USE_SEQUENCE

ComStatus SS_flash_com_sequence_validate(ComFrame *frame);
ComStatus SS_flash_sequence(uint8_t id, uint8_t operation, int16_t value);
void SS_flash_sequence_finish(uint8_t id);

#endif

#endif

#endif
