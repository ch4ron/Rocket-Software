/**
  * SS_grazyna.c
  *
  *  Created on: Jan 18, 2020
  *      Author: maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#ifdef SS_USE_CAN
#include "SS_can.h"
#endif
#include "FreeRTOS.h"
#include "SS_com.h"
#include "SS_com_debug.h"
#include "SS_grazyna_hal.h"
#include "queue.h"
#include "semphr.h"
#include "stdbool.h"
#include "string.h"

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef enum {
    GRAZYNA_LOOKING_FOR_HEADER,
    GRAZYNA_RECEIVING_FRAME
} GrazynaState;

typedef struct {
    GrazynaFrame rx_buff, tx_buff;
    GrazynaState grazyna_state;
    ComFrame frame;
    bool enabled;
    void *huart; /* UART_HandleTypedef */
    QueueHandle_t tx_queue;
    SemaphoreHandle_t mutex;
} Grazyna;

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void SS_grazyna_handle_frame(void);
static void SS_grazyna_handle_header(void);
static void SS_grazyna_tx(ComFrame *frame);
static void SS_grazyna_prepare_tx_frame(ComFrame *com_frame, GrazynaFrame *grazyna_frame);
static uint32_t SS_grazyna_crc_calculate(GrazynaFrame *grazyna_frame);

/* ==================================================================== */
/* ========================= Global variables ========================= */
/* ==================================================================== */

static Grazyna grazyna;

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_grazyna_init(UART_HandleTypeDef *huart) {
    SS_grazyna_init_hal(huart);
    grazyna.grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
    grazyna.tx_queue = xQueueCreate(SS_COM_TX_QUEUE_SIZE, sizeof(ComFrame));
    grazyna.mutex = xSemaphoreCreateBinary();
    SS_grazyna_receive_hal((uint8_t *) &grazyna.rx_buff, sizeof(grazyna.rx_buff.header));
    SS_grazyna_enable();
}

void SS_grazyna_enable(void) {
#ifdef SS_USE_CAN
    SS_can_enable_grazyna();
#endif
    grazyna.enabled = true;
}

void SS_grazyna_disable(void) {
#ifdef SS_USE_CAN
    SS_can_disable_grazyna();
#endif
    grazyna.enabled = false;
}

bool SS_grazyna_is_enabled(void) {
    return grazyna.enabled;
}

void SS_grazyna_transmit(ComFrame *frame) {
    if(xQueueSend(grazyna.tx_queue, frame, pdMS_TO_TICKS(25)) != pdTRUE) {
        SS_error("Grazyna TX queue full");
    }
}

void SS_grazyna_tx_handler_task(void *pvParameters) {
    ComFrame frame;
    while(1) {
        if(xQueueReceive(grazyna.tx_queue, &frame, portMAX_DELAY) == pdTRUE) {
            xSemaphoreTake(grazyna.mutex, 1000);
            SS_grazyna_tx(&frame);
        }
    }
}
/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static void SS_grazyna_tx(ComFrame *frame) {
    memcpy(&grazyna.frame, frame, sizeof(ComFrame));
    SS_grazyna_prepare_tx_frame(&grazyna.frame, &grazyna.tx_buff);
    SS_grazyna_transmit_hal((uint8_t *) &grazyna.tx_buff, sizeof(grazyna.tx_buff));
}

static void SS_grazyna_handle_frame() {
    uint32_t calculated_crc = SS_grazyna_crc_calculate(&grazyna.rx_buff);
    if(grazyna.rx_buff.crc == calculated_crc) {
        SS_com_message_received(&grazyna.rx_buff.com_frame);
    }
    grazyna.grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
    SS_grazyna_receive_hal((uint8_t *) &grazyna.rx_buff, sizeof(grazyna.rx_buff.header));
}

static void SS_grazyna_handle_header() {
    if(grazyna.rx_buff.header == GRAZYNA_HEADER) {
        grazyna.grazyna_state = GRAZYNA_RECEIVING_FRAME;
        SS_grazyna_receive_hal((uint8_t *) &grazyna.rx_buff + sizeof(grazyna.rx_buff.header),
                               sizeof(grazyna.rx_buff) - sizeof(grazyna.rx_buff.header));
    } else {
        grazyna.grazyna_state = GRAZYNA_LOOKING_FOR_HEADER;
        SS_grazyna_receive_hal((uint8_t *) &grazyna.rx_buff, sizeof(grazyna.rx_buff.header));
    }
}

static void SS_grazyna_prepare_tx_frame(ComFrame *com_frame, GrazynaFrame *grazyna_frame) {
    grazyna_frame->header = GRAZYNA_HEADER;
    memcpy(&grazyna_frame->com_frame, com_frame, sizeof(ComFrame));
    /* TODO Defer CRC calculation to task */
    grazyna_frame->crc = SS_grazyna_crc_calculate(grazyna_frame);
}

static uint32_t SS_grazyna_crc_calculate(GrazynaFrame *grazyna_frame) {
    static uint32_t buff[3];
    memcpy(buff, grazyna_frame, sizeof(GrazynaFrame) - sizeof(grazyna_frame->crc));
    return SS_grazyna_crc_hal(buff, 3);
}

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void SS_grazyna_rx_isr(void) {
    switch(grazyna.grazyna_state) {
        case GRAZYNA_LOOKING_FOR_HEADER:
            SS_grazyna_handle_header();
            break;
        case GRAZYNA_RECEIVING_FRAME:
            SS_grazyna_handle_frame();
            break;
    }
}

void SS_grazyna_tx_isr(void) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(grazyna.mutex, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}
