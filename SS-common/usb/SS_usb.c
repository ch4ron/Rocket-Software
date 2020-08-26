/**
  * SS_usb.c
  *
  *  Created on: June 6, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_usb.h"

static bool is_enabled = true;

UsbStatus SS_usb_stop(void)
{
    /*HAL_StatusTypeDef hal_status = USB_DevDisconnect(USB_OTG_FS);
    if (hal_status != HAL_OK) {
        return USB_STATUS_ERR;
    }*/

    is_enabled = false;
    return USB_STATUS_OK;
}

UsbStatus SS_usb_start(void)
{
    /*HAL_StatusTypeDef hal_status = USB_DevConnect(USB_OTG_FS);
    if (hal_status != HAL_OK) {
        return USB_STATUS_ERR;
    }*/

    is_enabled = true;
    return USB_STATUS_OK;
}

bool SS_usb_get_is_enabled(void)
{
    return is_enabled;
}
