/**
  * SS_com.c
  *
  *  Created on: Jan 16, 2020
  *      Author: maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com_ids.h"
#ifdef SS_USE_GRAZYNA

#include "SS_grazyna.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_measurements.h"
#endif
#ifdef SS_USE_RELAYS
#include "SS_relays.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos_com.h"
#endif
#ifdef SS_USE_CAN
#include "SS_can.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel_com.h"
#endif
#ifdef SS_USE_SEQUENCE
#include "SS_sequence.h"
#endif

#include "FreeRTOS.h"
#include "SS_com.h"
#include "SS_com_debug.h"
#include "SS_log.h"
#include "SS_misc.h"
#include "assert.h"
#include "queue.h"
#include "stdbool.h"
#include "string.h"
#include "task.h"

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define SS_COM_RX_QUEUE_SIZE 32
#define SS_com_get_handler(handlers, device) _SS_com_get_handler(handlers, sizeof(handlers)/sizeof(handlers[0]), device)

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct {
    void (*sender_fun)(ComFrame *);
    ComFrame frame;
} ComSender;


typedef ComStatus (*ComFunction)(ComFrame*);

typedef struct {
    ComDeviceID id;
    ComFunction func;
} ComHandler;

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

ComStatus SS_com_handle_action(ComFrame *frame);
static ComStatus SS_com_handle_frame(ComFrame *frame);
static ComStatus SS_com_handle_request(ComFrame *frame);
static ComStatus SS_com_handle_service(ComFrame *frame);
static ComStatus SS_com_handle_sequence(ComFrame *frame);
static ComFunction _SS_com_get_handler(ComHandler *handlers, uint16_t size, ComDeviceID device);

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static ComBoardID board_id;
static QueueHandle_t com_queue;

static ComHandler service_handlers[] = {
#ifdef SS_USE_SERVOS
    { COM_SERVO_ID, SS_servos_com_service},
#endif
#ifdef SS_USE_RELAYS
    { COM_RELAY_ID, SS_relay_com_service},
#endif
#ifdef SS_USE_DYNAMIXEL
    { COM_DYNAMIXEL_ID, SS_dynamixel_com_service},
#endif
#ifdef SS_USE_SEQUENCE
    { COM_SEQUENCE_ID, SS_sequence_com_service},
#endif
};

static ComHandler request_handlers[] = {
#ifdef SS_USE_SERVOS
    { COM_SERVO_ID, SS_servos_com_request},
#endif
#ifdef SS_USE_RELAYS
    { COM_RELAY_ID, SS_relays_com_request},
#endif
#ifdef SS_USE_DYNAMIXEL
    { COM_DYNAMIXEL_ID, SS_dynamixel_com_request},
#endif
#ifdef SS_USE_ADS1258
    { COM_MEASUREMENT_ID, SS_ADS1258_com_request},
#endif
};

#ifdef SS_USE_SEQUENCE
static ComHandler sequence_handlers[] = {
#ifdef SS_USE_SERVOS
    { COM_SERVO_ID, SS_servos_com_sequence_validate},
#endif
#ifdef SS_USE_RELAYS
    /* { COM_RELAY_ID, SS_relays_com_sequence_validate}, */
#endif
#ifdef SS_USE_DYNAMIXEL
    { COM_DYNAMIXEL_ID, SS_dynamixel_com_sequence_validate},
#endif
};
#endif

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_com_message_received(ComFrame *frame) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    if(xQueueSendFromISR(com_queue, frame, &higherPriorityTaskWoken) ==
       pdTRUE) {
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    } else {
        SS_error("Com RX queue full");
    }
}

void SS_com_init(ComBoardID board) {
    board_id = board;
    /* TODO add macros for priorities */
    com_queue = xQueueCreate(SS_COM_RX_QUEUE_SIZE, sizeof(ComFrame));
    assert(com_queue != NULL);
}

