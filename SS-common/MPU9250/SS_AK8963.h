/*
 * SS_AK8963.h
 *
 *  Created on: Apr 5, 2019
 *      Author: Maciek
 */

#ifndef SS_AK8963_H
#define SS_AK8963_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_MPU9250.h"
#include "stdint.h"

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

MPU_STATUS SS_AK8963_init(MPU9250 *mpu9250);
MPU_STATUS SS_AK8963_reset(MPU9250 *mpu9250);
uint8_t SS_AK8963_who_am_i(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_get_mgnt_data(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_math_scaled_mgnt(MPU9250 *mpu9250);
uint8_t SS_AK8963_check_data_ready(MPU9250 *mpu9250);
MPU_STATUS SS_AK8963_self_test(MPU9250 *mpu9250);
MPU_STATUS SS_AK8963_calibrate(MPU9250 *mpu9250);  //Reads data using interrupts, enable them first
MPU_STATUS SS_AK8963_calibrate2(MPU9250 *mpu1, MPU9250 *mpu2);
MPU_STATUS SS_AK8963_set_calibration_values(MPU9250 *mpu9250, int16_t bias_x, int16_t bias_y, int16_t bias_z, float scale_x, float scale_y, float scale_z);

/* ==================================================================== */
/* ============================ Registers ============================= */
/* ==================================================================== */

#define AK8963_ADDRESS   0x0C
#define AK8963_WHO_AM_I  0x00 // should return 0x48
#define AK8963_INFO      0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL1     0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_CNTL2     0x0B
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#endif /* SS_AK8963_H */
