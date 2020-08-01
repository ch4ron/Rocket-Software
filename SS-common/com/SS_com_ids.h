/*
 * SS_com_ids.h
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#ifndef SS_COM_IDS_H_
#define SS_COM_IDS_H_

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum {
    COM_HIGH_PRIORITY  = 0x00,
    COM_LOW_PRIORITY   = 0x01,
} ComPriority;

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
    COM_RESPONSE      = 0x06,
    /* Send a sequence element */
    COM_SEQUENCE      = 0x07
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
    COM_DYNAMIXEL_ID     = 0x09,
} ComDeviceID;

typedef enum {
    COM_PROHIBITED    = 0x00,
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
    NO_DATA = 0x00,
    UINT32 = 0x01,
    UINT16 = 0x02,
    UINT8 = 0x03,
    INT32 = 0x04,
    INT16 = 0x05,
    INT8 = 0x06,
    FLOAT = 0x07,
    INT16x2 = 0x08,
} ComDataType;

#endif /* SS_COM_IDS_H_ */
