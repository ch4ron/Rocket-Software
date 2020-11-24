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
#include "SS_flash_ctrl.h"

#include "FreeRTOS.h"
#include "SS_flash_lfs.h"
#include "SS_s25fl.h"
/* #include "SS_usb.h" */
#include "semphr.h"
#include "string.h"
#include "usart.h"

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define VARS_QUEUE_LEN 512

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct __attribute__((packed)) {
    uint8_t id;
    uint32_t timestamp;
    uint8_t data[FLASH_LOG_MAX_VAR_DATA_SIZE];
} StreamVar;

typedef struct {
    uint8_t data[sizeof(StreamVar)];
    uint16_t size;
} StreamElement;

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

FlashLogStream vars_stream = {
    .filename = FLASH_LOG_VARS_FILENAME,
};

FlashLogStream text_stream = {
    .filename = FLASH_LOG_TEXT_FILENAME,
};

static volatile uint32_t flash_timer;


/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static FlashLogStream *SS_flash_stream_get(char *filename);
static void SS_flash_stream_init(FlashLogStream *stream);
static FlashStatus SS_flash_stream_log(char *filename, void *data, uint16_t size);
static void SS_flash_stream_log_fromISR(char *filename, void *data, uint16_t size);
static void SS_flash_log_task(void *pvParameters);

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

/* TODO move init to a better location */
void SS_flash_create_tasks(UBaseType_t priority) {
    BaseType_t res = xTaskCreate(SS_flash_log_task, "Flash Var Stream Task", 256, &vars_stream, priority, NULL);
    assert(res == pdTRUE);
    //res = xTaskCreate(SS_flash_log_task, "Flash Text Stream Task", 256, &text_stream, priority, NULL);
    assert(res == pdTRUE);
}

FlashStatus SS_flash_stream_start(char *filename) {
    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    /* UsbStatus usb_status = SS_usb_stop(); */
    /* if(usb_status != USB_STATUS_OK && usb_status != USB_STATUS_DISABLED) { */
    /*     return FLASH_STATUS_ERR; */
    /* } */

    /* if(SS_flash_lfs_start() != FLASH_STATUS_OK) { */
    /*     return FLASH_STATUS_ERR; */
    /* } */

    /* lfs_t *lfs = SS_flash_lfs_get(); */

    stream->cfg.buffer = stream->buf;
    stream->cfg.attr_count = 0;

    /* if(lfs_file_opencfg(lfs, &stream->file, filename, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND, &stream->cfg)) { */
    /*     return FLASH_STATUS_ERR; */
    /* } */
    stream->is_logging = true;

    return FLASH_STATUS_OK;
}

