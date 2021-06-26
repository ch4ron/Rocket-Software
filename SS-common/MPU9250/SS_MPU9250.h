/*
 * SS_MPU9250.h
 *
 *  Created on: 27.12.2017
 *      Author: Tomasz
 */

#ifndef SS_MPU9250_H
#define SS_MPU9250_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "printf.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

//#define MPU_DEBUG

#define PRINT_CALIBRATION

#define MAGNETOMETER 4
#define GYROSCOPE 1
#define ACCELEROMETER 2

#define MAX_MPU_COUNT 2

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */
void print_data(int32_t data1,int32_t data2);

typedef enum {
    MPU_OK = 0,
    MPU_CALIBRATION_ERROR = 1,
    MPU_SELF_TEST_ERROR = 3,
    AK8963_ERROR = 7,
    MPU_COMM_ERROR = 15,
    MPU_ERR = 31,
} MPU_STATUS;

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *CS_Port;
    uint16_t CS_Pin;
    uint16_t INT_Pin;
    uint8_t id;
    uint8_t gyro_id;
    uint8_t accel_id;
    uint8_t mgnt_id;
    float gyro_resolution;
    float accel_resolution;
    uint8_t gyro_bandwidth;
    uint8_t accel_bandwidth;
    uint8_t gyro_scale;
    uint8_t accel_scale;
    uint8_t magnet_res;
    int16_t accel_raw_x;
    int16_t accel_raw_y;
    int16_t accel_raw_z;
    int16_t gyro_raw_x;
    int16_t gyro_raw_y;
    int16_t gyro_raw_z;
    int16_t mgnt_raw_x;
    int16_t mgnt_raw_y;
    int16_t mgnt_raw_z;
    float gyro_scaled_x;
    float gyro_scaled_y;
    float gyro_scaled_z;
    float accel_scaled_x;
    float accel_scaled_y;
    float accel_scaled_z;
    float mgnt_scaled_x;
    float mgnt_scaled_y;
    float mgnt_scaled_z;
    uint8_t xASens;
    uint8_t yASens;
    uint8_t zASens;
    float mgnt_scale_x;
    float mgnt_scale_y;
    float mgnt_scale_z;
    int16_t mgnt_bias_x;
    int16_t mgnt_bias_y;
    int16_t mgnt_bias_z;
    MPU_STATUS result;
    uint8_t rcv[23];
    int16_t old_data[9];
    int32_t bias[6];
} MPU9250;

extern MPU9250 *mpu_pointer;

