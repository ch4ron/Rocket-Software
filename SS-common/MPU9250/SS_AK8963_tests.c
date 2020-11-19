/*
 * SS_AK8963_Tests.c
 *
 *  Created on: 15.11.2020
 *      Author: Dawid
 */
/*
#include "SS_AK8963.c"
#include "SS_MPU9250.c"
#include "unity_fixture.h"

//MPU9250 *mpu9250;

MPU_STATUS function_response;

TEST_GROUP(MAGNETO);

TEST_GROUP_RUNNER(MAGNETO) {
    for(uint8_t i = 0; i < MAX_MAGNETO_COUNT; i++) {
        mpu9250 = mpu_pointers[i];
        if(mpu9250 == NULL) {
            continue;
        }
        RUN_TEST_CASE(MAGNETO,INIT);
        RUN_TEST_CASE(MAGNETO,CALIBRATE);
        RUN_TEST_CASE(MAGNETO,CALIBRATE2);
        RUN_TEST_CASE(MAGNETO,RESET);
        RUN_TEST_CASE(MAGNETO,WHO_AM_I);
        RUN_TEST_CASE(MAGNETO,GET_MGNT_DATA);
        RUN_TEST_CASE(MAGNETO,MATH_SCALED_MGNT);
        RUN_TEST_CASE(MAGNETO,CHECK_DATA_READY);
        RUN_TEST_CASE(MAGNETO,SELF_TEST);
        RUN_TEST_CASE(MAGNETO,SET_CALIBRATION_VALUES);
        RUN_TEST_CASE(MAGNETO,WRITE_REGISTER);
        RUN_TEST_CASE(MAGNETO,WRITE_CHECK_REGISTER);
        RUN_TEST_CASE(MAGNETO,READ_MULTIPLE);
        RUN_TEST_CASE(MAGNETO,START_READING_DATA);
        RUN_TEST_CASE(MAGNETO,READ_FUSE_DATA);
        RUN_TEST_CASE(MAGNETO,CALIBRATE_CYCLE);
        RUN_TEST_CASE(MAGNETO,CALIBRATE_CYCLE_2);

    }
}

static void set_up(void) {
    HAL_NVIC_DisableIRQ(MPU_INT_EXTI_IRQn);

    HAL_Delay(20);
}
static void tear_down(void) {
    HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQn);

    HAL_Delay(20);
}

TEST_SETUP(MAGNETO) {
    set_up();
}
TEST_TEAR_DOWN(MAGNETO) {
    tear_down();
}
TEST(MAGNETO, INIT) {
    function_response=SS_AK8963_init(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}

TEST(MAGNETO, CALIBRATE) {
    function_response=SS_AK8963_calibrate(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, CALIBRATE2) {
    function_response=SS_AK8963_calibrate2(mpu9250,mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, RESET) {
    function_response=SS_AK8963_reset(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, WHO_AM_I) {
    function_response=SS_AK8963_who_am_i(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, GET_MGNT_DATA) {
    function_response=SS_MPU_get_mgnt_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}

TEST(MAGNETO, MATH_SCALED_MGNT) {
    function_response=SS_MPU_math_scaled_mgnt(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, CHECK_DATA_READY) {
    function_response=SS_AK8963_check_data_ready(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, SELF_TEST) {
    function_response=SS_AK8963_self_test(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, SET_CALIBRATION_VALUSE) {
//    function_response=SS_AK8963_set_calibration_values(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, WRITE_REGISTER) {
    //function_response=SS_AK8963_write_register(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, WRITE_CHECK_REGISTER) {
    //function_response=SS_AK8963_write_check_register(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, READ_MULTIPLE) {
    //function_response=SS_AK8963_read_multiple(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, START_READING_DATA) {
    //function_response=SS_AK8963_start_reading_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, READ_FUSE_DATA) {
    //function_response = SS_AK8963_read_fuse_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, CALIBRATION_CYCLE) {
    //function_response=SS_AK8963_calibration_cycle(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MAGNETO, CALIBRATION_CYCLE_2) {
    //function_response=SS_AK8963_calibration_cycle2(mpu9250,mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}*/