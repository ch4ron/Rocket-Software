/*
 * SS_sequence_test.c
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#include "unity_fixture.h"
#include "SS_sequence.c"


TEST_GROUP(sequence);

TEST_GROUP_RUNNER(sequence) {
    RUN_TEST_CASE(sequence, add_single);
    RUN_TEST_CASE(sequence, add_multiple);
    RUN_TEST_CASE(sequence, add_multiple2);
    RUN_TEST_CASE(sequence, add_in_the_front);
    RUN_TEST_CASE(sequence, add_in_the_back);
    RUN_TEST_CASE(sequence, add_in_the_middle);
    RUN_TEST_CASE(sequence, add_same_time);
    RUN_TEST_CASE(sequence, add_full_array);
    RUN_TEST_CASE(sequence, clear);
    /* RUN_TEST_CASE(sequence, run); */
}

TEST_SETUP(sequence) {}

TEST_TEAR_DOWN(sequence) {
    SS_sequence_clear();
}

static void check_sequence_values(uint8_t item, ComDeviceID device, uint8_t id, uint8_t operation, int16_t value, int16_t time) {
    TEST_ASSERT_EQUAL(device, sequence.items[item].device);
    TEST_ASSERT_EQUAL(id, sequence.items[item].id);
    TEST_ASSERT_EQUAL(operation, sequence.items[item].operation);
    TEST_ASSERT_EQUAL(value, sequence.items[item].value);
    TEST_ASSERT_EQUAL(time, sequence.items[item].time);
}

TEST(sequence, add_single) {
    SS_sequence_add(1, 2, 3, 4, 5);
    check_sequence_values(0, 1, 2, 3, 4, 5);
}

TEST(sequence, add_multiple) {
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        SS_sequence_add(2, i, 2*i, 6*i, 9*i);
    }
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        check_sequence_values(i, 2, i, 2*i, 6*i, 9*i);
    }
}

TEST(sequence, add_multiple2) {
    for(int i = MAX_SEQUENCE_ITEMS-1; i >= 0; i--) {
        SS_sequence_add(3, i, 2*i, 6*i, 9*i);
    }
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        check_sequence_values(i, 3, i, 2*i, 6*i, 9*i);
    }
}

TEST(sequence, add_in_the_back) {
    SS_sequence_add(3, 3, 3, 1200, 1000);
    SS_sequence_add(1, 1, 1, 1000, 2000);
    SS_sequence_add(2, 2, 2, 1100, 3000);
    check_sequence_values(0, 3, 3, 3, 1200, 1000);
    check_sequence_values(1, 1, 1, 1, 1000, 2000);
    check_sequence_values(2, 2, 2, 2, 1100, 3000);
}

TEST(sequence, add_in_the_front) {
    SS_sequence_add(1, 1, 1, 1000, 2000);
    SS_sequence_add(2, 2, 2, 1100, 3000);
    SS_sequence_add(3, 3, 3, 1200, 1000);
    check_sequence_values(0, 3, 3, 3, 1200, 1000);
    check_sequence_values(1, 1, 1, 1, 1000, 2000);
    check_sequence_values(2, 2, 2, 2, 1100, 3000);
}

TEST(sequence, add_in_the_middle) {
    SS_sequence_add(3, 3, 3, 1200, 1000);
    SS_sequence_add(2, 2, 2, 1100, 3000);
    SS_sequence_add(1, 1, 1, 1000, 2000);
    check_sequence_values(0, 3, 3, 3, 1200, 1000);
    check_sequence_values(1, 1, 1, 1, 1000, 2000);
    check_sequence_values(2, 2, 2, 2, 1100, 3000);
}

TEST(sequence, add_same_time) {
    SS_sequence_add(2, 2, 2, 1100, 3000);
    SS_sequence_add(1, 1, 1, 1000, 3000);
    SS_sequence_add(3, 3, 3, 1200, 1000);
    check_sequence_values(0, 3, 3, 3, 1200, 1000);
    check_sequence_values(1, 2, 2, 2, 1100, 3000);
    check_sequence_values(2, 1, 1, 1, 1000, 3000);
}

TEST(sequence, add_full_array) {
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        SS_sequence_add(3, i, 2*i, 6*i, 9*i);
    }
    SS_sequence_add(3, 6, 3, 1200, 1000);
    /* Discard last element */
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        check_sequence_values(i, 3, i, 2*i, 6*i, 9*i);
    }
}

TEST(sequence, clear) {
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        SS_sequence_add(i, 1, 2*i, 6*i, 9*i);
    }
    SS_sequence_clear();
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        check_sequence_values(0, 0, 0, 0, 0, 0);
    }
    TEST_ASSERT_EQUAL(0, sequence.size);
}
