/**
  * SS_flash_log_tests.c
  *
  *  Created on: May 25, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "unity_fixture.h"
#include "SS_flash_log.h"
#include "SS_s25fl.h"

TEST_GROUP(flash_log);

TEST_GROUP_RUNNER(flash_log)
{
    RUN_TEST_CASE(flash_log, log_vars);
    //RUN_TEST_CASE(flash_log, log_text);
}

TEST_SETUP(flash_log)
{
}

TEST_TEAR_DOWN(flash_log)
{
}

TEST(flash_log, log_vars)
{
    static uint64_t data1 = 0x12345678;
    static uint64_t data2 = 0x9ABCDE00;
    static uint64_t data3 = 0x11223344;

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_start_logging());
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(FLASH_STREAM_VAR, 0x01, data1));
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(FLASH_STREAM_VAR, 0x02, data2));
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(FLASH_STREAM_VAR, 0x03, data3));
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_stop_logging());

	static uint8_t data[S25FL_PAGE_SIZE];

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_read_page_dma_wait(0x00085200UL/S25FL_PAGE_SIZE, data));
	TEST_ASSERT_EQUAL_HEX8(0xE0, data[0]);
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[1]);

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_s25fl_read_page_dma_wait(0x00089200UL/S25FL_PAGE_SIZE + 4, data));

    // FIXME: Do not cast (uint64_t *) to (uint8_t *).
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[3], &data1, 4));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[10], &data2, 4));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&data[17], &data3, 4));
}

// TODO.
//TEST(flash_log, log_text)
//{
//}