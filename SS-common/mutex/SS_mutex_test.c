/*
 * SS_mutex_test.c
 *
 *  Created on: Jan 3, 2020
 *      Author: maciek
 */

#include "SS_mutex.h"

#include "tim.h"
#include "unity_fixture.h"

extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;

Mutex mutex;

TEST_GROUP(mutex);

TEST_GROUP_RUNNER(mutex) {
    HAL_TIM_Base_Start_IT(&htim13);
    HAL_TIM_Base_Start_IT(&htim14);
    HAL_Delay(10);
    RUN_TEST_CASE(mutex, no_critical_section);
    RUN_TEST_CASE(mutex, critical_section);
    HAL_TIM_Base_Stop_IT(&htim13);
    HAL_TIM_Base_Stop_IT(&htim14);
}

TEST_SETUP(mutex) {}

TEST_TEAR_DOWN(mutex) {}
volatile uint32_t dynamixel_counter;
volatile uint32_t dynamixel_counter_synchronized;
/* TESTING CRITICAL SECTION - probably worth moving to separate file */
/* For this test to run, configure TIM13 and TIM14 to run on high frequency */

void tim13_callback(TIM_HandleTypeDef *htim) {
    static uint32_t counter0 = 0;
    static uint32_t counter1 = 0;
    if (htim->Instance == TIM13) {
        if (SS_mutex_lock(&mutex)) {
            if (counter0 < 100000) {
                dynamixel_counter_synchronized++;
                counter0++;
            }
            SS_mutex_unlock(&mutex);
        }
        if (counter1 < 100000) {
            dynamixel_counter++;
            counter1++;
        }
    }
}

void tim14_callback(TIM_HandleTypeDef *htim) {
    static uint32_t counter0 = 0;
    static uint32_t counter1 = 0;
    if (htim->Instance == TIM14) {
        if (SS_mutex_lock(&mutex)) {
            if (counter0 < 100000) {
                dynamixel_counter_synchronized++;
                counter0++;
            }
            SS_mutex_unlock(&mutex);
        }
        if (counter1 < 100000) {
            dynamixel_counter++;
            counter1++;
        }
    }
}

void SS_dynamixel_test_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    tim13_callback(htim);
    tim14_callback(htim);
}

TEST(mutex, no_critical_section) {
    TEST_ASSERT_NOT_EQUAL(200000, dynamixel_counter);
}

TEST(mutex, critical_section) {
    TEST_ASSERT_EQUAL(200000, dynamixel_counter_synchronized);
}
