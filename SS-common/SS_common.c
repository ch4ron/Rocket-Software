//
// Created by maciek on 26.02.2020.
//
#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif

#include <S25FL/test.h>
#include <can/SS_can.h>
#include <com/SS_com_feed.h>

#include "SS_common.h"
#include "stdio.h"

void SS_init(void) {
#if defined(SS_USE_SUPPLY) && !defined(SIMULATE)
//    SS_supply_init();
#endif
#ifdef RUN_TESTS
    SS_run_all_tests();
#endif
    printf("Elon!\r\n");
    SS_FreeRTOS_init();
}

void SS_main(void) {
#ifdef SS_USE_GRAZYNA
    SS_grazyna_main();
#endif
#ifdef SS_USE_COM
    SS_com_feed_main();
    SS_com_main();
#endif
#ifdef SS_USE_MS5X
    SS_MS56_DMA_read_convert_and_calculate();
    SS_MS56_get_altitude(&ms5607);
#endif
#ifdef SS_USE_CAN
#endif
}
