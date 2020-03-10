/*
 * SS_com_protocol.h
 *
 *  Created on: Jan 16, 2020
 *      Author: maciek
 */

#ifndef SS_COM_PROTOCOL_H_
#define SS_COM_PROTOCOL_H_

#include "stm32f4xx_hal.h"
#include "SS_com_ids.h"

typedef struct __attribute__((packed)) {
    uint32_t header;
    uint8_t message_type;
    uint32_t payload;
} ComFrame;

typedef struct {
    uint8_t priority; // 3 bits
    ComBoardID destination; // 5 bits
    ComBoardID source; // 5 bits
    ComActionID action; // 3 bits
    ComDeviceID device; // 6 bits
    uint8_t id; // 6 bits
    /* Set to 1 for frames from and to Grazyna, 0 otherwise */
    uint8_t grazyna_ind; // 1 bit
    ComDataType data_type; // 3 bits
    uint8_t message_type; // 8 bits
    uint32_t payload; // 32 bits
} ComFrameContent;

typedef enum {
    COM_OK,
    COM_ERROR
} ComStatus;

void SS_com_init(ComBoardID board);
void SS_com_transmit(ComFrameContent *frame_content);
ComStatus SS_com_handle_frame(ComFrame *frame);
ComStatus SS_com_handle_action(ComFrameContent *frame);
ComStatus SS_com_handle_request(ComFrameContent *frame);
ComStatus SS_com_handle_service(ComFrameContent *frame);
void SS_com_add_payload_to_frame(ComFrameContent *frame, ComDataType type, void *payload);
void SS_com_parse_frame(ComFrame *frame, ComFrameContent *content);
void SS_com_create_frame(ComFrame *frame, ComFrameContent *content);

#endif /* SS_COM_PROTOCOL_H_ */