MPU_STATUS SS_MPU_init(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_reset(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_get_accel_data(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_get_gyro_data(MPU9250 *mpu9250);
uint8_t SS_MPU_who_am_i(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_math_scaled_gyro(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_math_scaled_accel(MPU9250 *mpu9250);
MPU_STATUS SS_MPU_calibrate(MPU9250 *mpu9250);  //Device needs to be flat during calibration!!!

MPU_STATUS SS_MPU_write_byte(MPU9250 *mpu9250, uint8_t RegAdr, uint8_t RegDat);
MPU_STATUS SS_MPU_read_byte(MPU9250 *mpu9250, uint8_t RegAdr, uint8_t *RegDat);
MPU_STATUS SS_MPU_write_check_byte(MPU9250 *mpu9250, uint8_t RegAdr, uint8_t RegDat);
MPU_STATUS SS_MPU_read_multiple(MPU9250 *mpu9250, uint8_t RegAdr, uint8_t *RegDat, uint8_t nbr);

MPU_STATUS SS_MPU_set_is_logging(bool is_logging_);
bool SS_MPU_get_is_logging(void);

void SS_MPU_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void SS_MPU_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/* ==================================================================== */
/* ============================ Registers ============================= */
/* ==================================================================== */

#define MPU_ACCEL_FIFO_ON 0x01
#define MPU_GYRO_FIFO_ON 0x02
#define MPU_MAGNETO_FIFO_ON 0x04

#define MPU_ACCEL_SCALE_2 0b00
#define MPU_ACCEL_SCALE_4 0b01
#define MPU_ACCEL_SCALE_8 0b10
#define MPU_ACCEL_SCALE_16 0b11

#define MPU_GYRO_SCALE_250 0b00
#define MPU_GYRO_SCALE_500 0b01
#define MPU_GYRO_SCALE_1000 0b10
#define MPU_GYRO_SCALE_2000 0b11

#define MPU_GYRO_BAND_8800_FCHOICE 9  //32kHz
#define MPU_GYRO_BAND_3600_FCHOICE 8  //32kHz
#define MPU_GYRO_BAND_250 0           //8kHz
#define MPU_GYRO_BAND_184 1           //1Khz
#define MPU_GYRO_BAND_92 2            //1Khz
#define MPU_GYRO_BAND_41 3            //1Khz
#define MPU_GYRO_BAND_20 4            //1Khz
#define MPU_GYRO_BAND_10 5            //1Khz
#define MPU_GYRO_BAND_5 6             //1Khz
#define MPU_GYRO_BAND_3600 7          //8Khz

#define MPU_ACCEL_BAND_1130 8  //4Khz
#define MPU_ACCEL_BAND_184 1   //1Khz
#define MPU_ACCEL_BAND_92 2    //1Khz
#define MPU_ACCEL_BAND_41 3    //1Khz
#define MPU_ACCEL_BAND_20 4    //1Khz
#define MPU_ACCEL_BAND_10 5    //1Khz
#define MPU_ACCEL_BAND_5 6     //1Khz
#define MPU_ACCEL_BAND_460 7   //1Khz

#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

/*#define X_FINE_GAIN      0x03 // [7:0] fine gain
 #define Y_FINE_GAIN      0x04
 #define Z_FINE_GAIN      0x05
 #define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
 #define XA_OFFSET_L_TC   0x07
 #define YA_OFFSET_H      0x08
 #define YA_OFFSET_L_TC   0x09
 #define ZA_OFFSET_H      0x0A
 #define ZA_OFFSET_L_TC   0x0B */

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define SELF_TEST_A 0x10

#define MPU_XG_OFFSET_H 0x13  // User-defined trim values for gyroscope
#define MPU_XG_OFFSET_L 0x14
#define MPU_YG_OFFSET_H 0x15
#define MPU_YG_OFFSET_L 0x16
#define MPU_ZG_OFFSET_H 0x17
#define MPU_ZG_OFFSET_L 0x18
#define MPU_SMPLRT_DIV 0x19
#define MPU_CONFIG 0x1A
#define MPU_GYRO_CONFIG 0x1B
#define MPU_ACCEL_CONFIG 0x1C
#define MPU_ACCEL_CONFIG2 0x1D
#define MPU_LP_ACCEL_ODR 0x1E
#define MPU_WOM_THR 0x1F

#define MPU_MOT_DUR 0x20    // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MPU_ZMOT_THR 0x21   // Zero-motion detection threshold bits [7:0]
#define MPU_ZRMOT_DUR 0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define MPU_FIFO_EN 0x23

#define MPU_I2C_MST_CTRL 0x24
#define MPU_I2C_SLV0_ADDR 0x25

#define MPU_I2C_SLV0_REG 0x26
#define MPU_I2C_SLV0_CTRL 0x27
#define MPU_I2C_SLV1_ADDR 0x28
#define MPU_I2C_SLV1_REG 0x29
#define MPU_I2C_SLV1_CTRL 0x2A
#define MPU_I2C_SLV2_ADDR 0x2B
#define MPU_I2C_SLV2_REG 0x2C
#define MPU_I2C_SLV2_CTRL 0x2D
#define MPU_I2C_SLV3_ADDR 0x2E
#define MPU_I2C_SLV3_REG 0x2F
#define MPU_I2C_SLV3_CTRL 0x30
#define MPU_I2C_SLV4_ADDR 0x31
#define MPU_I2C_SLV4_REG 0x32
#define MPU_I2C_SLV4_DO 0x33
#define MPU_I2C_SLV4_CTRL 0x34
#define MPU_I2C_SLV4_DI 0x35
#define MPU_I2C_MST_STATUS 0x36
#define MPU_INT_PIN_CFG 0x37
#define MPU_INT_ENABLE 0x38
#define MPU_DMP_INT_STATUS 0x39  // Check DMP interrupt
#define MPU_INT_STATUS 0x3A
#define MPU_ACCEL_XOUT_H 0x3B
#define MPU_ACCEL_XOUT_L 0x3C
#define MPU_ACCEL_YOUT_H 0x3D
#define MPU_ACCEL_YOUT_L 0x3E
#define MPU_ACCEL_ZOUT_H 0x3F
#define MPU_ACCEL_ZOUT_L 0x40
#define MPU_TEMP_OUT_H 0x41
#define MPU_TEMP_OUT_L 0x42
#define MPU_GYRO_XOUT_H 0x43
#define MPU_GYRO_XOUT_L 0x44
#define MPU_GYRO_YOUT_H 0x45
#define MPU_GYRO_YOUT_L 0x46
#define MPU_GYRO_ZOUT_H 0x47
#define MPU_GYRO_ZOUT_L 0x48

#define MPU_EXT_SENS_DATA_00 0x49
#define MPU_I2C_SLV0_DO 0x63
#define MPU_I2C_MSTR_DELAY_CTRL 0x67
#define MPU_SIGNAL_PATH_RESET 0x68
#define MPU_MOT_DETECT_CTRL 0x69
#define MPU_USER_CTRL 0x6A   // Bit 7 enable DMP, bit 3 reset DMP
#define MPU_PWR_MGMT_1 0x6B  // Device defaults to the SLEEP mode
#define MPU_PWR_MGMT_2 0x6C
#define MPU_DMP_BANK 0x6D    // Activates a specific bank in the DMP
#define MPU_DMP_RW_PNT 0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define MPU_DMP_REG 0x6F     // Register in DMP from which to read or to which to write
#define MPU_DMP_REG_1 0x70
#define MPU_DMP_REG_2 0x71
#define MPU_FIFO_COUNTH 0x72
#define MPU_FIFO_COUNTL 0x73
#define MPU_FIFO_R_W 0x74
#define MPU_WHO_AM_I_MPU9250 0x75  // Should return 0x71
#define MPU_XA_OFFSET_H 0x77
#define MPU_XA_OFFSET_L 0x78
#define MPU_YA_OFFSET_H 0x7A
#define MPU_YA_OFFSET_L 0x7B
#define MPU_ZA_OFFSET_H 0x7D
#define MPU_ZA_OFFSET_L 0x7E

#endif /* SS_MPU9250_H */
