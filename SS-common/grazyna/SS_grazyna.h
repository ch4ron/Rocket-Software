/*
 * SS_grazyna.h
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#ifndef SS_GRAZYNA_H
#define SS_GRAZYNA_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com.h"
#include "stdbool.h"

/* ==================================================================== */
/* ============================= Defines ============================== */
/* ==================================================================== */

#define GRAZYNA_HEADER 0x05

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef struct __attribute__((packed)) {
    uint8_t header;
    ComFrame com_frame;
    uint32_t crc;
} GrazynaFrame;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

/* huart is of type UART_HandleTypedef */
void SS_grazyna_init(void *huart);
void SS_grazyna_transmit(ComFrame *frame);
void SS_grazyna_disable();
void SS_grazyna_enable();
bool SS_grazyna_is_enabled();

#endif /* SS_GRAZYNA_H */
