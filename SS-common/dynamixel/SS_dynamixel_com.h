/*
 * SS_dynamixel_com.h
 *
 *  Created on: 01.05.2020
 *      Author: Maciek
 */

#ifndef SS_DYNAMIXEL_COM_H_
#define SS_DYNAMIXEL_COM_H_

#include "SS_com.h"

typedef enum {
    COM_DYNAMIXEL_OPEN = 0x01,
    COM_DYNAMIXEL_CLOSE,
    COM_DYNAMIXEL_OPENED_POSITION,
    COM_DYNAMIXEL_CLOSED_POSITION,
    COM_DYNAMIXEL_POSITION,
    /* COM_DYNAMIXEL_DISABLE, */
} ComDynamixelID;

ComStatus SS_dynamixel_com_service(ComFrame *frame);
ComStatus SS_dynamixel_com_request(ComFrame *frame);

#endif
