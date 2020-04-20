/*
 * SS_grazyna_com_tests.c
 *
 *  Created on: Mar 03, 2020
 *      Author: maciek
 */

#include "SS_grazyna.c"
#include "crc.h"
#include "string.h"
#include "unity_fixture.h"

/* ==================================================================== */
/* ============================== Setup =============================== */
/* ==================================================================== */

TEST_GROUP(grazyna);

TEST_GROUP_RUNNER(grazyna) {
    RUN_TEST_CASE(grazyna, crc);
    RUN_TEST_CASE(grazyna, crc_incorrect);
    RUN_TEST_CASE(grazyna, prepare_tx_frame);
}

TEST_SETUP(grazyna) {}
TEST_TEAR_DOWN(grazyna) {}

/* ==================================================================== */
/* ============================== Tests =============================== */
/* ==================================================================== */
TEST(grazyna, crc) {
    GrazynaFrame received = {0};
    uint8_t buff[] = {0x05, 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a};
    memcpy(&received, buff, sizeof(buff));
    uint32_t expected = received.crc;
    uint32_t actual = SS_grazyna_crc_calculate(&received);
    TEST_ASSERT_EQUAL_HEX(expected, actual);
}

TEST(grazyna, crc_incorrect) {
    GrazynaFrame received = {0};
    uint8_t buff[] = {0x05, 0x28, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a};
    memcpy(&received, buff, sizeof(buff));
    uint32_t expected = received.crc;
    uint32_t actual = SS_grazyna_crc_calculate(&received);
    TEST_ASSERT_NOT_EQUAL(expected, actual);
}

TEST(grazyna, prepare_tx_frame) {
    uint8_t expected[] = {0x05, 0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0x58, 0x0d};
    GrazynaFrame grazyna_frame;
    uint8_t buff[] = {0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00};
    ComFrame *com_frame = (ComFrame *) buff;
    SS_grazyna_prepare_tx_frame(com_frame, &grazyna_frame);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, (uint8_t *) &grazyna_frame, sizeof(expected));
}
