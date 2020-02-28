/*
 * SS_servos_test.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#ifdef RUN_TESTS

#include "SS_servos.h"
#include "unity_fixture.h"

static uint32_t MIN_PULSE_WIDTH_tmp;
static uint32_t MAX_PULSE_WIDTH_tmp;
static uint32_t SERVO_FREQUENCY_tmp;
static uint32_t SERVO_RANGE_tmp;

extern uint16_t SS_servo_get_width(uint16_t position);

TEST_GROUP(servos);
TEST_GROUP(grazyna_servos);

TEST_GROUP_RUNNER(servos) {
    RUN_TEST_CASE(servos, get_width);
#ifndef SIMULATE
    RUN_TEST_CASE(servos, freq50_range100);
    RUN_TEST_CASE(servos, freq300_range2000);
#endif
#if !defined(SERVOS_NO_TIMEOUT) && !defined(SIMULATE)
    RUN_TEST_CASE(servos, timeout);
#endif
}

TEST_GROUP_RUNNER(grazyna_servos) {
    RUN_TEST_CASE(grazyna_servos, open);
    RUN_TEST_CASE(grazyna_servos, close);
    RUN_TEST_CASE(grazyna_servos, wrong_id);
    RUN_TEST_CASE(grazyna_servos, set_position);
    RUN_TEST_CASE(grazyna_servos, set_closed_position);
    RUN_TEST_CASE(grazyna_servos, set_opened_position);
    RUN_TEST_CASE(grazyna_servos, set_range);
    RUN_TEST_CASE(grazyna_servos, get_position);
    RUN_TEST_CASE(grazyna_servos, get_closed_position);
    RUN_TEST_CASE(grazyna_servos, get_opened_position);
    RUN_TEST_CASE(grazyna_servos, get_range);
#ifndef SIMULATE
    RUN_TEST_CASE(grazyna_servos, disable);
#endif
}

static void set_up() {
    MIN_PULSE_WIDTH_tmp = MIN_PULSE_WIDTH;
    MAX_PULSE_WIDTH_tmp = MAX_PULSE_WIDTH;
    SERVO_FREQUENCY_tmp = SERVO_FREQUENCY;
    SERVO_RANGE_tmp = SERVO_RANGE;
}

static void tear_down() {
    MIN_PULSE_WIDTH = MIN_PULSE_WIDTH_tmp;
    MAX_PULSE_WIDTH = MAX_PULSE_WIDTH_tmp;
    SERVO_FREQUENCY = SERVO_FREQUENCY_tmp;
    SERVO_RANGE = SERVO_RANGE_tmp;
    SS_servos_init();
}

TEST_SETUP(servos) {
    set_up();
}

TEST_TEAR_DOWN(servos) {
    tear_down();
}

TEST_SETUP(grazyna_servos) {
    set_up();
}

TEST_TEAR_DOWN(grazyna_servos) {
    tear_down();
}

TEST(servos, get_width) {
    MIN_PULSE_WIDTH = 1000;
    MAX_PULSE_WIDTH = 2000;
    SERVO_RANGE = 200;
    TEST_ASSERT_EQUAL_INT(1000, SS_servo_get_width(0));
    TEST_ASSERT_EQUAL_INT(1500, SS_servo_get_width(100));
    TEST_ASSERT_EQUAL_INT(2000, SS_servo_get_width(200));
    SERVO_RANGE = 1000;
    TEST_ASSERT_EQUAL_INT(1000, SS_servo_get_width(0));
    TEST_ASSERT_EQUAL_INT(1500, SS_servo_get_width(500));
    TEST_ASSERT_EQUAL_INT(2000, SS_servo_get_width(1000));
    MIN_PULSE_WIDTH = 700;
    MAX_PULSE_WIDTH = 2300;
    TEST_ASSERT_EQUAL_INT(700, SS_servo_get_width(0));
    TEST_ASSERT_EQUAL_INT(1500, SS_servo_get_width(500));
    TEST_ASSERT_EQUAL_INT(2300, SS_servo_get_width(1000));
}

TEST(servos, freq50_range100) {
    MIN_PULSE_WIDTH = 1000;
    MAX_PULSE_WIDTH = 2000;
    SERVO_FREQUENCY = 50;
    SERVO_RANGE = 1000;
    SS_servos_init();
    SS_servo_close(&servos[1]);
    TEST_ASSERT_EQUAL_INT(50, *servos[1].pointer);
    SS_servo_open(&servos[1]);
    TEST_ASSERT_EQUAL_INT(100, *servos[1].pointer);
    SS_servo_set_position(&servos[1], 500);
    TEST_ASSERT_EQUAL_INT(75, *servos[1].pointer);
    TEST_ASSERT_EQUAL_INT(500, servos[1].position);
}

TEST(servos, freq300_range2000) {
    MIN_PULSE_WIDTH = 1000;
    MAX_PULSE_WIDTH = 2000;
    SERVO_FREQUENCY = 300;
    SERVO_RANGE = 2000;
    SS_servos_init();
    SS_servo_close(&servos[1]);
    TEST_ASSERT_EQUAL_INT(300, *servos[1].pointer);
    SS_servo_open(&servos[1]);
    TEST_ASSERT_EQUAL_INT(600, *servos[1].pointer);
    SS_servo_set_position(&servos[1], 1000);
    TEST_ASSERT_EQUAL_INT(450, *servos[1].pointer);
    TEST_ASSERT_EQUAL_INT(1000, servos[1].position);
}

TEST(servos, timeout) {
    MIN_PULSE_WIDTH = 1000;
    MAX_PULSE_WIDTH = 2000;
    SERVO_FREQUENCY = 300;
    SERVO_RANGE = 1000;
    for (int i = 0; i < sizeof(servos) / sizeof(servos[1]); i++) {
        SS_servo_open(&servos[i]);
    }
    for (int i = 0; i < sizeof(servos) / sizeof(servos[1]); i++) {
        TEST_ASSERT_EQUAL(600, *servos[i].pointer);
        TEST_ASSERT_TRUE(SS_supply_get_state(servos[i].supply));
    }

    HAL_Delay(SERVO_TIMEOUT + 1);
    for (int i = 0; i < sizeof(servos) / sizeof(servos[1]); i++) {
        TEST_ASSERT_EQUAL(0, *servos[i].pointer);
        TEST_ASSERT_FALSE(SS_supply_get_state(servos[i].supply));
    }
    for (int i = 0; i < sizeof(servos) / sizeof(servos[1]); i++) {
        SS_servo_open(&servos[i]);
    }
    for (int i = 0; i < sizeof(servos) / sizeof(servos[1]); i++) {
        TEST_ASSERT_EQUAL(600, *servos[i].pointer);
        TEST_ASSERT_TRUE(SS_supply_get_state(servos[i].supply));
    }
}

static void test_grazyna_servo_open(uint8_t servo_id) {
    SS_servo_close(&servos[servo_id]);
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_OPEN };
    TEST_ASSERT_EQUAL(servos[servo_id].closed_position, servos[servo_id].position);
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(servos[servo_id].opened_position, servos[servo_id].position);
}

static void test_grazyna_servo_close(uint8_t servo_id) {
    SS_servo_open(&servos[servo_id]);
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_CLOSE };
    TEST_ASSERT_EQUAL(servos[servo_id].opened_position, servos[servo_id].position);
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(servos[servo_id].closed_position, servos[servo_id].position);
}

static void test_grazyna_servo_set_position(uint8_t servo_id) {
    uint16_t target = 300;
    SS_servo_open(&servos[servo_id]);
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_POSITION,
            .payload = target };
    TEST_ASSERT_EQUAL(servos[servo_id].opened_position, servos[servo_id].position);
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, servos[servo_id].position);
}

static void test_grazyna_servo_set_opened_position(uint8_t servo_id) {
    uint16_t target = 444;
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_OPENED_POSITION,
            .payload = target };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, servos[servo_id].opened_position);
}

static void test_grazyna_servo_set_closed_position(uint8_t servo_id) {
    uint32_t target = 531;
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_CLOSED_POSITION,
            .payload = target };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, servos[servo_id].closed_position);
}

static void test_grazyna_servo_get_position(uint8_t servo_id) {
    uint16_t target = 777;
    SS_servo_set_position(&servos[servo_id], target);
    ComFrameContent frame = {
            .action = COM_REQUEST,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_POSITION, };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, frame.payload);
    TEST_ASSERT_EQUAL(UINT16, frame.data_type);
}

static void test_grazyna_servo_get_opened_position(uint8_t servo_id) {
    uint16_t target = 3;
    SS_servo_set_opened_position(&servos[servo_id], target);
    ComFrameContent frame = {
            .action = COM_REQUEST,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_OPENED_POSITION, };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, frame.payload);
    TEST_ASSERT_EQUAL(UINT16, frame.data_type);
}

static void test_grazyna_servo_get_closed_position(uint8_t servo_id) {
    uint16_t target = 1000;
    SS_servo_set_closed_position(&servos[servo_id], target);
    ComFrameContent frame = {
            .action = COM_REQUEST,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_CLOSED_POSITION, };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, frame.payload);
    TEST_ASSERT_EQUAL(UINT16, frame.data_type);
}

TEST(grazyna_servos, open) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_open(i);
    }
}

TEST(grazyna_servos, close) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_close(i);
    }
}

TEST(grazyna_servos, wrong_id) {
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = sizeof(servos) / sizeof(servos[0]),
            .message_type = COM_SERVO_CLOSE };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_ERROR, res);
}

TEST(grazyna_servos, set_position) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_set_position(i);
    }
}

TEST(grazyna_servos, set_closed_position) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_set_closed_position(i);
    }
}

TEST(grazyna_servos, set_opened_position) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_set_opened_position(i);
    }
}

TEST(grazyna_servos, disable) {
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .message_type = COM_SERVO_DISABLE, };
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        SS_servo_open(&servos[i]);
        TEST_ASSERT_NOT_EQUAL(0, *servos[i].pointer);
        frame.id = i;
        ComActionID res = SS_com_handle_action(&frame);
        TEST_ASSERT_EQUAL(COM_OK, res);
    }
    HAL_Delay(SERVO_TIMEOUT +10);
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        TEST_ASSERT_EQUAL(0, *servos[i].pointer);
    }
}

TEST(grazyna_servos, get_position) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_get_position(i);
    }
}

TEST(grazyna_servos, get_closed_position) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_get_closed_position(i);
    }
}

TEST(grazyna_servos, get_opened_position) {
    for (uint8_t i = 0; i < sizeof(servos) / sizeof(servos[0]); i++) {
        test_grazyna_servo_get_opened_position(i);
    }
}

TEST(grazyna_servos, set_range) {
    uint16_t range = 2000;
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .message_type = COM_SERVOS_RANGE,
            .payload = range };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(range, SERVO_RANGE);
}

TEST(grazyna_servos, get_range) {
    SERVO_RANGE = 6999;
    ComFrameContent frame = {
            .action = COM_REQUEST,
            .device = COM_SERVO_ID,
            .message_type =
            COM_SERVOS_RANGE, };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(SERVO_RANGE, frame.payload);
    TEST_ASSERT_EQUAL(UINT16, frame.data_type);
}

#endif

