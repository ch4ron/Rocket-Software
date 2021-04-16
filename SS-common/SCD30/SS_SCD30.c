/*
* SS_SCD30.c
    *
    *  Created on: ?
    *  Author: ?
    *
    *  Modified on: 16.04.2021
    *  Author: IAmNotAProgramer
*/

/*
* ==================================================================== *
* ============================ HOW TO USE ============================ *
* ==================================================================== *
*
*  1. Configure I2C Connection in MxCube
*  2. Enter 'SCD_SDA''SCD_SCL' as user label for CPU's pin connected
*     with SCD30's chip select pin (in CubeMX).
*  3. #include "SS_SCD30.h" in SS_FreeRtos.c (main.c) file, and add SS_SCD30.c and SS_SCD30.h to your project.
*     create task SS_SCD_task(void *pvParameters)
*/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */


#include <stm32f4xx_hal.h>
#include <stdint.h>
#include "i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "SS_SCD30.h"

#ifdef SS_USE_FLASH
#include "SS_flash.h"
#endif

static int8_t  SS_SCD_i2c_read(uint8_t address, uint8_t *data, uint16_t count);
static int8_t  SS_SCD_i2c_write(uint8_t address, const uint8_t *data,uint16_t count);
static uint8_t SS_SCD_common_generate_crc(uint8_t *data, uint16_t count);
static int8_t  SS_SCD_common_check_crc(uint8_t *data, uint16_t count,uint8_t checksum);

static int16_t  SS_SCD_start_periodic_measurement(uint16_t ambient_pressure_mbar);
static int16_t  SS_SCD_read_measurement(float32_t *co2_ppm, float32_t *temperature,float32_t *humidity);
static int16_t  SS_SCD_stop_periodic_measurement();
static int16_t  SS_SCD_i2c_read_words_as_bytes(uint8_t address, uint8_t *data,uint16_t num_words);
static int16_t  SS_SCD_i2c_write_cmd(uint8_t address, uint16_t command);
static int16_t  SS_SCD_i2c_write_cmd_with_args(uint8_t address, uint16_t command,const uint16_t *data_words,uint16_t num_words);
static uint16_t SS_SCD_fill_cmd_send_buf(uint8_t *buf, uint16_t cmd,const uint16_t *args, uint8_t num_args);

static void SS_SCD_sleep_usec(uint32_t useconds);

/**
*static uint8_t SS_SCD_get_configured_address();
*static int16_t SS_SCD_i2c_read_words(uint8_t address, uint16_t *data_words,uint16_t num_words);
*static int16_t SS_SCD_i2c_delayed_read_cmd(uint8_t address, uint16_t cmd,uint32_t delay_us, uint16_t *data_words,uint16_t num_words);
*static int16_t SS_SCD_i2c_read_cmd(uint8_t address, uint16_t cmd,uint16_t *data_words, uint16_t num_words);
*static int16_t SS_SCD_get_data_ready(uint16_t *data_ready);
*static int16_t SS_SCD_set_temperature_offset(uint16_t temperature_offset);
*static int16_t SS_SCD_set_altitude(uint16_t altitude);
*static int16_t SS_SCD_get_automatic_self_calibration(uint8_t *asc_enabled);
*static int16_t SS_SCD_enable_automatic_self_calibration(uint8_t enable_asc);
*static int16_t SS_SCD_set_forced_recalibration(uint16_t co2_ppm);
*static int16_t SS_SCD_read_serial(char *serial);
*static int16_t SS_SCD_i2c_select_bus(uint8_t bus_idx);
*static int16_t SS_SCD_probe()
*static void SS_SCD_i2c_init(void);
*static void SS_SCD_i2c_release(void);
*/

SCD30 scd30;

