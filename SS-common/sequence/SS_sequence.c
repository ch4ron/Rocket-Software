/*
 * SS_sequence.c
 *
 *  Created on: Feb 1, 2020
 *      Author: maciek
 */

#define SS_DEBUG_ENABLED

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_sequence.h"

#include "FreeRTOS.h"
#include "SS_com.h"
#include "SS_log.h"
#include "semphr.h"
#include "stdbool.h"
#include "string.h"
#ifdef SS_USE_SERVOS
#include "SS_servos_com.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel_com.h"
#endif
#ifdef SS_USE_RELAYS
#include "SS_relays_com.h"
#endif

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct {
    ComDeviceID device;
    uint8_t id;
    uint8_t operation;
    int16_t value;
    int16_t time;
} SequenceItem;

typedef struct {
    uint8_t size;
    SequenceItem items[MAX_SEQUENCE_ITEMS];
} Sequence;

typedef ComStatus (*SequenceFunction)(uint8_t id, uint8_t operation, int16_t value);

typedef struct {
    ComDeviceID device;
    SequenceFunction func;
} SequenceHandler;

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void SS_sequence_run(void);
static void SS_sequence_ack_item(SequenceItem item);

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static Sequence sequence;
static SemaphoreHandle_t sequence_mutex;

static SequenceHandler sequence_handlers[] = {
#ifdef SS_USE_SERVOS
    {COM_SERVO_ID, SS_servos_sequence},
#endif
#ifdef SS_USE_DYNAMIXEL
    {COM_DYNAMIXEL_ID, SS_dynamixel_sequence},
#endif
#ifdef SS_USE_RELAYS
    {COM_RELAY_ID, SS_relays_sequence},
#endif
};

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_sequence_init(void) {
    sequence_mutex = xSemaphoreCreateBinary();
    assert(sequence_mutex != NULL);
}

int8_t SS_sequence_add(ComDeviceID device, uint8_t id, uint8_t operation, int16_t value, int16_t time) {
    if(sequence.size >= MAX_SEQUENCE_ITEMS) {
        SS_error("Sequence is full, dropping");
        return -1;
    }
    SequenceItem new_item = {
        .device = device,
        .id = id,
        .operation = operation,
        .value = value,
        .time = time};
    uint8_t i;
    for(i = 0; i < sequence.size; i++) {
        if(time < sequence.items[i].time) {
            memmove(&sequence.items[i + 1], &sequence.items[i], (sequence.size - i) * sizeof(SequenceItem));
            break;
        }
    }
    sequence.items[i] = new_item;
    sequence.size++;
    return 0;
}

void SS_sequence_clear(void) {
    memset(&sequence, 0, sizeof(sequence));
    SS_debugln("Sequence cleared");
}

void SS_sequence_start(void) {
    xSemaphoreGive(sequence_mutex);
}

void SS_sequence_abort(void) {
    SS_debugln("Sequence aborted");
}

ComStatus SS_sequence_com_service(ComFrame *frame) {
    ComSequenceID msgID = frame->operation;
    switch(msgID) {
        case COM_SEQUENCE_CLEAR:
            SS_sequence_clear();
            break;
        case COM_SEQUENCE_START:
            SS_sequence_start();
            break;
        case COM_SEQUENCE_ABORT:
            SS_sequence_abort();
            break;
        default:
            SS_error("Unhandled Com sequence service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

void SS_sequence_task(void *pvParameters) {
    while(true) {
        if(xSemaphoreTake(sequence_mutex, portMAX_DELAY) == pdTRUE) {
            SS_sequence_run();
        }
    }
}

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static SequenceFunction get_sequence_function(ComDeviceID id) {
    for(uint8_t i = 0; i < sizeof(sequence_handlers) / sizeof(sequence_handlers[0]); i++) {
        if(sequence_handlers[i].device == id) {
            return sequence_handlers[i].func;
        }
    }
    return NULL;
}

static void SS_sequence_run(void) {
    if(sequence.size == 0) {
        SS_error("Trying to run an empty sequence");
        return;
    }
    SS_debugln("Sequence started");
    for(uint8_t i = 0; i < sequence.size; i++) {
        SequenceItem item = sequence.items[i];
        int16_t delay = i == 0 ? item.time : item.time - sequence.items[i - 1].time;
        vTaskDelay(pdMS_TO_TICKS(delay));
        SequenceFunction function = get_sequence_function(item.id);
        if(function && function(item.id, item.operation, item.value) == COM_OK) {
            SS_sequence_ack_item(item);
        } else {
            /* TODO nack item */
        }
    }
    SS_debugln("Sequence finished");
}

static void SS_sequence_ack_item(SequenceItem item) {
    Com2xInt16 val = {
        .val = item.value,
        .time = item.time};
    uint32_t payload;
    memcpy(&payload, &val, sizeof(uint32_t));
    ComFrame sequence_frame = {
        .priority = COM_LOW_PRIORITY,
        .action = COM_SEQUENCE,
        .source = SS_com_get_board_id(),
        .destination = COM_GRAZYNA_ID,
        .data_type = INT16x2,
        .id = item.id,
        .device = item.device,
        .operation = item.operation,
        .payload = payload,
    };
    SS_com_transmit(&sequence_frame);
}
