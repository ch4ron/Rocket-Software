/*
 * SS_sequence_test.c
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#include "SS_sequence.c"

#include "unity_fixture.h"

TEST_GROUP(sequence);

TEST_GROUP_RUNNER(sequence) {
    RUN_TEST_CASE(sequence, add_single);
    RUN_TEST_CASE(sequence, add_multiple);
    RUN_TEST_CASE(sequence, add_in_the_front);
    RUN_TEST_CASE(sequence, add_in_the_middle);
    RUN_TEST_CASE(sequence, add_in_the_middle_almost_full);
    /* Note that current implementation removes the element with highest time when the array is full */
    RUN_TEST_CASE(sequence, add_in_the_middle_full);
    RUN_TEST_CASE(sequence, add_same_time);
    RUN_TEST_CASE(sequence, add_full_array);
    RUN_TEST_CASE(sequence, clear);
    /* RUN_TEST_CASE(sequence, run); */
}

TEST_SETUP(sequence) {}

TEST_TEAR_DOWN(sequence) {
    SS_sequence_clear();
}

extern SequenceItem sequence_items[MAX_SEQUENCE_ITEMS];

TEST(sequence, add_single) {
    void (*func)(uint32_t) = (void (*)(uint32_t)) 3333; // Dummy pointer, do not call it!
    SS_sequence_add(func, 99, 1000);
    TEST_ASSERT_EQUAL_PTR(func, sequence_items[0].func);
    TEST_ASSERT_EQUAL(99, sequence_items[0].value);
    TEST_ASSERT_EQUAL(1000, sequence_items[0].time);
}

TEST(sequence, add_multiple) {
    void (*func[MAX_SEQUENCE_ITEMS])(uint32_t);
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        func[i] = (void (*)(uint32_t)) i + 1; // Dummy pointer, do not call it!
        SS_sequence_add(func[i], i*5, i*6);
    }
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        TEST_ASSERT_EQUAL_PTR(func[i], sequence_items[i].func);
        TEST_ASSERT_EQUAL(i*5, sequence_items[i].value);
        TEST_ASSERT_EQUAL(i*6, sequence_items[i].time);
    }
}

TEST(sequence, add_in_the_front) {
    void (*func[3])(uint32_t);
    func[0] = (void (*)(uint32_t)) 100; // Dummy pointer, do not call it!
    SS_sequence_add(func[0], 1000, 2000);
    func[1] = (void (*)(uint32_t)) 110; // Dummy pointer, do not call it!
    SS_sequence_add(func[1], 1100, 3000);
    func[2] = (void (*)(uint32_t)) 120; // Dummy pointer, do not call it!
    SS_sequence_add(func[2], 1200, 1000);
    TEST_ASSERT_EQUAL_PTR(func[2], sequence_items[0].func);
    TEST_ASSERT_EQUAL(1200, sequence_items[0].value);
    TEST_ASSERT_EQUAL(1000, sequence_items[0].time);
    TEST_ASSERT_EQUAL_PTR(func[0], sequence_items[1].func);
    TEST_ASSERT_EQUAL(1000, sequence_items[1].value);
    TEST_ASSERT_EQUAL(2000, sequence_items[1].time);
    TEST_ASSERT_EQUAL_PTR(func[1], sequence_items[2].func);
    TEST_ASSERT_EQUAL(1100, sequence_items[2].value);
    TEST_ASSERT_EQUAL(3000, sequence_items[2].time);
    TEST_ASSERT_EQUAL_PTR(0, sequence_items[3].func);
    TEST_ASSERT_EQUAL(0, sequence_items[3].value);
    TEST_ASSERT_EQUAL(0, sequence_items[3].time);
}

