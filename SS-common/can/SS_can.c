//
// Created by maciek on 18.03.2020.
//
#include <error/SS_error.h>
#include <com/SS_com_debug.h>
#include "string.h"
#include "stdio.h"
#include "SS_can.h"
#include "SS_fifo.h"

volatile uint32_t error = 0;

#define CAN_FIFO_LENGTH 50

static CAN_HandleTypeDef *com_hcan;

FIFO_INIT(can_tx, CAN_FIFO_LENGTH, ComFrame)
FIFO_INIT(can_rx, CAN_FIFO_LENGTH, ComFrame)
FIFO_INIT(can_tx_priority, CAN_FIFO_LENGTH, ComFrame)
FIFO_INIT(can_rx_priority, CAN_FIFO_LENGTH, ComFrame)

static void SS_can_interrupts_enable(CAN_HandleTypeDef *hcan) {
    HAL_CAN_ActivateNotification(hcan,
                        CAN_IT_TX_MAILBOX_EMPTY     |
                                 CAN_IT_RX_FIFO0_MSG_PENDING |
                                 CAN_IT_RX_FIFO1_MSG_PENDING |
                                 CAN_IT_RX_FIFO0_FULL        |
                                 CAN_IT_RX_FIFO0_OVERRUN     |
                                 CAN_IT_RX_FIFO1_FULL        |
                                 CAN_IT_RX_FIFO1_OVERRUN     |
                                 CAN_IT_BUSOFF               |
                                 CAN_IT_ERROR);
}

static void SS_can_filter_init(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask, uint32_t fifo_assignment, uint8_t *filter_bank) {
    uint32_t id = filter_id, mask = filter_mask, bank = *filter_bank;
    CAN_FilterTypeDef can_filter;
    can_filter.FilterBank = bank;
    can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter.FilterIdHigh = (uint16_t) ((id >> 13));
    can_filter.FilterIdLow = (uint16_t) (((id << 3) & 0xFFF8));
    can_filter.FilterMaskIdHigh = (uint16_t) (mask >> 13);
    can_filter.FilterMaskIdLow = (uint16_t) (mask << 3 & 0xFFF8);
    can_filter.FilterFIFOAssignment = fifo_assignment;
    can_filter.FilterActivation = ENABLE;
    (*filter_bank)++;
    if (HAL_CAN_ConfigFilter(hcan, &can_filter) != HAL_OK) {
        Error_Handler();
    }
}

static uint32_t SS_can_get_header(ComFrame *frame) {
    return *((uint32_t*) frame) >> 3;
}

static void SS_can_filters_init(CAN_HandleTypeDef *hcan, uint8_t board) {
    uint8_t filter_bank = 0;
    ComFrame filter_frame = { 0 }, mask_frame = { 0 };
    filter_frame.destination = board;
    uint32_t filter = SS_can_get_header(&filter_frame);
    mask_frame.destination = 0x1F;
    mask_frame.priority = 0b111;
    uint32_t mask = SS_can_get_header(&mask_frame);
    SS_can_filter_init(hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
    filter_frame.priority = 1;
    filter = SS_can_get_header(&filter_frame);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
    filter_frame.destination = COM_BROADCAST_ID;
    filter = SS_can_get_header(&filter_frame);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
    filter_frame.priority = 1;
    filter = SS_can_get_header(&filter_frame);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
}

static void SS_can_pack_frame(ComFrame *frame, CAN_TxHeaderTypeDef *header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored in the first byte of data */
    header->ExtId = SS_can_get_header(frame);
    header->RTR = CAN_RTR_DATA;
    header->IDE = CAN_ID_EXT;
    header->TransmitGlobalTime = DISABLE;
    data[0] = frame->data_type;
    header->DLC = sizeof(frame->message_type) + sizeof(frame->payload) + 1;
    memcpy(data + 1, &frame->message_type, sizeof(frame->message_type) + sizeof(frame->payload));
}

static void SS_can_unpack_frame(ComFrame *frame, CAN_RxHeaderTypeDef *header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored in the first byte of data */
    *((uint32_t*) frame) = (header->ExtId << 3);
    frame->data_type = data[0];
    memcpy(&frame->message_type, data + 1, header->DLC - 1);
}


static void SS_can_handle_tx_fifo(ComFrame *frame) {
    CAN_TxHeaderTypeDef header;
    uint8_t data[sizeof(ComFrame)];
    uint32_t mailbox;
    SS_can_pack_frame(frame, &header, data);
    if (HAL_CAN_AddTxMessage(com_hcan, &header, data, &mailbox) != HAL_OK) {
        SS_can_error("HAL_CAN_AddTxMessage failed");
        return;
    }
}

void SS_can_init(CAN_HandleTypeDef *hcan, ComBoardID board) {
    com_hcan = hcan;
    SS_can_filters_init(hcan, board);
    SS_com_add_fifo(&can_rx_fifo, NULL, COM_GROUP_RECEIVE, COM_LOW_PRIORITY);
    SS_com_add_fifo(&can_rx_priority_fifo, NULL, COM_GROUP_RECEIVE, COM_LOW_PRIORITY);
    SS_com_add_fifo(&can_tx_fifo, SS_can_handle_tx_fifo, COM_GROUP_CAN1, COM_LOW_PRIORITY);
    SS_com_add_fifo(&can_tx_priority_fifo, NULL, COM_GROUP_CAN1, COM_LOW_PRIORITY);
    HAL_CAN_Start(hcan);
    SS_can_interrupts_enable(hcan);
}

void SS_can_handle_received(CAN_HandleTypeDef *hcan, uint8_t priority) {
    static ComFrame buff;
    static CAN_RxHeaderTypeDef header;
    static uint8_t data[sizeof(ComFrame)];

    uint32_t internal_fifo = (priority == COM_HIGH_PRIORITY ? CAN_RX_FIFO0 : CAN_RX_FIFO1);
    volatile Fifo *can_fifo = (priority == COM_HIGH_PRIORITY ? &can_rx_priority_fifo : &can_rx_fifo);
    HAL_CAN_GetRxMessage(hcan, internal_fifo, &header, data);
    SS_can_unpack_frame(&buff, &header, data);
    if((!SS_fifo_put_data(can_fifo, &buff))) {
        SS_can_error("can RX fifo full");
    }
}

void SS_can_transmit(ComFrame *frame, uint8_t priority) {
    volatile Fifo *can_fifo = priority == COM_HIGH_PRIORITY ? &can_tx_priority_fifo : &can_tx_fifo;
    SS_fifo_put_data(can_fifo, frame);
}

void SS_can_error(char *error) {
    HAL_GPIO_WritePin(COM_RED_GPIO_Port, COM_RED_Pin, SET);
#ifdef CAN_DEBUG_ERRORS
    SS_error(error);
#endif
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    HAL_GPIO_TogglePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    HAL_GPIO_TogglePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin);
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    HAL_GPIO_TogglePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin);
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
    if(hcan == com_hcan)
        SS_can_handle_received(hcan, COM_HIGH_PRIORITY);
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan == com_hcan)
        SS_can_handle_received(hcan, COM_LOW_PRIORITY);
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
}
