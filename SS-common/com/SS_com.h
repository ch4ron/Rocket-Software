/*
 * SS_com_protocol.h
 *
 *  Created on: Jan 16, 2020
 *      Author: maciek
 */

#ifndef SS_COM_H
#define SS_COM_H

#include "FreeRTOS.h"
#include "SS_com_ids.h"
#include "SS_fifo.h"
#include "queue.h"
#include "stm32f4xx_hal.h"

typedef struct __attribute__((packed)) {
    ComBoardID destination : 5;
    ComPriority priority : 3;
    ComActionID action : 3;
    ComBoardID source : 5;
    ComDeviceID device : 6;
    uint32_t id : 6;
    ComDataType data_type : 4;
    uint8_t message_type;
    uint32_t payload;
} ComFrame;

typedef struct {
    volatile Fifo *fifo;
    void (*fun)(ComFrame*);
    uint8_t group_id;
    ComPriority priority;
} ComFifoManager;

typedef enum {
    COM_OK,
    COM_ERROR
} ComStatus;

typedef enum {
    COM_GROUP_RECEIVE,
    COM_GROUP_CAN1,
    COM_GROUP_CAN2,
    COM_GROUP_GRAZYNA
} ComGroup;

QueueHandle_t SS_com_add_sender();
void SS_com_add_to_queue(ComFrame *frame, void (*sender_fun)(ComFrame *), QueueHandle_t queue);
void SS_com_message_received(ComFrame *frame);
void SS_com_init(ComBoardID board);
void SS_com_transmit(ComFrame *frame);
ComStatus SS_com_handle_frame(ComFrame *frame);
ComStatus SS_com_handle_action(ComFrame *frame);
ComStatus SS_com_handle_request(ComFrame *frame);
ComStatus SS_com_handle_service(ComFrame *frame);
void SS_com_add_payload_to_frame(ComFrame *frame, ComDataType type, void *payload);

/* Add fifo and callback to be handled by com manager, set group id to COM_GROUP_RECEIVE and fun to NULL for rx fifos */
void SS_com_add_fifo(volatile Fifo *fifo, void (*fun)(ComFrame *), ComGroup group_id, ComPriority priority);
void SS_com_main();
#endif /* SS_COM_H */
