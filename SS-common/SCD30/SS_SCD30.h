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

#ifndef SCD30_H
#define SCD30_H



typedef float float32_t;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t id;
    float32_t co2_ppm;
    float32_t temperature;
    float32_t relative_humidity;
    int16_t error;
    uint16_t interval_in_seconds ;
} SCD30;



void SS_SCD_task(void *pvParameters);

#define STATUS_OK 0
#define STATUS_FAIL (-1)
#define SENSIRION_BIG_ENDIAN 0
#define NULL ((void *)0)
#if SENSIRION_BIG_ENDIAN
#define be16_to_cpu(s) (s)
#define be32_to_cpu(s) (s)
#define be64_to_cpu(s) (s)
#define SENSIRION_WORDS_TO_BYTES(a, w) ()

#else /* SENSIRION_BIG_ENDIAN */

#define be16_to_cpu(s) (((uint16_t)(s) << 8) | (0xff & ((uint16_t)(s)) >> 8))
#define be32_to_cpu(s)                                                         \
    (((uint32_t)be16_to_cpu(s) << 16) | (0xffff & (be16_to_cpu((s) >> 16))))
#define be64_to_cpu(s)                                                         \
    (((uint64_t)be32_to_cpu(s) << 32) |                                        \
     (0xffffffff & ((uint64_t)be32_to_cpu((s) >> 32))))


#define SENSIRION_WORDS_TO_BYTES(a, w)                                         \
    for (uint16_t *__a = (uint16_t *)(a), __e = (w), __w = 0; __w < __e;       \
         ++__w) {                                                              \
        __a[__w] = be16_to_cpu(__a[__w]);                                      \
    }
#endif /* SENSIRION_BIG_ENDIAN */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
#endif

#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xFF
#define CRC8_LEN 1

#define SENSIRION_COMMAND_SIZE 2
#define SENSIRION_WORD_SIZE 2
#define SENSIRION_NUM_WORDS(x) (sizeof(x) / SENSIRION_WORD_SIZE)
#define SENSIRION_MAX_BUFFER_WORDS 32

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



/**
 * scd30_probe() - check if the SCD sensor is available and initialize it
 *
 * @return  0 on success, an error code otherwise.
 */

 /**
 *int16_t scd30_probe(void);
 */

/**
 * scd30_get_driver_version() - Returns the driver version
 *
 * @return  Driver version string
 */

 /**
 *const char *scd30_get_driver_version(void);
 */

/**
 * scd30_get_configured_address() - Returns the configured I2C address
 *
 * @return      uint8_t I2C address
 */

 /**
 *uint8_t scd30_get_configured_address(void);
 */

/**
 * scd30_start_periodic_measurement() - Start continuous measurement to measure
 * CO2 concentration, relative humidity and temperature or updates the ambient
 * pressure if the periodic measurement is already running.
 *
 * Measurement data which is not read from the sensor is continuously
 * overwritten. The CO2 measurement value can be compensated for ambient
 * pressure by setting the pressure value in mBar. Setting the ambient pressure
 * overwrites previous and future settings of altitude compensation. Setting the
 * pressure to zero deactivates the ambient pressure compensation.
 * The continuous measurement status is saved in non-volatile memory. The last
 * measurement mode is resumed after repowering.
 *
 * @param ambient_pressure_mbar Ambient pressure in millibars. 0 to deactivate
 *                              ambient pressure compensation (reverts to
 *                              altitude compensation, if set), 700-1200mBar
 *                              allowable range otherwise
 *
 * @return                      0 if the command was successful, an error code
 *                              otherwise
 */

/**
 * scd30_stop_periodic_measurement() - Stop the continuous measurement
 *
 * @return  0 if the command was successful, else an error code
 */

 /**
 *int16_t scd30_stop_periodic_measurement(void);
 */

/**
 * scd30_read_measurement() - Read out an available measurement when new
 * measurement data is available.
 * Make sure that the measurement is completed by reading the data ready status
 * bit with scd30_get_data_ready().
 *
 * @param co2_ppm       CO2 concentration in ppm
 * @param temperature   the address for the result of the temperature
 *                      measurement
 * @param humidity      the address for the result of the relative humidity
 *                      measurement
 *
 * @return              0 if the command was successful, an error code otherwise
 */


