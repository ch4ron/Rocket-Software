/**
  * SS_can.c
  *
  *  Created on: Mar 03, 2020
  *      Author: maciek
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */
#include "SS_can.h"

#include "FreeRTOS.h"
#include "SS_com_debug.h"
#include "SS_error.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"

/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define CAN_FIFO_LENGTH 50

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

volatile uint32_t error = 0;

static ComBoardID board_id;

typedef struct {
    CAN_HandleTypeDef *hcan;
    ComBoardID board_ids[5];  // Maximum of 5 different board filters
    QueueHandle_t tx_queue;
} Can;

static Can com_can;

#ifdef SS_USE_EXT_CAN
static Can ext_can;
#endif

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static void SS_can_interrupts_enable(CAN_HandleTypeDef *hcan);
static void SS_can_filter_init(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask, uint32_t fifo_assignment, uint8_t *filter_bank);
static uint32_t SS_can_get_header(ComFrame *frame);
static void SS_can_filters_init_with_mask(CAN_HandleTypeDef *hcan, ComBoardID board, uint8_t board_mask);
static void SS_can_filters_init(CAN_HandleTypeDef *hcan, ComBoardID board);
static void SS_can_pack_frame(ComFrame *frame, CAN_TxHeaderTypeDef *header, uint8_t *data);
static void SS_can_unpack_frame(ComFrame *frame, CAN_RxHeaderTypeDef *header, uint8_t *data);
static void SS_can_tx_common(ComFrame *frame, Can *can);
static void SS_can_tx(ComFrame *frame);
static void SS_can_handle_received(CAN_HandleTypeDef *hcan, uint8_t priority);
static void SS_can_error(char *error);
#ifdef SS_USE_EXT_CAN
static void SS_can_ext_tx(ComFrame *frame);
#endif

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

#ifdef SS_USE_GRAZYNA
void SS_can_enable_grazyna() {
    if(com_can.hcan == NULL) {
        SS_error("Initialize Can before Grazyna");
    }
    SS_can_filters_init_with_mask(com_can.hcan, board_id, 0);
}

void SS_can_disable_grazyna() {
    if(com_can.hcan == NULL) {
        SS_error("Initialize Can before Grazyna");
    }
    SS_can_filters_init(com_can.hcan, board_id);
}
#endif

void SS_can_init(CAN_HandleTypeDef *hcan, ComBoardID board) {
    SS_com_init(board);
    com_can.hcan = hcan;
    board_id = board;
    SS_can_filters_init(hcan, board);
    com_can.tx_queue = SS_com_add_sender();
    HAL_CAN_Start(hcan);
    SS_can_interrupts_enable(hcan);
}

void SS_can_transmit(ComFrame *frame) {
    SS_com_add_to_tx_queue(frame, SS_can_tx, com_can.tx_queue);
}

#ifdef SS_USE_EXT_CAN
void SS_can_ext_init(CAN_HandleTypeDef *hcan) {
    ext_can.hcan = hcan;
    SS_can_filters_init_with_mask(hcan, 0, 0);
    ext_can.tx_queue = SS_com_add_sender();
    HAL_CAN_Start(hcan);
    SS_can_interrupts_enable(hcan);
}

void SS_can_ext_transmit(ComFrame *frame) {
    SS_com_add_to_tx_queue(frame, SS_can_tx, ext_can.tx_queue);
}
#endif

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static void SS_can_interrupts_enable(CAN_HandleTypeDef *hcan) {
    HAL_CAN_ActivateNotification(hcan,
                                 CAN_IT_TX_MAILBOX_EMPTY |
                                     CAN_IT_RX_FIFO0_MSG_PENDING |
                                     CAN_IT_RX_FIFO1_MSG_PENDING |
                                     CAN_IT_RX_FIFO0_FULL |
                                     CAN_IT_RX_FIFO0_OVERRUN |
                                     CAN_IT_RX_FIFO1_FULL |
                                     CAN_IT_RX_FIFO1_OVERRUN |
                                     CAN_IT_BUSOFF |
                                     CAN_IT_ERROR);
}

static void SS_can_filter_init(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask, uint32_t fifo_assignment, uint8_t *filter_bank) {
    static CAN_FilterTypeDef can_filter;
    can_filter.FilterBank = *filter_bank;
    can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter.FilterIdHigh = (uint16_t) ((filter_id >> 13));
    can_filter.FilterIdLow = (uint16_t) (((filter_id << 3) & 0xFFF8));
    can_filter.FilterMaskIdHigh = (uint16_t) (filter_mask >> 13);
    can_filter.FilterMaskIdLow = (uint16_t) (filter_mask << 3 & 0xFFF8);
    can_filter.FilterFIFOAssignment = fifo_assignment;
    can_filter.FilterActivation = ENABLE;
    (*filter_bank)++;
    if (HAL_CAN_ConfigFilter(hcan, &can_filter) != HAL_OK) {
        Error_Handler();
    }
}

