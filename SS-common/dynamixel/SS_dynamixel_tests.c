/*
 * SS_dynamixel_test.c
 *
 *  Created on: Dec 29, 2019
 *      Author: maciek
 */

#include "SS_dynamixel.h"
#include "SS_fifo.h"
#include "SS_platform.h"
#include "SS_supply.h"
#include "unity_fixture.h"

static DynamixelStatus res;
/* Connect the servo before running this group */
TEST_GROUP(dynamixel);

TEST_GROUP_RUNNER(dynamixel) {
    SS_enable_supply(dynamixel.supply);
    vTaskDelay(1000);
    RUN_TEST_CASE(dynamixel, open);
    RUN_TEST_CASE(dynamixel, close);
    RUN_TEST_CASE(dynamixel, get_position);
    RUN_TEST_CASE(dynamixel, disable_torque);
    RUN_TEST_CASE(dynamixel, enable_torque);
    RUN_TEST_CASE(dynamixel, enable_led);
    RUN_TEST_CASE(dynamixel, disable_led);
    RUN_TEST_CASE(dynamixel, set_position);
    RUN_TEST_CASE(dynamixel, set_velocity);
    RUN_TEST_CASE(dynamixel, set_velocity_limit);
    RUN_TEST_CASE(dynamixel, set_opened_position);
    RUN_TEST_CASE(dynamixel, set_closed_position);
    RUN_TEST_CASE(dynamixel, get_moving);
    RUN_TEST_CASE(dynamixel, read);
    RUN_TEST_CASE(dynamixel, write_read_torque_enabled);
    RUN_TEST_CASE(dynamixel, write_read_torque_disabled);
    RUN_TEST_CASE(dynamixel, ping);
    /* RUN_TEST_CASE(dynamixel, opened_closed_postion); */
    /* RUN_TEST_CASE(dynamixel, opened_limit); */
    /* RUN_TEST_CASE(dynamixel, closed_limit); */
    /* RUN_TEST_CASE(dynamixel, write_read_dma); */
    /* RUN_TEST_CASE(dynamixel, write_read_torque_enabled_IT); */
    /* RUN_TEST_CASE(dynamixel, ping_IT); */
    /* RUN_TEST_CASE(dynamixel, connection); */
    /* RUN_TEST_CASE(dynamixel, write_read_torque_disabled_IT); */
    /* RUN_TEST_CASE(dynamixel, fifo_write); */
    /* RUN_TEST_CASE(dynamixel, fifo_multiple_messages); */
    /* RUN_TEST_CASE(dynamixel, IT_stress_test1); */
    /* RUN_TEST_CASE(dynamixel, IT_stress_test2); */
    /* RUN_TEST_CASE(dynamixel, IT_stress_test3); */
    /* RUN_TEST_CASE(dynamixel, IT_stress_test4); */
    /* RUN_TEST_CASE(dynamixel, systick_position); */
    /* RUN_TEST_CASE(dynamixel, disable_torque_on_stop); */
    /* RUN_TEST_CASE(dynamixel, systick_moving); */
    /* RUN_TEST_CASE(dynamixel, systick_current); */
    /* RUN_TEST_CASE(dynamixel, systick_temperature); */
    /*     RUN_TEST_CASE(dynamixel, opened_closed_postion_inverse); */
    RUN_TEST_CASE(dynamixel, factory_reset);
}

TEST_SETUP(dynamixel) {
    if(dynamixel.supply != NULL) {
        SS_enable_supply(dynamixel.supply);
    }
}

TEST_TEAR_DOWN(dynamixel) {
    /* SS_dynamixel_stop_communication(&dynamixel); */
}

