//
// Created by maciek on 26.02.2020.
//
#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif

#include "SS_can.h"
#include "SS_com_feed.h"
#include "SS_common.h"
#include "stdio.h"

void SS_init(void) {
#if defined(SS_USE_SUPPLY)
//    SS_supply_init();
#endif
#ifdef SS_RUN_TESTS
    SS_run_all_tests();
#endif
    printf("Elon!\r\n");
    SS_FreeRTOS_init();
}

void SS_main(void) {
#ifdef SS_USE_COM
    SS_com_feed_main();
#endif
#ifdef SS_USE_MS5X
    SS_MS56_DMA_read_convert_and_calculate();
    SS_MS56_get_altitude(&ms5607);
#endif
#ifdef SS_USE_CAN
#endif
}
