/*
 * SS_AK8963.h
 *
 *  Created on: Apr 5, 2019
 *      Author: Maciek
 */

#ifndef SS_AK8963_H_
#define SS_AK8963_H_

#include "SS_MPU9250.h"
#include "stdint.h"

struct MPU9250;
uint8_t SS_AK8963_who_am_i(struct MPU9250 *mpu9250);
uint8_t SS_AK8963_check_data_ready(struct MPU9250 *mpu9250);
enum MPU_RESULT SS_AK8963_init(struct MPU9250* mpu9250);
enum MPU_RESULT SS_MPU_math_scaled_mgnt(struct MPU9250* mpu9250);
enum MPU_RESULT SS_MPU_get_mgnt_data(struct MPU9250* mpu9250);
enum MPU_RESULT SS_AK8963_write_register(struct MPU9250 *mpu9250, uint8_t address, uint8_t data);
enum MPU_RESULT SS_AK8963_write_check_register(struct MPU9250 *mpu9250, uint8_t address, uint8_t data);
enum MPU_RESULT SS_AK8963_read_multiple(struct MPU9250 *mpu9250, uint8_t address, uint8_t *data, uint8_t count);
enum MPU_RESULT SS_AK8963_start_reading_data(struct MPU9250 *mpu9250);
enum MPU_RESULT SS_AK8963_self_test(struct MPU9250 *mpu9250);
enum MPU_RESULT SS_AK8963_reset(struct MPU9250 *mpu9250);
enum MPU_RESULT SS_AK8963_read_fuse_data(struct MPU9250 *mpu9250);
enum MPU_RESULT SS_AK8963_calibrate(struct MPU9250 *mpu9250);
enum MPU_RESULT SS_AK8963_calibration_cycle(struct MPU9250 *mpu9250, int16_t *mag_bias, float *mag_scale);
enum MPU_RESULT SS_AK8963_set_calibration_values(struct MPU9250* mpu9250, int16_t bias_x, int16_t bias_y, int16_t bias_z,float scale_x, float scale_y, float scale_z);
enum MPU_RESULT SS_AK8963_calibrate2(struct MPU9250 *mpu1, struct MPU9250 *mpu2);
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
#endif /* SS_AK8963_H_ */