TEST(dynamixel, open) {
    uint32_t value;
    res = SS_dynamixel_open(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_GOAL_POSITION, &value, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(dynamixel.opened_position, value);
}

TEST(dynamixel, close) {
    uint32_t value;
    res = SS_dynamixel_close(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_GOAL_POSITION, &value, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(dynamixel.closed_position, value);
}

TEST(dynamixel, enable_torque) {
    uint8_t value;
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C};

    /* Disable torque to ensure enable message is sent */
    res = SS_dynamixel_disable_torque(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);

    res = SS_dynamixel_enable_torque(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_TORQUE_ENABLE, &value, 1);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT8(0x01, value);
}

TEST(dynamixel, disable_torque) {
    uint8_t value;
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C};

    /* Enable torque to ensure enable message is sent */
    res = SS_dynamixel_enable_torque(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);

    res = SS_dynamixel_disable_torque(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_TORQUE_ENABLE, &value, 1);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT8(0x00, value);
}

TEST(dynamixel, enable_led) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C};
    res = SS_dynamixel_enable_led(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));
}

TEST(dynamixel, disable_led) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C};
    res = SS_dynamixel_disable_led(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));
}

TEST(dynamixel, set_position) {
    uint32_t value;
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C};

    res = SS_dynamixel_set_goal_position(&dynamixel, 333);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_GOAL_POSITION, &value, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(333, value);
}

TEST(dynamixel, set_velocity) {
    uint32_t velocity;
    SS_dynamixel_set_velocity(&dynamixel, 22);

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_GOAL_VELOCITY, &velocity, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(22, velocity);
}

TEST(dynamixel, set_velocity_limit) {
    uint32_t limit;
    SS_dynamixel_set_velocity_limit(&dynamixel, 1023);

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_VELOCITY_LIMIT, &limit, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(1023, limit);
}

TEST(dynamixel, set_opened_position) {
    uint32_t value;

    res = SS_dynamixel_set_opened_position(&dynamixel, 555);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_MAX_POSITION_LIMIT, &value, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(555, value);
}

TEST(dynamixel, set_closed_position) {
    uint32_t value;

    res = SS_dynamixel_set_closed_position(&dynamixel, 123);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);

    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_MIN_POSITION_LIMIT, &value, 4);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(123, value);
}

TEST(dynamixel, get_moving) {
    SS_dynamixel_set_goal_position(&dynamixel, 222);
    SS_dynamixel_set_goal_position(&dynamixel, 333);
    uint32_t start = HAL_GetTick();
    while(HAL_GetTick() - start < 2500) {
        SS_dynamixel_get_moving(&dynamixel);
        vTaskDelay(pdMS_TO_TICKS(2 * UART_TIMEOUT));
        if(dynamixel.moving) {
            return;
        }
    }
    TEST_FAIL_MESSAGE("Moving not true");
}

TEST(dynamixel, factory_reset) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x04, 0x00, 0x55, 0x00, 0xA1, 0x0C};
    res = SS_dynamixel_factory_reset(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));
}

TEST(dynamixel, ping) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x55, 0x00};
    res = SS_dynamixel_ping(&dynamixel);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));
}

TEST(dynamixel, read) {
    uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x05, 0x00, 0x55, 0x00, 0x01};
    uint8_t data;
    res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_ID, &data, 1);
    TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, res);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected));
    TEST_ASSERT_EQUAL_HEX8(0x01, data);
}

static void read_write_test() {
    uint32_t tmp, read;
    DynamixelStatus res;
    char buff[30];
    bool torque_status = dynamixel.torque_enabled;
    if((res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &tmp, 4))) {
        sprintf(buff, "Read failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    uint32_t data = 500;
    vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT));
    if((res = SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4))) {
        sprintf(buff, "Write failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT));
    if((res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4))) {
        sprintf(buff, "Read failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    TEST_ASSERT_EQUAL_UINT32(data, read);
    if((res = SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &tmp, 4))) {
        sprintf(buff, "Write failed, errorcode: %x", res);
        TEST_FAIL_MESSAGE(buff);
    }
    vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT));
    TEST_ASSERT_EQUAL(torque_status, dynamixel.torque_enabled);
}

TEST(dynamixel, write_read_torque_enabled) {
    SS_dynamixel_enable_torque(&dynamixel);
    vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT));
    read_write_test();
}