void __attribute__((weak)) SS_com_transmit(ComFrame *frame) {
    SS_com_print_message_sent(frame);
    SS_led_toggle_com(false, false, true);
#ifdef SS_USE_GRAZYNA
    if(frame->destination == COM_GRAZYNA_ID && SS_grazyna_is_enabled()) {
        SS_grazyna_transmit(frame);
    } else {
#ifdef SS_USE_CAN
        SS_can_transmit(frame);
#endif
    }
#else
    SS_can_transmit(frame);
#endif
}

void SS_com_rx_handler_task(void *pvParameters) {
    ComFrame frame_buff;
    while(1) {
        if(xQueueReceive(com_queue, &frame_buff, portMAX_DELAY) == pdTRUE) {
            SS_com_handle_frame(&frame_buff);
        }
    }
}

ComBoardID SS_com_get_board_id(void) {
    return board_id;
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static ComStatus SS_com_handle_frame(ComFrame *frame) {
    SS_com_print_message_received(frame);
#ifdef SS_USE_GRAZYNA
    if(SS_grazyna_is_enabled() && frame->destination != board_id) {
        SS_com_transmit(frame);
        return COM_OK;
    }
#else
    if(frame->destination != board_id) {
        SS_error("Received message with invalid destination: %d", frame->destination);
    }
#endif
    bool response_required = frame->action == COM_REQUEST || frame->source == COM_GRAZYNA_ID ? true : false;
    SS_led_toggle_com(false, true, false);
    ComStatus res = SS_com_handle_action(frame);
    if(response_required) {
        frame->destination = frame->source;
        frame->source = board_id;
        SS_com_transmit(frame);
    }
    return res;
}

ComStatus SS_com_handle_action(ComFrame *frame) {
    ComActionID action = frame->action;
    switch(action) {
        case COM_REQUEST:
            return SS_com_handle_request(frame);
        case COM_SERVICE:
            return SS_com_handle_service(frame);
#ifdef SS_USE_SEQUENCE
        case COM_SEQUENCE:
            return SS_com_handle_sequence(frame);
#endif
        default:
            SS_error("Unsupported action: %d\r\n", frame->action);
    }
    return COM_ERROR;
}

static ComStatus SS_com_handle_request(ComFrame *frame) {
    ComFunction function = SS_com_get_handler(request_handlers, frame->device);
    ComStatus res = function ? function(frame) : COM_ERROR;
    frame->action = res ? COM_RESPONSE : COM_NACK;
    return res;
}

static ComStatus SS_com_handle_service(ComFrame *frame) {
    ComFunction function = SS_com_get_handler(service_handlers, frame->device);
    ComStatus res = function ? function(frame) : COM_ERROR;
    frame->action = res ? COM_ACK : COM_NACK;
    return res;
}

#ifdef SS_USE_SEQUENCE
static ComStatus SS_com_handle_sequence(ComFrame *frame) {
    ComFunction function = SS_com_get_handler(sequence_handlers, frame->device);
    ComStatus res = function ? function(frame) : COM_ERROR;
    Com2xInt16 val;
    memcpy(&val, &frame->payload, sizeof(uint32_t));
    if(SS_sequence_add(frame->device, frame->id, frame->operation, val.val, val.time) != 0) {
        res = COM_ERROR;
    }
    frame->action = res == 0 ? COM_ACK : COM_NACK;
    return res;
}
#endif

static ComFunction _SS_com_get_handler(ComHandler *handlers, uint16_t size, ComDeviceID device) {
    for(uint16_t i = 0; i < size; i++) {
        ComHandler handler = handlers[i];
        if(handler.id == device) {
            return handler.func;
        }
    }
    return NULL;
}

void SS_com_add_payload_to_frame(ComFrame *frame, ComDataType type, void *payload) {
    frame->data_type = type;
    switch(type) {
        case UINT32:
        case INT32:
        case FLOAT:
        case INT16x2:
            memcpy(&frame->payload, payload, 4);
            break;
        case UINT16:
        case INT16:
            memcpy(&frame->payload, payload, 2);
            break;
        case UINT8:
        case INT8:
            memcpy(&frame->payload, payload, 1);
            break;
        default:
            break;
    }
}
