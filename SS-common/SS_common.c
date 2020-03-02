//
// Created by maciek on 26.02.2020.
//
#include "SS_common.h"
#include "stdio.h"

void SS_init() {
#ifdef SS_USE_DYNAMIXEL

#endif
#if defined(SS_USE_SUPPLY) && !defined(SIMULATE)
//    SS_supply_init();
#endif

#ifdef RUN_TESTS
    SS_run_all_tests();
#endif

/* These modules need to be initialized after tests */
#ifdef SS_USE_ADS1258
//  SS_ADS1258_measurements_init();
#endif
    printf("Elon!\r\n");
//  SS_settings_read_json(settings_json);
}

void SS_main() {
#ifdef SS_USE_GRAZYNA
    SS_grazyna_main();
#endif
}
