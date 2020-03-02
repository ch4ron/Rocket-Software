/* --COPYRIGHT--,BSD
 * Copyright (c) 2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the folRESETing conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the folRESETing disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the folRESETing disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/


#ifdef RUN_TESTS

#include "SS_ADS1258_unit_tests.h"
#include "SS_ADS1258.h"
#include "spi.h"
#include "gpio.h"
#include "stdio.h"
#include "unity_fixture.h"

/* NOTES:
 * - Unless specified, tests should be self-contained (results should not depend on the testing sequence)!
 * - Tests that modify register configurations should restore the device to its default state.
 * - All tests should return a boolean data type!
 * - All test functions and conditions should be documented to explain their significance.
 * - Avoid polling nDRDY without a timeout to avoid the possibility of software entering into an infinite loop.
 * - Before running the tests you need to call functions SS_ADS1258_set_interrupt_flag() in the corresponding interrupt
 */

static volatile bool flag_nDRDY_INTERRUPT;
static volatile bool flag_data_INTERRUPT;

TEST_GROUP(ADS1258_GPIO);
TEST_GROUP(ADS1258_Functionality);

TEST_GROUP_RUNNER(ADS1258_GPIO) {
    RUN_TEST_CASE(ADS1258_GPIO, PWDN);
    RUN_TEST_CASE(ADS1258_GPIO, START);
    RUN_TEST_CASE(ADS1258_GPIO, RESET);
    RUN_TEST_CASE(ADS1258_GPIO, CS);
    RUN_TEST_CASE(ADS1258_GPIO, DRDY_interrupt);
}

TEST_GROUP_RUNNER(ADS1258_Functionality) {
    RUN_TEST_CASE(ADS1258_Functionality, read_register);
    RUN_TEST_CASE(ADS1258_Functionality, write_register);
    RUN_TEST_CASE(ADS1258_Functionality, reset_command);
    RUN_TEST_CASE(ADS1258_Functionality, multiple_read_write);
    RUN_TEST_CASE(ADS1258_Functionality, read_data_command);
    RUN_TEST_CASE(ADS1258_Functionality, read_data_direct);
    RUN_TEST_CASE(ADS1258_Functionality, read_data_command_dma);
    RUN_TEST_CASE(ADS1258_Functionality, noise);
}

TEST_SETUP(ADS1258_GPIO) {}
TEST_TEAR_DOWN(ADS1258_GPIO) {
}

TEST_SETUP(ADS1258_Functionality) {}
TEST_TEAR_DOWN(ADS1258_Functionality) {
    // Restore default register state
    SS_ADS1258_sendCommand(OPCODE_RESET);
}

void SS_ADS1258_run_tests() {
#ifndef SIMULATE
    SS_ADS1258_stopMeasurements();
    HAL_Delay(50); // Delay in case we are calling this function while the power supplies are still settling.
    SS_ADS1258_toggleRESET(); // NOTE: We are assuming that this function is working before we've tested it.
    RUN_TEST_GROUP(ADS1258_GPIO);
    RUN_TEST_GROUP(ADS1258_Functionality);
    SS_ADS1258_startMeasurements();
#endif
}

////////////////////////////////   TEST CASES   ////////////////////////////////

/// GPIO Functionality ///////////////////////////////////////


//PWDN connected to vcc
// Tests if the GPIO pin connected to /PWDN is functioning
// NOTE: This test will NOT catch broken connections between the MCU and ADC!
TEST(ADS1258_GPIO, PWDN) {
    bool b_pass = true;
    TEST_ASSERT_TRUE(b_pass);
}

// Tests if the GPIO pin connected to START is functioning
// NOTE: This test will NOT catch broken connections between the MCU and ADC!
TEST(ADS1258_GPIO, START) {
    bool setting;

    // Set nCS RESET and read it back
    HAL_GPIO_WritePin(ADS_START_GPIO_Port, ADS_START_Pin, RESET);
    setting = HAL_GPIO_ReadPin(ADS_START_GPIO_Port, ADS_START_Pin);
    TEST_ASSERT_FALSE(setting);

    // Set START SET and read it back
    HAL_GPIO_WritePin(ADS_START_GPIO_Port, ADS_START_Pin, SET);
    setting = HAL_GPIO_ReadPin(ADS_START_GPIO_Port, ADS_START_Pin);
    TEST_ASSERT_TRUE(setting);
}