TEST(dynamixel, write_read_torque_disabled) {
    SS_dynamixel_disable_torque(&dynamixel);
    vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT));
    read_write_test();
}

TEST(dynamixel, get_position) {
    SS_dynamixel_set_goal_position(&dynamixel, 111);
    uint32_t start = HAL_GetTick();
    while(HAL_GetTick() - start < 2500) {
        SS_dynamixel_get_position(&dynamixel);
        vTaskDelay(pdMS_TO_TICKS(2 * UART_TIMEOUT));
        if(dynamixel.present_position > 108 && dynamixel.present_position < 114)
            break;
    }
    TEST_ASSERT_INT_WITHIN(10, 111, dynamixel.present_position);
}
/* static void read_write_test_IT() { */
/*     uint32_t read; */
/*     bool torque_status = dynamixel.torque_enabled; */
/*     uint32_t data = 500; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     vTaskDelay(pdMS_TO_TICKS(2 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     TEST_ASSERT_EQUAL_UINT32(data, read); */
/*     TEST_ASSERT_EQUAL(torque_status, dynamixel.torque_enabled); */
/* } */

/* TEST(dynamixel, write_read_torque_enabled_IT) { */
/*     SS_dynamixel_enable_torque(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT)); */
/*     read_write_test_IT(); */
/* } */

/* TEST(dynamixel, write_read_torque_disabled_IT) { */
/*     SS_dynamixel_disable_torque(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT)); */
/*     read_write_test_IT(); */
/* } */

/* TEST(dynamixel, write_read_dma) { */
/*     uint32_t read = 0, data = 500; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     /\* SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); *\/ */
/*     vTaskDelay(pdMS_TO_TICKS(3 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     TEST_ASSERT_EQUAL(data, read); */
/* } */

/* TEST(dynamixel_logic, Fifo) { */
/* Dynamixel_fifo_bufor buff = { { 0 }, 0 }; */
/* Dynamixel_fifo_bufor packet = { { 33 }, 0 }; */
/* while(SS_fifo_get_data(&dynamixel_fifo, &buff)); */
/* SS_fifo_put_data(&dynamixel_fifo, &packet); */
/* SS_fifo_get_data(&dynamixel_fifo, &buff); */
/* TEST_ASSERT_EQUAL_UINT8_ARRAY(packet.packet, buff.packet, MAX_PACKET_LENGTH); */
/* bool res = SS_fifo_get_data(&dynamixel_fifo, buff.packet); */
/* TEST_ASSERT_FALSE(res); */
/* } */

/* TEST(dynamixel, fifo_write) { */
/* uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCA, 0x89}; */
/* Dynamixel_fifo_bufor buff; */
/* while(SS_fifo_get_data(&dynamixel_fifo, &buff)) */
/* ; */
/* uint8_t data[] = {0x00, 0x02, 0x00, 0x00}; */
/* uint8_t buf = 1; */
/* SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_LED, &buf, 1); */
/* SS_dynamixel_write_IT(&dynamixel, 0x74, data, sizeof(data)); */
/* SS_fifo_get_data(&dynamixel_fifo, &buff); */
/* TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, buff.packet, sizeof(expected)); */
/* vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT); */
/* } */

/* TEST(dynamixel, fifo_multiple_messages) { */
/*     uint8_t led = 1, res, led_res; */
/*     uint32_t data = 500, tmp; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_LED, &led, 1); */
/*     vTaskDelay(pdMS_TO_TICKS(2 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &tmp, 4); */
/*     res = SS_dynamixel_read(&dynamixel, DYNAMIXEL_LED, &led_res, 1); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, res); */
/*     TEST_ASSERT_EQUAL(data, tmp); */
/*     TEST_ASSERT_EQUAL(led, led_res); */
/* } */

