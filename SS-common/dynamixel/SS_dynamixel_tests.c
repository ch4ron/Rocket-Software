/*
 * SS_dynamixel_test.c
 *
 *  Created on: Dec 29, 2019
 *      Author: maciek
 */

#include "SS_config.h"

#ifdef RUN_TESTS

#include "SS_dynamixel.h"
#include "unity_fixture.h"
#include "SS_supply_control.h"
#include "SS_fifo.h"

extern uint8_t tx_packet_buff[MAX_PACKET_LENGTH];
extern uint8_t rx_packet_buff[MAX_PACKET_LENGTH];
extern Dynamixel_fifo_bufor tmp_packet_buff;

TEST_GROUP(dynamixel_logic);

/* Connect the servo before running this group */
TEST_GROUP(dynamixel);

TEST_GROUP_RUNNER(dynamixel_logic) {
    RUN_TEST_CASE(dynamixel_logic, crc1);
    RUN_TEST_CASE(dynamixel_logic, crc2);
    RUN_TEST_CASE(dynamixel_logic, prepare_packet);
    RUN_TEST_CASE(dynamixel_logic, create_packet);
    RUN_TEST_CASE(dynamixel_logic, crc_check);
    RUN_TEST_CASE(dynamixel_logic, crc_check_error);
    RUN_TEST_CASE(dynamixel_logic, Fifo);
}

TEST_GROUP_RUNNER(dynamixel) {
    RUN_TEST_CASE(dynamixel_logic, opened_closed_postion);
    RUN_TEST_CASE(dynamixel_logic, write);
    RUN_TEST_CASE(dynamixel_logic, read);
    RUN_TEST_CASE(dynamixel_logic, read_DMA);
    RUN_TEST_CASE(dynamixel_logic, write_DMA);
    RUN_TEST_CASE(dynamixel_logic, write_homing_offset);
    RUN_TEST_CASE(dynamixel_logic, ping);
    RUN_TEST_CASE(dynamixel_logic, ping_DMA);
    RUN_TEST_CASE(dynamixel, get_position);
    RUN_TEST_CASE(dynamixel, write_read_dma);
    RUN_TEST_CASE(dynamixel, write_read_torque_enabled_DMA);
    RUN_TEST_CASE(dynamixel, ping_DMA);
    RUN_TEST_CASE(dynamixel, connection);
    RUN_TEST_CASE(dynamixel, write_read_torque_disabled_DMA);
    RUN_TEST_CASE(dynamixel, read);
    RUN_TEST_CASE(dynamixel, write_read_torque_enabled);
    RUN_TEST_CASE(dynamixel, write_read_torque_disabled);
    RUN_TEST_CASE(dynamixel, set_velocity_limit);
    RUN_TEST_CASE(dynamixel, ping);
    RUN_TEST_CASE(dynamixel, enable_led);
    RUN_TEST_CASE(dynamixel, enable_torque);
    RUN_TEST_CASE(dynamixel, disable_torque);
    RUN_TEST_CASE(dynamixel, set_position);
    RUN_TEST_CASE(dynamixel, enable_torque_status);
    RUN_TEST_CASE(dynamixel, disable_torque_status);
    RUN_TEST_CASE(dynamixel, fifo_write);
    RUN_TEST_CASE(dynamixel, fifo_multiple_messages);
    RUN_TEST_CASE(dynamixel, DMA_stress_test1);
    RUN_TEST_CASE(dynamixel, DMA_stress_test2);
    RUN_TEST_CASE(dynamixel, DMA_stress_test3);
    RUN_TEST_CASE(dynamixel, DMA_stress_test4);
    RUN_TEST_CASE(dynamixel, systick_position);
    RUN_TEST_CASE(dynamixel, disable_torque_on_stop);
    RUN_TEST_CASE(dynamixel, systick_moving);
    RUN_TEST_CASE(dynamixel, systick_current);
    RUN_TEST_CASE(dynamixel, systick_temperature);
    RUN_TEST_CASE(dynamixel, opened_closed_postion);
    RUN_TEST_CASE(dynamixel, opened_closed_postion_inverse);
    RUN_TEST_CASE(dynamixel, opened_limit);
    RUN_TEST_CASE(dynamixel, closed_limit);
    RUN_TEST_CASE(dynamixel, factory_reset);
}

