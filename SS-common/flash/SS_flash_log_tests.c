/**
  * SS_flash_log_tests.c
  *
  *  Created on: May 25, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_s25fl.h"
#include "SS_flash_log.h"
#include "SS_flash_lfs.h"

#include "unity_fixture.h"
#include "FreeRTOS.h"
#include "semphr.h"

static int open_file(lfs_file_t *file, struct lfs_file_config *cfg, uint8_t *buf, const char *path, int flags);

static uint32_t page_size;

TEST_GROUP(flash_log);

TEST_GROUP_RUNNER(flash_log)
{
    page_size = SS_s25fl_get_page_size();
    assert(SS_flash_lfs_format_and_remount() != FLASH_STATUS_ERR);

    SS_flash_stream_erase_all();

    RUN_TEST_CASE(flash_log, log_vars);
    RUN_TEST_CASE(flash_log, log_text);
    RUN_TEST_CASE(flash_log, reset_log_vars);
    RUN_TEST_CASE(flash_log, reset_log_text);
}

TEST_SETUP(flash_log)
{
}

TEST_TEAR_DOWN(flash_log)
{
}

TEST(flash_log, log_vars)
{
    static lfs_file_t vars_file;
    static struct lfs_file_config vars_cfg;
    static uint8_t vars_buf[FLASH_PAGE_BUF_SIZE];
    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&vars_file, &vars_cfg, vars_buf, FLASH_LOG_VARS_FILENAME, LFS_O_RDONLY));

    lfs_t *lfs = SS_flash_lfs_get();
    lfs_soff_t prev_size = lfs_file_size(lfs, &vars_file);
    TEST_ASSERT_GREATER_OR_EQUAL(0, prev_size);

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &vars_file));

    static uint8_t id1 = 0x01, id2 = 0x02, id3 = 0x03;
    static uint8_t data1[4] = {0x12, 0x34, 0x56, 0x78};
    static uint8_t data2[4] = {0x9A, 0xBC, 0xDE, 0x00};
    static uint8_t data3[4] = {0x11, 0x22, 0x33, 0x44};
    static uint8_t data[FLASH_LOG_MAX_VAR_DATA_SIZE];

    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_start(FLASH_LOG_VARS_FILENAME));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(id1, data1, sizeof(data1)));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(id2, data2, sizeof(data2)));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(id3, data3, sizeof(data3)));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_stop(FLASH_LOG_VARS_FILENAME));

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&vars_file, &vars_cfg, vars_buf, FLASH_LOG_VARS_FILENAME, LFS_O_RDONLY));

    TEST_ASSERT_EQUAL_INT(1, lfs_file_read(lfs, &vars_file, data, 1));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&id1, data, 1);
    TEST_ASSERT_EQUAL_INT(4, lfs_file_read(lfs, &vars_file, data, 4));

    TEST_ASSERT_EQUAL_INT(sizeof(data1), lfs_file_read(lfs, &vars_file, data, sizeof(data1)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data1, data, sizeof(data1));

    TEST_ASSERT_EQUAL_INT(1, lfs_file_read(lfs, &vars_file, data, 1));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&id2, data, 1);
    TEST_ASSERT_EQUAL_INT(4, lfs_file_read(lfs, &vars_file, data, 4));

    TEST_ASSERT_EQUAL_INT(sizeof(data2), lfs_file_read(lfs, &vars_file, data, sizeof(data2)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data2, data, sizeof(data2));

    TEST_ASSERT_EQUAL_INT(1, lfs_file_read(lfs, &vars_file, data, 1));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&id3, data, 1);
    TEST_ASSERT_EQUAL_INT(4, lfs_file_read(lfs, &vars_file, data, 4));

    TEST_ASSERT_EQUAL_INT(sizeof(data3), lfs_file_read(lfs, &vars_file, data, sizeof(data3)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data3, data, sizeof(data3));

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &vars_file));
}

TEST(flash_log, log_text)
{
    static lfs_file_t text_file;
    static struct lfs_file_config text_cfg;
    static uint8_t text_buf[FLASH_PAGE_BUF_SIZE];
    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&text_file, &text_cfg, text_buf, FLASH_LOG_TEXT_FILENAME, LFS_O_RDONLY));

    lfs_t *lfs = SS_flash_lfs_get();
    lfs_soff_t prev_size = lfs_file_size(lfs, &text_file);
    TEST_ASSERT_GREATER_OR_EQUAL(0, prev_size);

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &text_file));

    static const char str1[] = "Never gonna give you up\r\n";
    static const char str2[] = "Never gonna let you down\r\n";
    static const char str3[] = "Never gonna run around and desert you\r\n";
    static char data[FLASH_PAGE_BUF_SIZE];

    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_start(FLASH_LOG_TEXT_FILENAME));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text(str1));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text(str2));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text(str3));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_stop(FLASH_LOG_TEXT_FILENAME));

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&text_file, &text_cfg, text_buf, FLASH_LOG_TEXT_FILENAME, LFS_O_RDONLY));

    TEST_ASSERT_GREATER_OR_EQUAL(0, lfs_file_seek(lfs, &text_file, prev_size, LFS_SEEK_SET));

    TEST_ASSERT_EQUAL_INT(strlen(str1), lfs_file_read(lfs, &text_file, data, strlen(str1)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, str1, strlen(str1)));

    TEST_ASSERT_EQUAL_INT(strlen(str2), lfs_file_read(lfs, &text_file, data, strlen(str2)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, str2, strlen(str2)));

    TEST_ASSERT_EQUAL_INT(strlen(str3), lfs_file_read(lfs, &text_file, data, strlen(str3)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, str3, strlen(str3)));

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &text_file));
}

TEST(flash_log, reset_log_vars)
{
    static lfs_file_t vars_file;
    static struct lfs_file_config vars_cfg;
    static uint8_t vars_buf[FLASH_PAGE_BUF_SIZE];
    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&vars_file, &vars_cfg, vars_buf, FLASH_LOG_VARS_FILENAME, LFS_O_RDONLY));

    lfs_t *lfs = SS_flash_lfs_get();
    lfs_soff_t prev_size = lfs_file_size(lfs, &vars_file);
    TEST_ASSERT_GREATER_OR_EQUAL(0, prev_size);

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &vars_file));

    static uint8_t id1 = 0x01, id2 = 0x02, id3 = 0x03;
    static uint8_t data1[4] = {0x12, 0x34, 0x56, 0x78};
    static uint8_t data2[4] = {0x9A, 0xBC, 0xDE, 0x00};
    static uint8_t data3[4] = {0x11, 0x22, 0x33, 0x44};
    static uint8_t data[FLASH_LOG_MAX_VAR_DATA_SIZE];

    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_start(FLASH_LOG_VARS_FILENAME));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(id1, data1, sizeof(data1)));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(id2, data2, sizeof(data2)));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(id3, data3, sizeof(data3)));


    // Log full sector to force flushing
    for (uint32_t i = 0; i < SS_s25fl_get_sector_size()/2; ++i) {
        uint8_t c = 'X';
        TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_var(0x01, &c, sizeof(c)));
    }


    // Simulate reset.
    // Useless at the moment.
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_lfs_init());

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&vars_file, &vars_cfg, vars_buf, FLASH_LOG_VARS_FILENAME, LFS_O_RDONLY));

    TEST_ASSERT_EQUAL_INT(1, lfs_file_read(lfs, &vars_file, data, 1));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, &id1, 1));
    TEST_ASSERT_EQUAL_INT(4, lfs_file_read(lfs, &vars_file, data, 4));

    TEST_ASSERT_EQUAL_INT(sizeof(data1), lfs_file_read(lfs, &vars_file, data, sizeof(data1)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data1, sizeof(data1)));

    TEST_ASSERT_EQUAL_INT(1, lfs_file_read(lfs, &vars_file, data, 1));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, &id2, 1));
    TEST_ASSERT_EQUAL_INT(4, lfs_file_read(lfs, &vars_file, data, 4));

    TEST_ASSERT_EQUAL_INT(sizeof(data2), lfs_file_read(lfs, &vars_file, data, sizeof(data2)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data2, sizeof(data2)));

    TEST_ASSERT_EQUAL_INT(1, lfs_file_read(lfs, &vars_file, data, 1));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, &id3, 1));
    TEST_ASSERT_EQUAL_INT(4, lfs_file_read(lfs, &vars_file, data, 4));

    TEST_ASSERT_EQUAL_INT(sizeof(data3), lfs_file_read(lfs, &vars_file, data, sizeof(data3)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, data3, sizeof(data3)));

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &vars_file));

    // Stop after the entire test to maintain state consistency.
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_stop(FLASH_LOG_VARS_FILENAME));
}

TEST(flash_log, reset_log_text)
{
    static const char str1[] = "Never gonna make you cry\r\n";
    static const char str2[] = "Never gonna say goodbye\r\n";
    static const char str3[] = "Never gonna tell a lie and hurt you\r\n";
    static char data[FLASH_PAGE_BUF_SIZE];

    static lfs_file_t text_file;
    static struct lfs_file_config text_cfg;
    static uint8_t text_buf[FLASH_PAGE_BUF_SIZE];
    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&text_file, &text_cfg, text_buf, FLASH_LOG_TEXT_FILENAME, LFS_O_RDONLY));

    lfs_t *lfs = SS_flash_lfs_get();
    lfs_soff_t prev_size = lfs_file_size(lfs, &text_file);
    TEST_ASSERT_GREATER_OR_EQUAL(0, prev_size);

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &text_file));

    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_start(FLASH_LOG_TEXT_FILENAME));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text(str1));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text(str2));
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text(str3));

    // Log full sector bytes to force flushing.
    for (uint32_t i = 0; i < SS_s25fl_get_sector_size(); ++i) {
        TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_log_text("X"));
    }

    // Simulate reset.
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_lfs_init());
    
    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, open_file(&text_file, &text_cfg, text_buf, FLASH_LOG_TEXT_FILENAME, LFS_O_RDONLY));

    TEST_ASSERT_GREATER_OR_EQUAL(0, lfs_file_seek(lfs, &text_file, prev_size, LFS_SEEK_SET));

    TEST_ASSERT_EQUAL_INT(strlen(str1), lfs_file_read(lfs, &text_file, data, strlen(str1)));
    data[strlen(str1)] = '\0';
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, str1, strlen(str1)));

    TEST_ASSERT_EQUAL_INT(strlen(str2), lfs_file_read(lfs, &text_file, data, strlen(str2)));
    data[strlen(str2)] = '\0';
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, str2, strlen(str2)));

    TEST_ASSERT_EQUAL_INT(strlen(str3), lfs_file_read(lfs, &text_file, data, strlen(str3)));
    data[strlen(str3)] = '\0';
    TEST_ASSERT_EQUAL_INT(0, memcmp(data, str3, strlen(str3)));

    TEST_ASSERT_EQUAL_INT(LFS_ERR_OK, lfs_file_close(lfs, &text_file));
    
    // Stop after the entire test to maintain state consistency.
    TEST_ASSERT_EQUAL_INT(FLASH_STATUS_OK, SS_flash_stream_stop(FLASH_LOG_TEXT_FILENAME));
}

static int open_file(lfs_file_t *file, struct lfs_file_config *cfg, uint8_t *buf, const char *path, int flags)
{
    cfg->buffer = buf;
    cfg->attr_count = 0;

    lfs_t *lfs = SS_flash_lfs_get();
    return lfs_file_opencfg(lfs, file, path, flags, cfg);
}
