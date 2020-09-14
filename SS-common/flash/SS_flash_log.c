/**
  * SS_log_task.c
  *
  *  Created on: May 24, 2020
  *      Author: Mikolaj Wielgus
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_flash_log.h"

#include "FreeRTOS.h"
#include "SS_flash_lfs.h"
#include "SS_s25fl.h"
#include "SS_usb.h"
#include "semphr.h"
#include "string.h"

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define VARS_QUEUE_LEN 1048

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct
{
    uint8_t id;
    uint8_t data[FLASH_LOG_MAX_VAR_DATA_SIZE];
    uint32_t size;
} Var;

typedef struct FlashLogStream {
    QueueHandle_t queue;
    char *filename;
    uint16_t item_size;

    lfs_file_t file;
    struct lfs_file_config cfg;
    uint8_t buf[FLASH_PAGE_BUF_SIZE];
    bool is_logging;
    struct FlashLogStream *next;
} FlashLogStream;

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static FlashLogStream *flash_log_streams;

static FlashLogStream vars_stream = {
    .filename = FLASH_LOG_VARS_FILENAME,
    .item_size = sizeof(Var)
};

static FlashLogStream text_stream = {
    .filename = FLASH_LOG_TEXT_FILENAME,
    .item_size = sizeof(char)
};


/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static FlashLogStream *SS_flash_stream_get(char *filename);
static void SS_flash_stream_init(FlashLogStream *stream);
static FlashStatus SS_flash_stream_log(char *filename, void *data);
static void SS_flash_stream_log_fromISR(char *filename, void *data);
static void SS_flash_log_task(void *pvParameters);

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

/* TODO move init to a better location */
void SS_flash_create_tasks(UBaseType_t priority) {
    BaseType_t res = xTaskCreate(SS_flash_log_task, "Flash Var Stream Task", 256, &vars_stream, priority, NULL);
    assert(res == pdTRUE);
    res = xTaskCreate(SS_flash_log_task, "Flash Text Stream Task", 256, &text_stream, priority, NULL);
    assert(res == pdTRUE);
}