TEST_SETUP(dynamixel_logic) {}

TEST_TEAR_DOWN(dynamixel_logic) {}

TEST_SETUP(dynamixel) {
    SS_enable_supply(&kozackie_servo_supply);
}

TEST_TEAR_DOWN(dynamixel) {
    SS_dynamixel_stop_communication(&dynamixel);
}

TEST(dynamixel_logic, crc1) {
    uint8_t packet[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x55, 0x00, 0x06, 0x04, 0x26 };
    uint16_t crc = SS_dynamixel_update_crc(0, packet, sizeof(packet));
    TEST_ASSERT_EQUAL_HEX16(0x5D65, crc);
}

TEST(dynamixel_logic, crc2) {
    uint8_t packet[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00 };
    uint16_t crc = SS_dynamixel_update_crc(0, packet, sizeof(packet));
    TEST_ASSERT_EQUAL_HEX16(0x151D, crc);
}

TEST(dynamixel_logic, prepare_packet) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15 };
    Instruction_packet packet = { 0xFDFFFF, 0x00, 0x01, 0x07, 0x02 };
    uint8_t data[] = { 0x84, 0x00, 0x04, 0x00 };
    SS_dynamixel_prepare_packet(&packet, data, &tmp_packet_buff);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tmp_packet_buff.packet, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT(14, tmp_packet_buff.packet_size);
}

TEST(dynamixel_logic, create_packet) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15 };
    uint8_t data[] = { 0x84, 0x00, 0x04, 0x00 };
    SS_dynamixel_create_packet(&dynamixel, 0x02, data, sizeof(data), &tmp_packet_buff);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tmp_packet_buff.packet, sizeof(expected));
}

TEST(dynamixel_logic, write) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89 };
    uint8_t data[] = { 0x00, 0x02, 0x00, 0x00 };
    SS_dynamixel_write(&dynamixel, 0x74, data, sizeof(data));
    /* Wait for possible servo disable message to be transmitted */
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel_logic, write_DMA) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89 };
    uint8_t data[] = { 0x00, 0x02, 0x00, 0x00 };
    SS_dynamixel_write_DMA(&dynamixel, 0x74, data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel_logic, read) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15 };
    SS_dynamixel_read(&dynamixel, 0x84, rx_packet_buff, 4);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
}

TEST(dynamixel_logic, read_DMA) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1D, 0x15 };
    SS_dynamixel_read_DMA(&dynamixel, 0x84, rx_packet_buff, 4);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel_logic, crc_check) {
    uint8_t data[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89 };
    uint8_t result = SS_dynamixel_check_crc(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT8(true, result);
}

TEST(dynamixel_logic, crc_check_error) {
    uint8_t data[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x04, 0x68, 0x00, 0xC8, 0x00, 0x00, 0x00, 0xA9, 0x8E };
    uint8_t result = SS_dynamixel_check_crc(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT8(false, result);
}

TEST(dynamixel_logic, write_homing_offset) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x14, 0x00, 500 & 0xFF, 500 >> 8, 0x00, 0x00 };
    uint32_t data = 500;
    SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
}

TEST(dynamixel, factory_reset) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C };
    uint8_t res = SS_dynamixel_factory_reset(&dynamixel);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
}

TEST(dynamixel, ping) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x55, 0x00 };
    uint8_t res = SS_dynamixel_ping(&dynamixel);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, enable_led) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C };
    SS_dynamixel_enable_led(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
}

TEST(dynamixel, enable_torque) {
    uint8_t value;
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C };
    SS_dynamixel_enable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    uint8_t res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_TORQUE_ENABLE, &value, 1);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT8(0x01, value);
}

TEST(dynamixel, disable_torque) {
    uint8_t value;
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C };
    SS_dynamixel_disable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    uint8_t res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_TORQUE_ENABLE, &value, 1);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT8(0x00, value);
}

TEST(dynamixel, set_position) {
    uint32_t value;
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C };
    SS_dynamixel_enable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    SS_dynamixel_set_goal_position(&dynamixel, 333);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    uint8_t res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_GOAL_POSITION, &value, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(333, value);
}

