/*
 * SS_sequence.c
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#include "SS_sequence.h"
#include "string.h"
#include "stdbool.h"
#include "SS_log.h"

typedef struct {
    uint8_t size;
    SequenceItem items[MAX_SEQUENCE_ITEMS];
} Sequence;

static Sequence sequence;

void SS_sequence_add(uint8_t id, uint8_t operation, int16_t value, int16_t time) {
    if(sequence.size >= MAX_SEQUENCE_ITEMS) {
        SS_error("Sequence is full, dropping");
        return;
    }
    SequenceItem new_item = {
        .id = id,
        .operation = operation, 
        .value = value, 
        .time = time
    };
    uint8_t i;
    for(i = 0; i < sequence.size; i++) {
        if(time < sequence.items[i].time) {
            memmove(&sequence.items[i+1], &sequence.items[i], (sequence.size - i)*sizeof(SequenceItem));
            break;
        }
    }
    sequence.items[i] = new_item;
    sequence.size++;
}

void SS_sequence_clear() {
    memset(&sequence, 0, sizeof(sequence));
}

void SS_sequence_start() {
}

void SS_sequence_end() {
}