FlashStatus SS_flash_stream_stop(char *filename) {
    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    stream->is_logging = false;

    /* lfs_t *lfs = SS_flash_lfs_get(); */

    while(uxQueueSpacesAvailable(stream->queue) < VARS_QUEUE_LEN) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    /* if(lfs_file_close(lfs, &stream->file)) { */
    /*     return FLASH_STATUS_ERR; */
    /* } */

    FlashLogStream *s = flash_log_streams;
    while(s != NULL) {
        if(s->is_logging) {
            return FLASH_STATUS_OK;
        }
        s = s->next;
    }
    /* If none of the streams are logging start usb */
    /* if(SS_usb_start() != USB_STATUS_OK) { */
    /*     return FLASH_STATUS_ERR; */
    /* } */
    /* return FLASH_STATUS_OK; */
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
    /* lfs_t *lfs = SS_flash_lfs_get(); */

    FlashLogStream *stream = SS_flash_stream_get(filename);
    assert(stream != NULL);
    /* Why stary before erase? */
    FlashStatus status = SS_flash_stream_start(filename);
    assert(stream != NULL);
    if(status != FLASH_STATUS_OK) {
        return status;
    }
    SS_flash_ctrl_erase_logs();

    /* int err = lfs_file_truncate(lfs, &stream->file, 0); */
    /* if(err) { */
    /*     return FLASH_STATUS_ERR; */
    /* } */

    /* status = SS_flash_stream_stop(filename); */
    /* if(status != FLASH_STATUS_OK) { */
    /*     return status; */
    /* } */

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

FlashStatus SS_flash_log_var(uint8_t id, uint8_t *data, uint16_t size) {
    static StreamVar var;
    var.id = id;
    var.timestamp = flash_timer;
    memcpy(var.data, data, size);
    return SS_flash_stream_log(vars_stream.filename, &var, size + sizeof(var) - sizeof(var.data));
}

void SS_flash_log_var_fromISR(uint8_t id, uint8_t *data, uint16_t size) {
    static StreamVar var;
    var.id = id;
    var.timestamp = flash_timer;
    memcpy(var.data, data, size);
    return SS_flash_stream_log_fromISR(vars_stream.filename, &var, size + sizeof(var) - sizeof(var.data));
}

FlashStatus SS_flash_log_text(const char *str) {
    for (uint32_t i = 0; str[i] != '\0'; ++i) {
        FlashStatus res = SS_flash_stream_log(text_stream.filename, (void*) &str[i], 1);
        if(res != FLASH_STATUS_OK) {
            return res;
        }
    }
    return FLASH_STATUS_OK;
}

void SS_flash_log_25khz_timer_isr(void) {
    if(vars_stream.is_logging) {
        flash_timer++;
    } else {
        flash_timer = 0;
    }
}

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
    stream->item_size = sizeof(StreamElement);
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

/* Ensure data size is not larger than the queue item size */
static void SS_flash_stream_log_fromISR(char *filename, void *data, uint16_t size) {
    /* Logging from ISR disabled when running tests!!! */
#ifdef SS_RUN_TESTS
    return;
#endif
    FlashLogStream *stream = SS_flash_stream_get(filename);

    if(!stream->is_logging) {
        return;
    }

    static StreamElement element;
    element.size = size;
    memcpy(element.data, data, size);

    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(stream->queue, &element, &pxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

/* Ensure data size is not larger than to the queue item size */
static FlashStatus SS_flash_stream_log(char *filename, void *data, uint16_t size) {
    FlashLogStream *stream = SS_flash_stream_get(filename);

    static StreamElement element;
    element.size = size;
    memcpy(element.data, data, size);

    if(!stream->is_logging) {
        return FLASH_STATUS_DISABLED;
    }

    if(!xQueueSend(stream->queue, &element, pdMS_TO_TICKS(1))) {
        return FLASH_STATUS_BUSY;
    }
    return FLASH_STATUS_OK;
}

static void SS_flash_log_task(void *pvParameters) {
    FlashLogStream *stream = (FlashLogStream *) pvParameters;
    SS_flash_stream_init(stream);
    /* lfs_t *lfs = SS_flash_lfs_get(); */
    static StreamElement element;
    /* uint32_t sent_bytes = 0; */
    while(true) {
        /* TODO add mutex - for concurrent calls to SS_flash_stream_stop */
        if(xQueueReceive(stream->queue, &element, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
            SS_flash_control_log_bytes(element.data, element.size);
            for(int i=0;i<element.size;i++) SS_print("%d,", element.data[i]);
            SS_println("");
//            uint8_t test[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
//            SS_flash_control_log_bytes(test, 9);
            /* lfs_file_write(lfs, &stream->file, element.data, element.size); */
            /* sent_bytes += element.size; */

            /* if(sent_bytes >= SS_s25fl_get_sector_size()) { */
            /*     lfs_file_sync(lfs, &stream->file); */
            /*     sent_bytes = 0; */
            /* } */

        }
    }
}

static bool is_page_empty(uint8_t *data) {
    for(uint16_t i = 0; i < S25FL_PAGE_SIZE; i++) {
        if(data[i] != 0xFF) {
            return false;
        }
    }
    return true;
}

void SS_flash_print_logs(char *args) {
    /* SS_MPU_set_is_logging(false); */
    /* SS_println("Start transmiting"); */
    for(uint32_t i = 0;; i++) {
        static uint8_t data[S25FL_PAGE_SIZE];
        SS_s25fl_read_page(0x00089200UL/S25FL_PAGE_SIZE + i, data);
        if(is_page_empty(data)) {
            SS_s25fl_read_page(0x00089200UL/S25FL_PAGE_SIZE + i + 1, data);
            if(is_page_empty(data)) {
                /* SS_println("Transmit done, page: %d", i); */
                return;
            }
            SS_s25fl_read_page(0x00089200UL/S25FL_PAGE_SIZE + i, data);
        }
        /* SS_print("tx page: %d", i); */
        HAL_UART_Transmit(&huart5, data, S25FL_PAGE_SIZE, 4000);
        /* SS_print_bytes(data, S25FL_PAGE_SIZE); */
        /* for (uint32_t ii = 0; ii < S25FL_PAGE_SIZE; ++ii) { */
            /* if(ii%16 == 0) { */
                /* SS_println(""); */
            /* } */
            /* SS_print("%x ", data[ii]); */
        /* } */
        /* SS_print("\n\n------------------\n\n"); */
    }
}
void SS_flash_print_logs_debug(char *args) {
    /* SS_MPU_set_is_logging(false); */
    /* SS_println("Start transmiting"); */
    for(uint32_t i = 0; i < 2; i++) {
        static uint8_t data[S25FL_PAGE_SIZE];
        SS_s25fl_read_page(0x00089200UL/S25FL_PAGE_SIZE + i, data);


        /* SS_print("tx page: %d", i); */
        HAL_UART_Transmit(&huart5, data, S25FL_PAGE_SIZE, 4000);
        /* SS_print_bytes(data, S25FL_PAGE_SIZE); */
        /* for (uint32_t ii = 0; ii < S25FL_PAGE_SIZE; ++ii) { */
        /* if(ii%16 == 0) { */
        /* SS_println(""); */
        /* } */
        /* SS_print("%x ", data[ii]); */
        /* } */
        /* SS_print("\n\n------------------\n\n"); */
    }
}
