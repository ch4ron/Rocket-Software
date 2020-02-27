/*
 * SS_Kromek_tests.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "SS_config.h"

#ifdef RUN_TESTS

#include "unity_fixture.h"
#include "SS_ADS1258_unit_tests.h"

#ifdef SIMULATE
#include "jumper.h"
#endif

static void tests() {
#ifndef SIMULATE
    RUN_TEST_GROUP(servos);
    SS_ADS1258_run_tests();
    RUN_TEST_GROUP(measurements);
    RUN_TEST_GROUP(dynamixel);
    RUN_TEST_GROUP(mutex);
    RUN_TEST_GROUP(supply_control);
#endif
    RUN_TEST_GROUP(servos);
    RUN_TEST_GROUP(grazyna_servos);
    RUN_TEST_GROUP(com);
    RUN_TEST_GROUP(sequence);
    RUN_TEST_GROUP(parser);
    RUN_TEST_GROUP(fifo);
    RUN_TEST_GROUP(dynamixel_logic);
}

#ifdef SIMULATE
/* Enable verbose output */
const char* args[] = { "unity", "-v" };
int SS_Kromek_run_all_tests() {
    int unity_code =  UnityMain(2, args, tests);
    /* Exit with status code */
    jumper_sudo_exit_with_exit_code(unity_code);
}
#else
int SS_Kromek_run_all_tests() {
    int unity_code =  UnityMain(0, NULL, tests);
    jumper_sudo_exit_with_exit_code(unity_code);
    return unity_code;
}
#endif

#endif
