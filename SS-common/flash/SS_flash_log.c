/**
  * SS_log_task.c
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

#include "SS_flash_log.h"
#include "SS_flash_lfs.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>

#define VARS_QUEUE_LEN 1024
#define TEXT_QUEUE_LEN 1024

typedef struct
{
    uint8_t id;
    uint8_t data[FLASH_LOG_MAX_VAR_DATA_SIZE];
    uint32_t size;
}Var;

static StaticQueue_t vars_static_queue;
static StaticQueue_t text_static_queue;

static uint8_t vars_queue_storage[VARS_QUEUE_LEN * sizeof(Var)];
static uint8_t text_queue_storage[TEXT_QUEUE_LEN * sizeof(char)];

// TODO: Support var and text queues for more than 2 streams.
static QueueHandle_t vars_queue;
static QueueHandle_t text_queue;

static lfs_file_t vars_file;
static lfs_file_t text_file;

FlashStatus SS_flash_log_init(void)
{
    vars_queue = xQueueCreateStatic(VARS_QUEUE_LEN, sizeof(Var), vars_queue_storage, &vars_static_queue);
    if (vars_queue == NULL) {
        return FLASH_STATUS_ERR;
    }

    text_queue = xQueueCreateStatic(TEXT_QUEUE_LEN, sizeof(char), text_queue_storage, &text_static_queue);
    if (text_queue == NULL) {
        return FLASH_STATUS_ERR;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_erase(void)
{
    lfs_t *lfs = SS_flash_lfs_get();

    FlashStatus status = SS_flash_log_start();
    if (status != FLASH_STATUS_OK) {
        return status;
    }

    int err = lfs_file_truncate(lfs, &vars_file, 0);
    if (err) {
        return FLASH_STATUS_ERR;
    }

    err = lfs_file_truncate(lfs, &text_file, 0);
    if (err) {
        return FLASH_STATUS_ERR;
    }

    status = SS_flash_log_stop();
    if (status != FLASH_STATUS_OK) {
        return status;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_start(void)
{
    lfs_t *lfs = SS_flash_lfs_get();

    static struct lfs_file_config vars_cfg;
    static uint8_t vars_buf[FLASH_PAGE_BUF_SIZE];
    vars_cfg.buffer = vars_buf;
    vars_cfg.attr_count = 0;

    int err = lfs_file_opencfg(lfs, &vars_file, "vars.bin", LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND, &vars_cfg);
    if (err) {
        return FLASH_STATUS_ERR;
    }

    static struct lfs_file_config text_cfg;
    static uint8_t text_buf[FLASH_PAGE_BUF_SIZE];
    text_cfg.buffer = text_buf;
    text_cfg.attr_count = 0;

    err = lfs_file_opencfg(lfs, &text_file, "text.txt", LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND, &text_cfg);
    if (err) {
        return FLASH_STATUS_ERR;
    }
    
    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_stop(void)
{
    lfs_t *lfs = SS_flash_lfs_get();
    Var var;
    char c;

    while (xQueueReceive(vars_queue, &var, pdMS_TO_TICKS(1)) == pdTRUE) {
        lfs_file_write(lfs, &vars_file, &var.id, sizeof(var.id));
        lfs_file_write(lfs, &vars_file, var.data, var.size);
    }

    while (xQueueReceive(text_queue, &c, pdMS_TO_TICKS(1)) == pdTRUE) {
        lfs_file_write(lfs, &text_file, &c, sizeof(c));
    }

    if (lfs_file_close(lfs, &vars_file)) {
        return FLASH_STATUS_ERR;
    }

    if (lfs_file_close(lfs, &text_file)) {
        return FLASH_STATUS_ERR;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_var(uint8_t id, uint8_t *data, uint32_t size)
{
    Var var;
    var.id = id;
    var.size = size;
    memcpy(var.data, data, size);

    if (!xQueueSend(vars_queue, &var, pdMS_TO_TICKS(10))) {
        return FLASH_STATUS_BUSY;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_log_text(const char *str)
{
    for (uint32_t i = 0; str[i] != '\0'; ++i) {
        if (!xQueueSend(text_queue, &str[i], pdMS_TO_TICKS(1))) {
            return FLASH_STATUS_BUSY;
        }
    }

    return FLASH_STATUS_OK;
}

void SS_flash_log_task(void *pvParameters)
{
    lfs_t *lfs = SS_flash_lfs_get();
    Var var;
    char c;

    while (true) {
        if (xQueueReceive(vars_queue, &var, pdMS_TO_TICKS(1)) == pdTRUE) {
            lfs_file_write(lfs, &vars_file, &var.id, sizeof(var.id));
            lfs_file_write(lfs, &vars_file, var.data, var.size);
        }

        if (xQueueReceive(text_queue, &c, pdMS_TO_TICKS(1)) == pdTRUE) {
            lfs_file_write(lfs, &text_file, &c, sizeof(c));
        }
    }
}
