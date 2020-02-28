/*
 * SS_tests.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "SS_config.h"


#include "unity_fixture.h"
#include "SS_ADS1258_unit_tests.h"

#ifdef SIMULATE
#include "jumper.h"
#endif

static void tests() {
#ifndef SIMULATE
    SS_ADS1258_run_tests();
    RUN_TEST_GROUP(measurements);
//    RUN_TEST_GROUP(dynamixel);
/* Disable mutex test for now, as it takes a long time and causes troubles
 * (But current implementation works correctly */
//    RUN_TEST_GROUP(mutex);
    RUN_TEST_GROUP(supply_control);
#endif
    RUN_TEST_GROUP(servos);
    RUN_TEST_GROUP(grazyna_servos);
    RUN_TEST_GROUP(com);
    RUN_TEST_GROUP(sequence);
    RUN_TEST_GROUP(parser);
    RUN_TEST_GROUP(fifo);
#ifdef SS_USE_DYNAMIXEL
    RUN_TEST_GROUP(dynamixel_logic);
#endif
}

/* Enable verbose output */
int SS_run_all_tests() {
#ifdef VERBOSE_TEST_OUTPUT
    const char* args[] = { "unity", "-v" };
    int unity_code =  UnityMain(2, args, tests);
#else
    int unity_code =  UnityMain(0, NULL, tests);
#endif
#ifdef SIMULATE
    /* Exit with status code */
    jumper_sudo_exit_with_exit_code(unity_code);
#endif
    return unity_code;
}
