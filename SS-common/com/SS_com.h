/*
 * SS_com_protocol.h
 *
 *  Created on: Jan 16, 2020
 *      Author: maciek
 */

#ifndef SS_COM_H
#define SS_COM_H

#include "stm32f4xx_hal.h"
#include "SS_com_ids.h"

typedef struct __attribute__((packed)) {
    ComActionID action :  3;
    ComBoardID source :  5;
    ComBoardID destination :  5;
    ComPriority priority :  3;
    ComDeviceID device :  6;
    uint32_t id :  6;
    /* Set to 1 for frames from and to Grazyna, 0 otherwise */
    uint32_t grazyna_ind :  1;

    ComDataType data_type :  3;
    uint8_t message_type;
    uint32_t payload;
} ComFrame;

typedef enum {
    COM_OK,
    COM_ERROR
} ComStatus;

void SS_com_init(ComBoardID board);
void SS_com_transmit(ComFrame *frame);
ComStatus SS_com_handle_frame(ComFrame *frame);
ComStatus SS_com_handle_action(ComFrame *frame);
ComStatus SS_com_handle_request(ComFrame *frame);
ComStatus SS_com_handle_service(ComFrame *frame);
void SS_com_add_payload_to_frame(ComFrame *frame, ComDataType type, void *payload);

#endif /* SS_COM_H */
