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
uint8_t result;

#define COM_HIGH_PRIORITY 0
#define CAN_LOW_PRIORITY 1
#define CAN_FIFO_LENGTH 50


static ComBoardID board_id;
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

void SS_can_filter_init(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask, uint32_t fifo_assignment, uint8_t *filter_bank) {
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

//    *header = frame->header >> 3;
//    *data = (uint8_t) frame->header | 0b111;
//    memcpy(data + 1, &frame->message_type, sizeof(ComFrame) - sizeof(frame->header));
//    return sizeof(ComFrame) - sizeof(frame->header) + sizeof(uint8_t);
//}
//
//static int SS_can_unpack_frame(ComFrame *frame, uint32_t *header, uint8_t *data) {
//    *header = frame->header >> 3;
//    *data = (uint8_t) frame->header | 0b111;
//    memcpy(data + 1, &frame->message_type, sizeof(ComFrame) - sizeof(frame->header));
//    return sizeof(ComFrame) - sizeof(frame->header) + sizeof(uint8_t);
//}
static uint32_t SS_can_get_header(ComFrame *frame) {
    return *((uint32_t*) frame) >> 3;
}

static void SS_can_filters_init(CAN_HandleTypeDef *hcan, uint8_t board) {
    uint8_t filter_bank = 0;
    ComFrame frame = { 0 };
    frame.destination = board;
    uint32_t filter = SS_can_get_header(&frame);
    frame.destination = 0x1F;
    frame.priority = 1;
    uint32_t mask = 0x1FFFFFFF; //SS_can_get_header(&frame);
    SS_can_filter_init(hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
//    filter = SS_can_get_header(&frame);
//    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
//    frame.destination = COM_BROADCAST_ID;
//    filter = SS_can_get_header(&frame);
//    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
//    frame.priority = 1;
//    filter = SS_can_get_header(&frame);
//    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
}

void SS_can_init(CAN_HandleTypeDef *hcan, ComBoardID board) {
    com_hcan = hcan;
    board_id = board;
    SS_can_filters_init(hcan, board);
    HAL_CAN_Start(hcan);
    SS_can_interrupts_enable(hcan);
}

void SS_can_pack_frame(ComFrame *frame, CAN_TxHeaderTypeDef *header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored in the first byte of data */
    header->ExtId = SS_can_get_header(frame);
    header->RTR = CAN_RTR_DATA;
    header->IDE = CAN_ID_EXT;
    header->TransmitGlobalTime = DISABLE;
    data[0] = frame->data_type;
    header->DLC = sizeof(frame->message_type) + sizeof(frame->payload) + 1;
    memcpy(data + 1, &frame->message_type, sizeof(frame->message_type) + sizeof(frame->payload));
}

void SS_can_unpack_frame(ComFrame *frame, CAN_RxHeaderTypeDef *header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored in the first byte of data */
    *((uint32_t*) frame) = (header->ExtId << 3);
    frame->data_type = data[0];
    memcpy(&frame->message_type, data + 1, header->DLC - 1);
}

void SS_can_handle_received(CAN_HandleTypeDef *hcan, uint8_t priority) {
    static ComFrame buff;
    static CAN_RxHeaderTypeDef header;
    static uint8_t data[sizeof(ComFrame)];

    uint32_t internal_fifo = (priority == COM_HIGH_PRIORITY ? CAN_RX_FIFO0 : CAN_RX_FIFO1);
    volatile Fifo *can_fifo = (priority == COM_HIGH_PRIORITY ? &can_rx_priority_fifo : &can_rx_fifo);
    HAL_CAN_GetRxMessage(hcan, internal_fifo, &header, data);
    SS_can_unpack_frame(&buff, &header, data);
    SS_com_print_message_received(&buff);
//    if((!SS_fifo_put_data(can_fifo, &buff))) {
//        SS_can_error("RX fifo full");
//    }
}

void SS_can_transmit(ComFrame *frame, uint8_t priority) {
    volatile Fifo *can_fifo = priority == COM_HIGH_PRIORITY ? &can_tx_priority_fifo : &can_tx_fifo;
    SS_fifo_put_data(can_fifo, frame);
}

void SS_can_tx_data_fifo(uint8_t priority) {
    ComFrame buff;
    CAN_TxHeaderTypeDef header;
    uint8_t data[sizeof(ComFrame)];
    uint32_t mailbox;
    volatile Fifo *can_fifo = priority == COM_HIGH_PRIORITY ? &can_tx_priority_fifo : &can_tx_fifo;
    if (!SS_fifo_get_data(can_fifo, &buff)) return;
    SS_can_pack_frame(&buff, &header, data);
    SS_com_print_message_sent(&buff);
//    if (board == CAN_BOARD_ID) {
//        SS_can_error("Message to self");
//        return -1;
//    }
    if (HAL_CAN_AddTxMessage(com_hcan, &header, data, &mailbox) != HAL_OK) {
        SS_can_error("HAL_CAN_AddTxMessage failed");
        return;
    }
}

void SS_can_error(char *error) {
    HAL_GPIO_WritePin(COM_RED_GPIO_Port, COM_RED_Pin, SET);
#ifdef CAN_DEBUG_ERRORS
    SS_error("\r\n\x01b[41m%s       ", error);
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
    printf("fifo 0\r\n");
//    if(hcan == com_hcan)
        SS_can_handle_received(hcan, COM_HIGH_PRIORITY);
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    printf("fifo 1\r\n");
//    if (hcan == com_hcan)
        SS_can_handle_received(hcan, CAN_LOW_PRIORITY);
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
}
