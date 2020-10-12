/*
 * SS_MPU9250_Tests.c
 *
 *  Created on: 25.02.2020
 *      Author: dawid
 */

#include "SS_MPU9250.c"
#include "unity_fixture.h"

static int gpio_pin_state;
static int32_t accel_calibration_data[12] = {0};  // A place to hold the factory accelerometer trim biases
static uint8_t accel_prior_range;
static uint8_t gyro_prior_range;
static int32_t bias[6] = {0};
static int32_t prior_offset[6] = {1000, 1000, 1000};

static void SS_MPU_calibration_read();

MPU9250 *mpu9250;

MPU_STATUS function_response;

TEST_GROUP(MPU);

TEST_GROUP_RUNNER(MPU) {
    for(uint8_t i = 0; i < MAX_MPU_COUNT; i++) {
        mpu9250 = mpu_pointers[i];
        if(mpu9250 == NULL) {
            continue;
        }
        SS_MPU_calibration_read();
        /* RUN_TEST_CASE(MPU, init); */ // There is no reason to use it since test reinit exists
        RUN_TEST_CASE(MPU, set_accel_scale);
        RUN_TEST_CASE(MPU, get_accel_data);
        RUN_TEST_CASE(MPU, get_gyro_data);
        RUN_TEST_CASE(MPU, get_data_DMA);
        RUN_TEST_CASE(MPU, CS_ENABLE);
        RUN_TEST_CASE(MPU, CS_DISABLE);
        RUN_TEST_CASE(MPU, write_byte);
        RUN_TEST_CASE(MPU, read_byte);
        RUN_TEST_CASE(MPU, write_check_byte);
        RUN_TEST_CASE(MPU, read_multiple);
        RUN_TEST_CASE(MPU, who_am_i);
        RUN_TEST_CASE(MPU, set_gyro_bandwidth);
        RUN_TEST_CASE(MPU, set_gyro_dlpf_bypass);
        RUN_TEST_CASE(MPU, set_accel_bandwidth);
        RUN_TEST_CASE(MPU, set_gyro_scale);
        RUN_TEST_CASE(MPU, MPU_set_clk);
        RUN_TEST_CASE(MPU, MPU_self_test);
        RUN_TEST_CASE(MPU, math_scaled_gyro);
        RUN_TEST_CASE(MPU, math_scaled_accel);
        /*RUN_TEST_CASE(MPU, get_fifo_counter);*/ // There did not exist any of those functions
        /*RUN_TEST_CASE(MPU, get_fifo_data);   */
        /*RUN_TEST_CASE(MPU, set_fifo_data);   */
        RUN_TEST_CASE(MPU, INT_enable);
        RUN_TEST_CASE(MPU, sleep);
        RUN_TEST_CASE(MPU, calibrate);
        RUN_TEST_CASE(MPU, set_calibration);
        RUN_TEST_CASE(MPU, accel_write_calibration);
        RUN_TEST_CASE(MPU, gyro_write_calibration);
        RUN_TEST_CASE(MPU, set_smplrt);
        RUN_TEST_CASE(MPU, MPU_reset);
        RUN_TEST_CASE(MPU, bias);
        RUN_TEST_CASE(MPU, Reinit);  // MPU is Reinitialized to prior settings
    }
}
static void set_up(void) {
    /* HAL_NVIC_DisableIRQ(MPU1_INT_EXTI_IRQn); */

    /* HAL_NVIC_DisableIRQ(MPU2_INT_EXTI_IRQn); */

    HAL_Delay(20);
}
static void tear_down(void) {
    /* HAL_NVIC_EnableIRQ(MPU1_INT_EXTI_IRQn); */

    /* HAL_NVIC_EnableIRQ(MPU2_INT_EXTI_IRQn); */

    HAL_Delay(20);
}
void SS_MPU_different_scale_accel_data_check(int register_value, int sensor_value, int delta) {
    uint8_t data_container;

    SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, &data_container);
    TEST_ASSERT_EQUAL_INT(register_value, data_container);  //Check if Data was written correctly

    SS_MPU_get_accel_data(mpu9250);
    TEST_ASSERT_INT_WITHIN(delta, sensor_value, mpu9250->accel_raw_z);
    TEST_ASSERT_INT_WITHIN(delta, 0, mpu9250->accel_raw_y);
    TEST_ASSERT_INT_WITHIN(delta, 0, mpu9250->accel_raw_x);
}
void SS_MPU_different_scale_gyro_data_check(int register_value) {
    uint8_t data_container;

    SS_MPU_read_byte(mpu9250, MPU_GYRO_CONFIG, &data_container);
    TEST_ASSERT_EQUAL_INT(register_value, data_container);  //Check if Data was written correctly

    SS_MPU_get_gyro_data(mpu9250);

    TEST_ASSERT_LESS_THAN(prior_offset[0], abs(mpu9250->gyro_raw_x));
    TEST_ASSERT_LESS_THAN(prior_offset[1], abs(mpu9250->gyro_raw_y));
    TEST_ASSERT_LESS_THAN(prior_offset[2], abs(mpu9250->gyro_raw_z));

    prior_offset[0] = abs(mpu9250->gyro_raw_x);
    prior_offset[1] = abs(mpu9250->gyro_raw_y);
    prior_offset[2] = abs(mpu9250->gyro_raw_z);
}
static void SS_MPU_calibration_read() {
    SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, &accel_prior_range);
    SS_MPU_read_byte(mpu9250, MPU_GYRO_CONFIG, &gyro_prior_range);

    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    int8_t data1[6] = {0, 0, 0, 0, 0, 0};

    SS_MPU_read_multiple(mpu9250, MPU_XA_OFFSET_H, &data[0], 2);

    accel_calibration_data[0] = (int32_t)(((int16_t) data[0] << 8) | data[1]);
    SS_MPU_read_multiple(mpu9250, MPU_YA_OFFSET_H, &data[0], 2);

    accel_calibration_data[1] = (int32_t)(((int16_t) data[0] << 8) | data[1]);
    SS_MPU_read_multiple(mpu9250, MPU_ZA_OFFSET_H, &data[0], 2);

    accel_calibration_data[2] = (int32_t)(((int16_t) data[0] << 8) | data[1]);

    SS_MPU_read_byte(mpu9250, MPU_XG_OFFSET_L, &data[3]);
    data1[3] = (int8_t) data[3];
    SS_MPU_read_byte(mpu9250, MPU_YG_OFFSET_L, &data[4]);
    data1[4] = (int8_t) data[4];
    SS_MPU_read_byte(mpu9250, MPU_ZG_OFFSET_L, &data[5]);
    data1[5] = (int8_t) data[5];
    for(int i = 0; i < 3; i++) {
        bias[i] = (int32_t)((data1[i + 3] * (-4)));
    }
    SS_AK8963_reset(mpu9250);
    HAL_Delay(50);
    SS_MPU_reset(mpu9250);
    HAL_Delay(50);
    SS_MPU_init(mpu9250);
    SS_MPU_read_multiple(mpu9250, MPU_XA_OFFSET_H, &data[0], 2);  // Read factory accelerometer trim values
    accel_calibration_data[3] = (int32_t)(((int16_t) data[0] << 8) | data[1]);
    SS_MPU_read_multiple(mpu9250, MPU_YA_OFFSET_H, &data[0], 2);
    accel_calibration_data[4] = (int32_t)(((int16_t) data[0] << 8) | data[1]);
    SS_MPU_read_multiple(mpu9250, MPU_ZA_OFFSET_H, &data[0], 2);
    accel_calibration_data[5] = (int32_t)(((int16_t) data[0] << 8) | data[1]);

    for(int i = 0; i < 3; i++) {
        bias[i + 3] = (accel_calibration_data[i + 3] - accel_calibration_data[i]) * 8;
    }
}

