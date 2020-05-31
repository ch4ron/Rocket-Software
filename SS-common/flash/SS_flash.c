/**
  * SS_flash.h
  *
  *  Created on: May 25, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_flash.h"

FlashStatus SS_flash_init(QSPI_HandleTypeDef *hqspi, GPIO_TypeDef *nrst_gpio, uint16_t nrst_pin)
{
    FlashStatus status = SS_flash_ctrl_init();
    if (status != FLASH_STATUS_OK) {
        return status;
    }

    status = SS_flash_log_init();
    if (status != FLASH_STATUS_OK) {
        return status;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_translate_s25fl_status(S25flStatus s25fl_status)
{
    switch (s25fl_status) {
    case S25FL_STATUS_OK:
        return FLASH_STATUS_OK;
    case S25FL_STATUS_ERR:
    default:
        return FLASH_STATUS_ERR;
    }
}
