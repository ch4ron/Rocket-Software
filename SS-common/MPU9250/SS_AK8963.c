/*
 * SS_AK8963.c
 *
 *  Created on: Apr 5, 2019
 *      Author: Maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_AK8963.h"

#include "FreeRTOS.h"
#include "SS_misc.h"
#include "math.h"
#include "stm32f4xx_hal.h"
#include "task.h"

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static MPU_STATUS SS_AK8963_write_register(MPU9250 *mpu9250, uint8_t address, uint8_t data);
static MPU_STATUS SS_AK8963_write_check_register(MPU9250 *mpu9250, uint8_t address, uint8_t data);
static MPU_STATUS SS_AK8963_read_multiple(MPU9250 *mpu9250, uint8_t address, uint8_t *data, uint8_t count);
static MPU_STATUS SS_AK8963_start_reading_data(MPU9250 *mpu9250);
static MPU_STATUS SS_AK8963_read_fuse_data(MPU9250 *mpu9250);
static MPU_STATUS SS_AK8963_calibration_cycle(MPU9250 *mpu9250, int16_t *mag_bias, float *mag_scale);
static MPU_STATUS SS_AK8963_calibration_cycle2(MPU9250 *mpu1, MPU9250 *mpu2, int16_t *mag1_bias, float *mag1_scale, int16_t *mag2_bias, float *mag2_scale);

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */
MPU_STATUS SS_AK8963_init(MPU9250 *mpu9250) {
    MPU_STATUS result = MPU_OK;
    result |= SS_MPU_write_check_byte(mpu9250, MPU_USER_CTRL, 0x30);  //Enable I2C master, disable I2C slave
    HAL_Delay(1);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_I2C_MST_CTRL, 0x0D);  //Set I2C frequency to 400kHz
    HAL_Delay(50);
    result |= SS_AK8963_reset(mpu9250);
    HAL_Delay(1);
    result |= ((!(SS_AK8963_who_am_i(mpu9250) == 0x48)) << 2);  //Check if device id is correct
    HAL_Delay(1);
    result |= SS_AK8963_read_fuse_data(mpu9250);  //Read sensitivity adjustment values
    HAL_Delay(1);
    result |= SS_AK8963_self_test(mpu9250);
    HAL_Delay(1);
    result |= SS_AK8963_start_reading_data(mpu9250);  //Start taking measurements at 100Hz rate
    return result;
}

MPU_STATUS SS_AK8963_reset(MPU9250 *mpu9250) {
    return SS_AK8963_write_register(mpu9250, AK8963_CNTL2, 0x01);
}

uint8_t SS_AK8963_who_am_i(MPU9250 *mpu9250) {
    uint8_t i_am;
    SS_AK8963_read_multiple(mpu9250, AK8963_WHO_AM_I, &i_am, 1);
    return i_am;
}

MPU_STATUS SS_MPU_get_mgnt_data(MPU9250 *mpu9250) {
    uint8_t rcv[7];
    if(SS_MPU_read_multiple(mpu9250, MPU_EXT_SENS_DATA_00, rcv, 7))  //Read 7 bytes from MPU registers
        return MPU_COMM_ERROR;
    if((rcv[6] & 0x10)) {  //Check if the data is overflown
        mpu9250->mgnt_raw_x = (int16_t)((int16_t) rcv[1] << 8) | rcv[0];
        mpu9250->mgnt_raw_y = (int16_t)((int16_t) rcv[3] << 8) | rcv[2];
        mpu9250->mgnt_raw_z = (int16_t)((int16_t) rcv[5] << 8) | rcv[4];
    }
    return MPU_OK;
}

MPU_STATUS SS_MPU_math_scaled_mgnt(MPU9250 *mpu9250) {
    mpu9250->mgnt_scaled_x = (float) mpu9250->mgnt_scale_x * (mpu9250->mgnt_raw_x - mpu9250->mgnt_bias_x) * 0.15f * ((mpu9250->xASens - 128) * 0.5f / 128 + 1);
    mpu9250->mgnt_scaled_y = (float) mpu9250->mgnt_scale_y * (mpu9250->mgnt_raw_y - mpu9250->mgnt_bias_y) * 0.15f * ((mpu9250->yASens - 128) * 0.5f / 128 + 1);
    mpu9250->mgnt_scaled_z = (float) mpu9250->mgnt_scale_z * (mpu9250->mgnt_raw_z - mpu9250->mgnt_bias_z) * 0.15f * ((mpu9250->zASens - 128) * 0.5f / 128 + 1);
    return MPU_OK;
}

