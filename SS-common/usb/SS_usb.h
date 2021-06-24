/**
  * SS_usb.h
  *
  *  Created on: June 6, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_USB_H
#define SS_USB_H

#include "SS_common.h"
#include "SS_fatmap.h"

typedef enum
{
    USB_STATUS_OK,
    USB_STATUS_ERR,
    USB_STATUS_DISABLED,
}UsbStatus;

UsbStatus SS_usb_init(void);
UsbStatus SS_usb_stop(void);
UsbStatus SS_usb_start(void);
void SS_usb_start_task(void *pvParameters);

Fatmap *SS_usb_get_fatmap(void);
bool SS_usb_get_is_enabled(void);

#endif /* SS_USB_H */
