/*
 * SS_tests.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "unity_fixture.h"

/* TODO remove this header */
#ifdef SS_USE_ADS1258
#include "SS_ADS1258_unit_tests.h"
#endif

#ifdef SIMULATE
#include "jumper.h"
#endif

static void tests() {
#ifdef SS_USE_ADS1258
    /* SS_ADS1258_run_tests(); */
    /* RUN_TEST_GROUP(measurements); */
#endif
#ifdef SS_USE_GRAZYNA
    RUN_TEST_GROUP(grazyna);
#endif
#ifdef SS_USE_SERVOS
    /* RUN_TEST_GROUP(servos); */
    /* RUN_TEST_GROUP(grazyna_servos); */
#endif
#ifdef SS_USE_SEQUENCE
    /* RUN_TEST_GROUP(sequence); */
#endif
#ifdef SS_USE_COM
    /* RUN_TEST_GROUP(com); */
#endif
#ifdef SS_USE_JSON_SETTINGS
    /* RUN_TEST_GROUP(parser); */
#endif
#ifdef SS_USE_FIFO
    /* RUN_TEST_GROUP(fifo); */
#endif
#ifdef SS_USE_RELAYS
    /* RUN_TEST_GROUP(relays); */
#endif
#ifdef SS_USE_DYNAMIXEL
    /* RUN_TEST_GROUP(dynamixel); */
    /* RUN_TEST_GROUP(dynamixel_logic); */
#endif
#ifdef SS_USE_CAN
    /* RUN_TEST_GROUP(can); */
#endif
    /* Disable mutex test for now, as it takes a long time and causes troubles
 * (But current implementation works correctly */
    //    RUN_TEST_GROUP(mutex);
    //    RUN_TEST_GROUP(supply_control);
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