uint8_t SS_AK8963_check_data_ready(MPU9250 *mpu9250) {
    uint8_t rec;
    SS_AK8963_read_multiple(mpu9250, AK8963_ST1, &rec, 1);
    return rec & 0x01;
}

MPU_STATUS SS_AK8963_self_test(MPU9250 *mpu9250) {
    MPU_STATUS result = MPU_OK;
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_CNTL1, 0x00);  //Set Power-down mode
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_ASTC, 0x40);   //Write "1" to SELF bit of ASTC register(Generate magnetic field for self-test)
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_CNTL1, 0x18);  //Set Self-test mode
    uint32_t timeout = HAL_GetTick();
    while((HAL_GetTick() - timeout < 200) && !SS_AK8963_check_data_ready(mpu9250))
        ;  //Check Data Ready
    uint8_t rcv[7];
    result |= SS_AK8963_read_multiple(mpu9250, AK8963_XOUT_L, rcv, 7);      //Read measurement data (HXL to HZH)
    result |= !(rcv[6] & 0x10);                                             //check data overflow
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_ASTC, 0x00);   //Write "0" to SELF bit of ASTC register
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_CNTL1, 0x00);  //Set Power-down mode
    mpu9250->mgnt_raw_x = ((int16_t)((int16_t) rcv[1] << 8) | rcv[0]) * ((mpu9250->xASens - 128) * 0.5f / 128 + 1);
    mpu9250->mgnt_raw_y = ((int16_t)((int16_t) rcv[3] << 8) | rcv[2]) * ((mpu9250->yASens - 128) * 0.5f / 128 + 1);
    mpu9250->mgnt_raw_z = ((int16_t)((int16_t) rcv[5] << 8) | rcv[4]) * ((mpu9250->zASens - 128) * 0.5f / 128 + 1);
    if((mpu9250->mgnt_raw_x < -200) || (mpu9250->mgnt_raw_x > 200) ||
       (mpu9250->mgnt_raw_y < -200) || (mpu9250->mgnt_raw_y > 200) ||
       (mpu9250->mgnt_raw_z < -3200) || (mpu9250->mgnt_raw_z > -800)) {  //Self-test Judgment
        SS_print("MAGNETOMETER SELF TEST ERROR\r\n");
        return MPU_SELF_TEST_ERROR;
    }
    return result;
}

MPU_STATUS SS_AK8963_calibrate(MPU9250 *mpu9250) {  //Reads data using interrupts, enable them first
    SS_print("Magnetometer calibration: Wave the device in a figure eight until done!\r\n");
    HAL_Delay(1000);
    int16_t mag_bias[3] = {0, 0, 0};
    float mag_scale[3] = {0.0f, 0.0f, 0.0f};
    SS_AK8963_calibration_cycle(mpu9250, mag_bias, mag_scale);
    SS_AK8963_calibration_cycle(mpu9250, mag_bias, mag_scale);
    SS_AK8963_calibration_cycle(mpu9250, mag_bias, mag_scale);
    mpu9250->mgnt_bias_x = mag_bias[0] / 3;
    mpu9250->mgnt_bias_y = mag_bias[1] / 3;
    mpu9250->mgnt_bias_z = mag_bias[2] / 3;
    mpu9250->mgnt_scale_x = mag_scale[0] / 3.0f;
    mpu9250->mgnt_scale_y = mag_scale[1] / 3.0f;
    mpu9250->mgnt_scale_z = mag_scale[2] / 3.0f;
    SS_print("Magnetometer calibration done!\r\n");
    SS_print("The values are:\r\n");
    SS_print("%d, %d, %d, %f, %f, %f\r\n", mpu9250->mgnt_bias_x, mpu9250->mgnt_bias_y, mpu9250->mgnt_bias_z, mpu9250->mgnt_scale_x, mpu9250->mgnt_scale_y, mpu9250->mgnt_scale_z);
    return MPU_OK;
}

