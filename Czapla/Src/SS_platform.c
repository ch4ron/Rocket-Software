//
// Created by maciek on 28.02.2020.
//

/* #include "SS_s25fl.h" */
#include "SS_can.h"
#include "SS_common.h"
#include "SS_platform.h"
#include "usart.h"

/********** PRINTF *********/

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t*) ptr, (uint16_t) len, 1000);
    return len;
}
/********** MAIN INIT *********/

void SS_platform_init() {
#ifndef SIMULATE
    SS_can_init(&hcan2, COM_CZAPLA_ID);
#endif
    /* SS_grazyna_init(&huart2); */
    /* SS_s25fl_init(); */
}