static void SS_Check_bias_test() {
    int32_t bias_data[6];

    SS_MPU_write_check_byte(mpu9250, MPU_XA_OFFSET_H, 0);
    SS_MPU_write_check_byte(mpu9250, MPU_XA_OFFSET_L, 0);
    SS_MPU_write_check_byte(mpu9250, MPU_YA_OFFSET_H, 0);
    SS_MPU_write_check_byte(mpu9250, MPU_YA_OFFSET_L, 0);
    SS_MPU_write_check_byte(mpu9250, MPU_ZA_OFFSET_H, 0);
    SS_MPU_write_check_byte(mpu9250, MPU_ZA_OFFSET_L, 0);

    SS_MPU_write_check_byte(mpu9250, MPU_XG_OFFSET_H, 2);
    SS_MPU_write_check_byte(mpu9250, MPU_XG_OFFSET_L, 2);
    SS_MPU_write_check_byte(mpu9250, MPU_YG_OFFSET_H, 2);
    SS_MPU_write_check_byte(mpu9250, MPU_YG_OFFSET_L, 2);
    SS_MPU_write_check_byte(mpu9250, MPU_ZG_OFFSET_H, 2);
    SS_MPU_write_check_byte(mpu9250, MPU_ZG_OFFSET_L, 2);

    SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_16);
    SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_250);
    SS_MPU_get_accel_data(mpu9250);
    SS_MPU_get_gyro_data(mpu9250);

    bias_data[0] = (mpu9250->gyro_raw_x);
    bias_data[1] = (mpu9250->gyro_raw_y);
    bias_data[2] = (mpu9250->gyro_raw_z);
    bias_data[3] = (mpu9250->accel_raw_x);
    bias_data[4] = (mpu9250->accel_raw_y);
    bias_data[5] = (mpu9250->accel_raw_z);

    SS_MPU_get_gyro_data(mpu9250);
    TEST_ASSERT_INT_WITHIN(400, 2000, abs(bias_data[0]));  // it is only checked if data is different from 0
    TEST_ASSERT_INT_WITHIN(400, 2000, abs(bias_data[1]));
    TEST_ASSERT_INT_WITHIN(400, 2000, abs(bias_data[2]));
    TEST_ASSERT_INT_WITHIN(2000, 5500, abs(bias_data[3]));
    TEST_ASSERT_INT_WITHIN(2000, 5500, abs(bias_data[4]));
    TEST_ASSERT_INT_WITHIN(2000, 5500, abs(bias_data[5]));

    int32_t bias1[] = {bias[0], bias[1], bias[2], bias_data[3] * 8, bias_data[4] * 8, (bias_data[5] * 8 - 16300)};  // Gyro_set function is bizarre this is only way to set gyro offset properly

    function_response = SS_MPU_set_calibration(mpu9250, bias1);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    SS_MPU_get_accel_data(mpu9250);
    SS_MPU_get_gyro_data(mpu9250);

    TEST_ASSERT_INT_WITHIN(100, 0, abs(mpu9250->accel_raw_x));
    TEST_ASSERT_INT_WITHIN(100, 0, abs(mpu9250->accel_raw_y));
    TEST_ASSERT_INT_WITHIN(200, 2000, abs(mpu9250->accel_raw_z));  // oscillations are too big
    TEST_ASSERT_INT_WITHIN(100, 0, abs(mpu9250->gyro_raw_x));
    TEST_ASSERT_INT_WITHIN(100, 0, abs(mpu9250->gyro_raw_y));
    TEST_ASSERT_INT_WITHIN(100, 0, abs(mpu9250->gyro_raw_z));
    


    SS_AK8963_reset(mpu9250);
    HAL_Delay(50);

    SS_MPU_reset(mpu9250);
    HAL_Delay(50);
    SS_MPU_init(mpu9250);
    SS_MPU_set_calibration(mpu9250, bias);
}

