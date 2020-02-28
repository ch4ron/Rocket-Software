/*
 * SS_supply_control_test.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#ifdef RUN_TESTS

#include "SS_supply.h"
#include "unity_fixture.h"
#include "SS_platform_init.h"

uint8_t relay_supply_status;
uint8_t servos1_supply_status;
uint8_t servos2_supply_status;
uint8_t kozackie_servo_supply_status;

TEST_GROUP(supply_control);

TEST_GROUP_RUNNER(supply_control) {
    SS_disable_supply(&relay_supply);
    SS_disable_supply(&servos1_supply);
    SS_disable_supply(&servos2_supply);
    SS_disable_supply(&kozackie_servo_supply);
    /* Wait for the capacitors to discharge */
    HAL_Delay(5000);
    RUN_TEST_CASE(supply_control, relay_supply);
    RUN_TEST_CASE(supply_control, servos1_supply);
    RUN_TEST_CASE(supply_control, servos2_supply);
    RUN_TEST_CASE(supply_control, kozackie_servo_supply);
    RUN_TEST_CASE(supply_control, timeout);
}

TEST_SETUP(supply_control) {
    relay_supply_status = SS_supply_get_state(&relay_supply);
    servos1_supply_status = SS_supply_get_state(&servos1_supply);
    servos2_supply_status = SS_supply_get_state(&servos2_supply);
    kozackie_servo_supply_status = SS_supply_get_state(&kozackie_servo_supply);
}

TEST_TEAR_DOWN(supply_control) {
    relay_supply_status ? SS_enable_supply(&relay_supply) : SS_disable_supply(&relay_supply);
    servos1_supply_status ? SS_enable_supply(&servos1_supply) : SS_disable_supply(&servos1_supply);
    servos2_supply_status ? SS_enable_supply(&servos2_supply) : SS_disable_supply(&servos2_supply);
    kozackie_servo_supply_status ? SS_enable_supply(&kozackie_servo_supply) : SS_disable_supply(&kozackie_servo_supply);
}

static void run_supply_test(Supply *supply, float min_voltage, float max_voltage) {
    TEST_ASSERT_FALSE(SS_supply_get_state(supply));
    if(supply->measurement.value > 0.7f) {
        TEST_FAIL_MESSAGE("Supply not turned off");
    }
    SS_enable_supply(supply);
    HAL_Delay(10);
    TEST_ASSERT_TRUE(SS_supply_get_state(supply));
    if(supply->measurement.value < min_voltage) {
        TEST_FAIL_MESSAGE("Supply voltage too low");
    }
    if(supply->measurement.value > max_voltage) {
        TEST_FAIL_MESSAGE("Supply voltage too high");
    }
}

TEST(supply_control, relay_supply) {
    run_supply_test(&relay_supply, 11.8f, 12.2f);
}

TEST(supply_control, servos1_supply) {
    run_supply_test(&servos1_supply, 4.8f, 8.5f);
}

TEST(supply_control, servos2_supply) {
    run_supply_test(&servos2_supply, 4.8f, 8.5f);
}

TEST(supply_control, kozackie_servo_supply) {
    run_supply_test(&kozackie_servo_supply, 11.8f, 12.2f);
}

static void test_supply_timeout(Supply *supply) {
    SS_enable_supply(supply);
    TEST_ASSERT_TRUE(SS_supply_get_state(supply));
    SS_supply_set_timeout(supply, 100);
    HAL_Delay(98);
    TEST_ASSERT_TRUE(SS_supply_get_state(supply));
    HAL_Delay(3);
    TEST_ASSERT_FALSE(SS_supply_get_state(supply));
    SS_enable_supply(supply);
    TEST_ASSERT_TRUE(SS_supply_get_state(supply));
}

TEST(supply_control, timeout) {
    test_supply_timeout(&relay_supply);
    test_supply_timeout(&servos1_supply);
    test_supply_timeout(&servos2_supply);
    test_supply_timeout(&kozackie_servo_supply);
}

#endif
