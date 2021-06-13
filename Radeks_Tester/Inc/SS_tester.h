/**
  * SS_tester.c
  *
  *  Created on:
  *      Author: PR
 **/

#ifndef ROCKETSOFTWARE_SS_TESTER_H
#define ROCKETSOFTWARE_SS_TESTER_H

#include "FreeRTOS.h"
#include "task.h"

void SS_tester_init();
void SS_tester_MainTask(void *pvParameters);
void SS_tester_simDataManagementTask(void);
void SS_tester_SPIHandlerTask(void *pvParameters);
void SS_tester_BinDataDecoder_decodeHandlerTask(void *pvParameters);
void SS_tester_PROMRead(uint8_t RxBuff);
void SS_tester_SPIReceiveIT();
void SS_tester_SPI_ISR();
void SS_tester_USB_ISR(uint8_t * RxBufferFS);

#endif  //ROCKETSOFTWARE_SS_TESTER_H
