/*
 * test_s25fl.c
 *
 *  Created on: Feb 23, 2020
 *      Author: Mikolaj Wielgus
 */

#include "unity_fixture.h"
#include "SS_flash.h"
#include "SS_s25fl.h"

// TODO: Use random values for write and read tests.

static volatile uint32_t page_size;
static uint8_t data[FLASH_PAGE_BUF_SIZE];

TEST_GROUP(s25fl);

TEST_GROUP_RUNNER(s25fl)
{
    page_size = SS_s25fl_get_page_size();

    RUN_TEST_CASE(s25fl, read_id);
	//RUN_TEST_CASE(s25fl, read_rems_id);
	RUN_TEST_CASE(s25fl, erase);
    //RUN_TEST_CASE(s25fl, erase_all);
	RUN_TEST_CASE(s25fl, wait);
	RUN_TEST_CASE(s25fl, write_read_byte);
	RUN_TEST_CASE(s25fl, write_read_byte_dma);
	RUN_TEST_CASE(s25fl, write_read_byte_dma_wait);
	RUN_TEST_CASE(s25fl, write_read_bytes);
	RUN_TEST_CASE(s25fl, write_read_bytes_dma);
	RUN_TEST_CASE(s25fl, write_read_bytes_dma_wait);
	RUN_TEST_CASE(s25fl, write_read_page);
	RUN_TEST_CASE(s25fl, write_read_page_dma);
    // TODO: Add `write_read_page_dma_wait` test case.
}

TEST_SETUP(s25fl)
{
}

TEST_TEAR_DOWN(s25fl)
{
}

TEST(s25fl, read_id)
{
    uint16_t id;
    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_id(&id));
    TEST_ASSERT_EQUAL_HEX16(0x0102, id);
}

/*TEST(s25fl, read_rems_id)
{
	uint16_t id;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_rems_id(&id));

	// We want to get a message if an untested chip is used.
	TEST_ASSERT_EQUAL_HEX16(0x0119, id);
}*/

/*TEST(s25fl, erase_all)
{
	static uint8_t data[page_size];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(0, data));

	if (data[0] == 0xFF) {
		data[0] = 0xAA;
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(0, data));
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(0, data));
		TEST_ASSERT_EQUAL_HEX8(0xAA, data[0]);
	}

	TEST_MESSAGE("Testing bulk erase. This may take up to 500 seconds...");
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_erase_all());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(0, data));
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[0]);
}*/

TEST(s25fl, erase)
{
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));

	if (data[1] == 0xFF) {
		data[1] = 0xAA;
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(1, data));
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));
		TEST_ASSERT_EQUAL_HEX8(0xAA, data[1]);
	}

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_erase_sector(0));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));
    uint32_t page_size = SS_s25fl_get_page_size();

    for (uint32_t i = 0; i < page_size; ++i) {
        TEST_ASSERT_EQUAL_HEX8(0xFF, data[i]);
    }
}

TEST(s25fl, wait)
{
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(0, data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	// Call a non-DMA write operation to make the module busy.
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(0,  data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());
}

TEST(s25fl, write_read_byte)
{
	static uint8_t write_data = 0xAB, read_data = 0x00;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(page_size*2 + 2, &read_data, 1));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes(page_size*2 + 2, &write_data, 1));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(page_size*2 + 2, &read_data, 1));

	TEST_ASSERT_EQUAL_HEX8(read_data, write_data);
}

TEST(s25fl, write_read_byte_dma)
{
	static uint8_t write_data = 0xCD, read_data = 0x00;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes_dma(page_size*3 + 3, &write_data, 1));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes_dma(page_size*3 + 3, &read_data, 1));

    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

    TEST_ASSERT_EQUAL_HEX8(read_data, write_data);
}

TEST(s25fl, write_read_byte_dma_wait)
{
	static uint8_t write_data = 0xEF, read_data = 0x00;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes_dma(page_size*4 + 4, &write_data, 1));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes_dma_wait(page_size*4 + 4, &read_data, 1));

    TEST_ASSERT_EQUAL_HEX8(read_data, write_data);
}

TEST(s25fl, write_read_bytes)
{
    static uint8_t write_data[FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < page_size; ++i) {
		write_data[i] = i;
	}

    static uint8_t read_data[FLASH_PAGE_BUF_SIZE];

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(page_size*5 + 5, read_data, 10));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes(page_size*5 + 5, write_data, 10));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(page_size*5 + 5, read_data, 10));

	for (uint32_t i = 0; i < 10; ++i) {
		TEST_ASSERT_EQUAL_HEX8(write_data[i], read_data[i]);
	}
}

TEST(s25fl, write_read_bytes_dma)
{
    static uint8_t write_data[FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < page_size; ++i) {
		write_data[i] = i;
	}

    static uint8_t read_data[FLASH_PAGE_BUF_SIZE];

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes_dma(page_size*6 + 6, write_data, 10));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes_dma(page_size*6 + 6, read_data, 10));

    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	for (uint32_t i = 0; i < 10; ++i) {
		TEST_ASSERT_EQUAL_HEX8(write_data[i], read_data[i]);
	}
}

TEST(s25fl, write_read_bytes_dma_wait)
{
    static uint8_t write_data[FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < page_size; ++i) {
		write_data[i] = i;
	}

    static uint8_t read_data[FLASH_PAGE_BUF_SIZE];

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes_dma(page_size*7 + 7, write_data, 10));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes_dma_wait(page_size*7 + 7, read_data, 10));

	for (uint32_t i = 0; i < 10; ++i) {
		TEST_ASSERT_EQUAL_HEX8(write_data[i], read_data[i]);
	}
}

TEST(s25fl, write_read_page)
{
	static uint8_t write_data[FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < page_size; ++i) {
		write_data[i] = i;
	}

	static uint8_t read_data[FLASH_PAGE_BUF_SIZE];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(8, read_data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(8, write_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(8, read_data));

	TEST_ASSERT_EQUAL_HEX8(write_data[4], read_data[4]);
}

TEST(s25fl, write_read_page_dma)
{
	static uint8_t write_data[FLASH_PAGE_BUF_SIZE];
	for (uint32_t i = 0; i < page_size; ++i) {
		write_data[i] = i;
	}

	static uint8_t read_data[FLASH_PAGE_BUF_SIZE];

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page_dma(9, write_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(9, read_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
    TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	for (uint32_t i = 0; i < page_size; ++i) {
		TEST_ASSERT_EQUAL_HEX8(write_data[i], read_data[i]);
	}
}
