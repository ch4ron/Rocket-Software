/**
  * SS_usb.h
  *
  *  Created on: June 6, 2020
  *      Author: Mikolaj Wielgus
 **/

#ifndef SS_USB_H
#define SS_USB_H

// TODO: Use a common header for stuff like 'stdbool.h', 'stdio.h' and 'stdint.h'.
#include "main.h"

typedef enum
{
    USB_STATUS_OK,
    USB_STATUS_ERR,
}UsbStatus;

UsbStatus SS_usb_stop(void);
UsbStatus SS_usb_start(void);
bool SS_usb_get_is_enabled(void);

#endif /* SS_USB_H */