TEST_SETUP(MPU) {
    set_up();
}
TEST_TEAR_DOWN(MPU) {
    tear_down();
}
TEST(MPU, CS_ENABLE) {
    SS_MPU_CS_ENABLE(mpu9250);
    gpio_pin_state = HAL_GPIO_ReadPin(mpu9250->CS_Port, mpu9250->CS_Pin);
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, gpio_pin_state);
}
TEST(MPU, CS_DISABLE) {
    SS_MPU_CS_DISABLE(mpu9250);
    gpio_pin_state = HAL_GPIO_ReadPin(mpu9250->CS_Port, mpu9250->CS_Pin);
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, gpio_pin_state);
}
TEST(MPU, write_byte) {
    uint8_t data_container = 1;

    function_response = SS_MPU_write_byte(mpu9250, MPU_ACCEL_CONFIG, 0x00);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, &data_container);
    TEST_ASSERT_EQUAL_INT(0x00, data_container);  //Check if Data was written
}
TEST(MPU, read_byte) {
    uint8_t data_container;

    SS_MPU_write_byte(mpu9250, MPU_ACCEL_CONFIG, 0x18);
    function_response = SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, &data_container);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    TEST_ASSERT_EQUAL_INT(0x18, data_container);  //Check if Data was read correctly
    function_response = SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, '\0');
    TEST_ASSERT_EQUAL(MPU_COMM_ERROR, function_response);
}
TEST(MPU, write_check_byte) {
    function_response = SS_MPU_write_check_byte(mpu9250, MPU_ACCEL_CONFIG, 0x00);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, read_multiple) {
    function_response = SS_MPU_read_multiple(mpu9250, MPU_ACCEL_XOUT_H, mpu9250->rcv, 6);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    function_response = SS_MPU_read_multiple(mpu9250, MPU_ACCEL_XOUT_H, mpu9250->rcv, 0);
    TEST_ASSERT_EQUAL(MPU_COMM_ERROR, function_response);
    function_response = SS_MPU_read_multiple(mpu9250, MPU_ACCEL_XOUT_H, '\0', 6);
    TEST_ASSERT_EQUAL(MPU_COMM_ERROR, function_response);
}
TEST(MPU, who_am_i) {
    int my_address_is;

    my_address_is = SS_MPU_who_am_i(mpu9250);
    TEST_ASSERT_EQUAL(0x71, my_address_is);
}
TEST(MPU, set_gyro_bandwidth) {
    function_response = SS_MPU_set_gyro_bandwidth(mpu9250, MPU_GYRO_BAND_184);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, set_gyro_dlpf_bypass) {
    function_response = SS_MPU_set_gyro_dlpf_bypass(mpu9250, 2);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, set_accel_bandwidth) {
    function_response = SS_MPU_set_accel_bandwidth(mpu9250, MPU_ACCEL_BAND_184);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, set_gyro_scale) {
    int32_t offset[] = {500, 500, 500, -250, 350, 1100};

    SS_MPU_gyro_write_calibration(mpu9250, offset);

    function_response = SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_250);
    SS_MPU_different_scale_gyro_data_check(0x00);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_500);
    SS_MPU_different_scale_gyro_data_check(0x08);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_1000);
    SS_MPU_different_scale_gyro_data_check(0x10);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_2000);
    SS_MPU_different_scale_gyro_data_check(0x18);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    prior_offset[0] = 1000;
    prior_offset[1] = 1000;
    prior_offset[2] = 1000;
}
TEST(MPU, set_accel_scale) {
    function_response = SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_2);
    SS_MPU_different_scale_accel_data_check(0x00, 16000, 1000);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_4);
    SS_MPU_different_scale_accel_data_check(0x08, 8000, 500);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_8);
    SS_MPU_different_scale_accel_data_check(0x10, 4000, 250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_16);
    SS_MPU_different_scale_accel_data_check(0x18, 2000, 125);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, get_accel_data) {
    int16_t data_container;

    SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_8);
    function_response = SS_MPU_get_accel_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    data_container = mpu9250->accel_raw_z;
    SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_16);
    function_response = SS_MPU_get_accel_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    TEST_ASSERT_NOT_EQUAL_INT(mpu9250->accel_raw_z, data_container);  // Check if SS_MPU_get_accel_data read data

    SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_2);
}
TEST(MPU, get_gyro_data) {
    int16_t data_container;

    SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_2000);
    function_response = SS_MPU_get_gyro_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    data_container = mpu9250->gyro_raw_z;

    SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_500);
    function_response = SS_MPU_get_gyro_data(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    TEST_ASSERT_NOT_EQUAL_INT(mpu9250->gyro_raw_z, data_container);
    TEST_ASSERT_INT_WITHIN(200, 0, mpu9250->gyro_raw_z);  // check if Gyro in static position measure properly
    TEST_ASSERT_INT_WITHIN(200, 0, mpu9250->gyro_raw_y);
    TEST_ASSERT_INT_WITHIN(200, 0, mpu9250->gyro_raw_x);
}
TEST(MPU, get_data_DMA) {
    int16_t data_container;

     SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_2);
     HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQn);


     data_container = mpu9250->accel_raw_z;
     function_response = SS_MPU_get_data_DMA(mpu9250);
     TEST_ASSERT_EQUAL(MPU_OK, function_response);
     while(mpu9250->accel_raw_z == data_container)
         ;                                                       //Check if DMA is maned by Interrupts */
     TEST_ASSERT_INT_WITHIN(1000, 16000, mpu9250->accel_raw_z);  // check if Accel in static position measure properly
     TEST_ASSERT_INT_WITHIN(1000, 0, mpu9250->accel_raw_y);
     TEST_ASSERT_INT_WITHIN(1000, 0, mpu9250->accel_raw_x);

     HAL_NVIC_DisableIRQ(MPU_INT_EXTI_IRQn);

}