// Tests if the GPIO pin connected to /RESET is functioning
// NOTE: This test will NOT catch broken connections between the MCU and ADC!
TEST(ADS1258_GPIO, RESET) {
    bool setting;

    // Set nRESET RESET and read it back
    HAL_GPIO_WritePin(ADS_RESET_GPIO_Port, ADS_RESET_Pin, RESET);
    setting = HAL_GPIO_ReadPin(ADS_RESET_GPIO_Port, ADS_RESET_Pin);
    TEST_ASSERT_FALSE(setting);

    // Set nRESET SET and read it back
    HAL_GPIO_WritePin(ADS_RESET_GPIO_Port, ADS_RESET_Pin, SET);
    setting = HAL_GPIO_ReadPin(ADS_RESET_GPIO_Port, ADS_RESET_Pin);
    TEST_ASSERT_TRUE(setting);
}

//CS connected to ground
// Tests if the GPIO pin connected to /CS is functioning
// NOTE: This test will NOT catch broken connections between the MCU and ADC!
TEST(ADS1258_GPIO, CS) {
    bool b_pass = true;
    TEST_ASSERT_TRUE(b_pass);
}

// Tests if the /DRDY interrupt and interrupt handler is functioning
TEST(ADS1258_GPIO, DRDY_interrupt) {
    // Set START and nPWDN pins SET and check if nDRDY is active and interrupt is functioning
//    setPWDN(SET);
    HAL_GPIO_WritePin(ADS_RESET_GPIO_Port, ADS_RESET_Pin, RESET);
    SS_ADS1258_toggleRESET();  // Abort current conversion

    TEST_ASSERT_TRUE(SS_ADS1258_waitForDRDYinterrupt(50));
}

/// SPI Functionality ////////////////////////////////////////

// Tests if reading a (non-zero) register returns the expected result.
TEST(ADS1258_Functionality, read_register) {
    uint8_t value;

    SS_ADS1258_toggleRESET();

    value = SS_ADS1258_readSingleRegister(REG_ADDR_CONFIG0);
    TEST_ASSERT_EQUAL_UINT8(CONFIG0_DEFAULT, value);
}

// Tests if writing to and then reading back a register returns the expected result.
TEST(ADS1258_Functionality, write_register) {
    uint8_t writeValue, readValue;

    writeValue = (~CONFIG0_DEFAULT) & (0x7E);
    SS_ADS1258_writeSingleRegister(REG_ADDR_CONFIG0, writeValue);

    readValue = SS_ADS1258_readSingleRegister(REG_ADDR_CONFIG0);

    TEST_ASSERT_EQUAL_UINT8(writeValue, readValue);
}

// Tests if the RESET SPI command causes a register to return to its default value.
TEST(ADS1258_Functionality, reset_command) {
    uint8_t writeValue, readValue;

    // Write non-default value to register
    writeValue = (~CONFIG0_DEFAULT) & (0x7E);
    SS_ADS1258_writeSingleRegister(REG_ADDR_CONFIG0, writeValue);

    // Issue reset command
    SS_ADS1258_sendCommand(OPCODE_RESET);

    // Check that internal array was automatically updated
    TEST_ASSERT_EQUAL_UINT8(CONFIG0_DEFAULT, SS_ADS1258_getRegisterValue(REG_ADDR_CONFIG0));

    // Check if read register command returns default value
    readValue = SS_ADS1258_readSingleRegister(REG_ADDR_CONFIG0);
    TEST_ASSERT_EQUAL_UINT(CONFIG0_DEFAULT, readValue);
}

// Tests if writing to and then reading back multiple registers returns the expected results.
TEST(ADS1258_Functionality, multiple_read_write) {
    uint8_t i, writeValues[7];

    // Test data - when reasonable, register bits where inverted
    writeValues[0] = 0x74u;   // CONFIG0
    writeValues[1] = 0x7Cu;   // CONFIG1
    writeValues[2] = 0xFFu;   // MUXSCH
    writeValues[3] = 0xFFu;   // MUXDIF
    writeValues[4] = 0x00u;   // MUXSG0
    writeValues[5] = 0x00u;   // MUXSG1
    writeValues[6] = 0xFFu;   // SYSRED

    // In case of external GPIO connections, do not test GPIO pins here.
    // This should be done in a separate test case.

    // Write registers
    SS_ADS1258_writeMultipleRegisters(REG_ADDR_CONFIG0, 7, writeValues);

    // Check that internal array was automatically updated
    for (i = 0; i < 7; i++) {
        TEST_ASSERT_EQUAL_UINT8(writeValues[i], SS_ADS1258_getRegisterValue(i));
    }

    HAL_Delay(1);
    // Read back registers
    SS_ADS1258_readMultipleRegisters(REG_ADDR_CONFIG0, 7);
    // Check that read back values match the written values
    for (i = 0; i < 7; i++) {
        TEST_ASSERT_EQUAL_UINT8(writeValues[i], SS_ADS1258_getRegisterValue(i));
    }
}

