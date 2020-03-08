/*
 * test_s25fl.c
 *
 *  Created on: Feb 23, 2020
 *      Author: Mikolaj Wielgus
 */

#include "unity.h"
#include "SS_s25fl.h"

static void test_s25fl_init(void);
static void test_s25fl_read_rems_id(void);
static void test_s25fl_wait_until_ready(void);
static void test_s25fl_wait_until_dma_ready(void);
static void test_s25fl_erase_all(void);
static void test_s25fl_erase_sector(void);
static void test_s25fl_write_bytes(void);
static void test_s25fl_write_bytes_dma(void);
static void test_s25fl_write_page(void);
static void test_s25fl_write_page_dma(void);
static void test_s25fl_read_bytes(void);
static void test_s25fl_read_bytes_dma(void);
static void test_s25fl_read_page(void);
static void test_s25fl_read_page_dma(void);
static void test_s25fl_get_status(void);
static void test_s25fl_get_is_dma_ready(void);

static void test_s25fl_init(void)
{
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_init());
	// Calling second time shall cause no errors.
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_init());
}

static void test_s25fl_read_rems_id(void)
{
	uint16_t id;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_rems_id(&id));

	// We want to get a message if an untested chip is used.
	TEST_ASSERT_EQUAL_HEX16(0x0119, id);
}

static void test_s25fl_wait_until_ready(void)
{
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());

	// Call a DMA operation to make the module busy.
	uint8_t data[S25FL_PAGE_SIZE] = {0};
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(0, data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	// Call a non-DMA write operation to make the module busy.
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(0,  data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());
}

static void test_s25fl_wait_until_dma_ready(void)
{
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_dma_ready());

	uint8_t data[S25FL_PAGE_SIZE] = {0};
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(0, data));

	TEST_ASSERT_EQUAL_INT(false, SS_s25fl_get_is_dma_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_dma_ready());
	TEST_ASSERT_EQUAL_INT(true, SS_s25fl_get_is_dma_ready());
}

static void test_s25fl_erase_all(void)
{
	uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(0, data));

	if (data[0] == 0xFF) {
		data[0] = 0xAA;
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(0, data));
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(0, data));
		TEST_ASSERT_EQUAL_HEX8(0xAA, data[0]);
	}

	TEST_MESSAGE("Testing bulk erase. This may take up to 500 seconds...");
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_erase_all());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(0, data));
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[0]);
}

static void test_s25fl_erase_sector(void)
{
	uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));

	if (data[1] == 0xFF) {
		data[1] = 0xAA;
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(1, data));
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));
		TEST_ASSERT_EQUAL_HEX8(0xAA, data[1]);
	}

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_erase_sector(0));

	// Erasure normally takes some time, so the module should be busy.
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[0]);
}

static void test_s25fl_write_bytes(void)
{
	uint8_t write_data = 0xAA, read_data;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*2 + 2, &read_data, 1));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes(S25FL_PAGE_SIZE*2 + 2, &write_data, 1));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*2 + 2, &read_data, 1));
	TEST_ASSERT_EQUAL_HEX8(read_data, write_data);
}

static void test_s25fl_write_bytes_dma(void)
{
	uint8_t write_data = 0xAA, read_data = 0x00;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*3 + 3, &read_data, 1));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes_dma(S25FL_PAGE_SIZE*3 + 3, &write_data, 1));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_dma_ready());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*3 + 3, &read_data, 1));

	// For some reason, DMA single-byte reading is completed too quickly to detect busy state.
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_dma_ready());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_HEX8(write_data, read_data);
}

static void test_s25fl_write_page(void)
{
	uint8_t write_data[S25FL_PAGE_SIZE];
	memset(write_data, 0xAA, S25FL_PAGE_SIZE);

	uint8_t read_data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(4, read_data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(4, write_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(4, read_data));
	TEST_ASSERT_EQUAL_HEX8(write_data[4], read_data[4]);
}

static void test_s25fl_write_page_dma(void)
{
	uint8_t write_data[S25FL_PAGE_SIZE];
	for (uint32_t i = 0; i < S25FL_PAGE_SIZE; ++i) {
		write_data[i] = i;
	}

	uint8_t read_data[S25FL_PAGE_SIZE];
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(5, read_data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page_dma(5, write_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(5, read_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	for (uint32_t i = 0; i < S25FL_PAGE_SIZE; ++i) {
		TEST_ASSERT_EQUAL_HEX8(write_data[i], read_data[i]);
	}
	//TEST_ASSERT_EQUAL_HEX8(write_data[5], read_data[5]);
}

static void test_s25fl_read_bytes(void)
{
	test_s25fl_write_bytes();
}

static void test_s25fl_read_bytes_dma(void)
{
	test_s25fl_write_bytes_dma();
}

static void test_s25fl_read_page(void)
{
	test_s25fl_write_page();
}

static void test_s25fl_read_page_dma(void)
{
	test_s25fl_write_page_dma();
}

static void test_s25fl_get_status(void)
{
	test_s25fl_wait_until_ready();
}

static void test_s25fl_get_is_dma_ready(void)
{
	test_s25fl_wait_until_dma_ready();
}

int test_s25fl(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_s25fl_init);
	RUN_TEST(test_s25fl_read_rems_id);
	RUN_TEST(test_s25fl_wait_until_ready);

	// We do not want these tests to run too often.

	//RUN_TEST(test_s25fl_erase_all);

	// Let's not erase a sector twice each time.
	// Erasing one time per test is too much already.
	//RUN_TEST(test_s25fl_erase_sector);

	RUN_TEST(test_s25fl_write_bytes);
	RUN_TEST(test_s25fl_write_bytes_dma);
	RUN_TEST(test_s25fl_write_page);
	RUN_TEST(test_s25fl_write_page_dma);
	RUN_TEST(test_s25fl_read_bytes);
	RUN_TEST(test_s25fl_read_bytes_dma);
	RUN_TEST(test_s25fl_read_page);
	RUN_TEST(test_s25fl_read_page_dma);

	return UNITY_END();
}
