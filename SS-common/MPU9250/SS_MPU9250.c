/*
 * SS_MPU9250.c
 *
 *  Created on: 27.12.2017
 *      Author: Tomasz
 */
#include "SS_MPU9250.h"

#include "SS_AK8963.h"
#include "SS_misc.h"
#include "main.h"
/* TODO Remove math */
#include "SS_log.h"
#include "math.h"
#include "spi.h"

struct MPU9250 mpu1;
struct MPU9250 *mpu_pointer;
int16_t old_data[9];
uint32_t mpu_cnt, mpu_delay;
uint32_t accel_counter = 0;
uint32_t gyro_counter = 0;
uint32_t mgnt_counter = 0;

void SS_MPU_CS_ENABLE(struct MPU9250 *mpu9250) {
    HAL_GPIO_WritePin(mpu9250->CS_Port, mpu9250->CS_Pin, GPIO_PIN_RESET);
}

void SS_MPU_CS_DISABLE(struct MPU9250 *mpu9250) {
    HAL_GPIO_WritePin(mpu9250->CS_Port, mpu9250->CS_Pin, GPIO_PIN_SET);
}

enum MPU_RESULT SS_MPU_write_byte(struct MPU9250 *mpu9250, uint8_t RegAdr, uint8_t RegDat) {
    SS_MPU_CS_ENABLE(mpu9250);
    uint8_t temp = (RegAdr & 0b01111111);

    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;
    if(HAL_SPI_Transmit(mpu9250->hspi, &temp, 1, 100)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;
    if(HAL_SPI_Transmit(mpu9250->hspi, &RegDat, 1, 100)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    SS_MPU_CS_DISABLE(mpu9250);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_read_byte(struct MPU9250 *mpu9250, uint8_t RegAdr, uint8_t *RegDat) {
    SS_MPU_CS_ENABLE(mpu9250);
    uint8_t temp = 0x80 | RegAdr;

    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;

    if(HAL_SPI_Transmit(mpu9250->hspi, &temp, 1, 100)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;
    if(HAL_SPI_Receive(mpu9250->hspi, (uint8_t *) RegDat, 1, 100)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    SS_MPU_CS_DISABLE(mpu9250);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_write_check_byte(struct MPU9250 *mpu9250, uint8_t RegAdr, uint8_t RegDat) {
    if(SS_MPU_write_byte(mpu9250, RegAdr, RegDat))
        return MPU_COMM_ERROR;
    HAL_Delay(10);
    uint8_t res;
    if(SS_MPU_read_byte(mpu9250, RegAdr, &res) || res != RegDat)
        return MPU_COMM_ERROR;
    HAL_Delay(10);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_read_multiple(struct MPU9250 *mpu9250, uint8_t RegAdr, uint8_t *RegDat, uint8_t nbr) {
    SS_MPU_CS_ENABLE(mpu9250);
    uint8_t temp = 0x80 | RegAdr;

    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;

    if(HAL_SPI_Transmit(mpu9250->hspi, &temp, 1, 100)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;
    if(HAL_SPI_Receive(mpu9250->hspi, (uint8_t *) RegDat, nbr, 100)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    while(HAL_SPI_GetState(mpu9250->hspi) != HAL_SPI_STATE_READY)
        ;
    SS_MPU_CS_DISABLE(mpu9250);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_read_multiple_DMA(struct MPU9250 *mpu9250, uint8_t RegAdr, uint8_t *RegDat, uint8_t nbr, uint8_t sensor) {
    uint8_t temp = 0x80 | RegAdr;
    SS_MPU_CS_ENABLE(mpu9250);
    if(HAL_SPI_TransmitReceive_DMA(mpu9250->hspi, &temp, RegDat, 1 + nbr)) {
        SS_MPU_CS_DISABLE(mpu9250);
        return MPU_COMM_ERROR;
    }
    return MPU_OK;
}

uint8_t SS_MPU_who_am_i(struct MPU9250 *mpu9250) {
    uint8_t Iam = 1;
    SS_MPU_read_byte(mpu9250, MPU_WHO_AM_I_MPU9250, &Iam);
    HAL_Delay(1);
    return Iam;
}

enum MPU_RESULT SS_MPU_set_gyro_bandwidth(struct MPU9250 *mpu9250, uint8_t bandwidth) {
    uint8_t reg = 0x00;
    if(bandwidth <= MPU_GYRO_BAND_3600) {
        SS_MPU_read_byte(mpu9250, MPU_CONFIG, &reg);
        reg &= 0xF8;
        reg |= bandwidth;
        if(SS_MPU_write_check_byte(mpu9250, MPU_CONFIG, reg))
            return MPU_COMM_ERROR;
        if(SS_MPU_set_gyro_dlpf_bypass(mpu9250, bandwidth))
            return MPU_COMM_ERROR;
    } else if(bandwidth <= MPU_GYRO_BAND_8800_FCHOICE) {
        if(SS_MPU_set_gyro_dlpf_bypass(mpu9250, bandwidth))
            return MPU_COMM_ERROR;
    }
    mpu9250->gyro_bandwidth = bandwidth;
    HAL_Delay(1);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_set_gyro_dlpf_bypass(struct MPU9250 *mpu9250, uint8_t bandwidth) {
    uint8_t reg;
    if(SS_MPU_read_byte(mpu9250, MPU_GYRO_CONFIG, &reg))
        return MPU_COMM_ERROR;
    if(bandwidth < MPU_GYRO_BAND_3600_FCHOICE) {
        reg &= 0xFE;
    }
    if(bandwidth == MPU_GYRO_BAND_8800_FCHOICE) {
        reg |= 0x01;
    } else if(bandwidth == MPU_GYRO_BAND_3600_FCHOICE) {
        reg &= 0xFE;
        reg |= 0x02;
    }
    if(SS_MPU_write_check_byte(mpu9250, MPU_GYRO_CONFIG, reg))
        return MPU_COMM_ERROR;
    HAL_Delay(1);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_set_accel_bandwidth(struct MPU9250 *mpu9250, uint8_t bandwidth) {
    uint8_t reg = 0x00;
    SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG2, &reg);
    reg &= 0xF0;
    if(bandwidth == MPU_ACCEL_BAND_1130) {
        reg |= (0x01 << 3);
    } else {
        reg |= bandwidth;
        reg &= 0b11110111;
    }
    if(SS_MPU_write_check_byte(mpu9250, MPU_ACCEL_CONFIG2, reg))
        return MPU_COMM_ERROR;
    mpu9250->accel_bandwidth = bandwidth;
    HAL_Delay(1);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_set_gyro_scale(struct MPU9250 *mpu9250, uint8_t scale) {
    uint8_t reg;
    if(SS_MPU_read_byte(mpu9250, MPU_GYRO_CONFIG, &reg))
        return MPU_COMM_ERROR;
    reg &= 0b00000111;
    reg |= (scale << 3);
    if(SS_MPU_write_check_byte(mpu9250, MPU_GYRO_CONFIG, reg))
        return MPU_COMM_ERROR;

    mpu9250->gyro_scale = scale;
    switch(scale) {
        case MPU_GYRO_SCALE_250:
            mpu9250->gyro_resolution = 1.0 / 131.068;
            break;
        case MPU_GYRO_SCALE_500:
            mpu9250->gyro_resolution = 1.0 / 65.534;
            break;
        case MPU_GYRO_SCALE_1000:
            mpu9250->gyro_resolution = 1.0 / 32.767;
            break;
        case MPU_GYRO_SCALE_2000:
            mpu9250->gyro_resolution = 1.0 / 16.3835;
            break;
    }
    HAL_Delay(1);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_set_accel_scale(struct MPU9250 *mpu9250, uint8_t scale) {
    uint8_t reg;
    if(SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, &reg))
        return MPU_COMM_ERROR;
    reg &= 0b00000111;
    reg |= (scale << 3);
    if(SS_MPU_write_check_byte(mpu9250, MPU_ACCEL_CONFIG, reg))
        return MPU_COMM_ERROR;
    mpu9250->accel_scale = scale;
    switch(scale) {
        case MPU_ACCEL_SCALE_2:
            mpu9250->accel_resolution = 1.0 / 16383.5;
            break;
        case MPU_ACCEL_SCALE_4:
            mpu9250->accel_resolution = 1.0 / 8191.75;
            break;
        case MPU_ACCEL_SCALE_8:
            mpu9250->accel_resolution = 1.0 / 4095.875;
            break;
        case MPU_ACCEL_SCALE_16:
            mpu9250->accel_resolution = 1.0 / 2047.9375;
            break;
    }
    HAL_Delay(1);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_get_accel_data(struct MPU9250 *mpu9250) {
    uint8_t rcv[6];
    if(SS_MPU_read_multiple(mpu9250, MPU_ACCEL_XOUT_H, rcv, 6))
        return MPU_COMM_ERROR;
    mpu9250->accel_raw_x = (int16_t)((int16_t) rcv[0] << 8) | rcv[1];
    mpu9250->accel_raw_y = (int16_t)((int16_t) rcv[2] << 8) | rcv[3];
    mpu9250->accel_raw_z = (int16_t)((int16_t) rcv[4] << 8) | rcv[5];
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_get_gyro_data(struct MPU9250 *mpu9250) {
    uint8_t rcv[6];
    if(SS_MPU_read_multiple(mpu9250, MPU_GYRO_XOUT_H, rcv, 6))
        return MPU_COMM_ERROR;
    mpu9250->gyro_raw_x = (int16_t)((int16_t) rcv[0] << 8) | rcv[1];
    mpu9250->gyro_raw_y = (int16_t)((int16_t) rcv[2] << 8) | rcv[3];
    mpu9250->gyro_raw_z = (int16_t)((int16_t) rcv[4] << 8) | rcv[5];
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_get_data_DMA(struct MPU9250 *mpu9250) {
    //    if(HAL_SPI_GetState(mpu9250->hspi) == HAL_SPI_STATE_READY &&
    //       HAL_DMA_GetState(&MPU_HDMA_SPI_TX) == HAL_DMA_STATE_READY &&
    //       HAL_DMA_GetState(&MPU_HDMA_SPI_RX) == HAL_DMA_STATE_READY) {
    mpu_pointer = mpu9250;
    return SS_MPU_read_multiple_DMA(mpu9250, MPU_ACCEL_XOUT_H, mpu9250->rcv, 22, ACCELEROMETER);
//    }
#ifdef MULTIPLE_MPU
//    if(mpu9250 == &mpu1)
//        osSemaphoreRelease(mpu1_semaphore);
//    else
//        osSemaphoreRelease(mpu2_semaphore);
#endif
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_SPI_TxRxCpltCallback(struct MPU9250 *mpu9250) {
    SS_MPU_CS_DISABLE(mpu9250);
    mpu9250->accel_raw_x = (int16_t)((int16_t) mpu9250->rcv[1] << 8) | mpu9250->rcv[2];
    mpu9250->accel_raw_y = (int16_t)((int16_t) mpu9250->rcv[3] << 8) | mpu9250->rcv[4];
    mpu9250->accel_raw_z = (int16_t)((int16_t) mpu9250->rcv[5] << 8) | mpu9250->rcv[6];

    mpu9250->gyro_raw_x = (int16_t)((int16_t) mpu9250->rcv[9] << 8) | mpu9250->rcv[10];
    mpu9250->gyro_raw_y = (int16_t)((int16_t) mpu9250->rcv[11] << 8) | mpu9250->rcv[12];
    mpu9250->gyro_raw_z = (int16_t)((int16_t) mpu9250->rcv[13] << 8) | mpu9250->rcv[14];

    if((mpu9250->rcv[21] & 0x10)) {  //Check if the data is overflown
        mpu9250->mgnt_raw_x = (int16_t)((int16_t) mpu9250->rcv[16] << 8) | mpu9250->rcv[15];
        mpu9250->mgnt_raw_y = (int16_t)((int16_t) mpu9250->rcv[18] << 8) | mpu9250->rcv[17];
        mpu9250->mgnt_raw_z = (int16_t)((int16_t) mpu9250->rcv[20] << 8) | mpu9250->rcv[19];
    }
    if(mpu9250->old_data[0] != mpu9250->accel_raw_x || mpu9250->old_data[1] != mpu9250->accel_raw_y || mpu9250->old_data[2] != mpu9250->accel_raw_z) {
        mpu9250->old_data[0] = mpu9250->accel_raw_x;
        mpu9250->old_data[1] = mpu9250->accel_raw_y;
        mpu9250->old_data[2] = mpu9250->accel_raw_z;
        //        SS_S25FL_save_3x_int16_t(mpu9250->accel_id, mpu9250->accel_raw_x, mpu9250->accel_raw_y, mpu9250->accel_raw_z);
    }
    if(mpu9250->old_data[3] != mpu9250->gyro_raw_x || mpu9250->old_data[4] != mpu9250->gyro_raw_y || mpu9250->old_data[5] != mpu9250->gyro_raw_z) {
        mpu9250->old_data[3] = mpu9250->gyro_raw_x;
        mpu9250->old_data[4] = mpu9250->gyro_raw_y;
        mpu9250->old_data[5] = mpu9250->gyro_raw_z;
        //        SS_S25FL_save_3x_int16_t(mpu9250->gyro_id, mpu9250->gyro_raw_x, mpu9250->gyro_raw_y, mpu9250->gyro_raw_z);
    }
    if(mpu9250->old_data[6] != mpu9250->mgnt_raw_x || mpu9250->old_data[7] != mpu9250->mgnt_raw_y || mpu9250->old_data[8] != mpu9250->mgnt_raw_z) {
        mpu9250->old_data[6] = mpu9250->mgnt_raw_x;
        mpu9250->old_data[7] = mpu9250->mgnt_raw_y;
        mpu9250->old_data[8] = mpu9250->mgnt_raw_z;
        //        SS_S25FL_save_3x_int16_t(mpu9250->mgnt_id, mpu9250->mgnt_raw_x, mpu9250->mgnt_raw_y, mpu9250->mgnt_raw_z);
    }
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_set_clk(struct MPU9250 *mpu9250, uint8_t clksel) {
    uint8_t reg;
    if(SS_MPU_read_byte(mpu9250, MPU_PWR_MGMT_1, &reg))
        return MPU_COMM_ERROR;
    reg &= 0xf8;
    reg |= clksel;
    if(SS_MPU_write_check_byte(mpu9250, MPU_PWR_MGMT_1, reg))
        return MPU_COMM_ERROR;
    HAL_Delay(1);
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_self_test(struct MPU9250 *mpu9250) {  //Not tested
    HAL_Delay(100);
    enum MPU_RESULT result = MPU_OK;
    uint8_t gyro_config, accel_config;
    float accel_resolution = mpu9250->accel_resolution;
    float gyro_resolution = mpu9250->gyro_resolution;
    uint8_t config;
    result |= SS_MPU_read_byte(mpu9250, MPU_CONFIG, &config);
    result |= SS_MPU_read_byte(mpu9250, MPU_GYRO_CONFIG, &gyro_config);
    result |= SS_MPU_read_byte(mpu9250, MPU_ACCEL_CONFIG, &accel_config);
    result |= SS_MPU_set_gyro_bandwidth(mpu9250, 2);
    result |= SS_MPU_set_accel_bandwidth(mpu9250, 2);
    result |= SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_250);
    result |= SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_2);
    HAL_Delay(10);
    uint8_t ST_code[6];
    int32_t OS[6] = {0}, ST_OS[6] = {0}, ST[6] = {0};
    float ST_OTP[6];
    HAL_Delay(40);
    for(int16_t i = 0; i < 200; i++) {
        result |= SS_MPU_get_gyro_data(mpu9250);
        result |= SS_MPU_get_accel_data(mpu9250);
        OS[0] += mpu9250->gyro_raw_x;
        OS[1] += mpu9250->gyro_raw_y;
        OS[2] += mpu9250->gyro_raw_z;
        OS[3] += mpu9250->accel_raw_x;
        OS[4] += mpu9250->accel_raw_y;
        OS[5] += mpu9250->accel_raw_z;
        HAL_Delay(1);
    }
    result |= SS_MPU_write_check_byte(mpu9250, MPU_GYRO_CONFIG, (0xE0 | (MPU_GYRO_SCALE_250 << 3)));
    result |= SS_MPU_write_check_byte(mpu9250, MPU_ACCEL_CONFIG, (0xE0 | (MPU_ACCEL_SCALE_2 << 3)));

    HAL_Delay(20);
    for(int16_t i = 0; i < 200; i++) {
        result |= SS_MPU_get_gyro_data(mpu9250);
        result |= SS_MPU_get_accel_data(mpu9250);
        ST_OS[0] += mpu9250->gyro_raw_x;
        ST_OS[1] += mpu9250->gyro_raw_y;
        ST_OS[2] += mpu9250->gyro_raw_z;
        ST_OS[3] += mpu9250->accel_raw_x;
        ST_OS[4] += mpu9250->accel_raw_y;
        ST_OS[5] += mpu9250->accel_raw_z;
        HAL_Delay(1);
    }
    HAL_Delay(40);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_GYRO_CONFIG, gyro_config);
    HAL_Delay(1);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_ACCEL_CONFIG, accel_config);
    HAL_Delay(1);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_CONFIG, config);
    HAL_Delay(1);
    result |= SS_MPU_read_multiple(mpu9250, 0x00, ST_code, 3);
    HAL_Delay(1);
    result |= SS_MPU_read_multiple(mpu9250, 0x0D, ST_code + 3, 3);
    mpu9250->accel_resolution = accel_resolution;
    mpu9250->gyro_resolution = gyro_resolution;
    for(uint8_t i = 0; i < 6; i++) {
        ST[i] = (ST_OS[i] - OS[i]) / 200;
        ST_OTP[i] = (float) (2620.0 * pow(1.01, (float) ST_code[i] - 1.0));
#ifdef MPU_DEBUG
        SS_print("i: %d, ST: %d, ST_OTP: %f, ST_OS: %d, OS: %d\r\n", i, ST[i], ST_OTP[i], ST_OS[i], OS[i]);
#endif
    }
#ifdef MPU_DEBUG
    SS_print("--------------------------------\r\n");
    SS_print("MPU9250 SELF TEST\r\n");
    SS_print("Gyroscope self test values / factory values:\r\nx: %f, y: %f z: %f\r\n",
             (float) ST[0] / ST_OTP[0], (float) ST[1] / ST_OTP[1], (float) ST[2] / ST_OTP[2]);
    SS_print("Accelerometer self test values / factory values:\r\nx: %f, y: %f z: %f\r\n",
             (float) ST[3] / ST_OTP[3], (float) ST[4] / ST_OTP[4], (float) ST[5] / ST_OTP[5]);
    SS_print("--------------------------------\r\n");
#endif
    for(uint8_t i = 0; i < 6; i++) {
        if(ST_OTP[i] != 0) {
            if(i < 3 && ((float) ST[i] / ST_OTP[i]) <= 0.5) {
                SS_print("GYROSCOPE SELF TEST ERROR\r\n");
                result |= MPU_SELF_TEST_ERROR;
                i = 2;
            }
            if(i >= 3 && ((((float) ST[i] / ST_OTP[i]) <= 0.5) || (((float) ST[i] / ST_OTP[i]) >= 1.5))) {
                SS_print("ACCELEROMETER SELF TEST ERROR\r\n");
                result |= MPU_SELF_TEST_ERROR;
                i = 5;
            }
        } else {
            if(i < 3 && ((float) ST[i] * mpu9250->gyro_resolution < 60.0)) {
                SS_print("GYROSCOPE SELF TEST ERROR\r\n");
                result |= MPU_SELF_TEST_ERROR;
                i = 2;
            }
            if(i >= 3 && (((float) ST[i] * mpu9250->accel_resolution < 225.0) || ((float) ST[i] * mpu9250->accel_resolution > 675.0))) {
                SS_print("ACCELEROMETER SELF TEST ERROR\r\n");
                result |= MPU_SELF_TEST_ERROR;
                i = 5;
            }
        }
    }
    HAL_Delay(1);
    return result;
}

union iu {
    int16_t i;
    uint16_t u;
};

void int16_t_split(int16_t val, uint8_t *buff) {
    union iu iu = {.i = val};
    buff[0] = iu.u >> 8;
    buff[1] = iu.u;
}

union fu {
    float f;
    uint32_t u;
};

void float_split(float val, uint8_t *buff) {
    union fu fu = {.f = val};
    buff[0] = fu.u >> 24;
    buff[1] = fu.u >> 16;
    buff[2] = fu.u >> 8;
    buff[3] = fu.u;
}

void SS_MPU_send_calibration_values(struct MPU9250 *mpu9250) {
    /* uint8_t buff[4]; */
    /* HAL_UART_Transmit(&huart1, &(mpu9250->gyro_id), 1, 1000); */
    /* HAL_UART_Transmit(&huart1, &(mpu9250->accel_id), 1, 1000); */
    /* HAL_UART_Transmit(&huart1, &(mpu9250->mgnt_id), 1, 1000); */
    /* float_split(mpu9250->accel_resolution, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 4, 1000); */
    /* float_split(mpu9250->gyro_resolution, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 4, 1000); */
    /* float_split(mpu9250->mgnt_scale_x, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 4, 1000); */
    /* float_split(mpu9250->mgnt_scale_y, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 4, 1000); */
    /* float_split(mpu9250->mgnt_scale_z, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 4, 1000); */
    /* int16_t_split(mpu9250->mgnt_bias_x, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 2, 1000); */
    /* int16_t_split(mpu9250->mgnt_bias_y, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 2, 1000); */
    /* int16_t_split(mpu9250->mgnt_bias_z, buff); */
    /* HAL_UART_Transmit(&huart1, buff, 2, 1000); */
    /* HAL_UART_Transmit(&huart1, &(mpu9250->xASens), 1, 1000); */
    /* HAL_UART_Transmit(&huart1, &(mpu9250->yASens), 1, 1000); */
    /* HAL_UART_Transmit(&huart1, &(mpu9250->zASens), 1, 1000); */
}

enum MPU_RESULT SS_MPU_init_all() {
    enum MPU_RESULT result = MPU_OK;
    HAL_NVIC_DisableIRQ(MPU_INT_EXTI_IRQn);
    /* HAL_NVIC_DisableIRQ(MPU2_INT_EXTI_IRQn); */
    result |= SS_AK8963_set_calibration_values(&mpu1, 38, 217, 92, 1.040606, 1.018278, 0.946424);
    /* result |= SS_AK8963_set_calibration_values(&mpu2, 178, 94, -107, 1.042196, 1.027092, 0.952840); */
    mpu1.gyro_id = 10;
    mpu1.accel_id = 11;
    mpu1.mgnt_id = 12;
    mpu1.CS_Port = MPU_CS_GPIO_Port;
    mpu1.CS_Pin = MPU_CS_Pin;
    mpu1.INT_Pin = MPU_INT_Pin;
    mpu1.hspi = &hspi4;
    mpu1.accel_scale = MPU_ACCEL_SCALE_2;
    mpu1.gyro_scale = MPU_GYRO_SCALE_250;
    HAL_Delay(50);
    result |= SS_MPU_init(&mpu1);
    /* mpu2.gyro_id = 20; */
    /* mpu2.accel_id = 21; */
    /* mpu2.mgnt_id = 22; */
    /* mpu2.CS_Port = MPU2_CS_GPIO_Port; */
    /* mpu2.CS_Pin = MPU2_CS_Pin; */
    /* mpu2.accel_scale = MPU_ACCEL_SCALE_16; */
    /* mpu2.gyro_scale = MPU_GYRO_SCALE_2000; */
    /* result |= SS_MPU_init(&mpu2); */
    /* int32_t bias1[] = {-15, -11, 72, 230, 300, 537}; */
    /* result |= SS_MPU_set_calibration(&mpu1, bias1); */
    /* int32_t bias2[] = {-359, 53, -91, -100, 350, 1100}; */
    /* result |= SS_MPU_set_calibration(&mpu2, bias2); */
    /* HAL_Delay(50); */
    HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQn);
    /* HAL_NVIC_EnableIRQ(MPU2_INT_EXTI_IRQn); */
    return result;
}

enum MPU_RESULT SS_MPU_init(struct MPU9250 *mpu9250) {
    enum MPU_RESULT result = MPU_OK;
    result |= SS_AK8963_reset(mpu9250);
    HAL_Delay(50);
    result |= SS_MPU_reset(mpu9250);
    HAL_Delay(50);
    result |= SS_MPU_sleep(mpu9250, DISABLE);
    result |= SS_MPU_set_clk(mpu9250, 0x01);
    result |= SS_MPU_set_smplrt(mpu9250, 0);
    result |= SS_AK8963_init(mpu9250);
    result |= SS_MPU_INT_enable(mpu9250);
    HAL_Delay(100);
    result |= SS_MPU_set_accel_bandwidth(mpu9250, MPU_ACCEL_BAND_184);
    result |= SS_MPU_set_gyro_bandwidth(mpu9250, MPU_GYRO_BAND_184);
    result |= SS_MPU_set_accel_scale(mpu9250, mpu9250->accel_scale);
    result |= SS_MPU_set_gyro_scale(mpu9250, mpu9250->gyro_scale);
    result |= SS_MPU_self_test(mpu9250);
    //	result |= SS_MPU_calibrate(mpu9250);
    mpu9250->result = result;
#ifdef MPU_RGB_LED
    switch(result) {
        case MPU_OK:
            SS_led_set_meas(0, 50, 0);  //Green
            break;
        default:
            SS_led_set_meas(50, 0, 0);  //Red
            break;
    }
#endif
    return result;
}

enum MPU_RESULT SS_MPU_math_scaled_gyro(struct MPU9250 *mpu9250) {
    mpu9250->gyro_scaled_x = (float) mpu9250->gyro_raw_x * mpu9250->gyro_resolution * 3.14 / 180;
    mpu9250->gyro_scaled_y = (float) mpu9250->gyro_raw_y * mpu9250->gyro_resolution * 3.14 / 180;
    mpu9250->gyro_scaled_z = (float) mpu9250->gyro_raw_z * mpu9250->gyro_resolution * 3.14 / 180;
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_math_scaled_accel(struct MPU9250 *mpu9250) {
    mpu9250->accel_scaled_x = (float) mpu9250->accel_raw_x * mpu9250->accel_resolution;
    mpu9250->accel_scaled_y = (float) mpu9250->accel_raw_y * mpu9250->accel_resolution;
    mpu9250->accel_scaled_z = (float) mpu9250->accel_raw_z * mpu9250->accel_resolution;
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_get_fifo_counter(struct MPU9250 *mpu9250) {
    uint8_t reg[2];
    if(SS_MPU_read_multiple(mpu9250, MPU_FIFO_COUNTH, reg, 2))
        return MPU_COMM_ERROR;
    reg[0] &= 0x1F;
    mpu9250->fifo_counter = (uint16_t)((uint16_t) reg[0] << 8) | reg[1];
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_get_fifo_data(struct MPU9250 *mpu9250) {
    if(SS_MPU_get_fifo_counter(mpu9250))
        return MPU_COMM_ERROR;
    if(mpu9250->fifo_counter > 0) {
        if(mpu9250->fifo_counter > 255) {
            if(SS_MPU_read_multiple(mpu9250, MPU_FIFO_R_W, mpu9250->fifo_data, 255))
                return MPU_COMM_ERROR;
            if(SS_MPU_read_multiple(mpu9250, MPU_FIFO_R_W, &mpu9250->fifo_data[254], mpu9250->fifo_counter - 255))
                return MPU_COMM_ERROR;
        } else if(SS_MPU_read_multiple(mpu9250, MPU_FIFO_R_W, mpu9250->fifo_data, mpu9250->fifo_counter))
            return MPU_COMM_ERROR;
        mpu9250->FIFO_HANDLED_FLAG = 0;
    }
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_set_fifo_data(struct MPU9250 *mpu9250) {
    if(SS_MPU_write_check_byte(mpu9250, MPU_FIFO_EN, 0x78))
        return MPU_COMM_ERROR;
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_INT_enable(struct MPU9250 *mpu9250) {
    if(SS_MPU_write_check_byte(mpu9250, MPU_INT_PIN_CFG, 0x10))
        return MPU_COMM_ERROR;
    if(SS_MPU_write_check_byte(mpu9250, MPU_INT_ENABLE, 0x01))
        return MPU_COMM_ERROR;
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_reset(struct MPU9250 *mpu9250) {
    if(SS_MPU_write_byte(mpu9250, MPU_PWR_MGMT_1, 0x80))
        return MPU_COMM_ERROR;
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_sleep(struct MPU9250 *mpu9250, uint8_t state) {
    uint8_t reg = 0;
    if(SS_MPU_read_byte(mpu9250, MPU_PWR_MGMT_1, &reg))
        return MPU_COMM_ERROR;
    if(state == DISABLE)
        reg &= 0b10111111;
    else if(state == ENABLE)
        reg |= 0b01000000;
    if(SS_MPU_write_check_byte(mpu9250, MPU_PWR_MGMT_1, reg))
        return MPU_COMM_ERROR;
    return MPU_OK;
}

enum MPU_RESULT SS_MPU_calibrate(struct MPU9250 *mpu9250) {  //Device needs to be flat during calibration!!!
    int16_t calibration_time = 2000;
    uint16_t max_gyro_deviation = 15;
    uint16_t max_accel_deviation = 200;
    HAL_Delay(100);
    enum MPU_RESULT result = MPU_OK;
    uint8_t gyro_scale = mpu9250->gyro_scale;
    uint8_t accel_scale = mpu9250->accel_scale;
    uint8_t bandwidth = mpu9250->gyro_bandwidth;
    int32_t sum[] = {0, 0, 0, 0, 0, 0};  //gx, gy, gz, ax, ay, az
    int32_t bias[6];
    result |= SS_MPU_set_gyro_scale(mpu9250, MPU_GYRO_SCALE_250);
    result |= SS_MPU_set_accel_scale(mpu9250, MPU_ACCEL_SCALE_2);
    result |= SS_MPU_set_gyro_bandwidth(mpu9250, MPU_GYRO_BAND_5);
    HAL_Delay(50);

    int16_t data[6][calibration_time];
    for(int16_t i = 0; i < calibration_time; i++) {
        result |= SS_MPU_get_gyro_data(mpu9250);
        result |= SS_MPU_get_accel_data(mpu9250);
        data[0][i] = mpu9250->gyro_raw_x;
        data[1][i] = mpu9250->gyro_raw_y;
        data[2][i] = mpu9250->gyro_raw_z;
        data[3][i] = mpu9250->accel_raw_x;
        data[4][i] = mpu9250->accel_raw_y;
        data[5][i] = mpu9250->accel_raw_z;
        HAL_Delay(1);
    }
    for(uint16_t i = 0; i < calibration_time; i++) {
        for(uint8_t j = 0; j < 6; j++) {
            sum[j] += data[j][i];
        }
    }
    for(uint8_t j = 0; j < 6; j++) {
        bias[j] = ((int32_t) sum[j]) / ((int32_t) calibration_time);
    }
    double deviation[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    for(int16_t i = 0; i < calibration_time; i++) {
        for(uint8_t j = 0; j < 6; j++) {
            deviation[j] += pow(data[j][i] - bias[j], 2);
        }
    }
    for(uint8_t j = 0; j < 6; j++) {
        deviation[j] = pow((double) deviation[j] / (double) calibration_time, 0.5);
    }
    uint16_t accelsensitivity = 16384;  // = 16384 LSB/g
    if(bias[5] > 0L) {
        bias[5] -= (int32_t) accelsensitivity;
    }  // Remove gravity from the z-axis accelerometer bias calculation
    else {
        bias[5] += (int32_t) accelsensitivity;
    }
#ifdef MPU_DEBUG
    SS_print("--------------------------------\r\n");
    SS_print("MPU CALIBRATION\r\n");
    SS_print("Gyroscope bias x: %ld, y: %ld, z: %ld\r\n", bias[0], bias[1], bias[2]);
    SS_print("Accelerometer bias x: %ld, y: %ld, z: %ld\r\n", bias[3], bias[4], bias[5]);
    SS_print("Gyroscope deviation (should not exceed %d) x: %f, y: %f, z: %f\r\n", max_gyro_deviation, deviation[0], deviation[1], deviation[2]);
    SS_print("Accelerometer deviation (should not exceed %d) x: %f, y: %f, z: %f\r\n", max_accel_deviation, deviation[3], deviation[4], deviation[5]);
    SS_print("--------------------------------\r\n");
#endif
    result |= SS_MPU_set_gyro_scale(mpu9250, gyro_scale);
    result |= SS_MPU_set_accel_scale(mpu9250, accel_scale);
    result |= SS_MPU_set_gyro_bandwidth(mpu9250, bandwidth);
    if(deviation[0] > (double) max_gyro_deviation || deviation[1] > (double) max_gyro_deviation || deviation[2] > (double) max_gyro_deviation || deviation[3] > (double) max_accel_deviation || deviation[4] > (double) max_accel_deviation || deviation[5] > (double) max_accel_deviation) {
        SS_print("MPU CALIBRATION ERROR\r\n");
        return MPU_CALIBRATION_ERROR;
    }
#ifdef PRINT_CALIBRATION
    SS_print("--------------------------------\r\n");
    SS_print("Calibration values:\r\n{ ");
    for(uint8_t i = 0; i < 6; i++) {
        if(i < 5)
            SS_print("%d, ", bias[i]);
        else
            SS_print("%d }\r\n", bias[i]);
    }
    SS_print("--------------------------------\r\n");
#endif
    result |= SS_MPU_gyro_write_calibration(mpu9250, bias);
    result |= SS_MPU_accel_write_calibration(mpu9250, bias);
    return result;
}

enum MPU_RESULT SS_MPU_set_calibration(struct MPU9250 *mpu9250, int32_t bias[6]) {
    enum MPU_RESULT result = MPU_OK;
    result |= SS_MPU_gyro_write_calibration(mpu9250, bias);
    result |= SS_MPU_accel_write_calibration(mpu9250, bias);
    return result;
}

enum MPU_RESULT SS_MPU_accel_write_calibration(struct MPU9250 *mpu9250, int32_t *bias) {
    enum MPU_RESULT result = MPU_OK;
    uint8_t data[6];
    int32_t accel_bias_reg[3] = {0, 0, 0};                        // A place to hold the factory accelerometer trim biases
    SS_MPU_read_multiple(mpu9250, MPU_XA_OFFSET_H, &data[0], 2);  // Read factory accelerometer trim values
    accel_bias_reg[0] = (int32_t)(((int16_t) data[0] << 8) | data[1]);
    SS_MPU_read_multiple(mpu9250, MPU_YA_OFFSET_H, &data[0], 2);
    accel_bias_reg[1] = (int32_t)(((int16_t) data[0] << 8) | data[1]);
    SS_MPU_read_multiple(mpu9250, MPU_ZA_OFFSET_H, &data[0], 2);
    accel_bias_reg[2] = (int32_t)(((int16_t) data[0] << 8) | data[1]);

    accel_bias_reg[0] -= ((bias[3] / 8) & 0xFFFE);
    accel_bias_reg[1] -= ((bias[4] / 8) & 0xFFFE);
    accel_bias_reg[2] -= ((bias[5] / 8) & 0xFFFE);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0]) & 0xFF;
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1]) & 0xFF;
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2]) & 0xFF;

    result |= SS_MPU_write_check_byte(mpu9250, MPU_XA_OFFSET_H, data[0]);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_XA_OFFSET_L, data[1]);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_YA_OFFSET_H, data[2]);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_YA_OFFSET_L, data[3]);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_ZA_OFFSET_H, data[4]);
    result |= SS_MPU_write_check_byte(mpu9250, MPU_ZA_OFFSET_L, data[5]);
    return result;
}

enum MPU_RESULT SS_MPU_gyro_write_calibration(struct MPU9250 *mpu9250, int32_t *gyro_bias) {
    uint8_t data[6];
    data[0] = (-gyro_bias[0] / 4 >> 8) & 0xFF;  // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0] / 4) & 0xFF;       // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1] / 4 >> 8) & 0xFF;
    data[3] = (-gyro_bias[1] / 4) & 0xFF;
    data[4] = (-gyro_bias[2] / 4 >> 8) & 0xFF;
    data[5] = (-gyro_bias[2] / 4) & 0xFF;

    if(SS_MPU_write_check_byte(mpu9250, MPU_XG_OFFSET_H, data[0]))
        return MPU_COMM_ERROR;
    if(SS_MPU_write_check_byte(mpu9250, MPU_XG_OFFSET_L, data[1]))
        return MPU_COMM_ERROR;
    if(SS_MPU_write_check_byte(mpu9250, MPU_YG_OFFSET_H, data[2]))
        return MPU_COMM_ERROR;
    if(SS_MPU_write_check_byte(mpu9250, MPU_YG_OFFSET_L, data[3]))
        return MPU_COMM_ERROR;
    if(SS_MPU_write_check_byte(mpu9250, MPU_ZG_OFFSET_H, data[4]))
        return MPU_COMM_ERROR;
    if(SS_MPU_write_check_byte(mpu9250, MPU_ZG_OFFSET_L, data[5]))
        return MPU_COMM_ERROR;
    return MPU_OK;
}
enum MPU_RESULT SS_MPU_set_smplrt(struct MPU9250 *mpu9250, uint8_t smplrt) {
    if(SS_MPU_write_check_byte(mpu9250, MPU_SMPLRT_DIV, smplrt))
        return MPU_COMM_ERROR;
    return MPU_OK;
}

/* TODO Handle multiple mpu-s */
void SS_MPU_exti_isr_single(struct MPU9250 *mpu9250, uint16_t GPIO_Pin) {
    if(GPIO_Pin == mpu9250->INT_Pin) {
        SS_MPU_get_data_DMA(mpu9250);
    }
}

void SS_MPU_exti_isr(uint16_t GPIO_Pin) {
    SS_MPU_exti_isr_single(&mpu1, GPIO_Pin);
}

//enum MPU_RESULT SS_MPU_fifo_enable(struct MPU9250 *mpu9250) {
//	mpu9250->fifo_data = calloc(512, sizeof(uint8_t));
//	uint8_t reg;
//	if (SS_MPU_read_byte(mpu9250,  MPU_USER_CTRL, &reg))
//		return MPU_COMM_ERROR;
//	reg |= 0x40;
//	if (SS_MPU_write_check_byte(mpu9250, MPU_USER_CTRL, reg))
//		return MPU_COMM_ERROR;
//	return MPU_OK;
//}
//enum MPU_RESULT SS_MPU_fifo_disable(struct MPU9250 *mpu9250) {
//	uint8_t reg;
//	if (SS_MPU_read_byte(mpu9250,  MPU_USER_CTRL, &reg))
//		return MPU_COMM_ERROR;
//	reg &= 0xBF;
//	if (SS_MPU_write_check_byte(mpu9250, MPU_USER_CTRL, reg))
//		return MPU_COMM_ERROR;
//	free(mpu9250->fifo_data);
//	return MPU_OK;
//}
//enum MPU_RESULT SS_MPU_fifo_reset(struct MPU9250 *mpu9250) {
//	uint8_t reg;
//	if (SS_MPU_read_byte(mpu9250,  MPU_USER_CTRL, &reg))
//		return MPU_COMM_ERROR;
//	reg |= 0x04;
//	if (SS_MPU_write_byte(mpu9250,  MPU_USER_CTRL, reg))
//		return MPU_COMM_ERROR;
//	return MPU_OK;
//}
//uint16_t SS_MPU_fifo_get_raw_gyro(struct MPU9250* mpu9250, uint16_t inc) {
//	mpu9250->gyro_raw_x = (int16_t) ((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++];
//	mpu9250->gyro_raw_y = (int16_t) ((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++];
//	mpu9250->gyro_raw_z = (int16_t) ((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++];
//	return inc;
//}
//uint16_t SS_MPU_fifo_get_raw_accel(struct MPU9250* mpu9250, uint16_t inc) {
//	mpu9250->accel_raw_x = (int16_t) ((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++];
//	mpu9250->accel_raw_y = (int16_t) ((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++];
//	mpu9250->accel_raw_z = (int16_t) ((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++];
//	return inc;
//}
//void SS_MPU_fifo_handle(struct MPU9250* mpu9250, uint8_t sensor) {
//	static uint16_t nbr = 0;
//	static uint16_t inc = 0;
//	if (inc == 0 && !mpu9250->FIFO_HANDLED_FLAG)
//		nbr = mpu9250->fifo_counter;
//	switch (sensor) {
//		case (GYROSCOPE):
//			mpu9250->gyro_scaled_x = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->gyro_resolution;
//			mpu9250->gyro_scaled_y = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->gyro_resolution;
//			mpu9250->gyro_scaled_z = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->gyro_resolution;
//			break;
//		case (ACCELEROMETER):
//
//			break;
//		case (GYROSCOPE | ACCELEROMETER):
//			mpu9250->accel_scaled_x = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->accel_resolution;
//			mpu9250->accel_scaled_y = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->accel_resolution;
//			mpu9250->accel_scaled_z = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->accel_resolution;
//
//			mpu9250->gyro_scaled_x = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->gyro_resolution;
//			mpu9250->gyro_scaled_y = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->gyro_resolution;
//			mpu9250->gyro_scaled_z = (float) (((int16_t) mpu9250->fifo_data[inc++] << 8) | mpu9250->fifo_data[inc++]) * mpu9250->gyro_resolution;
//
//			break;
//		case (GYROSCOPE | MAGNETOMETER):
//
//			break;
//		case (ACCELEROMETER | MAGNETOMETER):
//
//			break;
//		case (ACCELEROMETER | MAGNETOMETER | GYROSCOPE):
//
//			break;
//		case (MAGNETOMETER):
//
//			break;
//	}
//	if (inc >= nbr) {
//		inc = 0;
//		mpu9250->FIFO_HANDLED_FLAG = 1;
//	}
//}