TEST(dynamixel, read) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x05, 0x00, 0x55, 0x00, 0x01 };
    uint8_t data;
    uint8_t res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_ID, &data, 1);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8(0x01, data);
}

static void read_write_test() {
    uint32_t tmp, read;
    Dynamixel_status res;
    char buff[30];
    bool torque_status = dynamixel.torque_enabled;
    if ((res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &tmp, 4))) {
        sprintf(buff, "Read failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    uint32_t data = 500;
    HAL_Delay(UART_TIMEOUT);
    if ((res = SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4))) {
        sprintf(buff, "Write failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    HAL_Delay(UART_TIMEOUT);
    if ((res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4))) {
        sprintf(buff, "Read failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    TEST_ASSERT_EQUAL_UINT32(data, read);
    if ((res = SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &tmp, 4))) {
        sprintf(buff, "Write failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL(torque_status, dynamixel.torque_enabled);
}

TEST(dynamixel, write_read_torque_enabled) {
    SS_dynamixel_enable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    read_write_test();
}

TEST(dynamixel, write_read_torque_disabled) {
    SS_dynamixel_disable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    read_write_test();
}

static void read_write_test_DMA() {
    uint32_t read;
    bool torque_status = dynamixel.torque_enabled;
    uint32_t data = 500;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    HAL_Delay(2*UART_TIMEOUT);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_EQUAL_UINT32(data, read);
    TEST_ASSERT_EQUAL(torque_status, dynamixel.torque_enabled);
}

TEST(dynamixel, write_read_torque_enabled_DMA) {
    SS_dynamixel_enable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    read_write_test_DMA();
}

TEST(dynamixel, write_read_torque_disabled_DMA) {
    SS_dynamixel_disable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    read_write_test_DMA();
}

TEST(dynamixel, write_read_dma) {
    uint32_t read = 0, data = 500;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    HAL_Delay(3*UART_TIMEOUT);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_EQUAL(data, read);
}

TEST(dynamixel, get_position) {
    SS_dynamixel_enable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    SS_dynamixel_set_goal_position(&dynamixel, 111);
    HAL_Delay(UART_TIMEOUT);
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < 2500) {
        SS_dynamixel_get_position(&dynamixel);
        HAL_Delay(2*UART_TIMEOUT);
        if (dynamixel.present_position > 108 && dynamixel.present_position < 114)
            break;
    }
    if (dynamixel.present_position < 108 || dynamixel.present_position > 114) {
        TEST_ASSERT_EQUAL(111, dynamixel.present_position);
    }
}

TEST(dynamixel, set_velocity_limit) {
    SS_dynamixel_set_velocity_limit(&dynamixel, 1023);
    HAL_Delay(UART_TIMEOUT);
    uint32_t limit;
    uint8_t res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_VELOCITY_LIMIT, &limit, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(1023, limit);
}

TEST(dynamixel, enable_torque_status) {
    SS_dynamixel_enable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_TRUE(dynamixel.torque_enabled);
}

TEST(dynamixel, disable_torque_status) {
    SS_dynamixel_disable_torque(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_FALSE(dynamixel.torque_enabled);
}

extern Fifo dynamixel_fifo;

TEST(dynamixel_logic, Fifo) {
    Dynamixel_fifo_bufor buff = { { 0 }, 0 };
    Dynamixel_fifo_bufor packet = { { 33 }, 0 };
    while(SS_fifo_get_data(&dynamixel_fifo, &buff));
    SS_fifo_put_data(&dynamixel_fifo, &packet);
    SS_fifo_get_data(&dynamixel_fifo, &buff);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(packet.packet, buff.packet, MAX_PACKET_LENGTH);
    bool res = SS_fifo_get_data(&dynamixel_fifo, buff.packet);
    TEST_ASSERT_FALSE(res);
}

TEST(dynamixel, fifo_write) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89 };
    Dynamixel_fifo_bufor buff;
    while(SS_fifo_get_data(&dynamixel_fifo, &buff));
    uint8_t data[] = { 0x00, 0x02, 0x00, 0x00 };
    uint8_t buf = 1;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_LED, &buf, 1);
    SS_dynamixel_write_DMA(&dynamixel, 0x74, data, sizeof(data));
    SS_fifo_get_data(&dynamixel_fifo, &buff);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, buff.packet, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, fifo_multiple_messages) {
    uint8_t led = 1, res, led_res;
    uint32_t data = 500, tmp;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_LED, &led, 1);
    HAL_Delay(2*UART_TIMEOUT);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &tmp, 4);
    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_LED, &led_res, 1);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL(data, tmp);
    TEST_ASSERT_EQUAL(led, led_res);
}