/**
 * scd30_set_measurement_interval() - Sets the measurement interval in
 * continuous measurement mode.
 *
 * The initial value on powerup is 2s. The chosen measurement interval is saved
 * in non-volatile memory and thus is not reset to its initial value after power
 * up.
 *
 * @param interval_sec  The measurement interval in seconds. The allowable range
 *                      is 2-1800s
 *
 * @return              0 if the command was successful, an error code otherwise
 */

 /**
 *int16_t scd30_set_measurement_interval(uint16_t interval_sec);
 */

/**
 * scd30_get_data_ready() - Get data ready status
 *
 * Data ready command is used to determine if a measurement can be read from the
 * sensor's buffer. Whenever there is a measurement available from the internal
 * buffer this command returns 1 and 0 otherwise. As soon as the measurement has
 * been read by the return value changes to 0. It is recommended to use the data
 * ready status byte before readout of the measurement values with
 * scd30_read_measurement().
 *
 * @param data_ready    Pointer to memory of where to set the data ready bit.
 *                      The memory is set to 1 if a measurement is ready to be
 *                      fetched, 0 otherwise.
 *
 * @return              0 if the command was successful, an error code otherwise
 */


/**
 * scd30_set_temperature_offset() - Set the temperature offset
 *
 * The on-board RH/T sensor is influenced by thermal self-heating of SCD30 and
 * other electrical components. Design-in alters the thermal properties of SCD30
 * such that temperature and humidity offsets may occur when operating the
 * sensor in end-customer devices. Compensation of those effects is achievable
 * by writing the temperature offset found in continuous operation of the device
 * into the sensor.
 * The temperature offset value is saved in non-volatile memory. The last set
 * value will be used after repowering.
 *
 * @param temperature_offset    Temperature offset, unit [degrees Celsius * 100]
 *                              i.e. one tick corresponds to 0.01 degrees C
 *
 * @return                      0 if the command was successful, an error code
 *                              otherwise
 */

 /**
 *int16_t scd30_set_temperature_offset(uint16_t temperature_offset);
 */

/**
 * scd30_set_altitude() - Set the altitude above sea level
 *
 * Measurements of CO2 concentration are influenced by altitude. When a value is
 * set, the altitude-effect is compensated. The altitude setting is disregarded
 * when an ambient pressure is set on the sensor with
 * scd30_start_periodic_measurement.
 * The altitude is saved in non-volatile memory. The last set value will be used
 * after repowering.
 *
 * @param altitude  altitude in meters above sea level, 0 meters is the default
 *                  value and disables altitude compensation
 *
 * @return          0 if the command was successful, an error code otherwise
 */
 /**
 *int16_t scd30_set_altitude(uint16_t altitude);
 */
/**
 * scd30_get_automatic_self_calibration() - Read if the sensor's automatic self
 * calibration is enabled or disabled
 *
 * See scd30_enable_automatic_self_calibration() for more details.
 *
 * @param asc_enabled   Pointer to memory of where to set the self calibration
 *                      state. 1 if ASC is enabled, 0 if ASC disabled. Remains
 *                      untouched if return is non-zero.
 *
 * @return              0 if the command was successful, an error code otherwise
 */

 /**
 *int16_t scd30_get_automatic_self_calibration(uint8_t *asc_enabled);
 */

/**
 * scd30_enable_automatic_self_calibration() - Enable or disable the sensor's
 * automatic self calibration
 *
 * When activated for the first time a period of minimum 7 days is needed so
 * that the algorithm can find its initial parameter set for ASC.
 * The sensor has to be exposed to fresh air for at least 1 hour every day.
 * Refer to the datasheet for further conditions
 *
 * ASC status is saved in non-volatile memory. When the sensor is powered down
 * while ASC is activated SCD30 will continue with automatic self-calibration
 * after repowering without sending the command.
 *
 * @param enable_asc    enable ASC if non-zero, disable otherwise
 *
 * @return              0 if the command was successful, an error code otherwise
 */
 /**
 *int16_t scd30_enable_automatic_self_calibration(uint8_t enable_asc);
 */
