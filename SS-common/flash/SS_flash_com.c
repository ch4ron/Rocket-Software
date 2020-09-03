/*
 * SS_flash_com.c
 *
 *  Created on: 03.09.2020
 *      Author: Maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_COM

#include "SS_flash_com.h"

#include "string.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

ComStatus SS_flash_com_service(ComFrame *frame) {
    ComFlashID msgID = frame->operation;
    switch(msgID) {
        case COM_FLASH_ERASE:
            SS_println("Erase");
            break;
        case COM_FLASH_PURGE:
            SS_println("Purge");
            break;
        case COM_FLASH_START_LOGGING:
            SS_println("Start logging");
            break;
        case COM_FLASH_STOP_LOGGING:
            SS_println("Stop logging");
            break;
        default:
            SS_error("Unhandled Grazyna flash service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

#endif /* SS_USE_COM */

