/*
 * SS_com_ids.h
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#ifndef SS_COM_IDS_H_
#define SS_COM_IDS_H_

typedef enum {
    /* Data feed from board to Ground Station */
    COM_FEED          = 0x00,
    /* Action which doesn't send any data, respond with ACK */
    COM_SERVICE       = 0x01,
    /* Acknowledge message */
    COM_ACK           = 0x02,
    /* Send if an error occurred */
    COM_NACK          = 0x03,
    /* Message for checking communication with Ground Station */
    COM_HEARTBEAT     = 0x04,
    /* Action that needs to send back data, respond with RESPONSE */
    COM_REQUEST       = 0x05,
    /* Acknowledge message and send back data */
    COM_RESPONSE      = 0x06
} ComActionID;

typedef enum {
    COM_SERVO_ID         = 0x00,
    COM_RELAY_ID         = 0x01,
    COM_MEASUREMENT_ID   = 0x02,
    COM_SUPPLY_ID        = 0x03,
    COM_MEMORY_ID        = 0x04,
    COM_IGNITER_ID       = 0x05,
    COM_TENSOMETER_ID    = 0x06,
    COM_BAROMETER_ID     = 0x07,
    COM_MPU9250_ID       = 0x08,
} ComDeviceID;

typedef enum {
    COM_GRAZYNA_ID    = 0x01,
    COM_STASZEK_ID    = 0x02,
    COM_RADEK_ID      = 0x03,
    COM_CZAPLA_ID     = 0x04,
    COM_PAUEK_ID      = 0x05,
    COM_KROMEK_ID     = 0x06,
    COM_LAST_BOARD,
    COM_BROADCAST_ID  = 0x1F
} ComBoardID;

typedef enum {
    NO_DATA,
    UINT32,
    UINT16,
    UINT8,
    INT32,
    INT16,
    INT8,
    FLOAT
} ComDataType;

#endif /* SS_COM_IDS_H_ */