/**
 * scd30_set_forced_recalibration() - Forcibly recalibrate the sensor to a known
 * value.
 *
 * Forced recalibration (FRC) is used to compensate for sensor drifts when a
 * reference value of the CO2 concentration in close proximity to the SCD30 is
 * available.
 *
 * For best results the sensor has to be run in a stable environment in
 * continuous mode at a measurement rate of 2s for at least two minutes before
 * applying the calibration command and sending the reference value.
 * Setting a reference CO2 concentration will overwrite the settings from ASC
 * (see scd30_enable_automatic_self_calibration) and vice-versa. The reference
 * CO2 concentration has to be in the range 400..2000 ppm.
 *
 * FRC value is saved in non-volatile memory, the last set FRC value will be
 * used for field-calibration after repowering.
 *
 * @param co2_ppm   recalibrate to this specific co2 concentration
 *
 * @return          0 if the command was successful, an error code otherwise
 */

 /**
 *int16_t scd30_set_forced_recalibration(uint16_t co2_ppm);
 */

/**
 * Read out the serial number
 *
 * @param serial    the address for the result of the serial number.
 *                  --------------------------------------
 *                  THE BUFFER MUST HOLD AT LEAST 33 BYTES
 *                  --------------------------------------
 *                  Usage example:
 *                  ```
 *                  char scd30_serial[33];
 *                  if (scd30_read_serial(scd30_serial) == 0) {
 *                      printf("SCD30 serial: %s\n", scd30_serial);
 *                  } else {
 *                      printf("Error reading SCD30 serial\n");
 *                  }
 *                  ```
 *                  Contains a zero-terminated string.
 * @return          0 if the command was successful, else an error code.
 */
 /**
 *int16_t scd30_read_serial(char *serial);
 */
/**
 * Select the current i2c bus by index.
 * All following i2c operations will be directed at that bus.
 *
 * THE IMPLEMENTATION IS OPTIONAL ON SINGLE-BUS SETUPS (all sensors on the same
 * bus)
 *
 * @param bus_idx   Bus index to select
 * @returns         0 on success, an error code otherwise
 */

 /**
 *int16_t sensirion_i2c_select_bus(uint8_t bus_idx);
 */

/**
 * Initialize all hard- and software components that are needed for the I2C
 * communication.
 */
/**
 * void sensirion_i2c_init(void);
 */


/**
 * Release all resources initialized by sensirion_i2c_init().
 */
 /**\
  * void sensirion_i2c_release(void);
  */


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
 /**
  * int8_t sensirion_i2c_read(uint8_t address, uint8_t *data, uint16_t count);
  */


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
 /**
  * int8_t sensirion_i2c_write(uint8_t address, const uint8_t *data,uint16_t count);
  */

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution approximately, but no less than, the given time.
 *
 * When using hardware i2c:
 * Despite the unit, a <10 millisecond precision is sufficient.
 *
 * When using software i2c:
 * The precision needed depends on the desired i2c frequency, i.e. should be
 * exact to about half a clock cycle (defined in
 * `SENSIRION_I2C_CLOCK_PERIOD_USEC` in `sensirion_arch_config.h`).
 *
 * Example with 400kHz requires a precision of 1 / (2 * 400kHz) == 1.25usec.
 *
 * @param useconds the sleep time in microseconds
 */
/**
 * void sensirion_sleep_usec(uint32_t useconds);
 */


// sensirion_common.h


/**
 * Convert a word-array to a bytes-array, effectively reverting the
 * host-endianness to big-endian
 * @a:  word array to change (must be (uint16_t *) castable)
 * @w:  number of word-sized elements in the array (SENSIRION_NUM_WORDS(a)).
 */


/**
 * uint8_t sensirion_common_generate_crc(uint8_t *data, uint16_t count);
 * int8_t sensirion_common_check_crc(uint8_t *data, uint16_t count,uint8_t checksum);
 */

