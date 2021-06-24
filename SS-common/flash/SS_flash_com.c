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
#include "SS_s25fl.h"
#include "SS_flash_log.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

ComStatus SS_flash_com_service(ComFrame *frame) {
    ComFlashID msgID = frame->operation;
    SS_println("test-frame received");
    switch(msgID) {
        case COM_FLASH_ERASE:
            SS_println("Purge");
            SS_s25fl_erase_all();
            SS_println("Purged");
            break;
        case COM_FLASH_PURGE:
            SS_println("Purge");
            SS_s25fl_erase_all();
            SS_println("Purged");
            break;
        case COM_FLASH_START_LOGGING:
            SS_println("Start logging");
            SS_flash_stream_start("vars.bin");
            break;
        case COM_FLASH_STOP_LOGGING:
            SS_println("Stop logging");
            SS_flash_stream_stop("vars.bin");
            break;
        default:
            SS_error("Unhandled Grazyna flash service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

#ifdef SS_USE_SEQUENCE

ComStatus SS_flash_com_sequence_validate(ComFrame *frame) {
    switch(frame->operation) {
        case COM_FLASH_START_LOGGING:
        case COM_FLASH_STOP_LOGGING:
            return COM_OK;
        default:
            return COM_ERROR;
    }
}

ComStatus SS_flash_sequence(uint8_t id, uint8_t operation, int16_t value) {

    switch(operation) {
        case COM_FLASH_START_LOGGING:
            SS_println("Start logging");
            SS_flash_stream_start("vars.bin");
            break;
        case COM_FLASH_STOP_LOGGING:
            SS_println("Stop logging");
            SS_flash_stream_stop("vars.bin");
            break;
        default:
            return COM_ERROR;
    }
    return COM_OK;
}

void SS_flash_sequence_finish(uint8_t id) {

    SS_println("Stop logging");
    SS_flash_stream_stop("vars.bin");
}

#endif /* SS_USE_SEQUENCE */


#endif /* SS_USE_COM */

