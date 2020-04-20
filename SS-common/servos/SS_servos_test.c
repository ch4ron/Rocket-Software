/*
 * SS_servos_test.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "SS_servos.h"
#include "unity_fixture.h"
#include "string.h"

extern Servo *servo_pointers[MAX_SERVO_COUNT];
extern uint16_t SS_servo_get_width(uint16_t position);
extern ServosConfig servos_config;
ServosConfig tmp_config;

TEST_GROUP(servos);
TEST_GROUP(grazyna_servos);

TEST_GROUP_RUNNER(servos) {
    RUN_TEST_CASE(servos, get_width);
    RUN_TEST_CASE(servos, freq50_range100);
    RUN_TEST_CASE(servos, freq300_range2000);
#if !defined(SERVOS_NO_TIMEOUT)
//    RUN_TEST_CASE(servos, timeout);
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
    RUN_TEST_CASE(grazyna_servos, uninitialized_id);
    RUN_TEST_CASE(grazyna_servos, disable);
}

static void set_up() {
    memcpy(&tmp_config, &servos_config, sizeof(ServosConfig));
    SS_platform_servos_init();
}

extern void SS_servos_reinit();

static void tear_down() {
    memcpy(&servos_config, &tmp_config, sizeof(ServosConfig));
    SS_servos_reinit();
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
    servos_config.MIN_PULSE_WIDTH = 1000;
    servos_config.MAX_PULSE_WIDTH = 2000;
    servos_config.SERVO_RANGE = 200;
    TEST_ASSERT_EQUAL_INT(1000, SS_servo_get_width(0));
    TEST_ASSERT_EQUAL_INT(1500, SS_servo_get_width(100));
    TEST_ASSERT_EQUAL_INT(2000, SS_servo_get_width(200));
    servos_config.SERVO_RANGE = 1000;
    TEST_ASSERT_EQUAL_INT(1000, SS_servo_get_width(0));
    TEST_ASSERT_EQUAL_INT(1500, SS_servo_get_width(500));
    TEST_ASSERT_EQUAL_INT(2000, SS_servo_get_width(1000));
    servos_config.MIN_PULSE_WIDTH = 700;
    servos_config.MAX_PULSE_WIDTH = 2300;
    TEST_ASSERT_EQUAL_INT(700, SS_servo_get_width(0));
    TEST_ASSERT_EQUAL_INT(1500, SS_servo_get_width(500));
    TEST_ASSERT_EQUAL_INT(2300, SS_servo_get_width(1000));
}

TEST(servos, freq50_range100) {
    servos_config.MIN_PULSE_WIDTH = 1000;
    servos_config.MAX_PULSE_WIDTH = 2000;
    servos_config.SERVO_FREQUENCY = 50;
    servos_config.SERVO_RANGE = 1000;
    SS_servos_reinit();
    SS_servo_close(servo_pointers[1]);
    TEST_ASSERT_EQUAL_INT(50, *servo_pointers[1]->pointer);
    SS_servo_open(servo_pointers[1]);
    TEST_ASSERT_EQUAL_INT(100, *servo_pointers[1]->pointer);
    SS_servo_set_position(servo_pointers[1], 500);
    TEST_ASSERT_EQUAL_INT(75, *servo_pointers[1]->pointer);
    TEST_ASSERT_EQUAL_INT(500, servo_pointers[1]->position);
}

TEST(servos, freq300_range2000) {
    servos_config.MIN_PULSE_WIDTH = 1000;
    servos_config.MAX_PULSE_WIDTH = 2000;
    servos_config.SERVO_FREQUENCY = 300;
    servos_config.SERVO_RANGE = 2000;
    SS_servos_reinit();
    SS_servo_close(servo_pointers[1]);
    TEST_ASSERT_EQUAL_INT(300, *servo_pointers[1]->pointer);
    SS_servo_open(servo_pointers[1]);
    TEST_ASSERT_EQUAL_INT(600, *servo_pointers[1]->pointer);
    SS_servo_set_position(servo_pointers[1], 1000);
    TEST_ASSERT_EQUAL_INT(450, *servo_pointers[1]->pointer);
    TEST_ASSERT_EQUAL_INT(1000, servo_pointers[1]->position);
}

#ifdef SS_USE_SUPPLY
TEST(servos, timeout) {
    servos_config.MIN_PULSE_WIDTH = 1000;
    servos_config.MAX_PULSE_WIDTH = 2000;
    servos_config.SERVO_FREQUENCY = 300;
    servos_config.SERVO_RANGE = 1000;
    for (int i = 0; i < sizeof(servo_pointers) / sizeof(servo_pointers[1]); i++) {
        SS_servo_open(servo_pointers[i]);
    }
    for (int i = 0; i < sizeof(servo_pointers) / sizeof(servo_pointers[1]); i++) {
        TEST_ASSERT_EQUAL(600, *servo_pointers[i]->pointer);
        TEST_ASSERT_TRUE(SS_supply_get_state(servo_pointers[i]->supply));
    }

    HAL_Delay(SERVO_TIMEOUT + 1);
    for (int i = 0; i < sizeof(servo_pointers) / sizeof(servo_pointers[1]); i++) {
        TEST_ASSERT_EQUAL(0, *servo_pointers[i]->pointer);
        TEST_ASSERT_FALSE(SS_supply_get_state(servo_pointers[i]->supply));
    }
    for (int i = 0; i < sizeof(servo_pointers) / sizeof(servo_pointers[1]); i++) {
        SS_servo_open(servo_pointers[i]);
    }
    for (int i = 0; i < sizeof(servo_pointers) / sizeof(servo_pointers[1]); i++) {
        TEST_ASSERT_EQUAL(600, *servo_pointers[i]->pointer);
        TEST_ASSERT_TRUE(SS_supply_get_state(servo_pointers[i]->supply));
    }
}
#endif

static void test_grazyna_servo_open(uint8_t servo_id) {
    SS_servo_close(servo_pointers[servo_id]);
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_OPEN };
    TEST_ASSERT_EQUAL(servo_pointers[servo_id]->closed_position, servo_pointers[servo_id]->position);
    ComActionID res = (ComActionID) SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(servo_pointers[servo_id]->opened_position, servo_pointers[servo_id]->position);
}

static void test_grazyna_servo_close(uint8_t servo_id) {
    SS_servo_open(servo_pointers[servo_id]);
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_CLOSE };
    TEST_ASSERT_EQUAL(servo_pointers[servo_id]->opened_position, servo_pointers[servo_id]->position);
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(servo_pointers[servo_id]->closed_position, servo_pointers[servo_id]->position);
}

static void test_grazyna_servo_set_position(uint8_t servo_id) {
    uint16_t target = 300;
    SS_servo_open(servo_pointers[servo_id]);
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_POSITION,
            .payload = target };
    TEST_ASSERT_EQUAL(servo_pointers[servo_id]->opened_position, servo_pointers[servo_id]->position);
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, servo_pointers[servo_id]->position);
}

static void test_grazyna_servo_set_opened_position(uint8_t servo_id) {
    uint16_t target = 444;
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_OPENED_POSITION,
            .payload = target };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, servo_pointers[servo_id]->opened_position);
}

static void test_grazyna_servo_set_closed_position(uint8_t servo_id) {
    uint32_t target = 531;
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = servo_id,
            .message_type = COM_SERVO_CLOSED_POSITION,
            .payload = target };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(target, servo_pointers[servo_id]->closed_position);
}

static void test_grazyna_servo_get_position(uint8_t servo_id) {
    uint16_t target = 777;
    SS_servo_set_position(servo_pointers[servo_id], target);
    ComFrame frame = {
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
    SS_servo_set_opened_position(servo_pointers[servo_id], target);
    ComFrame frame = {
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
    SS_servo_set_closed_position(servo_pointers[servo_id], target);
    ComFrame frame = {
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
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_open(i);
    }
}

TEST(grazyna_servos, close) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_close(i);
    }
}

TEST(grazyna_servos, wrong_id) {
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = MAX_SERVO_COUNT,
            .message_type = COM_SERVO_CLOSE };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_ERROR, res);
}

TEST(grazyna_servos, set_position) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_set_position(i);
    }
}

TEST(grazyna_servos, set_closed_position) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_set_closed_position(i);
    }
}

TEST(grazyna_servos, set_opened_position) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_set_opened_position(i);
    }
}

TEST(grazyna_servos, disable) {
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .message_type = COM_SERVO_DISABLE, };
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        SS_servo_open(servo_pointers[i]);
        TEST_ASSERT_NOT_EQUAL(0, *servo_pointers[i]->pointer);
        frame.id = i;
        frame.action = COM_SERVICE;
        ComStatus res = SS_com_handle_action(&frame);
        TEST_ASSERT_EQUAL(COM_OK, res);
    }
    HAL_Delay(SERVO_TIMEOUT +10);
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        TEST_ASSERT_EQUAL(0, *servo_pointers[i]->pointer);
    }
}

TEST(grazyna_servos, get_position) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_get_position(i);
    }
}

TEST(grazyna_servos, get_closed_position) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_get_closed_position(i);
    }
}

TEST(grazyna_servos, get_opened_position) {
    for (uint8_t i = 0; i < MAX_SERVO_COUNT; i++) {
        test_grazyna_servo_get_opened_position(i);
    }
}

TEST(grazyna_servos, set_range) {
    uint16_t range = 2000;
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .message_type = COM_SERVOS_RANGE,
            .payload = range };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(range, servos_config.SERVO_RANGE);
}

TEST(grazyna_servos, get_range) {
    servos_config.SERVO_RANGE = 6999;
    ComFrame frame = {
            .action = COM_REQUEST,
            .device = COM_SERVO_ID,
            .message_type =
            COM_SERVOS_RANGE, };
    ComActionID res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(servos_config.SERVO_RANGE, frame.payload);
    TEST_ASSERT_EQUAL(UINT16, frame.data_type);
}

Servo test_servo = {.id = 1, .channel = TIM_CHANNEL_2};

TEST(grazyna_servos, uninitialized_id) {
    Servo *pointers[MAX_SERVO_COUNT];
    test_servo.tim = servo_pointers[0]->tim;
    memcpy(pointers, servo_pointers, MAX_SERVO_COUNT);
    SS_servos_deinit();
    SS_servo_init(&test_servo);
    ComFrame frame = {
            .action = COM_SERVICE,
            .device = COM_SERVO_ID,
            .id = 3,
            .message_type = COM_SERVO_CLOSE };
    ComStatus res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_ERROR, res);
    frame.id = 1;
    frame.action = COM_SERVICE;
    ComStatus status = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, status);
    SS_servos_deinit();
}