/**
 * sensirion_fill_cmd_send_buf() - create the i2c send buffer for a command and
 *                                 a set of argument words. The output buffer
 *                                 interleaves argument words with their
 *                                 checksums.
 * @buf:        The generated buffer to send over i2c. Then buffer length must
 *              be at least SENSIRION_COMMAND_LEN + num_args *
 *              (SENSIRION_WORD_SIZE + CRC8_LEN).
 * @cmd:        The i2c command to send. It already includes a checksum.
 * @args:       The arguments to the command. Can be NULL if none.
 * @num_args:   The number of word arguments in args.
 *
 * @return      The number of bytes written to buf
 */
 /**
  * uint16_t sensirion_fill_cmd_send_buf(uint8_t *buf, uint16_t cmd,const uint16_t *args, uint8_t num_args)
  */

/**
 * sensirion_i2c_read_words() - read data words from sensor
 *
 * @address:    Sensor i2c address
 * @data_words: Allocated buffer to store the read words.
 *              The buffer may also have been modified on STATUS_FAIL return.
 * @num_words:  Number of data words to read (without CRC bytes)
 *
 * @return      STATUS_OK on success, an error code otherwise
 */
 /**
  * int16_t sensirion_i2c_read_words(uint8_t address, uint16_t *data_words,uint16_t num_words);
  */

/**
 * sensirion_i2c_read_words_as_bytes() - read data words as byte-stream from
 *                                       sensor
 *
 * Read bytes without adjusting values to the uP's word-order.
 *
 * @address:    Sensor i2c address
 * @data:       Allocated buffer to store the read bytes.
 *              The buffer may also have been modified on STATUS_FAIL return.
 * @num_words:  Number of data words(!) to read (without CRC bytes)
 *              Since only word-chunks can be read from the sensor the size
 *              is still specified in sensor-words (num_words = num_bytes *
 *              SENSIRION_WORD_SIZE)
 *
 * @return      STATUS_OK on success, an error code otherwise
 */

/**
 * int16_t sensirion_i2c_read_words_as_bytes(uint8_t address, uint8_t *data,uint16_t num_words);
 */

/**
 * sensirion_i2c_write_cmd() - writes a command to the sensor
 * @address:    Sensor i2c address
 * @command:    Sensor command
 *
 * @return      STATUS_OK on success, an error code otherwise
 */

/**
 * int16_t sensirion_i2c_write_cmd(uint8_t address, uint16_t command);
 */

/**
 * sensirion_i2c_write_cmd_with_args() - writes a command with arguments to the
 *                                       sensor
 * @address:    Sensor i2c address
 * @command:    Sensor command
 * @data:       Argument buffer with words to send
 * @num_words:  Number of data words to send (without CRC bytes)
 *
 * @return      STATUS_OK on success, an error code otherwise
 */

/**
 * int16_t sensirion_i2c_write_cmd_with_args(uint8_t address, uint16_t command,const uint16_t *data_words,uint16_t num_words);
 */
/**
 * sensirion_i2c_delayed_read_cmd() - send a command, wait for the sensor to
 *                                    process and read data back
 * @address:    Sensor i2c address
 * @cmd:        Command
 * @delay:      Time in microseconds to delay sending the read request
 * @data_words: Allocated buffer to store the read data
 * @num_words:  Data words to read (without CRC bytes)
 *
 * @return      STATUS_OK on success, an error code otherwise
 */

/**
 * int16_t sensirion_i2c_delayed_read_cmd(uint8_t address, uint16_t cmd,uint32_t delay_us, uint16_t *data_words,uint16_t num_words);
 */

/**
 * sensirion_i2c_read_cmd() - reads data words from the sensor after a command
 *                            is issued
 * @address:    Sensor i2c address
 * @cmd:        Command
 * @data_words: Allocated buffer to store the read data
 * @num_words:  Data words to read (without CRC bytes)
 *
 * @return      STATUS_OK on success, an error code otherwise
 */

/**
 * int16_t sensirion_i2c_read_cmd(uint8_t address, uint16_t cmd,uint16_t *data_words, uint16_t num_words);
 */

#endif /* SCD30_H */
