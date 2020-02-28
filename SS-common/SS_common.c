//
// Created by maciek on 26.02.2020.
//
#include "SS_common.h"

#ifdef SS_USE_ADC
#include "SS_adc.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_ADS1258.h"
#include "SS_measurements.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel.h"
#endif
#ifdef SS_USE_SUPPLY
#include "SS_supply.h"
#endif
#ifdef SS_USE_GRAZYNA
#include "SS_Grazyna_com.h"
#endif

void SS_init() {
#if defined(SS_USE_ADC) && !defined(SIMULATE)
    SS_adc_init();
#endif
#ifdef SS_USE_DYNAMIXEL

#endif
#if defined(SS_USE_SUPPLY) && !defined(SIMULATE)
//    SS_supply_init();
#endif
#ifdef SS_USE_GRAZYNA
    SS_grazyna_init();
#endif

#ifdef RUN_TESTS
    SS_run_all_tests();
#endif

/* These modules need to be initialized after tests */
#ifdef SS_USE_ADS1258
//  SS_measurements_init();
#endif
//    printf("Elon!\r\n");
//  SS_settings_read_json(settings_json);
}
