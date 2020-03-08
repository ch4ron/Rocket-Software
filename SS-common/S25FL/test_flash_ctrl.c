/*
 * test_flash_ctrl.c
 *
 *  Created on: Feb 25, 2020
 *      Author: Mikolaj Wielgus
 */

#include "unity.h"
#include "SS_s25fl.h"
#include "SS_flash_ctrl.h"

static void test_flash_ctrl_init(void);
static void test_flash_ctrl_start_logging(void);
static void test_flash_ctrl_stop_logging(void);
static void test_flash_ctrl_erase_log(void);
static void test_flash_ctrl_log_var_u32(void);
static void test_flash_ctrl_write_pages(void);
static void test_flash_ctrl_read_pages(void);
static void test_flash_ctrl_set_emulating(void);

static void test_flash_ctrl_init(void)
{
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_init());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_init());
}

static void test_flash_ctrl_start_logging(void)
{
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_DISABLED, SS_flash_ctrl_start_logging());

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_DISABLED, SS_flash_ctrl_stop_logging());
}

static void test_flash_ctrl_stop_logging(void)
{
	test_flash_ctrl_start_logging();
}

static void test_flash_ctrl_erase_log(void)
{
	TEST_MESSAGE("Testing log erase. This may take up to 500 seconds...");
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_erase_log());
}

static void test_flash_ctrl_log_var_u32(void)
{
	uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_pages(0x00085200UL/S25FL_PAGE_SIZE, 1, data));
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[0]);

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x01, 0x12345678));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x02, 0xAABBCCDD));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x03, 0x11223344));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_pages(0x00085200UL/S25FL_PAGE_SIZE, 1, data));
	TEST_ASSERT_EQUAL_HEX8(0xFE, data[0]);
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[1]);

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_pages(0x00089200UL/S25FL_PAGE_SIZE, 1, data));
	TEST_ASSERT_EQUAL_HEX8(0x78, data[3]);
	TEST_ASSERT_EQUAL_HEX8(0x56, data[4]);
	TEST_ASSERT_EQUAL_HEX8(0x34, data[5]);
	TEST_ASSERT_EQUAL_HEX8(0x12, data[6]);

	TEST_ASSERT_EQUAL_HEX8(0xDD, data[10]);
	TEST_ASSERT_EQUAL_HEX8(0xCC, data[11]);
	TEST_ASSERT_EQUAL_HEX8(0xBB, data[12]);
	TEST_ASSERT_EQUAL_HEX8(0xAA, data[13]);

	TEST_ASSERT_EQUAL_HEX8(0x44, data[17]);
	TEST_ASSERT_EQUAL_HEX8(0x33, data[18]);
	TEST_ASSERT_EQUAL_HEX8(0x22, data[19]);
	TEST_ASSERT_EQUAL_HEX8(0x11, data[20]);
	//TEST_ASSERT_TRUE(SS_flash_ctrl_set_emulating(false));

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x04, 0xAAAABBBB));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x04, 0xCCCCDDDD));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x04, 0xEEEEFFFF));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_pages(0x00085200UL/S25FL_PAGE_SIZE, 1, data));
	TEST_ASSERT_EQUAL_HEX8(0xFC, data[0]);
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[1]);

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_pages(0x00089200UL/S25FL_PAGE_SIZE + 1, 1, data));
	TEST_ASSERT_EQUAL_HEX8(0xBB, data[3]);
	TEST_ASSERT_EQUAL_HEX8(0xBB, data[4]);
	TEST_ASSERT_EQUAL_HEX8(0xAA, data[5]);
	TEST_ASSERT_EQUAL_HEX8(0xAA, data[6]);

	TEST_ASSERT_EQUAL_HEX8(0xDD, data[10]);
	TEST_ASSERT_EQUAL_HEX8(0xDD, data[11]);
	TEST_ASSERT_EQUAL_HEX8(0xCC, data[12]);
	TEST_ASSERT_EQUAL_HEX8(0xCC, data[13]);

	TEST_ASSERT_EQUAL_HEX8(0xFF, data[17]);
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[18]);
	TEST_ASSERT_EQUAL_HEX8(0xEE, data[19]);
	TEST_ASSERT_EQUAL_HEX8(0xEE, data[20]);

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());

	for (uint32_t i = 0; i < S25FL_PAGE_SIZE/7 + 1; ++i) {
		TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var_u32(0x05, i));
		// Delay to allow the timestamp to change.
		HAL_Delay(1);
	}

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_pages(0x00089200UL/S25FL_PAGE_SIZE + 2, 1, data));
	// One iteration less than in previous for loop because the last write is on the next sector.
	for (uint32_t i = 0; i < S25FL_PAGE_SIZE/7; ++i) {
		TEST_ASSERT_EQUAL_HEX8(0x05, data[i*7]);
		TEST_ASSERT_EQUAL_HEX8(i, data[i*7+3]);
		TEST_ASSERT_EQUAL_HEX8(i>>8, data[i*7+4]);
		TEST_ASSERT_EQUAL_HEX8(i>>16, data[i*7+5]);
		TEST_ASSERT_EQUAL_HEX8(i>>24, data[i*7+6]);
	}
}

static void test_flash_ctrl_write_pages(void)
{
}

static void test_flash_ctrl_read_pages(void)
{
}

static void test_flash_ctrl_set_emulating(void)
{
}

int test_flash_ctrl(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_flash_ctrl_init);
	RUN_TEST(test_flash_ctrl_start_logging);
	RUN_TEST(test_flash_ctrl_stop_logging);
	RUN_TEST(test_flash_ctrl_erase_log);
	RUN_TEST(test_flash_ctrl_log_var_u32);
	RUN_TEST(test_flash_ctrl_write_pages);
	RUN_TEST(test_flash_ctrl_read_pages);
	RUN_TEST(test_flash_ctrl_set_emulating);
	return UNITY_END();
}