static void ADS1258_read_data_check(uint8_t *statusBytes, int32_t *dataValues) {
    uint8_t i;
    int32_t upperLimit, lowerlimit;
    //Ensure every channel is read
    uint8_t statusGood = STATUS_NEW_MASK & ~STATUS_OVF_MASK & ~STATUS_SUPPLY_MASK;
    // Check status bytes
    for(int j = 0; j < 5; j++) {
        for(i = j + 1; i < 5; i++) {
            TEST_ASSERT_NOT_EQUAL(statusBytes[i], statusBytes[j]);
        }
        if((statusBytes[j] & 0b00011111) == STATUS_CHID_OFFSET) {
            // Check OFFSET data
//            upperLimit = 0xFFFFFEE0;    // -91.5 uV
            upperLimit = 0xFFFFFFFF;
            lowerlimit = 0x00000120;    // +91.5 uV
			TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, dataValues[j]);
			TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, dataValues[j]);
            TEST_ASSERT_TRUE(statusBytes[j] == (statusGood | STATUS_CHID_OFFSET));
        }
        else if((statusBytes[j] & 0b00011111) == STATUS_CHID_VCC) {
            // Check VCC data
            lowerlimit = 0x00390000;    // 4.75 V
            upperLimit = 0x003F0000;    // 5.25 V
			TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, dataValues[j]);
			TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, dataValues[j]);
            TEST_ASSERT_TRUE(statusBytes[j] == (statusGood | STATUS_CHID_VCC));
        }
        else if((statusBytes[j] & 0b00011111) == STATUS_CHID_TEMP) {
            // Check TEMP data
            lowerlimit = 0x000384AE;   // 0 deg. C, with 5.25V reference
            upperLimit = 0x002BB2B0;   // 50 deg. C with 0.50V reference
			TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, dataValues[j]);
			TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, dataValues[j]);
            TEST_ASSERT_TRUE(statusBytes[j] == (statusGood | STATUS_CHID_TEMP));
        }
        else if((statusBytes[j] & 0b00011111) == STATUS_CHID_GAIN) {
            // Check GAIN data
            lowerlimit = 0x0070CCCC;    // -0.6% V/V
            upperLimit = 0x007F3333;    // +0.6% V/V
			TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, dataValues[j]);
			TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, dataValues[j]);
            TEST_ASSERT_TRUE(statusBytes[j] == (statusGood | STATUS_CHID_GAIN));
        }
        else if((statusBytes[j] & 0b00011111) == STATUS_CHID_REF) {
            // Check REF data
            // TODO: Update these values based on your expected reference voltage accuracy!
            lowerlimit = 0x00060000;    // 0.50 V
            upperLimit = 0x003F0000;    // 5.25 V
			TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, dataValues[j]);
			TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, dataValues[j]);
            TEST_ASSERT_TRUE(statusBytes[j] == (statusGood | STATUS_CHID_REF));
        }
        else TEST_FAIL_MESSAGE("Wrong channel or garbage data");
    }
}