void SS_SCD_task(void *pvParameters)
{
    SS_SCD_start_periodic_measurement(0); //rozpoczęcie pomiarów
    while(1){
        scd30.err = SS_SCD_read_measurement(&scd30.co2_ppm, &scd30.temperature, &scd30.relative_humidity); //sczytanie wartosci pomiarow
        if (scd30.err != STATUS_OK) {
            SS_print("error reading measurement\r\n");

        } else {
            SS_print("%0.2f %0.2f %0.2f\r\n", scd30.co2_ppm, scd30.temperature, scd30.relative_humidity);
        }
        SS_SCD_sleep_usec(scd30.interval_in_seconds * 1000000u);
        vTaskDelay( 3000 / portTICK_RATE_MS );
    }
    SS_SCD_stop_periodic_measurement();
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
static int8_t SS_SCD_i2c_read(uint8_t address, uint8_t *data, uint16_t count)
{
    return (int8_t)HAL_I2C_Master_Receive(&hi2c3, (uint16_t)(address << 1),data, count, 100);
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
static int8_t SS_SCD_i2c_write(uint8_t address, const uint8_t *data,uint16_t count)
{
    return (int8_t)HAL_I2C_Master_Transmit(&hi2c3, (uint16_t)(address << 1),(uint8_t *)data, count, 100);
}

/**
 * calculates 8-Bit checksum with given polynomial
 *
 * @param data    pointer to the buffer containing data that checksum will be generated
 * @param count   number of bytes to calculate CRC
 * @returns 8-Bit checksum
 */

static uint8_t SS_SCD_common_generate_crc(uint8_t *data, uint16_t count)
{
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;

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

/**
 * checks 8-Bit checksum with given polynomial
 *
 * @param data    pointer to the buffer containing data that checksum will be generated
 * @param count   number of bytes to calculate CRC
 * @param checksum contain CRC data to compare
 * @returns 8-Bit checksum
 */

static int8_t SS_SCD_common_check_crc(uint8_t *data, uint16_t count,uint8_t checksum)
{
    if (SS_SCD_common_generate_crc(data, count) != checksum)
        return STATUS_FAIL;
    return STATUS_OK;
}
/**
 *  launch periodic data measure
 *
 * @param ambient_pressure_mbar
 * @return
 */
static int16_t SS_SCD_start_periodic_measurement(uint16_t ambient_pressure_mbar)
{
    if (ambient_pressure_mbar &&
        (ambient_pressure_mbar < 700 || ambient_pressure_mbar > 1400)) {
        /* out of allowable range */
        return STATUS_FAIL;
    }

    return SS_SCD_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS, SCD30_CMD_START_PERIODIC_MEASUREMENT,&ambient_pressure_mbar,
        SENSIRION_NUM_WORDS(ambient_pressure_mbar));
}

/**
 *
 * read data from sensor
 *
 * @param co2_ppm
 * @param temperature
 * @param humidity
 * @return
 */

static int16_t SS_SCD_read_measurement(float32_t *co2_ppm, float32_t *temperature,float32_t *humidity)
{
    int16_t ret;
    union {
        uint32_t u32_value;
        float32_t float32;
        uint8_t bytes[4];
    } tmp, data[3];

    ret =
        SS_SCD_i2c_write_cmd(SCD30_I2C_ADDRESS, SCD30_CMD_READ_MEASUREMENT);
    if (ret != STATUS_OK)
        return ret;

    ret = SS_SCD_i2c_read_words_as_bytes(SCD30_I2C_ADDRESS, data->bytes,SENSIRION_NUM_WORDS(data));
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

/**
 *  stops periodic measurement
 *
 * @return
 */

static int16_t SS_SCD_stop_periodic_measurement()
{
    return SS_SCD_i2c_write_cmd(SCD30_I2C_ADDRESS,SCD30_CMD_STOP_PERIODIC_MEASUREMENT);
}

/**
 *
 * @param address
 * @param data
 * @param num_words
 * @return
 */
static int16_t SS_SCD_i2c_read_words_as_bytes(uint8_t address, uint8_t *data,uint16_t num_words)
{
    int16_t ret;
    uint16_t i, j;
    uint16_t size = num_words * (SENSIRION_WORD_SIZE + CRC8_LEN);
    uint16_t word_buf[SENSIRION_MAX_BUFFER_WORDS];
    uint8_t *const buf8 = (uint8_t *)word_buf;

    ret = SS_SCD_i2c_read(address, buf8, size);
    if (ret != STATUS_OK)
        return ret;

    /* check the CRC for each word */
    for (i = 0, j = 0; i < size; i += SENSIRION_WORD_SIZE + CRC8_LEN) {

        ret = SS_SCD_common_check_crc(&buf8[i], SENSIRION_WORD_SIZE,
                                         buf8[i + SENSIRION_WORD_SIZE]);
        if (ret != STATUS_OK)
            return ret;

        data[j++] = buf8[i];
        data[j++] = buf8[i + 1];
    }

    return STATUS_OK;
}

/**
 *
 * @param address
 * @param command
 * @return
 */
static int16_t SS_SCD_i2c_write_cmd(uint8_t address, uint16_t command)
{
    uint8_t buf[SENSIRION_COMMAND_SIZE];

    SS_SCD_fill_cmd_send_buf(buf, command, NULL, 0);
    return SS_SCD_i2c_write(address, buf, SENSIRION_COMMAND_SIZE);
}

/**
 *
 * @param address
 * @param command
 * @param data_words
 * @param num_words
 * @return
 */
static int16_t SS_SCD_i2c_write_cmd_with_args(uint8_t address, uint16_t command,const uint16_t *data_words,uint16_t num_words)
{
    uint8_t buf[SENSIRION_MAX_BUFFER_WORDS];
    uint16_t buf_size;

    buf_size = SS_SCD_fill_cmd_send_buf(buf, command, data_words, num_words);
    return SS_SCD_i2c_write(address, buf, buf_size);
}

/**
 *
 * @param buf
 * @param cmd
 * @param args
 * @param num_args
 * @return
 */
static uint16_t SS_SCD_fill_cmd_send_buf(uint8_t *buf, uint16_t cmd,const uint16_t *args, uint8_t num_args)
{
    uint8_t crc;
    uint8_t i;
    uint16_t idx = 0;

    buf[idx++] = (uint8_t)((cmd & 0xFF00) >> 8);
    buf[idx++] = (uint8_t)((cmd & 0x00FF) >> 0);

    for (i = 0; i < num_args; ++i) {
        buf[idx++] = (uint8_t)((args[i] & 0xFF00) >> 8);
        buf[idx++] = (uint8_t)((args[i] & 0x00FF) >> 0);

        crc = SS_SCD_common_generate_crc((uint8_t *)&buf[idx - 2],SENSIRION_WORD_SIZE);
        buf[idx++] = crc;
    }
    return idx;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * @param useconds the sleep time in microseconds
 */

static void SS_SCD_sleep_usec(uint32_t useconds)
{
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



/**
 *
 *static int16_t SS_SCD_i2c_read_cmd(uint8_t address, uint16_t cmd,uint16_t *data_words, uint16_t num_words)
 *{
 *    return SS_SCD_i2c_delayed_read_cmd(address, cmd, 0, data_words,num_words);
 *}
 */

/**static uint8_t SS_SCD_get_configured_address()
*{
*   return SCD30_I2C_ADDRESS;
*}
*/

/**
 *static int16_t SS_SCD_set_measurement_interval(uint16_t interval_sec)
 *{
 *   int16_t ret;
 *
 *   if (interval_sec < 2 || interval_sec > 1800) {
 *return STATUS_FAIL;
 *}
 *
 *ret = SS_SCD_i2c_write_cmd_with_args(
 *   SCD30_I2C_ADDRESS, SCD30_CMD_SET_MEASUREMENT_INTERVAL, &interval_sec,SENSIRION_NUM_WORDS(interval_sec));
 *SS_SCD_sleep_usec(SCD30_WRITE_DELAY_US);
 *
 *return ret;
 *}
 */

 /**
 *static int16_t SS_SCD_i2c_read_words(uint8_t address, uint16_t *data_words,uint16_t num_words)
 *{
 *   int16_t ret;
 *   uint8_t i;
 *
 *   ret = SS_SCD_i2c_read_words_as_bytes(address, (uint8_t *)data_words,num_words);
 *   if (ret != STATUS_OK)
 *       return ret;
 *
 *   for (i = 0; i < num_words; ++i)
 *       data_words[i] = be16_to_cpu(data_words[i]);
 *
 *   return STATUS_OK;
 *}
*/

/**
 *static int16_t SS_SCD_i2c_delayed_read_cmd(uint8_t address, uint16_t cmd,uint32_t delay_us, uint16_t *data_words,uint16_t num_words)
 *{
 *   int16_t ret;
 *   uint8_t buf[SENSIRION_COMMAND_SIZE];
 *   SS_SCD_fill_cmd_send_buf(buf, cmd, NULL, 0);
 *   ret = SS_SCD_i2c_write(address, buf, SENSIRION_COMMAND_SIZE);
 *   if (ret != STATUS_OK)
 *       return ret;
 *
 *   if (delay_us)
 *       SS_SCD_sleep_usec(delay_us);
 *
 *   return SS_SCD_i2c_read_words(address, data_words, num_words);
 *}
 */

/**
 * static int16_t SS_SCD_get_data_ready(uint16_t *data_ready)
 *{
 *   return SS_SCD_i2c_read_cmd(SCD30_I2C_ADDRESS, SCD30_CMD_GET_DATA_READY,data_ready,
 *                                 SENSIRION_NUM_WORDS(*data_ready));
 *}
 */

/**
 * static int16_t SS_SCD_set_temperature_offset(uint16_t temperature_offset)
 *{
 *   int16_t ret;
 *
 *   ret = sensirion_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS, SCD30_CMD_SET_TEMPERATURE_OFFSET,&temperature_offset,
 *                                           SENSIRION_NUM_WORDS(temperature_offset));
 *   sensirion_sleep_usec(SCD30_WRITE_DELAY_US);
 *
 *   return ret;
 *}
 */

 /**
 * static int16_t SS_SCD_set_altitude(uint16_t altitude)
 *{
 *   int16_t ret;
 *
 *   ret = SS_SCD_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS,SCD30_CMD_SET_ALTITUDE, &altitude,
 *                                           SENSIRION_NUM_WORDS(altitude));
 *   SS_SCD_sleep_usec(SCD30_WRITE_DELAY_US);
 *
 *   return ret;
 *}
 */

/**
 * static int16_t SS_SCD_get_automatic_self_calibration(uint8_t *asc_enabled)
 *{
 *   uint16_t word;
 *   int16_t ret;
 *   ret = SS_SCD_i2c_read_cmd(SCD30_I2C_ADDRESS,SCD30_CMD_AUTO_SELF_CALIBRATION, &word,
 *                                SENSIRION_NUM_WORDS(word));
 *   if (ret != STATUS_OK)
 *       return ret;
 *
 *   *asc_enabled = (uint8_t)word;
 *
 *   return STATUS_OK;
 *}
 */

/**
 * static int16_t SS_SCD_enable_automatic_self_calibration(uint8_t enable_asc)
 *{
 *   int16_t ret;
 *  uint16_t asc = !!enable_asc;
 *
 *    ret = SS_SCD_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS,SCD30_CMD_AUTO_SELF_CALIBRATION,
 *                                           &asc, SENSIRION_NUM_WORDS(asc));
 *   SS_SCD_sleep_usec(SCD30_WRITE_DELAY_US);
 *
 *    return ret;
 *}
 */

/**
 * static int16_t SS_SCD_set_forced_recalibration(uint16_t co2_ppm)
 *{
 *   int16_t ret;
 *
 *   ret = SS_SCD_i2c_write_cmd_with_args(SCD30_I2C_ADDRESS, SCD30_CMD_SET_FORCED_RECALIBRATION,
 *                                           &co2_ppm,SENSIRION_NUM_WORDS(co2_ppm));
 *   SS_SCD_sleep_usec(SCD30_WRITE_DELAY_US);
 *
 *   return ret;
 *}
 */

/**
 * static int16_t SS_SCD_read_serial(char *serial)
 *{
 *   int16_t ret;
 *
 *   ret = SS_SCD_i2c_write_cmd(SCD30_I2C_ADDRESS, SCD30_CMD_READ_SERIAL);
 *   if (ret)
 *        return ret;
 *
 *   sensirion_sleep_usec(SCD30_WRITE_DELAY_US);
 *  ret = SS_SCD_i2c_read_words_as_bytes(
 *      SCD30_I2C_ADDRESS, (uint8_t *)serial, SCD30_SERIAL_NUM_WORDS);
 *    serial[2 * SCD30_SERIAL_NUM_WORDS] = '\0';
 *    return ret;
 *}
 */

/**
 * static int16_t SS_SCD_probe()
 *{
 *   uint16_t data_ready;
 * return scd30_get_data_ready(&data_ready);
 *}
 */

/**
 * Release all resources initialized by sensirion_i2c_init().
 */
/**
 * static void SS_SCD_i2c_release(void){}
 */
