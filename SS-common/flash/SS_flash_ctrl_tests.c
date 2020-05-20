/*
 * test_flash_ctrl.c
 *
 *  Created on: Feb 25, 2020
 *      Author: Mikolaj Wielgus
 */

#include "unity_fixture.h"
#include "SS_s25fl.h"
#include "SS_flash_ctrl.h"

TEST_GROUP(flash_ctrl);

TEST_GROUP_RUNNER(flash_ctrl) {
	RUN_TEST_CASE(flash_ctrl, logging_start_stop);
	RUN_TEST_CASE(flash_ctrl, erase);
	RUN_TEST_CASE(flash_ctrl, logging1);
	RUN_TEST_CASE(flash_ctrl, logging2);
	RUN_TEST_CASE(flash_ctrl, logging3);
	RUN_TEST_CASE(flash_ctrl, write_read_pages);
	RUN_TEST_CASE(flash_ctrl, emulation);
}

TEST_SETUP(flash_ctrl)
{
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_init());
}

TEST_TEAR_DOWN(flash_ctrl)
{
}

TEST(flash_ctrl, logging_start_stop)
{
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_DISABLED, SS_flash_ctrl_start_logging());

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_DISABLED, SS_flash_ctrl_stop_logging());
}

TEST(flash_ctrl, erase)
{
	TEST_MESSAGE("Testing log erase. This may take up to 500 seconds...");
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_erase_logs());

	static uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_page_dma_wait(0x00085200UL/S25FL_PAGE_SIZE, data));
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[0]);
}

TEST(flash_ctrl, logging1)
{
	static uint8_t data[S25FL_PAGE_SIZE];
	static const uint8_t data1[] = {0x12, 0x34, 0x56, 0x78};
	static const uint8_t data2[] = {0xAA, 0xBB, 0xCC, 0xDD};
	static const uint8_t data3[] = {0x11, 0x22, 0x33, 0x44};

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x01, data1, 4));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x02, data2, 4));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x03, data3, 4));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_page_dma_wait(0x00085200UL/S25FL_PAGE_SIZE, data));
	TEST_ASSERT_EQUAL_HEX8(0xFE, data[0]);
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[1]);

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_s25fl_read_page_dma_wait(0x00089200UL/S25FL_PAGE_SIZE, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[3], data1, 4));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[10], data2, 4));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[17], data3, 4));
}

TEST(flash_ctrl, logging2)
{
	static const uint8_t data1[] = {0xAA, 0xAA, 0xBB, 0xBB};
	static const uint8_t data2[] = {0xCC, 0xCC, 0xDD, 0xDD};
	static const uint8_t data3[] = {0xEE, 0xEE, 0xFF, 0xFF};

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x04, data1, 4));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x04, data2, 4));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x04, data3, 4));
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());

	static uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_page_dma_wait(0x00085200UL/S25FL_PAGE_SIZE, data));
	TEST_ASSERT_EQUAL_HEX8(0xFC, data[0]);
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[1]);

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_page_dma_wait(0x00089200UL/S25FL_PAGE_SIZE + 1, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[3], data1, 4));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[10], data2, 4));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[17], data3, 4));
}

TEST(flash_ctrl, logging3)
{
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_start_logging());

	for (uint32_t i = 0; i < S25FL_PAGE_SIZE/7 + 1; ++i) {
		TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_log_var(FLASH_CTRL_STREAM_FRONT, 0x05, (uint8_t *)&i, 4));
		// Delay to allow the timestamp to change.
		HAL_Delay(1);
	}

	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_stop_logging());

	static uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(FLASH_CTRL_STATUS_OK, SS_flash_ctrl_read_page_dma_wait(0x00089200UL/S25FL_PAGE_SIZE + 2, data));
	// One iteration less than in previous for loop because the last write is on the next sector.
	for (uint32_t i = 0; i < S25FL_PAGE_SIZE/7; ++i) {
		TEST_ASSERT_EQUAL_HEX8(0x05, data[i*7]);
		TEST_ASSERT_EQUAL_HEX8(i, data[i*7+3]);
		TEST_ASSERT_EQUAL_HEX8(i>>8, data[i*7+4]);
		TEST_ASSERT_EQUAL_HEX8(i>>16, data[i*7+5]);
		TEST_ASSERT_EQUAL_HEX8(i>>24, data[i*7+6]);
	}
}

TEST(flash_ctrl, write_read_pages)
{
	// TODO.
}

TEST(flash_ctrl, emulation)
{
	// TODO.
}