MPU_STATUS SS_AK8963_calibrate2(MPU9250 *mpu1, MPU9250 *mpu2) {
    vTaskDelay(1000);
    int16_t mag1_bias[3] = {0, 0, 0};
    float mag1_scale[3] = {0.0f, 0.0f, 0.0f};
    int16_t mag2_bias[3] = {0, 0, 0};
    float mag2_scale[3] = {0.0f, 0.0f, 0.0f};
    SS_AK8963_calibration_cycle2(mpu1, mpu2, mag1_bias, mag1_scale, mag2_bias, mag2_scale);
    SS_AK8963_calibration_cycle2(mpu1, mpu2, mag1_bias, mag1_scale, mag2_bias, mag2_scale);
    mpu1->mgnt_bias_x = mag1_bias[0] / 2;
    mpu1->mgnt_bias_y = mag1_bias[1] / 2;
    mpu1->mgnt_bias_z = mag1_bias[2] / 2;
    mpu1->mgnt_scale_x = mag1_scale[0] / 2.0f;
    mpu1->mgnt_scale_y = mag1_scale[1] / 2.0f;
    mpu1->mgnt_scale_z = mag1_scale[2] / 2.0f;
    mpu2->mgnt_bias_x = mag2_bias[0] / 2;
    mpu2->mgnt_bias_y = mag2_bias[1] / 2;
    mpu2->mgnt_bias_z = mag2_bias[2] / 2;
    mpu2->mgnt_scale_x = mag2_scale[0] / 2.0f;
    mpu2->mgnt_scale_y = mag2_scale[1] / 2.0f;
    mpu2->mgnt_scale_z = mag2_scale[2] / 2.0f;
    SS_led_set_meas(0, 0, 0);
    return MPU_OK;
}

MPU_STATUS SS_AK8963_set_calibration_values(MPU9250 *mpu9250, int16_t bias_x, int16_t bias_y, int16_t bias_z, float scale_x, float scale_y, float scale_z) {
    mpu9250->mgnt_bias_x = bias_x;
    mpu9250->mgnt_bias_y = bias_y;
    mpu9250->mgnt_bias_z = bias_z;
    mpu9250->mgnt_scale_x = scale_x;
    mpu9250->mgnt_scale_y = scale_y;
    mpu9250->mgnt_scale_z = scale_z;
    return MPU_OK;
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static MPU_STATUS SS_AK8963_write_register(MPU9250 *mpu9250, uint8_t address, uint8_t data) {
    MPU_STATUS result = MPU_OK;
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_ADDR, AK8963_ADDRESS);  //Set operation as write
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_REG, address);          //Choose where to write data
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_DO, data);              //Set data to write
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_CTRL, 0x81);            //Write 1 byte
    return MPU_OK;
}

static MPU_STATUS SS_AK8963_write_check_register(MPU9250 *mpu9250, uint8_t address, uint8_t data) {
    MPU_STATUS result = MPU_OK;
    result |= SS_MPU_write_check_byte(mpu9250, MPU_I2C_SLV0_ADDR, AK8963_ADDRESS);  //Set operation as write
    result |= SS_MPU_write_check_byte(mpu9250, MPU_I2C_SLV0_REG, address);          //Choose where to write data
    result |= SS_MPU_write_check_byte(mpu9250, MPU_I2C_SLV0_DO, data);              //Set data to write
    result |= SS_MPU_write_check_byte(mpu9250, MPU_I2C_SLV0_CTRL, 0x81);            //Write 1 byte
    uint8_t res;
    HAL_Delay(1);
    if((SS_AK8963_read_multiple(mpu9250, address, &res, 1) || res != data)) {  //Check if data was written successfully
        SS_print("MAGNETOMETER COMMUNICATION ERROR\r\n");
        return AK8963_ERROR;
    }
    return MPU_OK;
}

static MPU_STATUS SS_AK8963_read_multiple(MPU9250 *mpu9250, uint8_t address, uint8_t *data, uint8_t count) {
    MPU_STATUS result = MPU_OK;
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);  //Set operation as read
    HAL_Delay(1);
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_REG, address);  //Choose from where to read data
    HAL_Delay(1);
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_CTRL, 0x80 | count);  //Read data to MPU registers
    HAL_Delay(1);
    result |= SS_MPU_read_multiple(mpu9250, MPU_EXT_SENS_DATA_00, data, count);  //Read data from MPU registers
    return MPU_OK;
}

