/*
 * SS_grazyna_com_tests.c
 *
 *  Created on: Mar 03, 2020
 *      Author: maciek
 */

#include "MockSS_com.h"
#include "MockSS_grazyna_hal.h"
#include "SS_grazyna.c"
#include "stdio.h"
#include "string.h"
#include "unity_fixture.h"

/* ==================================================================== */
/* ============================== Setup =============================== */
/* ==================================================================== */

TEST_GROUP(grazyna);

TEST_GROUP_RUNNER(grazyna) {
    RUN_TEST_CASE(grazyna, init);
    RUN_TEST_CASE(grazyna, crc);
    RUN_TEST_CASE(grazyna, prepare_tx_frame);
    RUN_TEST_CASE(grazyna, enable);
    RUN_TEST_CASE(grazyna, disable);
    RUN_TEST_CASE(grazyna, is_enabled);
    RUN_TEST_CASE(grazyna, handle_valid_frame);
    RUN_TEST_CASE(grazyna, handle_invalid_frame);
}

TEST_SETUP(grazyna) {}
TEST_TEAR_DOWN(grazyna) {}

/* ==================================================================== */
/* ============================== Tests =============================== */
/* ==================================================================== */

TEST(grazyna, init) {
    SS_grazyna_receive_hal_Expect((uint8_t *) &grazyna.rx_buff, 1);
    SS_grazyna_init_hal_Expect(NULL);
    QueueHandle_t queue = {0};
    SS_com_add_sender_ExpectAndReturn(queue);
    SS_grazyna_init(NULL);
    TEST_ASSERT_TRUE(grazyna.enabled);
}

TEST(grazyna, crc) {
    GrazynaFrame received = {0x3, {0x03, 0x02, 0x3, 0x4, 0x33, 0x11, 0x4, 0x11}, 0};
    uint32_t grazyna_buff[4];
    memcpy(grazyna_buff, &received, sizeof(received));
    SS_grazyna_crc_hal_ExpectWithArrayAndReturn(grazyna_buff, 3, 3, 0);
    SS_grazyna_crc_calculate(&received);
}

TEST(grazyna, prepare_tx_frame) {
    ComFrame frame = {0x05, 0x1, 0x2, 0x10, 0x9, 0x05, 0x00, 0x00, 0x04};

    GrazynaFrame expected = {0};
    expected.header = GRAZYNA_HEADER;
    memcpy(&expected.com_frame, &frame, sizeof(ComFrame));
    uint32_t buff[4] = {0};
    memcpy(buff, &expected, sizeof(GrazynaFrame) - 4);

    SS_grazyna_crc_hal_ExpectWithArrayAndReturn(buff, 3, 3, 0x444);

    GrazynaFrame grazyna_frame;
    SS_grazyna_prepare_tx_frame(&frame, &grazyna_frame);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) &frame, (uint8_t *) &grazyna_frame.com_frame, sizeof(frame));
    TEST_ASSERT_EQUAL_HEX32(grazyna_frame.crc, 0x444);
}

TEST(grazyna, enable) {
    SS_grazyna_enable();
    TEST_ASSERT_TRUE(grazyna.enabled);
}

TEST(grazyna, disable) {
    SS_grazyna_disable();
    TEST_ASSERT_FALSE(grazyna.enabled);
}

TEST(grazyna, is_enabled) {
    SS_grazyna_disable();
    TEST_ASSERT_FALSE(SS_grazyna_is_enabled());
    SS_grazyna_enable();
    TEST_ASSERT_TRUE(SS_grazyna_is_enabled());
    SS_grazyna_disable();
    TEST_ASSERT_FALSE(SS_grazyna_is_enabled());
}

TEST(grazyna, transmit) {
    ComFrame frame;
    SS_com_add_to_tx_queue_Expect(&frame, SS_grazyna_tx, NULL);
    SS_com_add_to_tx_queue_IgnoreArg_queue();
    SS_grazyna_transmit(&frame);
}

TEST(grazyna, handle_valid_frame) {
    grazyna.rx_buff.crc = 0x4321;
    SS_grazyna_crc_hal_IgnoreAndReturn(0x4321);
    SS_com_message_received_Expect(&grazyna.rx_buff.com_frame);
    SS_grazyna_receive_hal_ExpectAnyArgs();
    TEST_ASSERT_EQUAL(GRAZYNA_LOOKING_FOR_HEADER, grazyna.grazyna_state);
}

TEST(grazyna, handle_invalid_frame) {
    grazyna.rx_buff.crc = 0x1234;
    SS_grazyna_crc_hal_IgnoreAndReturn(0x4321);
    SS_grazyna_receive_hal_ExpectAnyArgs();
    TEST_ASSERT_EQUAL(GRAZYNA_LOOKING_FOR_HEADER, grazyna.grazyna_state);
}

void tests(void) {
    RUN_TEST_GROUP(grazyna);
}

int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, tests);
}