TEST(sequence, add_in_the_middle) {
    void (*func[3])(uint32_t);
    func[0] = (void (*)(uint32_t)) 100; // Dummy pointer, do not call it!
    SS_sequence_add(func[0], 1000, 1000);
    func[1] = (void (*)(uint32_t)) 110; // Dummy pointer, do not call it!
    SS_sequence_add(func[1], 1100, 3000);
    func[2] = (void (*)(uint32_t)) 120; // Dummy pointer, do not call it!
    SS_sequence_add(func[2], 1200, 2000);
    TEST_ASSERT_EQUAL_PTR(func[0], sequence_items[0].func);
    TEST_ASSERT_EQUAL(1000, sequence_items[0].value);
    TEST_ASSERT_EQUAL(1000, sequence_items[0].time);
    TEST_ASSERT_EQUAL_PTR(func[2], sequence_items[1].func);
    TEST_ASSERT_EQUAL(1200, sequence_items[1].value);
    TEST_ASSERT_EQUAL(2000, sequence_items[1].time);
    TEST_ASSERT_EQUAL_PTR(func[1], sequence_items[2].func);
    TEST_ASSERT_EQUAL(1100, sequence_items[2].value);
    TEST_ASSERT_EQUAL(3000, sequence_items[2].time);
    TEST_ASSERT_EQUAL_PTR(0, sequence_items[3].func);
    TEST_ASSERT_EQUAL(0, sequence_items[3].value);
    TEST_ASSERT_EQUAL(0, sequence_items[3].time);
}

TEST(sequence, add_same_time) {
    void (*func[3])(uint32_t);
    func[0] = (void (*)(uint32_t)) 100; // Dummy pointer, do not call it!
    SS_sequence_add(func[0], 1000, 2000);
    func[1] = (void (*)(uint32_t)) 110; // Dummy pointer, do not call it!
    SS_sequence_add(func[1], 1100, 3000);
    func[2] = (void (*)(uint32_t)) 120; // Dummy pointer, do not call it!
    SS_sequence_add(func[2], 1200, 2000);
    TEST_ASSERT_EQUAL_PTR(func[0], sequence_items[0].func);
    TEST_ASSERT_EQUAL(1000, sequence_items[0].value);
    TEST_ASSERT_EQUAL(2000, sequence_items[0].time);
    TEST_ASSERT_EQUAL_PTR(func[2], sequence_items[1].func);
    TEST_ASSERT_EQUAL(1200, sequence_items[1].value);
    TEST_ASSERT_EQUAL(2000, sequence_items[1].time);
    TEST_ASSERT_EQUAL_PTR(func[1], sequence_items[2].func);
    TEST_ASSERT_EQUAL(1100, sequence_items[2].value);
    TEST_ASSERT_EQUAL(3000, sequence_items[2].time);
    TEST_ASSERT_EQUAL_PTR(0, sequence_items[3].func);
    TEST_ASSERT_EQUAL(0, sequence_items[3].value);
    TEST_ASSERT_EQUAL(0, sequence_items[3].time);
}

TEST(sequence, add_in_the_middle_almost_full) {
    void (*func[MAX_SEQUENCE_ITEMS])(uint32_t);
    for(int i = 0; i < MAX_SEQUENCE_ITEMS - 1; i++) {
        func[i] = (void (*)(uint32_t)) i + 1; // Dummy pointer, do not call it!
        SS_sequence_add(func[i], i*5, i*6);
    }
    SS_sequence_add(func[MAX_SEQUENCE_ITEMS - 1], 10, 8);
    TEST_ASSERT_EQUAL_PTR(func[0], sequence_items[0].func);
    TEST_ASSERT_EQUAL(0, sequence_items[0].value);
    TEST_ASSERT_EQUAL(0, sequence_items[0].time);
    TEST_ASSERT_EQUAL_PTR(func[1], sequence_items[1].func);
    TEST_ASSERT_EQUAL(5, sequence_items[1].value);
    TEST_ASSERT_EQUAL(6, sequence_items[1].time);
    TEST_ASSERT_EQUAL_PTR(func[MAX_SEQUENCE_ITEMS - 1], sequence_items[2].func);
    TEST_ASSERT_EQUAL(10, sequence_items[2].value);
    TEST_ASSERT_EQUAL(8, sequence_items[2].time);
    for(int i = 3; i < MAX_SEQUENCE_ITEMS; i++) {
        TEST_ASSERT_EQUAL_PTR(func[i-1], sequence_items[i].func);
        TEST_ASSERT_EQUAL((i-1)*5, sequence_items[i].value);
        TEST_ASSERT_EQUAL((i-1)*6, sequence_items[i].time);
    }
}