static MPU_STATUS SS_AK8963_start_reading_data(MPU9250 *mpu9250) {
    MPU_STATUS result = MPU_OK;
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_CNTL1, 0x16);  //Enter 16-bit 100Hz continuous measurement mode
    HAL_Delay(1);
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);  //Set operation as read
    HAL_Delay(1);
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_REG, AK8963_XOUT_L);  //Choose from where to read data
    HAL_Delay(1);
    result |= SS_MPU_write_byte(mpu9250, MPU_I2C_SLV0_CTRL, 0x87);  //Start reading 7 bytes at sample rate
    HAL_Delay(1);
    return MPU_OK;
}
static MPU_STATUS SS_AK8963_read_fuse_data(MPU9250 *mpu9250) {
    MPU_STATUS result = MPU_OK;
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_CNTL1, 0x1F);  //Set Fuse ROM access mode
    uint8_t adj[3];
    result |= SS_AK8963_read_multiple(mpu9250, AK8963_ASAX, adj, 3);        //Read sensitivity adjustment values
    result |= SS_AK8963_write_check_register(mpu9250, AK8963_CNTL1, 0x00);  //Set Power-down mode
    mpu9250->xASens = adj[0];
    mpu9250->yASens = adj[1];
    mpu9250->zASens = adj[2];
    return result;
}

static MPU_STATUS SS_AK8963_calibration_cycle(MPU9250 *mpu9250, int16_t *mag_bias, float *mag_scale) {
    int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_tmp[3];
    for(uint16_t i = 0; i < 1000; i++) {
        if(mpu9250->mgnt_raw_x > mag_max[0]) mag_max[0] = mpu9250->mgnt_raw_x;
        if(mpu9250->mgnt_raw_x < mag_min[0]) mag_min[0] = mpu9250->mgnt_raw_x;
        if(mpu9250->mgnt_raw_y > mag_max[1]) mag_max[1] = mpu9250->mgnt_raw_y;
        if(mpu9250->mgnt_raw_y < mag_min[1]) mag_min[1] = mpu9250->mgnt_raw_y;
        if(mpu9250->mgnt_raw_z > mag_max[2]) mag_max[2] = mpu9250->mgnt_raw_z;
        if(mpu9250->mgnt_raw_z < mag_min[2]) mag_min[2] = mpu9250->mgnt_raw_z;
        HAL_Delay(11);
    }
    // Get hard iron correction
    mag_bias[0] += (mag_max[0] + mag_min[0]) / 2;  // get average x mag bias in counts
    mag_bias[1] += (mag_max[1] + mag_min[1]) / 2;  // get average y mag bias in counts
    mag_bias[2] += (mag_max[2] + mag_min[2]) / 2;  // get average z mag bias in counts

    mag_tmp[0] = (mag_max[0] - mag_min[0]) / 2;  // get average x axis max chord length in counts
    mag_tmp[1] = (mag_max[1] - mag_min[1]) / 2;  // get average y axis max chord length in counts
    mag_tmp[2] = (mag_max[2] - mag_min[2]) / 2;  // get average z axis max chord length in counts

    float avg_rad = ((float) (mag_tmp[0] + mag_tmp[1] + mag_tmp[2])) / 3.0f;
    // Get soft iron correction estimate
    mag_scale[0] += avg_rad / ((float) ((mag_max[0] - mag_min[0]) / 2));  // get average x axis max chord length in counts
    mag_scale[1] += avg_rad / ((float) ((mag_max[1] - mag_min[1]) / 2));  // get average y axis max chord length in counts
    mag_scale[2] += avg_rad / ((float) ((mag_max[2] - mag_min[2]) / 2));  // get average z axis max chord length in counts
    return MPU_OK;
}