static uint32_t SS_can_get_header(ComFrame *frame) {
    return *((uint32_t*) frame) & 0x7fffffff;
}

static void SS_can_filters_init_with_mask(CAN_HandleTypeDef *hcan, ComBoardID board, uint8_t board_mask) {
    uint8_t filter_bank = 0;
    ComFrame filter_frame = {0}, mask_frame = {0};
    filter_frame.destination = board;
    uint32_t filter = SS_can_get_header(&filter_frame);
    mask_frame.destination = board_mask;
    mask_frame.priority = 0b111;
    uint32_t mask = SS_can_get_header(&mask_frame);
    SS_can_filter_init(hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
    filter_frame.priority = 1;
    filter = SS_can_get_header(&filter_frame);
    SS_can_filter_init(hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
    filter_frame.destination = COM_BROADCAST_ID;
    filter = SS_can_get_header(&filter_frame);
    SS_can_filter_init(hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
    filter_frame.priority = 1;
    filter = SS_can_get_header(&filter_frame);
    SS_can_filter_init(hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
}

static void SS_can_filters_init(CAN_HandleTypeDef *hcan, ComBoardID board) {
    SS_can_filters_init_with_mask(hcan, board, 0x1F);
}

static void SS_can_pack_frame(ComFrame *frame, CAN_TxHeaderTypeDef *header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored in the first byte of data */
    header->ExtId = SS_can_get_header(frame);
    header->RTR = CAN_RTR_DATA;
    header->IDE = CAN_ID_EXT;
    header->TransmitGlobalTime = DISABLE;
    data[0] = frame->data_type;
    header->DLC = sizeof(frame->operation) + sizeof(frame->payload) + 1;
    memcpy(data + 1, &frame->operation, sizeof(frame->operation) + sizeof(frame->payload));
}

static void SS_can_unpack_frame(ComFrame *frame, CAN_RxHeaderTypeDef *header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored
     * in the first byte of data */
    *((uint32_t *) frame) = header->ExtId;
    frame->data_type = data[0];
    memcpy(&frame->operation, data + 1, header->DLC - 1);
}

static void SS_can_tx_common(ComFrame *frame, Can *can) {
    /* TODO Investigate if a mutex is necessary for can transmission */
    CAN_TxHeaderTypeDef header;
    uint8_t data[sizeof(ComFrame)];
    uint32_t mailbox;
    SS_can_print_message_received(frame);
    SS_can_pack_frame(frame, &header, data);
    if(HAL_CAN_AddTxMessage(can->hcan, &header, data, &mailbox) != HAL_OK) {
        SS_can_error("HAL_CAN_AddTxMessage failed");
        return;
    }
}

static void SS_can_tx(ComFrame *frame) {
    SS_can_tx_common(frame, &com_can);
}

static void SS_can_handle_received(CAN_HandleTypeDef *hcan, uint8_t priority) {
    static ComFrame buff;
    static CAN_RxHeaderTypeDef header;
    static uint8_t data[sizeof(ComFrame)];
    uint32_t internal_fifo = (priority == COM_HIGH_PRIORITY ? CAN_RX_FIFO0 : CAN_RX_FIFO1);
    HAL_CAN_GetRxMessage(hcan, internal_fifo, &header, data);
    SS_can_unpack_frame(&buff, &header, data);
    SS_com_message_received(&buff);
}

#ifdef SS_USE_EXT_CAN
static void SS_can_ext_tx(ComFrame *frame) {
    SS_can_tx_common(frame, &ext_can);
}

#endif

static void SS_can_error(char *error) {
#ifdef CAN_DEBUG_ERRORS
    SS_error(error);
#endif
}

/* ==================================================================== */
/* ============================ Callbacks ============================= */
/* ==================================================================== */

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
}

void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan) {
    SS_can_error("Mailbox Abort");
}

void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan) {
    SS_can_error("Mailbox Abort");
}

void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan) {
    SS_can_error("Mailbox Abort");
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan) {
    SS_can_error("Internal can fifo 0 full");
}

void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan) {
    SS_can_error("Internal can fifo 1 full");
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    error = HAL_CAN_GetError(hcan);
    SS_can_error("Can error");
    HAL_CAN_ResetError(hcan);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    printf("can rx\r\n");
    SS_can_handle_received(hcan, COM_HIGH_PRIORITY);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    printf("can rx\r\n");
    SS_can_handle_received(hcan, COM_LOW_PRIORITY);
}