TEST(sequence, add_full_array) {
    void (*func[MAX_SEQUENCE_ITEMS + 1])(uint32_t);
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        func[i] = (void (*)(uint32_t)) i + 1; // Dummy pointer, do not call it!
        SS_sequence_add(func[i], i*5, i*6);
    }
    SS_sequence_add(func[MAX_SEQUENCE_ITEMS], MAX_SEQUENCE_ITEMS*10, MAX_SEQUENCE_ITEMS*11);
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        TEST_ASSERT_EQUAL_PTR(func[i], sequence_items[i].func);
        TEST_ASSERT_EQUAL(i*5, sequence_items[i].value);
        TEST_ASSERT_EQUAL(i*6, sequence_items[i].time);
    }
}

TEST(sequence, add_in_the_middle_full) {
    void (*func[MAX_SEQUENCE_ITEMS+1])(uint32_t);
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        func[i] = (void (*)(uint32_t)) i + 1; // Dummy pointer, do not call it!
        SS_sequence_add(func[i], i*5, i*6);
    }
    SS_sequence_add(func[MAX_SEQUENCE_ITEMS], 10, 8);
    TEST_ASSERT_EQUAL_PTR(func[0], sequence_items[0].func);
    TEST_ASSERT_EQUAL(0, sequence_items[0].value);
    TEST_ASSERT_EQUAL(0, sequence_items[0].time);
    TEST_ASSERT_EQUAL_PTR(func[1], sequence_items[1].func);
    TEST_ASSERT_EQUAL(5, sequence_items[1].value);
    TEST_ASSERT_EQUAL(6, sequence_items[1].time);
    TEST_ASSERT_EQUAL_PTR(func[MAX_SEQUENCE_ITEMS], sequence_items[2].func);
    TEST_ASSERT_EQUAL(10, sequence_items[2].value);
    TEST_ASSERT_EQUAL(8, sequence_items[2].time);
    for(int i = 3; i < MAX_SEQUENCE_ITEMS; i++) {
        TEST_ASSERT_EQUAL_PTR(func[i-1], sequence_items[i].func);
        TEST_ASSERT_EQUAL((i-1)*5, sequence_items[i].value);
        TEST_ASSERT_EQUAL((i-1)*6, sequence_items[i].time);
    }
}

TEST(sequence, clear) {
    void (*func[MAX_SEQUENCE_ITEMS])(uint32_t);
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        func[i] = (void (*)(uint32_t)) i + 1; // Dummy pointer, do not call it!
        SS_sequence_add(func[i], i*5, i*6);
    }
    SS_sequence_clear();
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        TEST_ASSERT_EQUAL_PTR(0, sequence_items[i].func);
        TEST_ASSERT_EQUAL(0, sequence_items[i].value);
        TEST_ASSERT_EQUAL(0, sequence_items[i].time);
    }
}

static uint32_t last_value;
static void fun(uint32_t value) {
    last_value = value;
}

TEST(sequence, run) {
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        SS_sequence_add(fun, i + 1, (i + 1) * (MAX_SEQUENCE_ITEMS + 5));
    }
    SS_sequence_start();
    for(int i = 0; i < MAX_SEQUENCE_ITEMS; i++) {
        HAL_Delay(MAX_SEQUENCE_ITEMS + 5);
        TEST_ASSERT_EQUAL_UINT32(i + 1, last_value);
    }
}

void tests(void) {
    RUN_TEST_GROUP(sequence);
}

int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, tests);
}
