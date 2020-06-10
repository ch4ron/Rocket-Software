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
#include "stm32f4xx_hal.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
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

void SS_grazyna_init(UART_HandleTypeDef *huart);
void SS_grazyna_transmit(ComFrame *frame);
void SS_grazyna_disable(void);
void SS_grazyna_enable(void);
bool SS_grazyna_is_enabled(void);
void SS_grazyna_tx_handler_task(void *pvParameters);
void SS_grazyna_tx_isr(void);

#endif /* SS_GRAZYNA_H */
