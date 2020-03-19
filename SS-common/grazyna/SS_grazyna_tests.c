//
// Created by maciek on 02.03.2020.
//

#include "SS_grazyna.h"
#include "unity_fixture.h"
#include "string.h"
#include "crc.h"

TEST_GROUP(grazyna);

TEST_GROUP_RUNNER(grazyna) {
        RUN_TEST_CASE(grazyna, crc);
        RUN_TEST_CASE(grazyna, crc_incorrect);
        RUN_TEST_CASE(grazyna, prepare_tx_frame);
#ifndef SIMULATE
        RUN_TEST_CASE(grazyna, software_crc);
        RUN_TEST_CASE(grazyna, software_crc_frame);
#endif
}

TEST_SETUP(grazyna) {}
TEST_TEAR_DOWN(grazyna) {}

extern uint32_t SS_grazyna_CRC_calculate(GrazynaFrame *grazyna_frame);

TEST(grazyna, crc) {
    GrazynaFrame received = { 0 };
    uint8_t buff[] = { 0x05, 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
    memcpy(&received, buff, sizeof(buff));
    uint32_t expected = received.crc;
    uint32_t actual = SS_grazyna_CRC_calculate(&received);
    TEST_ASSERT_EQUAL_HEX(expected, actual);
}

TEST(grazyna, crc_incorrect) {
    GrazynaFrame received = { 0 };
    uint8_t buff[] = { 0x05, 0x28, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
    memcpy(&received, buff, sizeof(buff));
    uint32_t expected = received.crc;
    uint32_t actual = SS_grazyna_CRC_calculate(&received);
    TEST_ASSERT_NOT_EQUAL(expected, actual);
}


extern uint32_t crc32 (uint32_t *data, int len);
TEST(grazyna, software_crc) {
    char message[] = "an interesting message for testing crc";
    uint32_t expected = HAL_CRC_Calculate(&hcrc, (uint32_t *) message, sizeof(message)/4);
    uint32_t actual = crc32((uint32_t*) message, sizeof(message)/4);
    TEST_ASSERT_EQUAL_HEX(expected, actual);
}

TEST(grazyna, software_crc_frame) {
//    GrazynaFrame frame = {
//            .com_frame = {
//                    .frame = {
//                    .header = 3,
//                    .message_type = 4,
//                    .payload = 11
//                    }
//            },
//            .header = 3
//    };
//    uint32_t len = sizeof(frame)/4;
//    uint32_t buff[len];
//    memcpy(buff, &frame, len*4);
//    uint32_t expected = HAL_CRC_Calculate(&hcrc, buff, len);
//    uint32_t actual = crc32(buff, len);
//    TEST_ASSERT_EQUAL_HEX(expected, actual);
}



extern void SS_grazyna_prepare_tx_frame(ComFrame *com_frame, GrazynaFrame *grazyna_frame);

TEST(grazyna, prepare_tx_frame) {
    uint8_t expected[] = {0x05, 0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0x58, 0x0d};
    GrazynaFrame grazyna_frame;
    uint8_t buff[] = {0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00};
    ComFrame *com_frame = (ComFrame *) buff;
    SS_grazyna_prepare_tx_frame(com_frame, &grazyna_frame);
/* TODO - write CRC mock */
#ifndef SIMULATE
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, (uint8_t*) &grazyna_frame, sizeof(expected));
#else
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, (uint8_t *) &grazyna_frame, sizeof(expected) - 4);
#endif
}