static MPU_STATUS SS_AK8963_calibration_cycle2(MPU9250 *mpu1, MPU9250 *mpu2, int16_t *mag1_bias, float *mag1_scale, int16_t *mag2_bias, float *mag2_scale) {
    int16_t mag1_max[3] = {-32767, -32767, -32767}, mag1_min[3] = {32767, 32767, 32767}, mag1_tmp[3];
    int16_t mag2_max[3] = {-32767, -32767, -32767}, mag2_min[3] = {32767, 32767, 32767}, mag2_tmp[3];
    for(uint16_t i = 0; i < 1000; i++) {
        /* SS_MPU_get_mgnt_data(mpu1); */
        /* SS_MPU_get_mgnt_data(mpu2); */
        SS_print("%d %d %d %d %d %d\r\n", mpu1->mgnt_raw_x, mpu1->mgnt_raw_y, mpu1->mgnt_raw_z, mpu2->mgnt_raw_x, mpu2->mgnt_raw_y, mpu2->mgnt_raw_z);
        if(mpu1->mgnt_raw_x > mag1_max[0]) mag1_max[0] = mpu1->mgnt_raw_x;
        if(mpu1->mgnt_raw_x < mag1_min[0]) mag1_min[0] = mpu1->mgnt_raw_x;
        if(mpu1->mgnt_raw_y > mag1_max[1]) mag1_max[1] = mpu1->mgnt_raw_y;
        if(mpu1->mgnt_raw_y < mag1_min[1]) mag1_min[1] = mpu1->mgnt_raw_y;
        if(mpu1->mgnt_raw_z > mag1_max[2]) mag1_max[2] = mpu1->mgnt_raw_z;
        if(mpu1->mgnt_raw_z < mag1_min[2]) mag1_min[2] = mpu1->mgnt_raw_z;
        if(mpu2->mgnt_raw_x > mag2_max[0]) mag2_max[0] = mpu2->mgnt_raw_x;
        if(mpu2->mgnt_raw_x < mag2_min[0]) mag2_min[0] = mpu2->mgnt_raw_x;
        if(mpu2->mgnt_raw_y > mag2_max[1]) mag2_max[1] = mpu2->mgnt_raw_y;
        if(mpu2->mgnt_raw_y < mag2_min[1]) mag2_min[1] = mpu2->mgnt_raw_y;
        if(mpu2->mgnt_raw_z > mag2_max[2]) mag2_max[2] = mpu2->mgnt_raw_z;
        if(mpu2->mgnt_raw_z < mag2_min[2]) mag2_min[2] = mpu2->mgnt_raw_z;
        /* if(HAL_GetTick() - counter > 100) { */
            /* SS_led_MPU_toggle(0, 60, 100); */
            /* counter = HAL_GetTick(); */
        /* } */
        vTaskDelay(11);
    }
    // Get hard iron correction
    mag1_bias[0] += (mag1_max[0] + mag1_min[0]) / 2;  // get average x mag bias in counts
    mag1_bias[1] += (mag1_max[1] + mag1_min[1]) / 2;  // get average y mag bias in counts
    mag1_bias[2] += (mag1_max[2] + mag1_min[2]) / 2;  // get average z mag bias in counts

    mag1_tmp[0] = (mag1_max[0] - mag1_min[0]) / 2;  // get average x axis max chord length in counts
    mag1_tmp[1] = (mag1_max[1] - mag1_min[1]) / 2;  // get average y axis max chord length in counts
    mag1_tmp[2] = (mag1_max[2] - mag1_min[2]) / 2;  // get average z axis max chord length in counts

    float avg_rad1 = ((float) (mag1_tmp[0] + mag1_tmp[1] + mag1_tmp[2])) / 3.0f;
    // Get soft iron correction estimate
    mag1_scale[0] += avg_rad1 / ((float) ((mag1_max[0] - mag1_min[0]) / 2));  // get average x axis max chord length in counts
    mag1_scale[1] += avg_rad1 / ((float) ((mag1_max[1] - mag1_min[1]) / 2));  // get average y axis max chord length in counts
    mag1_scale[2] += avg_rad1 / ((float) ((mag1_max[2] - mag1_min[2]) / 2));  // get average z axis max chord length in counts

    // Get hard iron correction
    mag2_bias[0] += (mag2_max[0] + mag2_min[0]) / 2;  // get average x mag bias in counts
    mag2_bias[1] += (mag2_max[1] + mag2_min[1]) / 2;  // get average y mag bias in counts
    mag2_bias[2] += (mag2_max[2] + mag2_min[2]) / 2;  // get average z mag bias in counts

    mag2_tmp[0] = (mag2_max[0] - mag2_min[0]) / 2;  // get average x axis max chord length in counts
    mag2_tmp[1] = (mag2_max[1] - mag2_min[1]) / 2;  // get average y axis max chord length in counts
    mag2_tmp[2] = (mag2_max[2] - mag2_min[2]) / 2;  // get average z axis max chord length in counts

    float avg_rad2 = ((float) (mag2_tmp[0] + mag2_tmp[1] + mag2_tmp[2])) / 3.0f;
    // Get soft iron correction estimate
    mag2_scale[0] += avg_rad2 / ((float) ((mag2_max[0] - mag2_min[0]) / 2));  // get average x axis max chord length in counts
    mag2_scale[1] += avg_rad2 / ((float) ((mag2_max[1] - mag2_min[1]) / 2));  // get average y axis max chord length in counts
    mag2_scale[2] += avg_rad1 / ((float) ((mag2_max[2] - mag2_min[2]) / 2));  // get average z axis max chord length in counts
    return MPU_OK;
}

