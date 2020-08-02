/*
 * SS_sequence.h
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#ifndef SS_SEQUENCE_H_
#define SS_SEQUENCE_H_

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com_ids.h"
#include "SS_com.h"
#include "stdint.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

#define MAX_SEQUENCE_ITEMS 25

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum {
    COM_SEQUENCE_CLEAR = 0x01,
    COM_SEQUENCE_START = 0x02,
    COM_SEQUENCE_ABORT = 0x03
} ComSequenceID;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_sequence_init(void);
void SS_sequence_clear(void);
void SS_sequence_start(void);
int8_t SS_sequence_add(ComDeviceID device, uint8_t id, uint8_t operation, int16_t value, int16_t time);
ComStatus SS_sequence_com_service(ComFrame *frame);
void SS_sequence_task(void *pvParameters);

#endif /* SS_SEQUENCE_H_ */
