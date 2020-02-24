/*
 * SS_sequence.c
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#include "SS_sequence.h"
#include "string.h"
#include "stdbool.h"

SequenceItem sequence_items[MAX_SEQUENCE_ITEMS];
static bool sequence_ongoing;
volatile static uint32_t time_elapsed;
volatile static uint8_t item_number;

static void SS_sequence_parse(void (*func)(uint32_t), uint32_t value, uint32_t time, uint8_t i) {
    sequence_items[i].func = func;
    sequence_items[i].value = value;
    sequence_items[i].time = time;
}

void SS_sequence_add(void (*func)(uint32_t), uint32_t value, uint32_t time) {
    for(uint8_t i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        if(sequence_items[i].func == 0) {
            SS_sequence_parse(func, value, time, i);
            break;
        }
        else if(sequence_items[i].time > time) {
            for(uint8_t j =  MAX_SEQUENCE_ITEMS - 1; j > i; j--) {
                sequence_items[j] = sequence_items[j - 1];
            }
            SS_sequence_parse(func, value, time, i);
            break;
        }
    }
}

void SS_sequence_clear() {
    memset(sequence_items, 0, sizeof(sequence_items));
}

void SS_sequence_start() {
    sequence_ongoing = true;
    time_elapsed = 0;
    item_number = 0;
}

void SS_sequence_end() {
    sequence_ongoing = false;
}

void SS_sequence_SYSTICK() {
    if(!sequence_ongoing) return;
    time_elapsed++;
    if(item_number >= MAX_SEQUENCE_ITEMS || sequence_items[item_number].func == 0) {
        SS_sequence_end();
        return;
    }
    while(sequence_items[item_number].time <= time_elapsed) {
//        printf("t: %d, i: %d\r\n", time_elapsed, item_number);
        sequence_items[item_number].func(sequence_items[item_number].value);
        item_number++;
        if(item_number >= MAX_SEQUENCE_ITEMS || sequence_items[item_number].func == 0) {
            SS_sequence_end();
            return;
        }
    }
}
