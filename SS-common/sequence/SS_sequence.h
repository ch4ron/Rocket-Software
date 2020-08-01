/*
 * SS_sequence.h
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#ifndef SS_SEQUENCE_H_
#define SS_SEQUENCE_H_

#include "stdint.h"

#define MAX_SEQUENCE_ITEMS 25

typedef struct {
    uint8_t id;
    uint8_t operation;
    int16_t value;
    int16_t time;
} SequenceItem;

/* typedef struct { */
/*     int16_t time; */
/*     int16_t value; */
/* } SequenceItemData; */

void SS_sequence_add(uint8_t id, uint8_t operation, int16_t value, int16_t time);
void SS_sequence_clear();
void SS_sequence_start();

#endif /* SS_SEQUENCE_H_ */