/* TEST(dynamixel, IT_stress_test1) { */
/*     uint32_t data = 1, read = 0; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 2; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 3; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 4; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 5; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 6; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 7; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 8; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     data = 9; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     vTaskDelay(pdMS_TO_TICKS(6 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     TEST_ASSERT_EQUAL(data, read); */
/* } */

/* TEST(dynamixel, IT_stress_test2) { */
/*     uint32_t data = 1, read = 0; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     data = 69; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     vTaskDelay(pdMS_TO_TICKS(6 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     TEST_ASSERT_EQUAL(data, read); */
/* } */

/* TEST(dynamixel, IT_stress_test3) { */
/*     uint32_t data = 1, read = 0; */
/*     SS_dynamixel_write_IT(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     data = 69; */
/*     SS_dynamixel_write(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &data, 4); */
/*     SS_dynamixel_read(&dynamixel, DYNAMIXEL_HOMING_OFFSET, &read, 4); */
/*     vTaskDelay(pdMS_TO_TICKS(6 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     TEST_ASSERT_EQUAL(data, read); */
/* } */

/* TEST(dynamixel, IT_stress_test4) { */
/*     uint8_t torque, led; */
/*     SS_dynamixel_disable_torque(&dynamixel); */
/*     SS_dynamixel_enable_led(&dynamixel); */
/*     SS_dynamixel_disable_led(&dynamixel); */
/*     SS_dynamixel_enable_led(&dynamixel); */
/*     SS_dynamixel_disable_led(&dynamixel); */
/*     SS_dynamixel_enable_led(&dynamixel); */
/*     SS_dynamixel_enable_torque(&dynamixel); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_LED, &led, 1); */
/*     SS_dynamixel_read_IT(&dynamixel, DYNAMIXEL_TORQUE_ENABLE, &torque, 1); */
/*     vTaskDelay(pdMS_TO_TICKS(6 * UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/*     TEST_ASSERT_EQUAL(1, led); */
/*     TEST_ASSERT_EQUAL(1, torque); */
/* } */

/* TEST(dynamixel, systick_position) { */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     int32_t goal_position = 3011; */
/*     SS_dynamixel_set_goal_position(&dynamixel, goal_position); */
/*     uint32_t start = HAL_GetTick(); */
/*     while (HAL_GetTick() - start < 2500) { */
/*         if (dynamixel.present_position > goal_position - 5 && dynamixel.present_position < goal_position + 5) */
/*             break; */
/*     } */
/*     if (dynamixel.present_position < goal_position - 5 || dynamixel.present_position > goal_position + 5) */
/*         TEST_ASSERT_EQUAL(goal_position, dynamixel.present_position); */
/* } */

/* TEST(dynamixel, systick_moving) { */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     /\* TEST_ASSERT_FALSE(dynamixel.moving); *\/ */
/*     SS_dynamixel_set_goal_position(&dynamixel, 2600); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     SS_dynamixel_set_goal_position(&dynamixel, 2400); */
/*     vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT)); */
/*     /\* TEST_ASSERT_TRUE(dynamixel.moving); *\/ */
/*     vTaskDelay(pdMS_TO_TICKS(500)); */
/*     /\* TEST_ASSERT_FALSE(dynamixel.moving); *\/ */
/* } */

/* TEST(dynamixel, systick_current) { */
/*     TEST_IGNORE(); */
/* } */

/* TEST(dynamixel, systick_temperature) { */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(60)); */
/*     TEST_ASSERT_LESS_OR_EQUAL(40, dynamixel.temperature); */
/*     TEST_ASSERT_GREATER_OR_EQUAL(15, dynamixel.temperature); */
/* } */

/* TEST(dynamixel, opened_closed_postion) { */
/*     uint32_t closed_pos = 300; */
/*     uint32_t opened_pos = 700; */
/*     SS_dynamixel_set_closed_position(&dynamixel, closed_pos); */
/*     SS_dynamixel_set_opened_position(&dynamixel, opened_pos); */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     SS_dynamixel_open(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < opened_pos - 5 || dynamixel.present_position > opened_pos + 5) */
/*         TEST_ASSERT_EQUAL(opened_pos, dynamixel.present_position); */
/*     SS_dynamixel_close(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < closed_pos - 5 || dynamixel.present_position > closed_pos + 5) */
/*         TEST_ASSERT_EQUAL(closed_pos, dynamixel.present_position); */
/* } */

