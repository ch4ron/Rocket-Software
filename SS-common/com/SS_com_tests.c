/*
 * SS_com_tests.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "unity_fixture.h"
#include "SS_Grazyna_com.h"
#include "SS_com_protocol.h"
#include "string.h"

TEST_GROUP(com);

TEST_GROUP_RUNNER(com) {
/* TODO - write CRC mock */
#ifndef SIMULATE
    RUN_TEST_CASE(com, crc);
    RUN_TEST_CASE(com, crc_incorrect);
#endif
    RUN_TEST_CASE(com, parse_content);
    RUN_TEST_CASE(com, create_frame);
    RUN_TEST_CASE(com, create_frame2);
    RUN_TEST_CASE(com, create_frame3);
    RUN_TEST_CASE(com, prepare_tx_frame);
}

TEST_SETUP(com) {}
TEST_TEAR_DOWN(com) {}

extern uint32_t SS_grazyna_CRC_calculate(GrazynaFrame *grazyna_frame);

TEST(com, crc) {
    GrazynaFrame received;
    uint8_t buff[] = { 0x05, 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
    memcpy(&received, buff, sizeof(buff));
    uint32_t expected = received.crc;
    uint32_t actual = SS_grazyna_CRC_calculate(&received);
    TEST_ASSERT_EQUAL_HEX(expected, actual);
}

TEST(com, crc_incorrect) {
    GrazynaFrame received;
    uint8_t buff[] = { 0x05, 0x28, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
    memcpy(&received, buff, sizeof(buff));
    uint32_t expected = received.crc;
    uint32_t actual = SS_grazyna_CRC_calculate(&received);
    TEST_ASSERT_NOT_EQUAL(expected, actual);
}

TEST(com, parse_content) {
    uint8_t buff[] = { 0x05, 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
    GrazynaFrame *frame = (GrazynaFrame*) buff;
    ComFrameContent content;
    SS_com_parse_frame(&frame->com_frame, &content);
    TEST_ASSERT_EQUAL_HEX(COM_SERVICE, content.action);
    TEST_ASSERT_EQUAL_HEX(UINT32, content.data_type);
    TEST_ASSERT_EQUAL_HEX(COM_KROMEK_ID, content.destination);
    TEST_ASSERT_EQUAL_HEX(COM_GRAZYNA_ID, content.source);
    TEST_ASSERT_EQUAL_HEX(0x04, content.device);
    TEST_ASSERT_TRUE(content.grazyna_ind);
    TEST_ASSERT_EQUAL_HEX(0x05, content.message_type);
    TEST_ASSERT_EQUAL_HEX(0x01, content.priority);
}

TEST(com, create_frame) {
    uint8_t buff[] = { 0x05, 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
    GrazynaFrame *frame = (GrazynaFrame*) buff;
    ComFrameContent content;
    SS_com_parse_frame(&frame->com_frame, &content);
    ComFrame com_frame;
    SS_com_create_frame(&com_frame, &content);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&frame->com_frame, (uint8_t*) &com_frame, sizeof(ComFrame));
}
TEST(com, create_frame2) {
    uint8_t buff[] = { 0x05, 0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0x58, 0x0d };
    GrazynaFrame *frame = (GrazynaFrame*) buff;
    ComFrameContent content;
    SS_com_parse_frame(&frame->com_frame, &content);
    ComFrame com_frame;
    SS_com_create_frame(&com_frame, &content);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&frame->com_frame, (uint8_t*) &com_frame, sizeof(ComFrame));
}

TEST(com, create_frame3) {
    uint8_t buff[] = { 0x5, 0x26, 0x9, 0x12, 0xe8, 0x5, 0x0, 0x0, 0x0, 0x0, 0x39, 0x64, 0xae, 0xa };
    GrazynaFrame *frame = (GrazynaFrame*) buff;
    ComFrameContent content;
    SS_com_parse_frame(&frame->com_frame, &content);
    ComFrame com_frame;
    SS_com_create_frame(&com_frame, &content);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&frame->com_frame, (uint8_t*) &com_frame, sizeof(ComFrame));
}

extern void SS_grazyna_prepare_tx_frame(ComFrame *com_frame, GrazynaFrame *grazyna_frame);

TEST(com, prepare_tx_frame) {
    uint8_t expected[] = { 0x05, 0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0x58, 0x0d };
    GrazynaFrame grazyna_frame;
    uint8_t buff[] = { 0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00 };
    ComFrame *com_frame = (ComFrame*) buff;
    SS_grazyna_prepare_tx_frame(com_frame, &grazyna_frame);
/* TODO - write CRC mock */
#ifndef SIMULATE
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, (uint8_t*) &grazyna_frame, sizeof(expected));
#else
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, (uint8_t*) &grazyna_frame, sizeof(expected) - 4);
#endif
}