TEST(dynamixel, DMA_stress_test1) {
    uint32_t data = 1, read = 0;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 2;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 3;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 4;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 5;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 6;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 7;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 8;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    data = 9;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    HAL_Delay(6*UART_TIMEOUT);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_EQUAL(data, read);
}

TEST(dynamixel, DMA_stress_test2) {
    uint32_t data = 1, read = 0;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    data = 69;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    HAL_Delay(6*UART_TIMEOUT);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_EQUAL(data, read);
}

TEST(dynamixel, DMA_stress_test3) {
    uint32_t data = 1, read = 0;
    SS_dynamixel_write_DMA(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    data = 69;
    SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4);
    SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4);
    HAL_Delay(6*UART_TIMEOUT);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_EQUAL(data, read);
}

TEST(dynamixel, DMA_stress_test4) {
    uint8_t torque, led;
    SS_dynamixel_disable_torque(&dynamixel);
    SS_dynamixel_enable_led(&dynamixel);
    SS_dynamixel_disable_led(&dynamixel);
    SS_dynamixel_enable_led(&dynamixel);
    SS_dynamixel_disable_led(&dynamixel);
    SS_dynamixel_enable_led(&dynamixel);
    SS_dynamixel_enable_torque(&dynamixel);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_LED, &led, 1);
    SS_dynamixel_read_DMA(&dynamixel, DYNAMIXEL_TORQUE_ENABLE, &torque, 1);
    HAL_Delay(6*UART_TIMEOUT);
    TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
    TEST_ASSERT_EQUAL(1, led);
    TEST_ASSERT_EQUAL(1, torque);
}

TEST(dynamixel, systick_position) {
    SS_dynamixel_start_communication(&dynamixel);
    int32_t goal_position = 3011;
    SS_dynamixel_set_goal_position(&dynamixel, goal_position);
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < 2500) {
        if (dynamixel.present_position > goal_position - 5 && dynamixel.present_position < goal_position + 5)
            break;
    }
    if (dynamixel.present_position < goal_position - 5 || dynamixel.present_position > goal_position + 5)
        TEST_ASSERT_EQUAL(goal_position, dynamixel.present_position);
}

TEST(dynamixel, systick_moving) {
    SS_dynamixel_start_communication(&dynamixel);
    HAL_Delay(100);
    TEST_ASSERT_FALSE(dynamixel.moving);
    SS_dynamixel_set_goal_position(&dynamixel, 2600);
    HAL_Delay(100);
    SS_dynamixel_set_goal_position(&dynamixel, 2400);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_TRUE(dynamixel.moving);
    HAL_Delay(500);
    TEST_ASSERT_FALSE(dynamixel.moving);
}


TEST(dynamixel, systick_current) {
    TEST_IGNORE();
}

TEST(dynamixel, systick_temperature) {
    SS_dynamixel_start_communication(&dynamixel);
    HAL_Delay(60);
    TEST_ASSERT_LESS_OR_EQUAL(40, dynamixel.temperature);
    TEST_ASSERT_GREATER_OR_EQUAL(15, dynamixel.temperature);
}

