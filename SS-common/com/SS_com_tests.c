/*
 * SS_com_tests.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include <fifo/SS_fifo.h>
#include "unity_fixture.h"
#include "SS_com.h"
#include "string.h"

TEST_GROUP(com);

TEST_GROUP_RUNNER(com) {
    RUN_TEST_CASE(com, fifo_manager);
}

static ComFrame mock_frame;
static ComFifoManager tmp_manager[10];

extern ComFifoManager fifo_manager[10];
TEST_SETUP(com) {
    memset(&mock_frame, 0, sizeof(ComFrame));
    memcpy(tmp_manager, fifo_manager, sizeof(fifo_manager));
}

TEST_TEAR_DOWN(com) {
    memcpy(fifo_manager, tmp_manager, sizeof(fifo_manager));
}

static void mock(ComFrame *frame) {
    memcpy(&mock_frame, frame, sizeof(ComFrame));
}


TEST(com, fifo_manager) {
    FIFO_INIT(test, 10, ComFrame)
    SS_com_add_fifo(&test_fifo, mock, 1, 0);
    ComFrame frame = { .destination = 3, .payload = 147 };
    SS_fifo_put_data(&test_fifo, &frame);
    SS_com_main();
    TEST_ASSERT_EQUAL(frame.destination, mock_frame.destination);
    TEST_ASSERT_EQUAL(frame.payload, mock_frame.payload);
}

