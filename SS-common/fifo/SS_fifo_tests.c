/*
 * SS_can_unit_tests.c
 *
 *  Created on: Dec 17, 2019
 *      Author: maciek
 */


#include "SS_fifo.h"
#include "unity_fixture.h"

TEST_GROUP(fifo);

TEST_GROUP_RUNNER(fifo) {
    RUN_TEST_CASE(fifo, put_get);
    RUN_TEST_CASE(fifo, put_get_full);
    RUN_TEST_CASE(fifo, buffer_full);
    RUN_TEST_CASE(fifo, buffer_empty);
    RUN_TEST_CASE(fifo, buffer_circular);
}

TEST_SETUP(fifo) {}
TEST_TEAR_DOWN(fifo) {}

TEST(fifo, put_get) {
    int LENGTH = 5;
    uint8_t array[LENGTH][sizeof(int)];
    volatile Fifo test_fifo = {(uint8_t*) array, 0, 0, LENGTH, sizeof(int) };
    int output, input = 420;
    SS_fifo_put_data(&test_fifo, &input);
    SS_fifo_get_data(&test_fifo, &output);
    TEST_ASSERT_EQUAL_INT(input, output);
}


TEST(fifo, put_get_full) {
    int LENGTH = 5;
    FIFO_INIT(test, LENGTH, int)
    int output[LENGTH - 1], input[LENGTH - 1];
    for(int i = 0; i < LENGTH - 1 ; i++) {
        input[i] = i;
        SS_fifo_put_data(&test_fifo, &input[i]);
    }
    for(int i = 0; i < LENGTH - 1; i++) {
        SS_fifo_get_data(&test_fifo, &output[i]);
    }
    TEST_ASSERT_EQUAL_INT32_ARRAY(input, output, LENGTH - 1);
}

TEST(fifo, buffer_full) {
    int LENGTH = 5;
    FIFO_INIT(test, LENGTH, int)
    int input[LENGTH - 1];
    for(int i = 0; i < LENGTH - 1 ; i++) {
        input[i] = i;
        SS_fifo_put_data(&test_fifo, &input[i]);
    }
    TEST_ASSERT_FALSE(SS_fifo_put_data(&test_fifo, &input[0]));
}

TEST(fifo, buffer_empty) {
    int LENGTH = 5;
    FIFO_INIT(test, LENGTH, int)
    int out;
    int output[LENGTH - 1], input[LENGTH - 1];
    TEST_ASSERT_FALSE(SS_fifo_get_data(&test_fifo, &out));
    for(int i = 0; i < LENGTH - 1 ; i++) {
        SS_fifo_put_data(&test_fifo, &input[i]);
    }
    for(int i = 0; i < LENGTH - 1 ; i++) {
        SS_fifo_get_data(&test_fifo, &output[i]);
    }
    TEST_ASSERT_FALSE(SS_fifo_get_data(&test_fifo, &out));
}

TEST(fifo, buffer_circular) {
    int LENGTH = 5;
    FIFO_INIT(test, LENGTH, int)
    int output[LENGTH - 1], input[LENGTH - 1];
    for(int i = 0; i < LENGTH - 1 ; i++) {
        SS_fifo_put_data(&test_fifo, &input[i]);
    }
    for(int i = 0; i < LENGTH - 1 ; i++) {
        SS_fifo_get_data(&test_fifo, &output[i]);
    }
    for(int i = 0; i < LENGTH - 1 ; i++) {
        input[i] = i;
        SS_fifo_put_data(&test_fifo, &input[i]);
    }
    for(int i = 0; i < LENGTH - 1 ; i++) {
        SS_fifo_get_data(&test_fifo, &output[i]);
    }
    TEST_ASSERT_EQUAL_INT32_ARRAY(input, output, LENGTH - 1);
}
