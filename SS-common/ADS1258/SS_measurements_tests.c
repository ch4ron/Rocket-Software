/*
 * SS_measurements.c
 *
 *  Created on: 05.02.2020
 *      Author: maciek
 */

#include "unity_fixture.h"
#include "SS_measurements.h"
#include "SS_ADS1258.h"

TEST_GROUP(measurements);

TEST_GROUP_RUNNER(measurements) {
	RUN_TEST_CASE(measurements, chid_to_ch);
	RUN_TEST_CASE(measurements, start);
	RUN_TEST_CASE(measurements, values);
	RUN_TEST_CASE(measurements, read_vcc);
}

TEST_SETUP(measurements) {}

TEST_TEAR_DOWN(measurements) {
	SS_ADS1258_stopMeasurements();
	SS_measurements_clear();
}

extern void SS_measurements_chid_to_channel(uint8_t chid, uint8_t* reg_addr, uint8_t* reg_mask);
extern void SS_measurements_start();

TEST(measurements, chid_to_ch) {
	uint8_t reg_addr, reg_mask;
	SS_measurements_chid_to_channel(0, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXDIF, reg_addr);
	TEST_ASSERT_EQUAL(MUXDIF_DIFF0_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(3, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXDIF, reg_addr);
	TEST_ASSERT_EQUAL(MUXDIF_DIFF3_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(7, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXDIF, reg_addr);
	TEST_ASSERT_EQUAL(MUXDIF_DIFF7_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(8, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXSG0, reg_addr);
	TEST_ASSERT_EQUAL(MUXSG0_AIN0_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(0x0B, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXSG0, reg_addr);
	TEST_ASSERT_EQUAL(MUXSG0_AIN3_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(0x0F, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXSG0, reg_addr);
	TEST_ASSERT_EQUAL(MUXSG0_AIN7_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(0x10, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXSG1, reg_addr);
	TEST_ASSERT_EQUAL(MUXSG1_AIN8_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(0x13, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXSG1, reg_addr);
	TEST_ASSERT_EQUAL(MUXSG1_AIN11_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(0x17, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_MUXSG1, reg_addr);
	TEST_ASSERT_EQUAL(MUXSG1_AIN15_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(STATUS_CHID_OFFSET, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_SYSRED, reg_addr);
	TEST_ASSERT_EQUAL(SYSRED_OFFSET_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(STATUS_CHID_VCC, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_SYSRED, reg_addr);
	TEST_ASSERT_EQUAL(SYSRED_VCC_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(STATUS_CHID_TEMP, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_SYSRED, reg_addr);
	TEST_ASSERT_EQUAL(SYSRED_TEMP_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(STATUS_CHID_GAIN, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_SYSRED, reg_addr);
	TEST_ASSERT_EQUAL(SYSRED_GAIN_ENABLE, reg_mask);
	SS_measurements_chid_to_channel(STATUS_CHID_REF, &reg_addr, &reg_mask);
	TEST_ASSERT_EQUAL(REG_ADDR_SYSRED, reg_addr);
	TEST_ASSERT_EQUAL(SYSRED_REF_ENABLE, reg_mask);
}

TEST(measurements, start) {
	SS_measurements_add(0x04);
	SS_measurements_add(0x01);
	SS_measurements_add(0x0A);
	SS_measurements_add(0x17);
	SS_measurements_add(STATUS_CHID_GAIN);
	SS_measurements_start();
	uint8_t reg = SS_ADS1258_readSingleRegister(REG_ADDR_MUXDIF);
	TEST_ASSERT_EQUAL_HEX8(MUXDIF_DIFF1_ENABLE | MUXDIF_DIFF4_ENABLE, reg);
	reg = SS_ADS1258_readSingleRegister(REG_ADDR_MUXSG0);
	TEST_ASSERT_EQUAL_HEX8(MUXSG0_AIN2_ENABLE, reg);
	reg = SS_ADS1258_readSingleRegister(REG_ADDR_MUXSG1);
	TEST_ASSERT_EQUAL_HEX8(MUXSG1_AIN15_ENABLE, reg);
	reg = SS_ADS1258_readSingleRegister(REG_ADDR_SYSRED);
	TEST_ASSERT_EQUAL_HEX8(SYSRED_GAIN_ENABLE, reg);
}

extern Measurement measurements[MEASUREMENTS_NUM];

TEST(measurements, values) {
	SS_measurements_add(STATUS_CHID_OFFSET);
	SS_measurements_add(STATUS_CHID_VCC);
	SS_measurements_add(STATUS_CHID_TEMP);
	SS_measurements_add(STATUS_CHID_GAIN);
	SS_measurements_add(STATUS_CHID_REF);
	SS_measurements_start();
	HAL_Delay(5);
	TEST_ASSERT_EQUAL(SYSRED_GAIN_ENABLE | SYSRED_OFFSET_ENABLE | SYSRED_REF_ENABLE | SYSRED_TEMP_ENABLE | SYSRED_VCC_ENABLE, SS_ADS1258_getRegisterValue(REG_ADDR_SYSRED));
	HAL_Delay(50);
	uint32_t lowerlimit, upperLimit, data;
	for(uint8_t i = 0; i < MEASUREMENTS_NUM; i++) {
		switch(measurements[i].channel) {
			case(STATUS_CHID_OFFSET):
//				upperLimit = 0xFFFFFEE0;    // -91.5 uV
				upperLimit = 0xFFFFFFFF;
				lowerlimit = 0x00000120;    // +91.5 uV
				data = measurements[i].raw;
				TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, data);
				TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, data);
				break;
			case(STATUS_CHID_VCC):
				lowerlimit = 0x00390000;    // 4.75 V
				upperLimit = 0x003F0000;    // 5.25 V
				data = measurements[i].raw;
				TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, data);
				TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, data);
				break;
			case(STATUS_CHID_TEMP):
				lowerlimit = 0x000384AE;   // 0 deg. C, with 5.25V reference
				upperLimit = 0x002BB2B0;   // 50 deg. C with 0.50V reference
				data = measurements[i].raw;
				TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, data);
				TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, data);
				break;
			case(STATUS_CHID_GAIN):
				lowerlimit = 0x0070CCCC;    // -0.6% V/V
				upperLimit = 0x007F3333;    // +0.6% V/V
				data = measurements[i].raw;
				TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, data);
				TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, data);
				break;
			case(STATUS_CHID_REF):
				lowerlimit = 0x00060000;    // 0.50 V
				upperLimit = 0x003F0000;    // 5.25 V
				data = measurements[i].raw;
				TEST_ASSERT_LESS_OR_EQUAL_HEX32(upperLimit, data);
				TEST_ASSERT_GREATER_OR_EQUAL_HEX32(lowerlimit, data);
				break;
		}
	}
}

TEST(measurements, read_vcc) {
    float vcc = SS_measurements_read_VCC();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, vcc);
}
