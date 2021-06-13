/**
  * SS_tester.c
  *
  *      Author: PR
 **/

#include "SS_tester.h"
#include "FreeRTOS.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "spi.h"
#include "usbd_cdc_if.h"
#include "SS_receivedSimBinaryDataDecoder.h"

SemaphoreHandle_t xSemaphore_SPIISR;
SemaphoreHandle_t xSemaphore_decodeISR;
uint8_t RxBuff;
uint8_t * pointerUserRxBufferFS;
static uint8_t checkStart = 0;

void SS_tester_init()
{
    xSemaphore_SPIISR = xSemaphoreCreateBinary();
    xSemaphore_decodeISR = xSemaphoreCreateBinary();
    SS_tester_SPIReceiveIT();
    SS_BinDataDecoder_init();
}

void SS_tester_MainTask(void *pvParameters)
{
    uint32_t justToGetFromBufs;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS( 100));
        SS_print("*MAIN TASK*!\r\n");
        SS_print("Second cell value: %d\r\n", SS_BinDataDecoder_GetMS56Pres());
        SS_print("Count: %u \r\n", SS_BinDataDecoder_isNumberOfDataBelowMin());
        justToGetFromBufs = SS_BinDataDecoder_GetMS56Temp();
        justToGetFromBufs = SS_BinDataDecoder_GetMS58Temp();
        justToGetFromBufs = SS_BinDataDecoder_GetMS58Pres();
    }
    vTaskDelete( NULL );
}

void SS_tester_simDataManagementTask(void)
{
    uint8_t counter = 0;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS( 20));
        if (checkStart == 1)
        {
            if (counter < 100)
            {
                counter++;
            }
            else
            {
                if (SS_BinDataDecoder_isNumberOfDataBelowMin())
                {
                    char DataToSend[] = "NEED";
                    CDC_Transmit_FS(DataToSend, 4);
                    counter = 0;
                }
            }
        }
    }
    vTaskDelete( NULL );
}

void SS_tester_SPIHandlerTask(void *pvParameters)
{
    while(1)
    {
        xSemaphoreTake( xSemaphore_SPIISR, portMAX_DELAY );
        switch(RxBuff)
        {
            case 0x1E:      // MS56_RESET - 0x1E
                SS_tester_SPIReceiveIT();
                break;
            case 0xA0:      // MS56_PROM_READ_BASE -  0xA0
            case 0xA2:      // Next address of PROM_READ_BASE
            case 0xA4:      // Next address of PROM_READ_BASE
            case 0xA6:      // Next address of PROM_READ_BASE
            case 0xA8:      // Next address of PROM_READ_BASE
            case 0xAA:      // Next address of PROM_READ_BASE
            case 0xAC:      // Next address of PROM_READ_BASE
            case 0xAE:      // Next address of PROM_READ_BASE
                SS_tester_PROMRead(RxBuff);
                break;
        }
    }
    vTaskDelete( NULL );
}

void SS_tester_BinDataDecoder_decodeHandlerTask(void *pvParameters)
{
    while(1)
    {
        xSemaphoreTake( xSemaphore_decodeISR, portMAX_DELAY );
        SS_BinDataDecoder_decode(pointerUserRxBufferFS);
    }
    vTaskDelete( NULL );
}

void SS_tester_PROMRead(uint8_t RxBuff)
{
    uint8_t TxBuff[2];
    switch(RxBuff)
    {
        case 0xA0:      // MS56_PROM_READ_BASE -  0xA0
            TxBuff[0] = 0xFF;
            TxBuff[1] = 0xFF;   // d65535
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xA2:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x9E;
            TxBuff[1] = 0x75;   // d40565
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xA4:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x92;
            TxBuff[1] = 0x31;   // d37425
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xA6:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x60;
            TxBuff[1] = 0xF7;   // d24823
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xA8:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x57;
            TxBuff[1] = 0x98;   // d22424
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xAA:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x7D;
            TxBuff[1] = 0xD6;   // d32214
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xAC:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x6D;
            TxBuff[1] = 0x38;   // d27960
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
        case 0xAE:      // Next address of PROM_READ_BASE
            TxBuff[0] = 0x37;
            TxBuff[1] = 0x0A;   // d14090
            HAL_SPI_Transmit_DMA(&hspi1, &TxBuff, 2);
            break;
    }
    TxBuff[0] = 0;
    TxBuff[1] = 0;
}

void SS_tester_SPIReceiveIT()
{
    HAL_SPI_Receive_IT(&hspi1, &RxBuff, 1);
}

void SS_tester_SPI_ISR()
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(xSemaphore_SPIISR, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void SS_tester_USB_ISR(uint8_t * RxBufferFS)
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    checkStart = 1;
    pointerUserRxBufferFS = RxBufferFS;
    xSemaphoreGiveFromISR(xSemaphore_decodeISR, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}