TEST(MPU, MPU_set_clk) {
    function_response = SS_MPU_set_clk(mpu9250, 0x01);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, MPU_self_test) {
    function_response = SS_MPU_self_test(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}

TEST(MPU, bias) {  // it is not used
    SS_Check_bias_test();
}
TEST(MPU, Reinit) {
    function_response = SS_MPU_init(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    SS_MPU_set_calibration(mpu9250, bias);

    function_response = SS_MPU_write_check_byte(mpu9250, MPU_ACCEL_CONFIG, accel_prior_range);  // here is set the previous range for accel
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    function_response = SS_MPU_write_check_byte(mpu9250, MPU_GYRO_CONFIG, gyro_prior_range);  // here is set the previous range for gyro
    TEST_ASSERT_EQUAL(MPU_OK, function_response);

    SS_MPU_get_accel_data(mpu9250);
    HAL_Delay(50);
    SS_MPU_get_gyro_data(mpu9250);
    HAL_Delay(50);
}
TEST(MPU, init) {
    SS_AK8963_reset(mpu9250);
    HAL_Delay(50);
    SS_MPU_reset(mpu9250);
    HAL_Delay(50);
    function_response = SS_MPU_init(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
    SS_Check_bias_test();
}
TEST(MPU, math_scaled_gyro) {
    function_response = SS_MPU_math_scaled_gyro(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, math_scaled_accel) {
    function_response = SS_MPU_math_scaled_accel(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
/* TEST(MPU, get_fifo_counter) { */
/*   function_response = SS_MPU_get_fifo_counter(mpu9250); */
/*     TEST_ASSERT_EQUAL(MPU_OK, function_response); */
/* } */
/* TEST(MPU, get_fifo_data) { */
   /* function_response = SS_MPU_get_fifo_data(mpu9250); */
/*     TEST_ASSERT_EQUAL(MPU_OK, function_response); */
/* } */
/* TEST(MPU, set_fifo_data) { */
   /*function_response = SS_MPU_set_fifo_data(mpu9250);
/*     TEST_ASSERT_EQUAL(MPU_OK, function_response); */
/* } */

TEST(MPU, INT_enable) {
    function_response = SS_MPU_INT_enable(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, MPU_reset) {
    SS_AK8963_reset(mpu9250);
    HAL_Delay(50);
    function_response = SS_MPU_reset(mpu9250);
    HAL_Delay(50);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, sleep) {
    function_response = SS_MPU_sleep(mpu9250, DISABLE);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, calibrate) {

    function_response = SS_MPU_calibrate(mpu9250);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, set_calibration) {
    function_response = SS_MPU_set_calibration(mpu9250, bias);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, accel_write_calibration) {
    function_response = SS_MPU_accel_write_calibration(mpu9250, bias);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, gyro_write_calibration) {
    function_response = SS_MPU_gyro_write_calibration(mpu9250, bias);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
TEST(MPU, set_smplrt) {
    function_response = SS_MPU_set_smplrt(mpu9250, 0);
    TEST_ASSERT_EQUAL(MPU_OK, function_response);
}
