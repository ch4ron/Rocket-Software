/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stm32f4xx_hal.h>
#include "FreeRTOS.h"
#include "task.h"
#include "i2c.h"

#include "SS_SCD30.h"

#ifdef SCD30_ADDRESS
static const uint8_t SCD30_I2C_ADDRESS = SCD30_ADDRESS;
#else
static const uint8_t SCD30_I2C_ADDRESS = 0x61;
#endif

#define SCD30_CMD_START_PERIODIC_MEASUREMENT 0x0010
#define SCD30_CMD_STOP_PERIODIC_MEASUREMENT 0x0104
#define SCD30_CMD_READ_MEASUREMENT 0x0300
#define SCD30_CMD_SET_MEASUREMENT_INTERVAL 0x4600
#define SCD30_CMD_GET_DATA_READY 0x0202
#define SCD30_CMD_SET_TEMPERATURE_OFFSET 0x5403
#define SCD30_CMD_SET_ALTITUDE 0x5102
#define SCD30_CMD_SET_FORCED_RECALIBRATION 0x5204
#define SCD30_CMD_AUTO_SELF_CALIBRATION 0x5306
#define SCD30_CMD_READ_SERIAL 0xD033
#define SCD30_SERIAL_NUM_WORDS 16
#define SCD30_WRITE_DELAY_US 20000

#define SCD30_MAX_BUFFER_WORDS 24
#define SCD30_CMD_SINGLE_WORD_BUF_LEN                                          \
    (SENSIRION_COMMAND_SIZE + SENSIRION_WORD_SIZE + CRC8_LEN)


float32_t co2_ppm, temperature, relative_humidity; //zmienne do przechowywania danych
int16_t err;
uint16_t interval_in_seconds = 2;


void SS_scd30_task(void *pvParameters){
    scd30_start_periodic_measurement(0); //rozpoczęcie pomiarów
    while(1){
        //scd30_start_periodic_measurement(0); //rozpoczęcie pomiarów
        // sensirion_sleep_usec(interval_in_seconds * 1000000u); wyłącza nam czujnik na jakiś czas
        err = scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity); //sczytanie wartosci pomiarow
        if (err != STATUS_OK) {
            SS_print("error reading measurement\r\n");

        } else {
            SS_print("%0.2f %0.2f %0.2f\r\n", co2_ppm, temperature, relative_humidity);
        }
        sensirion_sleep_usec(interval_in_seconds * 1000000u);
        //scd30_stop_periodic_measurement();  //zakonczenie wykonywania pomiarow
        vTaskDelay( 300 / portTICK_RATE_MS );
    }
}


int16_t scd30_start_periodic_measurement(uint16_t ambient_pressure_mbar) {
    if (ambient_pressure_mbar &&
        (ambient_pressure_mbar < 700 || ambient_pressure_mbar > 1400)) {
        /* out of allowable range */
        return STATUS_FAIL;
    }

    return sensirion_i2c_write_cmd_with_args(
        SCD30_I2C_ADDRESS, SCD30_CMD_START_PERIODIC_MEASUREMENT,
        &ambient_pressure_mbar, SENSIRION_NUM_WORDS(ambient_pressure_mbar));
}

int16_t scd30_stop_periodic_measurement() {
    return sensirion_i2c_write_cmd(SCD30_I2C_ADDRESS,
                                   SCD30_CMD_STOP_PERIODIC_MEASUREMENT);
}

int16_t scd30_read_measurement(float32_t *co2_ppm, float32_t *temperature,
                               float32_t *humidity) {
    int16_t ret;
    union {
        uint32_t u32_value;
        float32_t float32;
        uint8_t bytes[4];
    } tmp, data[3];

    ret =
        sensirion_i2c_write_cmd(SCD30_I2C_ADDRESS, SCD30_CMD_READ_MEASUREMENT);
    if (ret != STATUS_OK)
        return ret;

    ret = sensirion_i2c_read_words_as_bytes(SCD30_I2C_ADDRESS, data->bytes,
                                            SENSIRION_NUM_WORDS(data));
    if (ret != STATUS_OK)
        return ret;

    tmp.u32_value = be32_to_cpu(data[0].u32_value);
    *co2_ppm = tmp.float32;

    tmp.u32_value = be32_to_cpu(data[1].u32_value);
    *temperature = tmp.float32;

    tmp.u32_value = be32_to_cpu(data[2].u32_value);
    *humidity = tmp.float32;

    return STATUS_OK;
}

int16_t scd30_set_measurement_interval(uint16_t interval_sec) {
    int16_t ret;

    if (interval_sec < 2 || interval_sec > 1800) {
        /* out of allowable range */
        return STATUS_FAIL;
    }

    ret = sensirion_i2c_write_cmd_with_args(
        SCD30_I2C_ADDRESS, SCD30_CMD_SET_MEASUREMENT_INTERVAL, &interval_sec,
        SENSIRION_NUM_WORDS(interval_sec));
    sensirion_sleep_usec(SCD30_WRITE_DELAY_US);

    return ret;
}

