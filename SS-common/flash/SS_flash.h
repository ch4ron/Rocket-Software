/**
  * SS_flash.h
  *
  *  Created on: May 25, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_FLASH_H
#define SS_FLASH_H

// XXX: Is this necessary?
#include "SS_s25fl.h"

#define SS_FLASH_MAX_VAR_DATA_SIZE 16

typedef enum
{
    FLASH_STATUS_OK,
    FLASH_STATUS_ERR,
    FLASH_STATUS_BUSY,
    FLASH_STATUS_DISABLED,
    FLASH_STATUS_WRITE_PROTECTED,
    FLASH_STATUS_STREAM_OVERFLOW,
    FLASH_STATUS_BUF_OVERFLOW,
    FLASH_STATUS_COUNT,
}FlashStatus;

typedef enum
{
    FLASH_STREAM_VAR,
    FLASH_STREAM_TEXT,
    FLASH_STREAM_COUNT,
}FlashStream;

#include "SS_flash_ctrl.h"
#include "SS_flash_log.h"
#include "SS_flash_caching.h"

FlashStatus SS_flash_init(QSPI_HandleTypeDef *hqspi, GPIO_TypeDef *nrst_gpio, uint16_t nrst_pin);
FlashStatus SS_flash_translate_hal_status(HAL_StatusTypeDef hal_status);
FlashStatus SS_flash_translate_s25fl_status(S25flStatus s25fl_status);

#endif /* SS_FLASH_H */
