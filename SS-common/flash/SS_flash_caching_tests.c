/*
 * SS_flash_caching_tests.c
 *
 *  Created on: Mar 25, 2020
 *      Author: Mikolaj Wielgus
 */

#include "SS_flash_caching.h"
#include "unity_fixture.h"
#include "SS_s25fl.h"
#include "SS_flash_ctrl.h"

TEST_GROUP(flash_caching);

TEST_GROUP_RUNNER(flash_caching)
{
	RUN_TEST_CASE(flash_caching, start_stop);
	RUN_TEST_CASE(flash_caching, read_write_page);
	RUN_TEST_CASE(flash_caching, read_write_pages1);
	RUN_TEST_CASE(flash_caching, read_write_pages2);
}

TEST_SETUP(flash_caching)
{
	FlashStatus status = SS_flash_caching_start();
	TEST_ASSERT_TRUE(status == FLASH_STATUS_OK || status == FLASH_STATUS_DISABLED);
}

TEST_TEAR_DOWN(flash_caching)
{
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_stop());
}

TEST(flash_caching, start_stop)
{
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_DISABLED, SS_flash_caching_start());
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_stop());
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_DISABLED, SS_flash_caching_stop());
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_start());
}

TEST(flash_caching, read_write_page)
{
	static uint8_t data1[FLASH_PAGE_BUF_SIZE], data2[FLASH_PAGE_BUF_SIZE], data3[FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < FLASH_PAGE_BUF_SIZE; ++i) {
		data1[i] = i;
		data2[FLASH_PAGE_BUF_SIZE-1-i] = i*2;
	}

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_set_is_emulating(false));

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(30, 1, data1));
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(31, 1, data2));
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(32, 1, data3));

	static uint8_t data[FLASH_PAGE_BUF_SIZE];
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(30, 1, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data1, FLASH_PAGE_BUF_SIZE));

	HAL_Delay(10);
	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(31, SS_flash_caching_debug_get_cached_page());


	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(31, 1, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data2, FLASH_PAGE_BUF_SIZE));

	HAL_Delay(10);
	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(32, SS_flash_caching_debug_get_cached_page());


	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(32, 1, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data3, FLASH_PAGE_BUF_SIZE));

	HAL_Delay(10);
	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(33, SS_flash_caching_debug_get_cached_page());

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_set_is_emulating(true));
}

TEST(flash_caching, read_write_pages1)
{
	static uint8_t data1[FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN];
	static uint8_t data2[FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN];
	for (uint32_t i = 0; i < FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN; ++i) {
		data1[i] = i;
		data2[FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN-1-i] = i*2;
	}

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_set_is_emulating(false));

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(33, FLASH_CACHING_MAX_CACHE_LEN, data1));
	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(33+FLASH_CACHING_MAX_CACHE_LEN, FLASH_CACHING_MAX_CACHE_LEN, data2));
	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());


	static uint8_t data[FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN];
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(33, FLASH_CACHING_MAX_CACHE_LEN, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data1, FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN));

	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(33+FLASH_CACHING_MAX_CACHE_LEN, SS_flash_caching_debug_get_cached_page());


	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(33+FLASH_CACHING_MAX_CACHE_LEN, FLASH_CACHING_MAX_CACHE_LEN, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data2, FLASH_PAGE_BUF_SIZE*FLASH_CACHING_MAX_CACHE_LEN));

	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(33+2*FLASH_CACHING_MAX_CACHE_LEN, SS_flash_caching_debug_get_cached_page());

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_set_is_emulating(true));
}

TEST(flash_caching, read_write_pages2)
{
#define LEN (3*FLASH_CACHING_MAX_CACHE_LEN)
	static uint8_t data1[LEN*FLASH_PAGE_BUF_SIZE];
	static uint8_t data2[LEN*FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < LEN*FLASH_PAGE_BUF_SIZE; ++i) {
		data1[i] = i;
		data2[LEN-1-i] = i*2;
	}

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_set_is_emulating(false));

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(40, LEN, data1));
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_write_pages(40+LEN, LEN, data2));


	static uint8_t data[LEN*FLASH_PAGE_BUF_SIZE];
	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(40, LEN, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data1, LEN*FLASH_PAGE_BUF_SIZE));

	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(40+LEN, SS_flash_caching_debug_get_cached_page());


	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_caching_read_pages(40+LEN, LEN, data));
	TEST_ASSERT_EQUAL_INT(0, memcmp(data, data2, LEN*FLASH_PAGE_BUF_SIZE));

	TEST_ASSERT_TRUE(SS_flash_caching_debug_get_is_cache_ready());
	TEST_ASSERT_EQUAL_INT(40+2*LEN, SS_flash_caching_debug_get_cached_page());

	TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_ctrl_set_is_emulating(true));
#undef LEN
}
