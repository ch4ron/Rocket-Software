/***************************************
|-- AUTHOR: ANDRZEJ LACZEWSKI
|-- WEBSITE: WWW.BYTECHLAB.COM
|-- PROJECT: MAGELLAN-1 NANO SATELITE
|-- MODULE: GPS UBLOX MAX-M8C func
|-- DESCRIPTION:
.................
***************************************/
#include "gps.h"

uint8_t GPS_data_buff[100];  // 100 exact length

void UBLOX_append_frame_crc(unsigned char *data) {
    unsigned short i, j;
    unsigned char a, b;
    a = 0;
    b = 0;

    j = (unsigned short) data[4] + ((unsigned short) data[5] << 8) + 6;  // extract length fields

    for(i = 2; i < j; i++)  // Sum over header and payload
    {
        a += data[i];
        b += a;
    }

    // ADD CRC to end of frame
    data[i + 0] = a;
    data[i + 1] = b;
}

void GPS_init(void) {
    // Turn on device
    HAL_GPIO_WritePin(GPS_RST_GPIO_Port, GPS_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(250);
    HAL_GPIO_WritePin(GPS_PWR_GPIO_Port, GPS_PWR_Pin, GPIO_PIN_RESET);
    HAL_Delay(250);
    HAL_GPIO_WritePin(GPS_RST_GPIO_Port, GPS_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(1500);  // delay long enough to power up gps

    //**************************************************
    // disable MNEMEA messages  one by one by setting update rate to 0 in USART interface

    //CONFIG FRAME STRUCTURE:  [HEADER,      CLASS,  ID,    LENGTH,      PAYLOAD,                                    CHECKSUM ]
    uint8_t UBX_CFG_MSG[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x04, 0x33};
    //PORT:                                                                         I2C UART1 USB SPI    --   --

    UBLOX_append_frame_crc(UBX_CFG_MSG);
    HAL_UART_Transmit(&GPS_UART, UBX_CFG_MSG, 16, 200);  // GGA
    HAL_Delay(50);

    UBX_CFG_MSG[7]++;
    UBLOX_append_frame_crc(UBX_CFG_MSG);
    HAL_UART_Transmit(&GPS_UART, UBX_CFG_MSG, 16, 200);  // GLL
    HAL_Delay(50);

    UBX_CFG_MSG[7]++;
    UBLOX_append_frame_crc(UBX_CFG_MSG);
    HAL_UART_Transmit(&GPS_UART, UBX_CFG_MSG, 16, 200);  // GSA
    HAL_Delay(50);

    UBX_CFG_MSG[7]++;
    UBLOX_append_frame_crc(UBX_CFG_MSG);
    HAL_UART_Transmit(&GPS_UART, UBX_CFG_MSG, 16, 200);  // GSV
    HAL_Delay(50);

    UBX_CFG_MSG[7]++;
    UBLOX_append_frame_crc(UBX_CFG_MSG);
    HAL_UART_Transmit(&GPS_UART, UBX_CFG_MSG, 16, 200);  // RMC
    HAL_Delay(50);

    UBX_CFG_MSG[7]++;
    UBLOX_append_frame_crc(UBX_CFG_MSG);
    HAL_UART_Transmit(&GPS_UART, UBX_CFG_MSG, 16, 200);  // VTG
    HAL_Delay(50);

    //**************************************************
    // ENABLE 1 PPS when FIX NOT ACQUIRED

    //	// EVERY PARAMETER LSB FIRST !!!!!!!!!!!!!!!
    //	//CONFIG FRAME STRUCTURE:  [HEADER,      CLASS,  ID,    LENGTH,      PAYLOAD,                                                                                                                                                                                                 CHECKSUM ]
    //	uint8_t UBX_CFG_TP5[]    = {0xb5,0x62,   0x06,   0x31,  0x20,0x00,   0x00,  0x01,    0x00,0x00,  0x00,0x00,     0x00,0x00,     0x01,0x00,0x00,0x00,   0x01,0x00,0x00,0x00,   0xA0,0x86,0x01,0x00,   0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,    0x79,0x00,0x00,0x00,     0xFA,0xE2};
    //    //                                                                   tpIdx, version, reserved,   antCableDelay, rfGroupDelay,  freqPeriod             freqPeriodLock,        pulseLenRatio,         pulseLenRatioLock,      userConfigDelay,        flags,
    //	HAL_Delay(500);
    //	UBLOX_append_frame_crc(UBX_CFG_TP5);
    //	HAL_UART_Transmit(&GPS_UART,UBX_CFG_TP5,40,200);

    //**************************************************
    // ENABLE CYCLIC BINARY MESSAGES

    //	//CONFIG FRAME STRUCTURE:     [HEADER,      CLASS,  ID,    LENGTH,      PAYLOAD,                                    CHECKSUM ]
    //	uint8_t UBX_NAV_POSLLH[]    = {0xb5,0x62,   0x06,   0x01,  0x08,0x00,   0x01,0x02, 0x00,0x01,0x00,0x00,0x00,0x00,    0x13,0xbe};
    //    //PORT:                                                                            I2C UART1 USB SPI    --   --
    //	UBLOX_append_frame_crc(UBX_NAV_POSLLH);
    //	HAL_UART_Transmit(&GPS_UART,UBX_NAV_POSLLH,16,20); ///UBX-NAV-POSLLH

    //CONFIG FRAME STRUCTURE:  [HEADER,      CLASS,  ID,    LENGTH,      PAYLOAD,                                    CHECKSUM ]
    //	uint8_t UBX_NAV_PVT[]    = {0xb5,0x62,   0x06,   0x01,  0x08,0x00,   0x01,0x07, 0x00,0x01,0x00,0x00,0x00,0x00,    0x18,0xE1};
    //    //PORT:                                                                         I2C UART1 USB SPI    --   --
    //	UBLOX_append_frame_crc(UBX_NAV_PVT);
    //	HAL_UART_Transmit(&GPS_UART,UBX_NAV_PVT,16,20); ///UBX-NAV-PVT

    //**************************************************
    // CONFIG
    HAL_UART_Transmit(&GPS_UART, (uint8_t[]){0xb5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xff, 0xff, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xfa, 0x00, 0xfa, 0x00, 0x64, 0x00, 0x2c, 0x01, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x6c}, 44, 55);
    HAL_Delay(500);
}

void GPS_get_nav_pvt_dat(GPS_NAV_PVT_Data_t *GPS_Data_pointer) {
    GPS_Data_pointer->iTOW = ((GPS_data_buff[6 + 3] << 24) | (GPS_data_buff[6 + 2] << 16) | (GPS_data_buff[6 + 1] << 8) | (GPS_data_buff[6]));
    GPS_Data_pointer->year = ((GPS_data_buff[6 + 4 + 1] << 8) | (GPS_data_buff[6 + 4]));
    GPS_Data_pointer->month = GPS_data_buff[6 + 6];
    GPS_Data_pointer->day = GPS_data_buff[6 + 7];
    GPS_Data_pointer->hour = GPS_data_buff[6 + 8];
    GPS_Data_pointer->min = GPS_data_buff[6 + 9];
    GPS_Data_pointer->sec = GPS_data_buff[6 + 10];
    GPS_Data_pointer->valid = GPS_data_buff[6 + 11];
    GPS_Data_pointer->tAcc = ((GPS_data_buff[6 + 12 + 3] << 24) | (GPS_data_buff[6 + 12 + 2] << 16) | (GPS_data_buff[6 + 12 + 1] << 8) | (GPS_data_buff[6 + 12]));
    GPS_Data_pointer->nano = ((GPS_data_buff[6 + 16 + 3] << 24) | (GPS_data_buff[6 + 16 + 2] << 16) | (GPS_data_buff[6 + 16 + 1] << 8) | (GPS_data_buff[6 + 16]));
    GPS_Data_pointer->fixType = GPS_data_buff[6 + 20];
    GPS_Data_pointer->flags = GPS_data_buff[6 + 21];
    GPS_Data_pointer->flags2 = GPS_data_buff[6 + 22];
    GPS_Data_pointer->num_of_sat = GPS_data_buff[6 + 23];
    GPS_Data_pointer->longitude = ((GPS_data_buff[6 + 24 + 3] << 24) | (GPS_data_buff[6 + 24 + 2] << 16) | (GPS_data_buff[6 + 24 + 1] << 8) | (GPS_data_buff[6 + 24]));
    GPS_Data_pointer->latitude = ((GPS_data_buff[6 + 28 + 3] << 24) | (GPS_data_buff[6 + 28 + 2] << 16) | (GPS_data_buff[6 + 28 + 1] << 8) | (GPS_data_buff[6 + 28]));
    GPS_Data_pointer->height_elipsoid = ((GPS_data_buff[6 + 32 + 3] << 24) | (GPS_data_buff[6 + 32 + 2] << 16) | (GPS_data_buff[6 + 32 + 1] << 8) | (GPS_data_buff[6 + 32]));
    GPS_Data_pointer->height_sea_lvl = ((GPS_data_buff[6 + 36 + 3] << 24) | (GPS_data_buff[6 + 36 + 2] << 16) | (GPS_data_buff[6 + 36 + 1] << 8) | (GPS_data_buff[6 + 36]));
    GPS_Data_pointer->hAcc = ((GPS_data_buff[6 + 40 + 3] << 24) | (GPS_data_buff[6 + 40 + 2] << 16) | (GPS_data_buff[6 + 40 + 1] << 8) | (GPS_data_buff[6 + 40]));
    GPS_Data_pointer->vAcc = ((GPS_data_buff[6 + 44 + 3] << 24) | (GPS_data_buff[6 + 44 + 2] << 16) | (GPS_data_buff[6 + 44 + 1] << 8) | (GPS_data_buff[6 + 44]));
    GPS_Data_pointer->velN = ((GPS_data_buff[6 + 48 + 3] << 24) | (GPS_data_buff[6 + 48 + 2] << 16) | (GPS_data_buff[6 + 48 + 1] << 8) | (GPS_data_buff[6 + 48]));
    GPS_Data_pointer->velE = ((GPS_data_buff[6 + 52 + 3] << 24) | (GPS_data_buff[6 + 52 + 2] << 16) | (GPS_data_buff[6 + 52 + 1] << 8) | (GPS_data_buff[6 + 52]));
    GPS_Data_pointer->velD = ((GPS_data_buff[6 + 56 + 3] << 24) | (GPS_data_buff[6 + 56 + 2] << 16) | (GPS_data_buff[6 + 56 + 1] << 8) | (GPS_data_buff[6 + 56]));
    GPS_Data_pointer->gSpeed = ((GPS_data_buff[6 + 60 + 3] << 24) | (GPS_data_buff[6 + 60 + 2] << 16) | (GPS_data_buff[6 + 60 + 1] << 8) | (GPS_data_buff[6 + 60]));
    GPS_Data_pointer->headMot = ((GPS_data_buff[6 + 64 + 3] << 24) | (GPS_data_buff[6 + 64 + 2] << 16) | (GPS_data_buff[6 + 64 + 1] << 8) | (GPS_data_buff[6 + 64]));
    GPS_Data_pointer->sAcc = ((GPS_data_buff[6 + 68 + 3] << 24) | (GPS_data_buff[6 + 68 + 2] << 16) | (GPS_data_buff[6 + 68 + 1] << 8) | (GPS_data_buff[6 + 68]));
    GPS_Data_pointer->headAcc = ((GPS_data_buff[6 + 72 + 3] << 24) | (GPS_data_buff[6 + 72 + 2] << 16) | (GPS_data_buff[6 + 72 + 1] << 8) | (GPS_data_buff[6 + 72]));
    GPS_Data_pointer->pDOP = ((GPS_data_buff[6 + 76 + 1] << 8) | (GPS_data_buff[6 + 76]));
    GPS_Data_pointer->headVeh = ((GPS_data_buff[6 + 84 + 3] << 24) | (GPS_data_buff[6 + 84 + 2] << 16) | (GPS_data_buff[6 + 84 + 1] << 8) | (GPS_data_buff[6 + 84]));
    GPS_Data_pointer->magDec = ((GPS_data_buff[6 + 88 + 1] << 8) | (GPS_data_buff[6 + 88]));
    GPS_Data_pointer->magAcc = ((GPS_data_buff[6 + 90 + 1] << 8) | (GPS_data_buff[6 + 90]));
}

void GPS_poll_nav_pvt_msg(GPS_NAV_PVT_Data_t *GPS_Data_pointer) {
    uint8_t UBX_NAV_PVT_request[] = {0xb5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x18, 0xE1};
    UBLOX_append_frame_crc(UBX_NAV_PVT_request);
    HAL_UART_Transmit(&GPS_UART, UBX_NAV_PVT_request, 8, 20);  ///UBX-NAV-PVT

    HAL_Delay(10);
    HAL_UART_Receive(&GPS_UART, GPS_data_buff, 100, 2000);
    //	if(HAL_UART_Receive(&GPS_UART,GPS_data_buff,100,2000) == HAL_OK)
    //	{
    //		__print_debug_to_UART("UART OK !\r\n");
    //	}
    HAL_Delay(10);

    //	while (HAL_UART_GetState(&GPS_UART) != HAL_UART_STATE_READY)
    //	{
    //		__print_debug_to_UART("BUSY !\r\n");
    ////		HAL_Delay(100);
    //	}

    GPS_get_nav_pvt_dat(GPS_Data_pointer);
}

uint8_t GPS_get_nav_pvt_valid(GPS_NAV_PVT_Data_t *GPS_Data_pointer, uint8_t flag_to_test) {
    if((GPS_Data_pointer->valid & flag_to_test) == flag_to_test) {
        return TRUE;
    } else {
        return FALSE;
    }
}

uint8_t GPS_get_nav_pvt_flags(GPS_NAV_PVT_Data_t *GPS_Data_pointer, uint8_t flag_to_test) {
    if((GPS_Data_pointer->flags & flag_to_test) == flag_to_test) {
        return TRUE;
    } else {
        return FALSE;
    }
}

uint8_t GPS_get_nav_pvt_flags2(GPS_NAV_PVT_Data_t *GPS_Data_pointer, uint8_t flag_to_test) {
    if((GPS_Data_pointer->flags2 & flag_to_test) == flag_to_test) {
        return TRUE;
    } else {
        return FALSE;
    }
}

GPS_DMS_COORD_t GPS_conv_raw_coord_to_dms(GPS_NAV_PVT_Data_t GPS_Data) {
    //	int32_t lat_raw = -501503992; // +- 90
    //	int32_t longi_raw = -199532102; //+- 180

    GPS_DMS_COORD_t DMS_coordinates;

    if(GPS_Data.latitude > 0) {
        DMS_coordinates.latitude_designator = 'N';
    } else {
        DMS_coordinates.latitude_designator = 'S';
    }

    if(GPS_Data.longitude > 0) {
        DMS_coordinates.longitude_designator = 'E';
    } else {
        DMS_coordinates.longitude_designator = 'W';
    }

    float lat_decimal_deg_abs = abs(GPS_Data.latitude) / 10000000.0;
    float longi_decimal_deg_abs = abs(GPS_Data.longitude) / 10000000.0;

    DMS_coordinates.latitude_deg = (uint16_t) lat_decimal_deg_abs;
    DMS_coordinates.longitude_deg = (uint16_t) longi_decimal_deg_abs;

    float latitude_min_fp = (lat_decimal_deg_abs - DMS_coordinates.latitude_deg) * 60.0;
    float longitude_min_fp = (longi_decimal_deg_abs - DMS_coordinates.longitude_deg) * 60.0;

    DMS_coordinates.latitude_min = (uint16_t) latitude_min_fp;
    DMS_coordinates.longitude_min = (uint16_t) longitude_min_fp;

    float latitude_sec_fp = (latitude_min_fp - DMS_coordinates.latitude_min) * 60.0;
    float longitude_sec_fp = (longitude_min_fp - DMS_coordinates.longitude_min) * 60.0;

    DMS_coordinates.latitude_sec = (uint16_t) latitude_sec_fp;
    DMS_coordinates.longitude_sec = (uint16_t) longitude_sec_fp;

    return DMS_coordinates;
}

// ----------------------------------------------------------------------------------------

// ********************
// make HAL_callbacks.c file for callbacks

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // interupt on rising edge
//{
//	if(GPIO_Pin == GPS_PULSE_Pin)
//	{
////		HAL_GPIO_TogglePin(GPIOA, LED_1_Pin|LED_2_Pin);
////	    UBLOX_receive();
//	}
//}

//https://www.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_%28UBX-13003221%29_Public.pdf#page=370&zoom=100,0,0
//str 371 -> num of sat
//time hh:mm:ss
