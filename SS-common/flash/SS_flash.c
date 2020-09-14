/**
  * SS_flash.h
  *
  *  Created on: May 25, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_flash.h"
#include "SS_flash_lfs.h"
#include "SS_flash_log.h"

FlashStatus SS_flash_init(QSPI_HandleTypeDef *hqspi, GPIO_TypeDef *nrst_gpio, uint16_t nrst_pin)
{
    FlashStatus status = SS_flash_lfs_init();
    if (status != FLASH_STATUS_OK) {
        return status;
    }

    return FLASH_STATUS_OK;
}