int16_t scd30_get_data_ready(uint16_t *data_ready) {
    return sensirion_i2c_read_cmd(SCD30_I2C_ADDRESS, SCD30_CMD_GET_DATA_READY,
                                  data_ready, SENSIRION_NUM_WORDS(*data_ready));
}

int16_t scd30_set_temperature_offset(uint16_t temperature_offset) {
    int16_t ret;

    ret = sensirion_i2c_write_cmd_with_args(
        SCD30_I2C_ADDRESS, SCD30_CMD_SET_TEMPERATURE_OFFSET,
        &temperature_offset, SENSIRION_NUM_WORDS(temperature_offset));
    sensirion_sleep_usec(SCD30_WRITE_DELAY_US);

    return ret;
}

int16_t scd30_set_altitude(uint16_t altitude) {
    int16_t ret;

    ret = sensirion_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS,
                                            SCD30_CMD_SET_ALTITUDE, &altitude,
                                            SENSIRION_NUM_WORDS(altitude));
    sensirion_sleep_usec(SCD30_WRITE_DELAY_US);

    return ret;
}

int16_t scd30_get_automatic_self_calibration(uint8_t *asc_enabled) {
    uint16_t word;
    int16_t ret;

    ret = sensirion_i2c_read_cmd(SCD30_I2C_ADDRESS,
                                 SCD30_CMD_AUTO_SELF_CALIBRATION, &word,
                                 SENSIRION_NUM_WORDS(word));
    if (ret != STATUS_OK)
        return ret;

    *asc_enabled = (uint8_t)word;

    return STATUS_OK;
}

int16_t scd30_enable_automatic_self_calibration(uint8_t enable_asc) {
    int16_t ret;
    uint16_t asc = !!enable_asc;

    ret = sensirion_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS,
                                            SCD30_CMD_AUTO_SELF_CALIBRATION,
                                            &asc, SENSIRION_NUM_WORDS(asc));
    sensirion_sleep_usec(SCD30_WRITE_DELAY_US);

    return ret;
}

int16_t scd30_set_forced_recalibration(uint16_t co2_ppm) {
    int16_t ret;

    ret = sensirion_i2c_write_cmd_with_args(
        SCD30_I2C_ADDRESS, SCD30_CMD_SET_FORCED_RECALIBRATION, &co2_ppm,
        SENSIRION_NUM_WORDS(co2_ppm));
    sensirion_sleep_usec(SCD30_WRITE_DELAY_US);

    return ret;
}

int16_t scd30_read_serial(char *serial) {
    int16_t ret;

    ret = sensirion_i2c_write_cmd(SCD30_I2C_ADDRESS, SCD30_CMD_READ_SERIAL);
    if (ret)
        return ret;

    sensirion_sleep_usec(SCD30_WRITE_DELAY_US);
    ret = sensirion_i2c_read_words_as_bytes(
        SCD30_I2C_ADDRESS, (uint8_t *)serial, SCD30_SERIAL_NUM_WORDS);
    serial[2 * SCD30_SERIAL_NUM_WORDS] = '\0';
    return ret;
}

const char *scd30_get_driver_version() {
    return "awesome version";
}

uint8_t scd30_get_configured_address() {
    return SCD30_I2C_ADDRESS;
}

int16_t scd30_probe() {
    uint16_t data_ready;

    /* try to read data-ready state */
    return scd30_get_data_ready(&data_ready);
}

/**
 * Release all resources initialized by sensirion_i2c_init().
 */
void sensirion_i2c_release(void) {
}

/**
 * Execute one read transaction on the I2C bus, reading a given number of bytes.
 * If the device does not acknowledge the read command, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to read from
 * @param data    pointer to the buffer where the data is to be stored
 * @param count   number of bytes to read from I2C and store in the buffer
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_read(uint8_t address, uint8_t *data, uint16_t count) {
    return (int8_t)HAL_I2C_Master_Receive(&hi2c3, (uint16_t)(address << 1),
                                          data, count, 100);
}

/**
 * Execute one write transaction on the I2C bus, sending a given number of
 * bytes. The bytes in the supplied buffer must be sent to the given address. If
 * the slave device does not acknowledge any of the bytes, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to write to
 * @param data    pointer to the buffer containing the data to write
 * @param count   number of bytes to read from the buffer and send over I2C
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_write(uint8_t address, const uint8_t *data,
                           uint16_t count) {
    return (int8_t)HAL_I2C_Master_Transmit(&hi2c3, (uint16_t)(address << 1),
                                           (uint8_t *)data, count, 100);
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(uint32_t useconds) {
    uint32_t msec = useconds / 1000;
    if (useconds % 1000 > 0) {
        msec++;
    }

    /*
     * Increment by 1 if STM32F1 driver version less than 1.1.1
     * Old firmwares of STM32F1 sleep 1ms shorter than specified in HAL_Delay.
     * This was fixed with firmware 1.6 (driver version 1.1.1), so we have to
     * fix it ourselves for older firmwares
     */
    if (HAL_GetHalVersion() < 0x01010100) {
        msec++;
    }

    HAL_Delay(msec);
}

