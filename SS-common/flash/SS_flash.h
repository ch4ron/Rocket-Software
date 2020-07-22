/**
  * SS_flash.h
  *
  *  Created on: May 25, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_FLASH_H
#define SS_FLASH_H

#include "stm32f4xx_hal.h"

#ifndef FLASH_PAGE_BUF_SIZE
#define FLASH_PAGE_BUF_SIZE 512
#endif

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

FlashStatus SS_flash_init(QSPI_HandleTypeDef *hqspi, GPIO_TypeDef *nrst_gpio, uint16_t nrst_pin);

#endif /* SS_FLASH_H */
