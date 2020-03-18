//
// Created by maciek on 18.03.2020.
//
#include <error/SS_error.h>
#include "string.h"
#include "stdio.h"
#include "SS_can.h"
#include "SS_fifo.h"
#include "SS_com.h"

volatile uint32_t error = 0;
uint8_t result;

CAN_RxHeaderTypeDef rx_header;
CAN_TxHeaderTypeDef tx_header;

#define CAN_HIGH_PRIORITY 0
#define CAN_LOW_PRIORITY 1
#define CAN_FIFO_LENGTH 50


static ComBoardID board_id;
static CAN_HandleTypeDef *com_hcan; //hcan2

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
    uint32_t bank = *filter_bank;
    if(bank >= 28) {
        SS_error("Maximum number of filter banks is 28");
        return;
    }
    CAN_FilterTypeDef can_filter;
    can_filter.FilterBank = bank;
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
static uint32_t SS_can_get_filter(ComFrameContent *frame_content) {
    ComFrame frame = { 0 };
    SS_com_pack_frame(&frame, frame_content);
    return frame.header >> 3;
}

void SS_can_filters_init(uint8_t board) {
    uint8_t filter_bank = 0;
    ComFrameContent frame_content = { 0 };
    frame_content.destination = board;
    uint32_t filter = SS_can_get_filter(&frame_content);
    frame_content.destination = 0x1F;
    uint32_t mask = SS_can_get_filter(&frame_content);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
    frame_content.priority = 1;
    filter = SS_can_get_filter(&frame_content);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
    frame_content.destination = COM_BROADCAST_ID;
    filter = SS_can_get_filter(&frame_content);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO0, &filter_bank);
    frame_content.priority = 1;
    filter = SS_can_get_filter(&frame_content);
    SS_can_filter_init(com_hcan, filter, mask, CAN_FILTER_FIFO1, &filter_bank);
}

void SS_can_init(CAN_HandleTypeDef *hcan, ComBoardID board) {
    SS_can_filters_init(board);
    com_hcan = hcan;
    board_id = board;
    HAL_CAN_Start(hcan);
    SS_can_interrupts_enable(hcan);
}

int8_t SS_can_rx_handle_from_fifo_data(uint8_t priority) {
    volatile Fifo *can_fifo = priority == CAN_HIGH_PRIORITY ? &can_rx_priority_fifo : &can_rx_fifo;
    if (SS_fifo_get_data(can_fifo, &fifo_bufor_rx) == -1)
        return -1;
    SS_com_handle_can_received(&fifo_bufor_rx, priority);
    return 0;
}

void SS_can_unpack_frame(ComFrame *frame, CAN_RxHeaderTypeDef *rx_header, uint8_t *data) {
    /* Can frame header has 29 bits, so the remaining 3 header bits are stored in the first byte of data */
    frame->header = (rx_header->ExtId << 3) | data[0];
    memcpy(&frame->message_type, data + 1, rx_header->DLC - 1);
}

int8_t SS_can_rx_put_to_fifo_data(CAN_HandleTypeDef *hcan, uint8_t priority) {
    uint32_t internal_fifo = priority == CAN_HIGH_PRIORITY ? CAN_RX_FIFO0 : CAN_RX_FIFO1;
    HAL_CAN_GetRxMessage(hcan, internal_fifo, &rx_header, rx_data);
    SS_can_parse_rx_bufor(&fifo_bufor_rx, &rx_header, rx_data);
    volatile Fifo *can_fifo = priority == CAN_HIGH_PRIORITY ? &can_rx_priority_fifo : &can_rx_fifo;
    if((result =  SS_fifo_put_data(can_fifo, &fifo_bufor_rx)) == -1) {
        SS_can_error("RX fifo full");
    }
    return result;
}

void SS_can_prepare_tx_bufor(can_fifo_bufor *bufor, enum CAN_HEADER_TYPE header, can_board board, uint8_t *data,
                             uint8_t data_length, uint8_t to_grazyna, uint8_t priority) {
    bufor->header.data.id = header;
    bufor->header.data.board = board;
    bufor->header.data.priority = priority;
    bufor->header.data.grazyna = to_grazyna;
    bufor->header.data.source = board_id;
    bufor->length = data_length;
    if(data_length > 0 && data != NULL) {
        memcpy(bufor->data, data, data_length);
    }
}

int8_t SS_can_send_to_grazyna(enum CAN_HEADER_TYPE can_header_type, uint8_t *data, uint8_t data_length, can_board board, uint8_t priority) {
    volatile Fifo *can_fifo = priority == CAN_HIGH_PRIORITY ? &can_tx_priority_fifo : &can_tx_fifo;
    SS_can_prepare_tx_bufor(&fifo_bufor_tx, can_header_type, board, data, data_length, 1, priority);
    if((result = SS_fifo_put_data(can_fifo, &fifo_bufor_tx)) == -1) {
        SS_can_error("TX fifo full");
    }
    return result;
}

int8_t SS_can_put_to_fifo_data(enum CAN_HEADER_TYPE can_header_type, uint8_t *data, uint8_t data_length, can_board board, uint8_t priority) {
    volatile Fifo *can_fifo = priority == CAN_HIGH_PRIORITY ? &can_tx_priority_fifo : &can_tx_fifo;
    SS_can_prepare_tx_bufor(&fifo_bufor_tx, can_header_type, board, data, data_length, 0, priority);
    if((result = SS_fifo_put_data(can_fifo, &fifo_bufor_tx)) == -1) {
        SS_can_error("TX fifo full");
    }
    return result;
}

int8_t SS_can_put_to_fifo_no_data(enum CAN_HEADER_TYPE can_header_type, can_board board, uint8_t priority) {
    return SS_can_put_to_fifo_data(can_header_type, NULL, 0, board, priority);
}

void SS_can_prepare_message(CAN_TxHeaderTypeDef *header, can_fifo_bufor *bufor, uint8_t data[]) {
    header->ExtId = bufor->header.binary;
    header->RTR = CAN_RTR_DATA;
    header->IDE = CAN_ID_EXT;
    header->DLC = bufor->length;
    header->TransmitGlobalTime = DISABLE;
    memcpy(data, bufor->data, bufor->length);
}

int8_t SS_can_tx_data_fifo(uint8_t priority) {
    uint32_t mailbox;
    volatile fifo *can_fifo = priority == CAN_HIGH_PRIORITY ? &can_tx_priority_fifo : &can_tx_fifo;
    if (SS_fifo_get_data(can_fifo, &fifo_bufor_tx) == -1) return -1;
    SS_can_prepare_message(&tx_header, &fifo_bufor_tx, tx_data);
    if (fifo_bufor_tx.header.data.board == CAN_BOARD_ID) {
        SS_can_error("Message to self");
        return -1;
    }
    if (HAL_CAN_AddTxMessage(com_hcan, &tx_header, tx_data, &mailbox) != HAL_OK) {
        SS_can_error("HAL_CAN_AddTxMessage failed");
        return -1;
    }
#ifdef CAN_DEBUG
    else
//        print_can_message_sent(&fifo_bufor_tx);
#endif
    return 0;
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
    error = HAL_CAN_GetError(com_hcan);
    SS_can_error("Can error");
    HAL_CAN_ResetError(com_hcan);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == com_hcan)
        SS_can_rx_put_to_fifo_data(hcan, CAN_HIGH_PRIORITY);
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == com_hcan)
        SS_can_rx_put_to_fifo_data(hcan, CAN_LOW_PRIORITY);
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