//sensirio_common.c
/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * This module provides functionality that is common to all Sensirion drivers
 */

uint8_t sensirion_common_generate_crc(uint8_t *data, uint16_t count) {
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;

    /* calculates 8-Bit checksum with given polynomial */
    for (current_byte = 0; current_byte < count; ++current_byte) {
        crc ^= (data[current_byte]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

int8_t sensirion_common_check_crc(uint8_t *data, uint16_t count,
                                  uint8_t checksum) {
    if (sensirion_common_generate_crc(data, count) != checksum)
        return STATUS_FAIL;
    return STATUS_OK;
}

uint16_t sensirion_fill_cmd_send_buf(uint8_t *buf, uint16_t cmd,
                                     const uint16_t *args, uint8_t num_args) {
    uint8_t crc;
    uint8_t i;
    uint16_t idx = 0;

    buf[idx++] = (uint8_t)((cmd & 0xFF00) >> 8);
    buf[idx++] = (uint8_t)((cmd & 0x00FF) >> 0);

    for (i = 0; i < num_args; ++i) {
        buf[idx++] = (uint8_t)((args[i] & 0xFF00) >> 8);
        buf[idx++] = (uint8_t)((args[i] & 0x00FF) >> 0);

        crc = sensirion_common_generate_crc((uint8_t *)&buf[idx - 2],
                                            SENSIRION_WORD_SIZE);
        buf[idx++] = crc;
    }
    return idx;
}

int16_t sensirion_i2c_read_words_as_bytes(uint8_t address, uint8_t *data,
                                          uint16_t num_words) {
    int16_t ret;
    uint16_t i, j;
    uint16_t size = num_words * (SENSIRION_WORD_SIZE + CRC8_LEN);
    uint16_t word_buf[SENSIRION_MAX_BUFFER_WORDS];
    uint8_t *const buf8 = (uint8_t *)word_buf;

    ret = sensirion_i2c_read(address, buf8, size);
    if (ret != STATUS_OK)
        return ret;

    /* check the CRC for each word */
    for (i = 0, j = 0; i < size; i += SENSIRION_WORD_SIZE + CRC8_LEN) {

        ret = sensirion_common_check_crc(&buf8[i], SENSIRION_WORD_SIZE,
                                         buf8[i + SENSIRION_WORD_SIZE]);
        if (ret != STATUS_OK)
            return ret;

        data[j++] = buf8[i];
        data[j++] = buf8[i + 1];
    }

    return STATUS_OK;
}

int16_t sensirion_i2c_read_words(uint8_t address, uint16_t *data_words,
                                 uint16_t num_words) {
    int16_t ret;
    uint8_t i;

    ret = sensirion_i2c_read_words_as_bytes(address, (uint8_t *)data_words,
                                            num_words);
    if (ret != STATUS_OK)
        return ret;

    for (i = 0; i < num_words; ++i)
        data_words[i] = be16_to_cpu(data_words[i]);

    return STATUS_OK;
}

int16_t sensirion_i2c_write_cmd(uint8_t address, uint16_t command) {
    uint8_t buf[SENSIRION_COMMAND_SIZE];

    sensirion_fill_cmd_send_buf(buf, command, NULL, 0);
    return sensirion_i2c_write(address, buf, SENSIRION_COMMAND_SIZE);
}

int16_t sensirion_i2c_write_cmd_with_args(uint8_t address, uint16_t command,
                                          const uint16_t *data_words,
                                          uint16_t num_words) {
    uint8_t buf[SENSIRION_MAX_BUFFER_WORDS];
    uint16_t buf_size;

    buf_size = sensirion_fill_cmd_send_buf(buf, command, data_words, num_words);
    return sensirion_i2c_write(address, buf, buf_size);
}

int16_t sensirion_i2c_delayed_read_cmd(uint8_t address, uint16_t cmd,
                                       uint32_t delay_us, uint16_t *data_words,
                                       uint16_t num_words) {
    int16_t ret;
    uint8_t buf[SENSIRION_COMMAND_SIZE];
    sensirion_fill_cmd_send_buf(buf, cmd, NULL, 0);
    ret = sensirion_i2c_write(address, buf, SENSIRION_COMMAND_SIZE);
    if (ret != STATUS_OK)
        return ret;

    if (delay_us)
        sensirion_sleep_usec(delay_us);

    return sensirion_i2c_read_words(address, data_words, num_words);
}

int16_t sensirion_i2c_read_cmd(uint8_t address, uint16_t cmd,
                               uint16_t *data_words, uint16_t num_words) {
    return sensirion_i2c_delayed_read_cmd(address, cmd, 0, data_words,
                                          num_words);
}