TEST(dynamixel_logic, opened_closed_postion) {
    SS_dynamixel_set_closed_position(&dynamixel, 11);
    SS_dynamixel_set_opened_position(&dynamixel, 3999);
    TEST_ASSERT_EQUAL(11, dynamixel.closed_position);
    TEST_ASSERT_EQUAL(3999, dynamixel.opened_position);
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, opened_closed_postion) {
    uint32_t closed_pos = 300;
    uint32_t opened_pos = 700;
    SS_dynamixel_set_closed_position(&dynamixel, closed_pos);
    SS_dynamixel_set_opened_position(&dynamixel, opened_pos);
    SS_dynamixel_start_communication(&dynamixel);
    SS_dynamixel_open(&dynamixel);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < opened_pos - 5 || dynamixel.present_position > opened_pos + 5)
        TEST_ASSERT_EQUAL(opened_pos, dynamixel.present_position);
    SS_dynamixel_close(&dynamixel);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < closed_pos - 5 || dynamixel.present_position > closed_pos + 5)
        TEST_ASSERT_EQUAL(closed_pos, dynamixel.present_position);
}

TEST(dynamixel, opened_closed_postion_inverse) {
    uint32_t closed_pos = 700;
    uint32_t opened_pos = 300;
    SS_dynamixel_set_closed_position(&dynamixel, closed_pos);
    SS_dynamixel_set_opened_position(&dynamixel, opened_pos);
    SS_dynamixel_start_communication(&dynamixel);
    SS_dynamixel_open(&dynamixel);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < opened_pos - 5 || dynamixel.present_position > opened_pos + 5)
        TEST_ASSERT_EQUAL(opened_pos, dynamixel.present_position);
    SS_dynamixel_close(&dynamixel);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < closed_pos - 5 || dynamixel.present_position > closed_pos + 5)
        TEST_ASSERT_EQUAL(closed_pos, dynamixel.present_position);
}


TEST(dynamixel, opened_limit) {
    uint32_t opened_pos = 111;
    SS_dynamixel_set_opened_position(&dynamixel, opened_pos);
    SS_dynamixel_start_communication(&dynamixel);
    SS_dynamixel_set_goal_position(&dynamixel, 11);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < opened_pos - 5 || dynamixel.present_position > opened_pos + 5)
        TEST_ASSERT_EQUAL(opened_pos, dynamixel.present_position);
}

TEST(dynamixel, closed_limit) {
    uint32_t closed_pos = 666;
    SS_dynamixel_set_closed_position(&dynamixel, closed_pos);
    SS_dynamixel_start_communication(&dynamixel);
    SS_dynamixel_set_goal_position(&dynamixel, 999);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < closed_pos - 5 || dynamixel.present_position > closed_pos + 5)
        TEST_ASSERT_EQUAL(closed_pos, dynamixel.present_position);
}

TEST(dynamixel, disable_torque_on_stop) {
    TEST_IGNORE();
    //Implement timeout if needed
    int32_t pos = 888;
    SS_dynamixel_set_goal_position(&dynamixel, pos);
    HAL_Delay(100);
    while(dynamixel.moving);
    if (dynamixel.present_position < pos - 5 || dynamixel.present_position > pos + 5)
        TEST_ASSERT_EQUAL(pos, dynamixel.present_position);
    TEST_ASSERT_FALSE(dynamixel.torque_enabled);
}

TEST(dynamixel, connection) {
   SS_disable_supply(&kozackie_servo_supply);
   HAL_Delay(3000);
   TEST_ASSERT_FALSE(dynamixel.connected);
    if(kozackie_servo_supply.voltage > 0.5) {
        TEST_FAIL_MESSAGE("Supply not turned off correctly");
    }
   TEST_ASSERT_LESS_OR_EQUAL(0.5, kozackie_servo_supply.voltage);
   SS_enable_supply(&kozackie_servo_supply);
   HAL_Delay(500);
   TEST_ASSERT_TRUE(dynamixel.connected);
}

TEST(dynamixel_logic, ping) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x03, 0x00, 0x01, 0x19, 0x4E };
    SS_dynamixel_ping(&dynamixel);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
}

TEST(dynamixel_logic, ping_DMA) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x03, 0x00, 0x01, 0x19, 0x4E };
    SS_dynamixel_ping_DMA(&dynamixel);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, tx_packet_buff, sizeof(expected));
    HAL_Delay(UART_TIMEOUT);
}

TEST(dynamixel, ping_DMA) {
    uint8_t expected[] = { 0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x55, 0x00 };
    SS_dynamixel_ping_DMA(&dynamixel);
    HAL_Delay(UART_TIMEOUT);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status);
}

#endif
