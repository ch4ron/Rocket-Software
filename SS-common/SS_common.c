//
// Created by maciek on 26.02.2020.
//
#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif

#include "SS_com_feed.h"
#include "SS_common.h"
#include "SS_log.h"

void SS_init(void) {
    SS_print("Elon!\r\n");
    SS_FreeRTOS_init();
}

void SS_main(void) {
}
