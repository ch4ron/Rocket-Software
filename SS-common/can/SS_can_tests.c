/*
 * SS_com_tests.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "SS_com.h"
#include "can.h"
#include "string.h"
#include "unity_fixture.h"

TEST_GROUP(can);

TEST_GROUP_RUNNER(can) {
    RUN_TEST_CASE(can, pack_unpack1);
    RUN_TEST_CASE(can, pack_unpack2);
    RUN_TEST_CASE(can, pack_unpack3);
    RUN_TEST_CASE(can, pack_unpack4);
}

TEST_SETUP(can) {}

TEST_TEAR_DOWN(can) {}

extern void SS_can_pack_frame(ComFrame *frame, CAN_TxHeaderTypeDef *header, uint8_t *data);
extern void SS_can_unpack_frame(ComFrame *frame, CAN_RxHeaderTypeDef *header, uint8_t *data);

static void can_pack_unpack_test(ComFrame *frame) {
    ComFrame res = { 0 };
    uint8_t data[sizeof(ComFrame)];
    CAN_RxHeaderTypeDef rx_header;
    CAN_TxHeaderTypeDef tx_header;
    SS_can_pack_frame(frame, &tx_header, data);
    rx_header.ExtId = tx_header.ExtId;
    rx_header.DLC = tx_header.DLC;
    SS_can_unpack_frame(&res, &rx_header, data);
    TEST_ASSERT_EQUAL_HEX8_ARRAY((uint8_t*) frame, (uint8_t*) &res, sizeof(ComFrame));
}

TEST(can, pack_unpack1) {
    ComFrame frame = {
            .destination = 0x01,
            .source = 0x12,
            .priority = 0x02,
            .message_type = 0x31,
            .payload = 0xFFAF,
            .data_type = 0x03,
            .device = 0x13,
            .id = 0x7
    };
    can_pack_unpack_test(&frame);
}

TEST(can, pack_unpack2) {
    uint8_t buf[] = { 0x11, 0x05, 0xc1, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00 };
    can_pack_unpack_test((ComFrame*) buf);
}

TEST(can, pack_unpack3) {
    uint8_t buf[] = { 0xFA, 0x15, 0x21, 0xF0, 0x00, 0x10, 0xF0, 0x0C, 0xD3 };
    can_pack_unpack_test((ComFrame*) buf);
}

TEST(can, pack_unpack4) {
    uint8_t buf[] = { 0xCB, 0xD5, 0x21, 0xA7, 0x12, 0x34, 0x81, 0x22, 0x33 };
    can_pack_unpack_test((ComFrame*) buf);
}


//TEST(com, parse_content) {
//    uint8_t buff[] = { 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
//    ComFrame *frame = (ComFrame*) buff;
//    ComFrame content;
//    SS_com_unpack_frame(frame, &content);
//    TEST_ASSERT_EQUAL_HEX(COM_SERVICE, content.action);
//    TEST_ASSERT_EQUAL_HEX(UINT32, content.data_type);
//    TEST_ASSERT_EQUAL_HEX(COM_KROMEK_ID, content.destination);
//    TEST_ASSERT_EQUAL_HEX(COM_GRAZYNA_ID, content.source);
//    TEST_ASSERT_EQUAL_HEX(0x04, content.device);
//    TEST_ASSERT_TRUE(content.grazyna_ind);
//    TEST_ASSERT_EQUAL_HEX(0x05, content.message_type);
//    TEST_ASSERT_EQUAL_HEX(0x01, content.priority);
//}
//
//TEST(com, create_frame) {
//    uint8_t buff[] = { 0x26, 0x09, 0x10, 0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xc6, 0x84, 0x4a };
//    ComFrame *frame = (ComFrame*) buff;
//    ComFrame content;
//    SS_com_unpack_frame(frame, &content);
//    ComFrame com_frame;
//    SS_com_pack_frame(&com_frame, &content);
//    TEST_ASSERT_EQUAL_UINT8_ARRAY(frame, (uint8_t*) &com_frame, sizeof(ComFrame));
//}
//TEST(com, create_frame2) {
//    uint8_t buff[] = { 0x26, 0x09, 0x13, 0xe8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0x58, 0x0d };
//    ComFrame *frame = (ComFrame*) buff;
//    ComFrame content;
//    SS_com_unpack_frame(frame, &content);
//    ComFrame com_frame;
//    SS_com_pack_frame(&com_frame, &content);
//    TEST_ASSERT_EQUAL_UINT8_ARRAY(frame, (uint8_t*) &com_frame, sizeof(ComFrame));
//}
//
//TEST(com, create_frame3) {
//    uint8_t buff[] = { 0x26, 0x9, 0x12, 0xe8, 0x5, 0x0, 0x0, 0x0, 0x0, 0x39, 0x64, 0xae, 0xa };
//    ComFrame *frame = (ComFrame*) buff;
//    ComFrame content;
//    SS_com_unpack_frame(frame, &content);
//    ComFrame com_frame;
//    SS_com_pack_frame(&com_frame, &content);
//    TEST_ASSERT_EQUAL_UINT8_ARRAY(frame, (uint8_t*) &com_frame, sizeof(ComFrame));
//}
