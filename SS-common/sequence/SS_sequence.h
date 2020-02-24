/*
 * SS_sequence.h
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#ifndef SS_SEQUENCE_H_
#define SS_SEQUENCE_H_

#include "stm32f4xx_hal.h"

#define MAX_SEQUENCE_ITEMS 25

typedef struct {
    void (*func)(uint32_t);
    uint32_t value;
    uint32_t time;
} SequenceItem;

void SS_sequence_add(void (*func)(uint32_t), uint32_t value, uint32_t time);
void SS_sequence_clear();
void SS_sequence_start();
void SS_sequence_SYSTICK();

#endif /* SS_SEQUENCE_H_ */
