/*
 * test_s25fl.c
 *
 *  Created on: Feb 23, 2020
 *      Author: Mikolaj Wielgus
 */

#include "unity_fixture.h"
#include "SS_s25fl.h"

TEST_GROUP(s25fl);

TEST_GROUP_RUNNER(s25fl)
{
	RUN_TEST_CASE(s25fl, rems_id);
	//RUN_TEST_CASE(s25fl, erase_all);
	RUN_TEST_CASE(s25fl, erase);
	RUN_TEST_CASE(s25fl, wait);
	//RUN_TEST_CASE(s25fl, wait_dma);
	RUN_TEST_CASE(s25fl, write_read_bytes);
	RUN_TEST_CASE(s25fl, write_read_bytes_dma);
	RUN_TEST_CASE(s25fl, write_read_page);
	RUN_TEST_CASE(s25fl, write_read_page_dma);
}

TEST_SETUP(s25fl)
{
	SS_s25fl_init();
}

TEST_TEAR_DOWN(s25fl)
{
}

TEST(s25fl, rems_id)
{
	uint16_t id;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_rems_id(&id));

	// We want to get a message if an untested chip is used.
	TEST_ASSERT_EQUAL_HEX16(0x0119, id);
}

TEST(s25fl, erase_all)
{
	static uint8_t data[S25FL_PAGE_SIZE];
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

TEST(s25fl, erase)
{
	static uint8_t data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));

	if (data[1] == 0xFF) {
		data[1] = 0xAA;
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(1, data));
		TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));
		TEST_ASSERT_EQUAL_HEX8(0xAA, data[1]);
	}

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_erase_sector(0));

	// Erasure normally takes some time, so the module should be busy.
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());
    
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(1, data));
	TEST_ASSERT_EQUAL_HEX8(0xFF, data[0]);
}

TEST(s25fl, wait)
{
	// Call a DMA operation to make the module busy. XXX: Why DMA?
	static uint8_t data[S25FL_PAGE_SIZE] = {0};
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(0, data));

	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	// Call a non-DMA write operation to make the module busy.
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(0,  data));

	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());
}

/*TEST(s25fl, wait_dma)
{
	uint8_t data[S25FL_PAGE_SIZE] = {0};
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(0, data));

	TEST_ASSERT_EQUAL_INT(false, SS_s25fl_get_is_dma_ready());
	TEST_ASSERT_EQUAL_INT(true, SS_s25fl_get_is_dma_ready());
}*/

TEST(s25fl, write_read_bytes)
{
	static uint8_t write_data = 0xAA, read_data = 0x00;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*2 + 2, &read_data, 1));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes(S25FL_PAGE_SIZE*2 + 2, &write_data, 1));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*2 + 2, &read_data, 1));
	TEST_ASSERT_EQUAL_HEX8(read_data, write_data);

	write_data = 0xAA;
	read_data = 0x00;
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*3 + 3, &read_data, 1));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_bytes_dma(S25FL_PAGE_SIZE*3 + 3, &write_data, 1));
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_dma_ready());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_bytes(S25FL_PAGE_SIZE*3 + 3, &read_data, 1));

	// For some reason, DMA single-byte reading is completed too quickly to detect busy state.
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_dma_ready());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_HEX8(write_data, read_data);
}

TEST(s25fl, write_read_bytes_dma)
{
	// TODO.
}

TEST(s25fl, write_read_page)
{
	static uint8_t write_data[S25FL_PAGE_SIZE];
	memset(write_data, 0xAA, S25FL_PAGE_SIZE);

	static uint8_t read_data[S25FL_PAGE_SIZE];
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(4, read_data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page(4, write_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(4, read_data));
	TEST_ASSERT_EQUAL_HEX8(write_data[4], read_data[4]);
}

TEST(s25fl, write_read_page_dma)
{
	static uint8_t write_data[S25FL_PAGE_SIZE];
	for (uint32_t i = 0; i < S25FL_PAGE_SIZE; ++i) {
		write_data[i] = i;
	}

	static uint8_t read_data[S25FL_PAGE_SIZE];
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page(5, read_data));

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_write_page_dma(5, write_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_read_page_dma(5, read_data));
	TEST_ASSERT_EQUAL_INT(S25FL_STATUS_BUSY, SS_s25fl_get_status());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_wait_until_ready());
	//TEST_ASSERT_EQUAL_INT(S25FL_STATUS_OK, SS_s25fl_get_status());

	for (uint32_t i = 0; i < S25FL_PAGE_SIZE; ++i) {
		TEST_ASSERT_EQUAL_HEX8(write_data[i], read_data[i]);
	}
	//TEST_ASSERT_EQUAL_HEX8(write_data[5], read_data[5]);
}