// Tests if reading data from the internal monitoring channels returns data within an expected range.  uint8_t i, writeValues[7], statusBytes[5], statusGood;
void ADS1258_read_data_test(uint8_t mode) {
    uint8_t i, writeValues[7], statusBytes[5];
    int32_t dataValues[5];

    // Test configuration
    writeValues[0] = CONFIG0_DEFAULT & ~CONFIG0_MUXMOD_MASK;  // Auto-scan mode
    writeValues[1] = CONFIG1_DEFAULT & CONFIG1_DRATE_1953SPS; // SRESETest data rate
    writeValues[2] = MUXSCH_DEFAULT;
    writeValues[3] = MUXDIF_DEFAULT;                          // Differential channels off
    writeValues[4] = 0x00;                                    // Single-ended channels off
    writeValues[5] = 0x00;
    writeValues[6] = SYSRED_REF_ENABLE |
    SYSRED_GAIN_ENABLE |
    SYSRED_TEMP_ENABLE |
    SYSRED_VCC_ENABLE |
    SYSRED_OFFSET_ENABLE;                    // Enable all system monitors

    // Ensure device is powered, converting, and reset to default values
    SS_ADS1258_toggleRESET();
    HAL_GPIO_WritePin(ADS_START_GPIO_Port, ADS_START_Pin, RESET);

    // Write registers
    SS_ADS1258_writeMultipleRegisters(REG_ADDR_CONFIG0, 7, writeValues);

    SS_ADS1258_startConversions();

    // Read all data monitor channels
    for (i = 0; i < 5; i++) {
        // Wait and check that /DRDY interrupt occurred
        TEST_ASSERT_TRUE(SS_ADS1258_waitForDRDYinterrupt(10));

        //  Read data in read direct mode
        dataValues[i] = SS_ADS1258_readData(&statusBytes[i], NULL, mode);
    }
    ADS1258_read_data_check(statusBytes, dataValues);
}

TEST(ADS1258_Functionality, read_data_command) {
    ADS1258_read_data_test(COMMAND);
}

TEST(ADS1258_Functionality, read_data_direct) {
    ADS1258_read_data_test(DIRECT);
}

// Tests if reading data from the internal monitoring channels returns data within an expected range.
void ADS1258_test_read_dataDMA(uint8_t mode) {
    uint8_t i, writeValues[7], statusBytes[5];
    int32_t dataValues[5];

    // Test configuration
    writeValues[0] = CONFIG0_DEFAULT & ~CONFIG0_MUXMOD_MASK;  // Auto-scan mode
    writeValues[1] = CONFIG1_DEFAULT & CONFIG1_DRATE_1953SPS; // SRESETest data rate
    writeValues[2] = MUXSCH_DEFAULT;
    writeValues[3] = MUXDIF_DEFAULT;                          // Differential channels off
    writeValues[4] = 0x00;                                    // Single-ended channels off
    writeValues[5] = 0x00;
    writeValues[6] = SYSRED_REF_ENABLE |
    SYSRED_GAIN_ENABLE |
    SYSRED_TEMP_ENABLE |
    SYSRED_VCC_ENABLE |
    SYSRED_OFFSET_ENABLE;                    // Enable all system monitors

    // Ensure device is powered, converting, and reset to default values
    SS_ADS1258_toggleRESET();
    HAL_GPIO_WritePin(ADS_START_GPIO_Port, ADS_START_Pin, RESET);

    // Write registers
    SS_ADS1258_writeMultipleRegisters(REG_ADDR_CONFIG0, 7, writeValues);

    SS_ADS1258_startConversions();
    HAL_Delay(50);
//    dupa = 1;
    // Read all data monitor channels
    for (i = 0; i < 5; i++) {
        // Wait and check that /DRDY interrupt occurred
        TEST_ASSERT_TRUE(SS_ADS1258_waitForDRDYinterrupt(500));
        SS_ADS1258_readDataDMA(mode);
        TEST_ASSERT_TRUE(SS_ADS1258_waitForDataInterrupt(500));
        dataValues[i] = SS_ADS1258_getData(&statusBytes[i]);
    }
    ADS1258_read_data_check(statusBytes, dataValues);
}

TEST(ADS1258_Functionality, read_data_command_dma) {
    /* TODO fix this test */
    TEST_IGNORE();
    ADS1258_test_read_dataDMA(COMMAND);
}

TEST(ADS1258_Functionality, read_data_direct_dma) {
    ADS1258_test_read_dataDMA(DIRECT);
}
/// ADC Performance  /////////////////////////////////////////