/* TEST(dynamixel, opened_closed_postion_inverse) { */
/*     uint32_t closed_pos = 700; */
/*     uint32_t opened_pos = 300; */
/*     SS_dynamixel_set_closed_position(&dynamixel, closed_pos); */
/*     SS_dynamixel_set_opened_position(&dynamixel, opened_pos); */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     SS_dynamixel_open(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < opened_pos - 5 || dynamixel.present_position > opened_pos + 5) */
/*         TEST_ASSERT_EQUAL(opened_pos, dynamixel.present_position); */
/*     SS_dynamixel_close(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < closed_pos - 5 || dynamixel.present_position > closed_pos + 5) */
/*         TEST_ASSERT_EQUAL(closed_pos, dynamixel.present_position); */
/* } */

/* TEST(dynamixel, opened_limit) { */
/*     uint32_t opened_pos = 111; */
/*     SS_dynamixel_set_opened_position(&dynamixel, opened_pos); */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     SS_dynamixel_set_goal_position(&dynamixel, 11); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < opened_pos - 5 || dynamixel.present_position > opened_pos + 5) */
/*         TEST_ASSERT_EQUAL(opened_pos, dynamixel.present_position); */
/* } */

/* TEST(dynamixel, closed_limit) { */
/*     uint32_t closed_pos = 666; */
/*     SS_dynamixel_set_closed_position(&dynamixel, closed_pos); */
/*     SS_dynamixel_start_communication(&dynamixel); */
/*     SS_dynamixel_set_goal_position(&dynamixel, 999); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < closed_pos - 5 || dynamixel.present_position > closed_pos + 5) */
/*         TEST_ASSERT_EQUAL(closed_pos, dynamixel.present_position); */
/* } */

/* TEST(dynamixel, disable_torque_on_stop) { */
/*     TEST_IGNORE(); */
/*     //Implement timeout if needed */
/*     int32_t pos = 888; */
/*     SS_dynamixel_set_goal_position(&dynamixel, pos); */
/*     vTaskDelay(pdMS_TO_TICKS(100)); */
/*     while(dynamixel.moving); */
/*     if (dynamixel.present_position < pos - 5 || dynamixel.present_position > pos + 5) */
/*         TEST_ASSERT_EQUAL(pos, dynamixel.present_position); */
/*     TEST_ASSERT_FALSE(dynamixel.torque_enabled); */
/* } */

/* TEST(dynamixel, connection) { */
/*     SS_disable_supply(&kozackie_servo_supply); */
/*     vTaskDelay(pdMS_TO_TICKS(3000)); */
/*     TEST_ASSERT_FALSE(dynamixel.connected); */
/*     if(kozackie_servo_supply.measurement.value > 0.5) { */
/*         TEST_FAIL_MESSAGE("Supply not turned off correctly"); */
/*     } */
/*     TEST_ASSERT_LESS_OR_EQUAL(0.5, kozackie_servo_supply.measurement.value); */
/*     SS_enable_supply(&kozackie_servo_supply); */
/*     vTaskDelay(pdMS_TO_TICKS(500)); */
/*     TEST_ASSERT_TRUE(dynamixel.connected); */
/* } */

/* TEST(dynamixel, ping_IT) { */
/*     uint8_t expected[] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x07, 0x00, 0x55, 0x00}; */
/*     SS_dynamixel_ping_IT(&dynamixel); */
/*     vTaskDelay(pdMS_TO_TICKS(UART_TIMEOUT)); */
/*     TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dynamixel.rx_packet_buff, sizeof(expected)); */
/*     TEST_ASSERT_EQUAL_UINT8(DYNAMIXEL_RESULT_OK, dynamixel.last_status); */
/* } */