FlashStatus SS_flash_stream_start(char *filename) {
    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    UsbStatus usb_status = SS_usb_stop();
    if(usb_status != USB_STATUS_OK && usb_status != USB_STATUS_DISABLED) {
        return FLASH_STATUS_ERR;
    }

    if(SS_flash_lfs_start() != FLASH_STATUS_OK) {
        return FLASH_STATUS_ERR;
    }

    lfs_t *lfs = SS_flash_lfs_get();

    stream->cfg.buffer = stream->buf;
    stream->cfg.attr_count = 0;

    if(lfs_file_opencfg(lfs, &stream->file, filename, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND, &stream->cfg)) {
        return FLASH_STATUS_ERR;
    }
    stream->is_logging = true;

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_stream_stop(char *filename) {
    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    stream->is_logging = false;

    lfs_t *lfs = SS_flash_lfs_get();

    while(uxQueueSpacesAvailable(stream->queue) < VARS_QUEUE_LEN) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    if(lfs_file_close(lfs, &stream->file)) {
        return FLASH_STATUS_ERR;
    }

    FlashLogStream *s = flash_log_streams;
    while(s != NULL) {
        if(s->is_logging) {
            return FLASH_STATUS_OK;
        }
        s = s->next;
    }
    /* If none of the streams are logging start usb */
    if(SS_usb_start() != USB_STATUS_OK) {
        return FLASH_STATUS_ERR;
    }

    SS_println("usb start");
    return FLASH_STATUS_OK;
}

/* Return true if logging */
bool SS_flash_stream_toggle(char *filename) {
    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    if(stream->is_logging) {
        SS_flash_stream_stop(filename);
        return false;
    } else {
        SS_flash_stream_start(filename);
        return true;
    }
}

FlashStatus SS_flash_stream_erase(char *filename) {
    lfs_t *lfs = SS_flash_lfs_get();

    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    /* Why stary before erase? */
    FlashStatus status = SS_flash_stream_start(filename);
    assert(stream != NULL);
    if(status != FLASH_STATUS_OK) {
        return status;
    }

    int err = lfs_file_truncate(lfs, &stream->file, 0);
    if(err) {
        return FLASH_STATUS_ERR;
    }

    status = SS_flash_stream_stop(filename);
    if(status != FLASH_STATUS_OK) {
        return status;
    }

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_stream_erase_all(void) {
    FlashLogStream *stream = flash_log_streams;
    while(stream != NULL) {
        FlashStatus res;
        if((res = SS_flash_stream_erase(stream->filename) != FLASH_STATUS_OK)) {
            return res;
        }
        stream = stream->next;
    }
    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_stream_log_var(uint8_t id, uint8_t *data, uint32_t size) {
    Var var;
    var.id = id;
    var.size = size;
    memcpy(var.data, data, size);
    return SS_flash_stream_log(vars_stream.filename, &var);
}

void SS_flash_stream_log_var_fromISR(uint8_t id, uint8_t *data, uint32_t size) {
    Var var;
    var.id = id;
    var.size = size;
    memcpy(var.data, data, size);
    SS_flash_stream_log_fromISR(vars_stream.filename, &var);
}

/* FlashStatus SS_flash_log_bytes(const char *str, uint16_t size) */
/* { */
/*     if (!is_logging) { */
/*         return FLASH_STATUS_DISABLED; */
/*     } */

/*     for (uint32_t i = 0; i < size; ++i) { */
/*         if (!xQueueSend(text_queue, &str[i], pdMS_TO_TICKS(1))) { */
/*             return FLASH_STATUS_BUSY; */
/*         } */
/*     } */

/*     return FLASH_STATUS_OK; */
/* } */

/* FlashStatus SS_flash_log_text(const char *str) */
/* { */
/*     if (!is_logging) { */
/*         return FLASH_STATUS_DISABLED; */
/*     } */

/* for (uint32_t i = 0; str[i] != '\0'; ++i) { */
/*     if (!xQueueSend(text_queue, &str[i], pdMS_TO_TICKS(1))) { */
/*         return FLASH_STATUS_BUSY; */
/*     } */
/* } */

/* return FLASH_STATUS_OK; */
/* } */

/* bool SS_flash_log_get_is_logging(void) */
/* { */
/*     return true; */
/* } */

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static FlashLogStream *SS_flash_stream_get(char *filename) {
    FlashLogStream *stream = flash_log_streams;
    while(stream != NULL) {
        if(strcmp(stream->filename, filename) == 0) {
            return stream;
        }
        stream = stream->next;
    }
    return NULL;
}

static void SS_flash_stream_init(FlashLogStream *stream) {
    stream->queue = xQueueCreate(VARS_QUEUE_LEN, stream->item_size);
    assert(stream->queue != NULL);
    if(flash_log_streams == NULL) {
        flash_log_streams = stream;
    }
    FlashLogStream *s = flash_log_streams;
    while(s->next != NULL) {
        s = s->next;
    }
    s->next = stream;
    stream->next = NULL;
}

/* Ensure data size is equal to the queue item size */
static void SS_flash_stream_log_fromISR(char *filename, void *data) {
    FlashLogStream *stream = SS_flash_stream_get(filename);

    if(!stream->is_logging) {
        return;
    }

    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(stream->queue, data, &pxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

/* Ensure data size is equal to the queue item size */
static FlashStatus SS_flash_stream_log(char *filename, void *data) {
    FlashLogStream *stream = SS_flash_stream_get(filename);

    if(!stream->is_logging) {
        return FLASH_STATUS_DISABLED;
    }

    if(!xQueueSend(stream->queue, data, pdMS_TO_TICKS(1))) {
        return FLASH_STATUS_BUSY;
    }
    return FLASH_STATUS_OK;
}

static void SS_flash_log_task(void *pvParameters) {
    FlashLogStream *stream = (FlashLogStream *) pvParameters;
    SS_flash_stream_init(stream);
    lfs_t *lfs = SS_flash_lfs_get();
    uint8_t buf[stream->item_size];
    uint32_t sent_bytes = 0;
    while(true) {
        /* TODO add mutex */
        if(xQueueReceive(stream->queue, buf, pdMS_TO_TICKS(portMAX_DELAY))) {
            lfs_file_write(lfs, &stream->file, buf, stream->item_size);
            sent_bytes += stream->item_size;
            if(sent_bytes >= SS_s25fl_get_sector_size()) {
                lfs_file_sync(lfs, &stream->file);
                sent_bytes = 0;
            }
        }
    }
}