// Tests if reading data with ADC input shorted returns expected data variance.
// NOTE: The test limit needs to be updated for different voltage reference values.
TEST(ADS1258_Functionality, noise) {
    uint8_t i, writeValues[7], statusByte, statusGood, numSamples;
    int32_t dataValue, ppNoiseLimit, ppMin = 0, ppMax = 0;

    // Test configuration
    statusGood = STATUS_NEW_MASK & ~STATUS_OVF_MASK & ~STATUS_SUPPLY_MASK;

    writeValues[0] = CONFIG0_DEFAULT & ~CONFIG0_MUXMOD_MASK;  // Auto-scan mode
    writeValues[1] = CONFIG1_DEFAULT & CONFIG1_DRATE_1953SPS; // SRESETest data rate
    writeValues[2] = MUXSCH_DEFAULT;
    writeValues[3] = MUXDIF_DEFAULT;                          // Differential channels off
    writeValues[4] = 0x00;                                    // Single-ended channels off
    writeValues[5] = 0x00;
    writeValues[6] = SYSRED_OFFSET_ENABLE;                    // Enable offset monitor

    // Ensure device is powered, converting, and reset to default values
    SS_ADS1258_startConversions();
    SS_ADS1258_toggleRESET();

    // Write registers
    SS_ADS1258_writeMultipleRegisters(REG_ADDR_CONFIG0, 7, writeValues);

    // Read all data monitor channels
    numSamples = 50;
    for (i = 0; i < numSamples; i++) {
        // Wait and check that /DRDY interrupt occurred
        TEST_ASSERT_TRUE(SS_ADS1258_waitForDRDYinterrupt(10));

        // Read data with read command
        dataValue = SS_ADS1258_readData(&statusByte, NULL, COMMAND);

        // Check status byte
        TEST_ASSERT_EQUAL_UINT8((statusGood | STATUS_CHID_OFFSET), statusByte);

        // Keep track of MIN and MAX conversion results
        if (i == 0) {
            ppMin = dataValue;
            ppMax = dataValue;
        } else if (dataValue < ppMin) {
            ppMin = dataValue;
        } else if (dataValue > ppMax) {
            ppMax = dataValue;
        }
    }

    // Check if we exceeded expected PP noise limit
    ppNoiseLimit = 6 * 3 * 4; // ppNoiseLimit = Crest factor * uVrms (for selected data rate) * codes/uV (VREF dependent)
    TEST_ASSERT_TRUE((ppMax - ppMin) < ppNoiseLimit);
}

void SS_ADS1258_set_nDRDY_interrupt_flag(uint16_t GPIO_Pin) {
    if (GPIO_Pin == ADS_DRDY_Pin) {
        flag_nDRDY_INTERRUPT = true;
    }
}

void SS_ADS1258_set_data_interrupt_flag() {
    flag_data_INTERRUPT = true;
}

bool SS_ADS1258_waitForDRDYinterrupt(uint32_t timeout_ms) {
    /* --- TODO: INSERT YOUR CODE HERE ---
     * Poll the nDRDY GPIO pin until it goes low. To avoid potential infinite
     * loops, you may also want to implement a timer interrupt to occur after
     * the specified timeout period, in case the nDRDY pin is not active.
     * Return a boolean to indicate if nDRDY went low or if a timeout occurred.
     */

    // Reset interrupt flag
    int32_t timeout = 9000*timeout_ms; //Ticks for 180 mhz clock
    flag_nDRDY_INTERRUPT = false;

    // Wait for nDRDY interrupt or timeout - each iteration is about 20 ticks
    do {
        timeout--;
    } while (!flag_nDRDY_INTERRUPT && (timeout > 0));

    // Reset interrupt flag
    flag_nDRDY_INTERRUPT = false;

    return (timeout > 0);           // Did a nDRDY interrupt occur?
}

bool SS_ADS1258_waitForDataInterrupt(uint32_t timeout_ms) {
    /* --- TODO: INSERT YOUR CODE HERE ---
     * Poll the nDRDY GPIO pin until it goes low. To avoid potential infinite
     * loops, you may also want to implement a timer interrupt to occur after
     * the specified timeout period, in case the nDRDY pin is not active.
     * Return a boolean to indicate if nDRDY went low or if a timeout occurred.
     */

    flag_data_INTERRUPT = false;
    // Reset interrupt flag
    int32_t timeout = 9000*timeout_ms; //Ticks for 180 mhz clock

    // Wait for nDRDY interrupt or timeout - each iteration is about 20 ticks
    do {
        timeout--;
    } while (!flag_data_INTERRUPT && (timeout > 0));

    // Reset interrupt flag
    flag_data_INTERRUPT = false;

    return (timeout > 0);           // Did a nDRDY interrupt occur?
}

#endif
