/*
 * SD_card_func.c
 *
 *  Created on: 18.04.2019
 *      Author: Andrzej
 */
#include "SD_card_func.h"

#include "PITOT_func.h"

char SD_current_file_path[50];

void SD_CARD_send_file_debug(FRESULT file_op_result, SD_file_type_of_op operation) {
    switch(operation) {
        case INIT:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Init: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Init: OK, ");
            }

            break;

        case MOUNT:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Mount: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Mount: OK, ");
            }
            break;

        case UNMOUNT:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Unmount: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Unmount: OK, ");
            }
            break;

        case READ:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Read: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Read: OK, ");
            }
            break;

        case WRITE:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Write: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Write: OK, ");
            }
            break;

        case CLOSE:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Close: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Close: OK, ");
            }
            break;

        case OPEN:
            if(file_op_result != FR_OK) {
                SD_CARD_ERROR_LED_ON;
                UART_send_debug_string("Open: ERROR, ");
            } else {
                SD_CARD_ERROR_LED_OFF;
                UART_send_debug_string("Open: OK, ");
            }
            break;

        default:

            break;
    }
}

void SD_CARD_init(void) {
    UART_send_debug_string("___ SD CARD INIT PROCEDURE START ___\r\n");

    FRESULT res;
    //------------------------------------------------------->>
    res = BSP_SD_Init();
    SD_CARD_send_file_debug(res, INIT);
    //------------------------------------------------------->>
    res = f_mount(&SDFatFS, SDPath, 1);
    SD_CARD_send_file_debug(res, MOUNT);
    //------------------------------------------------------->>
    UART_send_debug_string("\r\nFILENAMES FOUND:\r\n");

    uint8_t next_file_num = 0;

    char filenames_str[100];
    uint16_t filenames_str_len;

    FRESULT fr;   // Return value
    DIR dj;       // Directory search object
    FILINFO fno;  // File information

    fr = f_findfirst(&dj, &fno, "", "*.txt");  // Start to search for photo files

    while(fr == FR_OK && fno.fname[0]) {                              //Repeat while an item is found
        filenames_str_len = sprintf(filenames_str, "%s", fno.fname);  // Display the object name
        HAL_UART_Transmit(&huart4, (uint8_t *) filenames_str, filenames_str_len, MAX_TIMEOUT);

        fr = f_findnext(&dj, &fno);  //Search for next item

        next_file_num++;
    }

    f_closedir(&dj);

    UART_send_debug_string("\r\n");
    //------------------------------------------------------->>
    sprintf(SD_current_file_path, "%d.txt", next_file_num);
    res = f_open(&SDFile, SD_current_file_path, FA_CREATE_ALWAYS | FA_WRITE);

    SD_CARD_send_file_debug(res, OPEN);
    //------------------------------------------------------->>
    char myData[] = "<-----START LOGGING----->\r\nTIME[ms],BAT[v],CONV_VOUT[v],PRESSURE[mBar],Attlitude[m],Temp[C],PITOT_pressure_diff[PSI],PITOT_sensor_temp[C]\r\n";  // CSV FILE HEADER FIRST LINE
    UINT BytesWritten;

    res = f_write(&SDFile, myData, sizeof(myData), &BytesWritten);

    SD_CARD_send_file_debug(res, WRITE);
    //------------
    res = f_close(&SDFile);

    SD_CARD_send_file_debug(res, CLOSE);
    UART_send_debug_string("\r\n");
}
