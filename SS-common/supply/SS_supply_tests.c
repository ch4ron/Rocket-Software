/*
 * SS_supply_control_test.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "SS_platform.h"
#include "SS_supply.h"
#include "unity_fixture.h"

/* TODO Make tests platform independent  */

uint8_t relay_supply_status;
uint8_t servos1_supply_status;
uint8_t servos2_supply_status;
uint8_t dynamixel_supply_status;

TEST_GROUP(supply_control);

TEST_GROUP_RUNNER(supply_control) {
    SS_disable_supply(&relay_supply);
    SS_disable_supply(&servos1_supply);
    SS_disable_supply(&servos2_supply);
    SS_disable_supply(&dynamixel_supply);
    /* Wait for the capacitors to discharge */
    HAL_Delay(5000);
    RUN_TEST_CASE(supply_control, relay_supply);
    RUN_TEST_CASE(supply_control, servos1_supply);
    RUN_TEST_CASE(supply_control, servos2_supply);
    RUN_TEST_CASE(supply_control, dynamixel_supply);
    RUN_TEST_CASE(supply_control, timeout);
}

TEST_SETUP(supply_control) {
    relay_supply_status = SS_supply_get_state(&relay_supply);
    servos1_supply_status = SS_supply_get_state(&servos1_supply);
    servos2_supply_status = SS_supply_get_state(&servos2_supply);
    dynamixel_supply_status = SS_supply_get_state(&dynamixel_supply);
}

TEST_TEAR_DOWN(supply_control) {
    relay_supply_status ? SS_enable_supply(&relay_supply) : SS_disable_supply(&relay_supply);
    servos1_supply_status ? SS_enable_supply(&servos1_supply) : SS_disable_supply(&servos1_supply);
    servos2_supply_status ? SS_enable_supply(&servos2_supply) : SS_disable_supply(&servos2_supply);
    dynamixel_supply_status ? SS_enable_supply(&dynamixel_supply) : SS_disable_supply(&dynamixel_supply);
}

static void run_supply_test(Supply *supply, float target, float delta) {
    TEST_ASSERT_FALSE(SS_supply_get_state(supply));
    TEST_ASSERT_FLOAT_WITHIN(0.7f, 0.0f, supply->measurement.value);
    SS_enable_supply(supply);
    HAL_Delay(10);
    TEST_ASSERT_TRUE(SS_supply_get_state(supply));
    TEST_ASSERT_FLOAT_WITHIN(delta, target, supply->measurement.value);
}

TEST(supply_control, relay_supply) {
    run_supply_test(&relay_supply, 12.0f, 0.2f);
}

TEST(supply_control, servos1_supply) {
    run_supply_test(&servos1_supply, 6.0f, 2.5f);
}

TEST(supply_control, servos2_supply) {
    run_supply_test(&servos2_supply, 6.0f, 2.5f);
}

TEST(supply_control, dynamixel_supply) {
    run_supply_test(&dynamixel_supply, 12.0f, 0.2f);
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
    TEST_IGNORE();
    test_supply_timeout(&relay_supply);
    test_supply_timeout(&servos1_supply);
    test_supply_timeout(&servos2_supply);
    test_supply_timeout(&dynamixel_supply);
}
