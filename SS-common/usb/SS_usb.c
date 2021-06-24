/**
  * SS_usb.c
  *
  *  Created on: June 6, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_usb.h"
#include "FreeRTOS.h"
#include "task.h"

static Fatmap fatmap;
static FatmapConfig fatmap_cfg;
static bool is_enabled = false;

UsbStatus SS_usb_init(void)
{
    if (SS_fatmap_init(&fatmap, &fatmap_cfg) != FATMAP_STATUS_OK) {
        return USB_STATUS_ERR;
    }

    return USB_STATUS_OK;
}

UsbStatus SS_usb_start(void)
{
    /*HAL_StatusTypeDef hal_status = USB_DevConnect(USB_OTG_FS);
    if (hal_status != HAL_OK) {
        return USB_STATUS_ERR;
    }*/
    if (is_enabled) {
        return USB_STATUS_DISABLED;
    }

    if (SS_fatmap_start(&fatmap) != FATMAP_STATUS_OK) {
        return USB_STATUS_ERR;
    }

    is_enabled = true;
    return USB_STATUS_OK;
}

UsbStatus SS_usb_stop(void)
{
    /*HAL_StatusTypeDef hal_status = USB_DevDisconnect(USB_OTG_FS);
    if (hal_status != HAL_OK) {
        return USB_STATUS_ERR;
    }*/

    if (!is_enabled) {
        return USB_STATUS_DISABLED;
    }

    if (SS_fatmap_stop(&fatmap) != FATMAP_STATUS_OK) {
        return USB_STATUS_ERR;
    }

    is_enabled = false;
    return USB_STATUS_OK;
}

void SS_usb_start_task(void *pvParameters) {
    /* Usb has to be started after initializing log streams */
    SS_usb_start();
    vTaskDelete(NULL);
}

Fatmap *SS_usb_get_fatmap(void)
{
    return &fatmap;
}

bool SS_usb_get_is_enabled(void)
{
    return is_enabled;
}
