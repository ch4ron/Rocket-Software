/*
 * SS_dynamixel_test.c
 *
 *  Created on: Dec 29, 2019
 *      Author: maciek
 */

#include "SS_dynamixel.c"
#include "stdio.h"
#include "unity_fixture.h"

Dynamixel dynamixel;

TEST_GROUP(dynamixel);

TEST_GROUP_RUNNER(dynamixel) {
    RUN_TEST_CASE(dynamixel, crc1);
    RUN_TEST_CASE(dynamixel, crc2);
    RUN_TEST_CASE(dynamixel, prepare_packet);
    RUN_TEST_CASE(dynamixel, create_packet);
    RUN_TEST_CASE(dynamixel, crc_check);
    RUN_TEST_CASE(dynamixel, crc_check_error);
    /* RUN_TEST_CASE(dynamixel, write); */
    /*     RUN_TEST_CASE(dynamixel, read); */
    /*     RUN_TEST_CASE(dynamixel, read_IT); */
    /*     RUN_TEST_CASE(dynamixel, write_IT); */
    /*     RUN_TEST_CASE(dynamixel, write_homing_offset); */
    /*     RUN_TEST_CASE(dynamixel, ping); */
    /*     RUN_TEST_CASE(dynamixel, ping_IT); */
}

TEST_SETUP(dynamixel) {}

TEST_TEAR_DOWN(dynamixel) {}

TEST(dynamixel, crc1) {
    uint8_t packet[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x55, 0x00, 0x06, 0x04, 0x26};
    uint16_t crc = SS_dynamixel_update_crc(0, packet, sizeof(packet));
    TEST_ASSERT_EQUAL_HEX16(0x5D65, crc);
}

TEST(dynamixel, crc2) {
    uint8_t packet[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00};
    uint16_t crc = SS_dynamixel_update_crc(0, packet, sizeof(packet));
    TEST_ASSERT_EQUAL_HEX16(0x151D, crc);
}

TEST(dynamixel, prepare_packet) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15};
    Instruction_packet packet = {0xFDFFFF, 0x00, 0x01, 0x07, 0x02};
    uint8_t data[] = {0x84, 0x00, 0x04, 0x00};
    SS_dynamixel_prepare_packet(&packet, data, &tmp_packet_buff);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tmp_packet_buff.packet, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT(14, tmp_packet_buff.packet_size);
}

TEST(dynamixel, create_packet) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15};
    uint8_t data[] = {0x84, 0x00, 0x04, 0x00};
    SS_dynamixel_create_packet(&dynamixel, 0x02, data, sizeof(data), &tmp_packet_buff);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tmp_packet_buff.packet, sizeof(expected));
}

TEST(dynamixel, write) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89};
    uint8_t data[] = {0x00, 0x02, 0x00, 0x00};
    SS_dynamixel_write(&dynamixel, 0x74, data, sizeof(data));
    /* Wait for possible servo disable message to be transmitted */
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, write_IT) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89};
    uint8_t data[] = {0x00, 0x02, 0x00, 0x00};
    SS_dynamixel_write_IT(&dynamixel, 0x74, data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, read) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15};
    SS_dynamixel_read(&dynamixel, 0x84, rx_packet_buff, 4);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
}

TEST(dynamixel, read_IT) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15};
    SS_dynamixel_read_IT(&dynamixel, 0x84, rx_packet_buff, 4);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, crc_check) {
    uint8_t data[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89};
    uint8_t result = SS_dynamixel_check_crc(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT8(true, result);
}

TEST(dynamixel, crc_check_error) {
    uint8_t data[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x04, 0x68, 0x00, 0xC8, 0x00, 0x00, 0x00, 0xA9, 0x8E};
    uint8_t result = SS_dynamixel_check_crc(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT8(false, result);
}

TEST(dynamixel, write_homing_offset) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x14, 0x00, 500 & 0xFF, 500 >> 8, 0x00, 0x00};
    uint32_t data = 500;
    SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
}

void tests(void) {
    RUN_TEST_GROUP(dynamixel);
}

int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, tests);
}
