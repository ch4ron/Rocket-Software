/*
 * SS_tests.c
 *
 *  Created on: Dec 25, 2019
 *      Author: maciek
 */

#include "SS_tests.h"

#include "unity_fixture.h"

/* TODO remove this header */
#ifdef SS_USE_ADS1258
#include "SS_ADS1258_unit_tests.h"
#endif
#ifdef SS_USE_USB
#include "SS_usb.h"
    //RUN_TEST_GROUP(usb)
#endif

static void tests(void) {
#ifdef SS_USE_ADS1258
    SS_ADS1258_run_tests();
    RUN_TEST_GROUP(measurements);
#endif
#ifdef SS_USE_GRAZYNA
    RUN_TEST_GROUP(grazyna);
#endif
#ifdef SS_USE_SERVOS
    RUN_TEST_GROUP(servos);
    RUN_TEST_GROUP(grazyna_servos);
#endif
#ifdef SS_USE_SEQUENCE
    RUN_TEST_GROUP(sequence);
#endif
#ifdef SS_USE_COM
    RUN_TEST_GROUP(com);
#endif
#ifdef SS_USE_JSON_SETTINGS
    RUN_TEST_GROUP(parser);
#endif
#ifdef SS_USE_RELAYS
    RUN_TEST_GROUP(relays);
#endif
#ifdef SS_USE_DYNAMIXEL
    RUN_TEST_GROUP(dynamixel);
#endif
#ifdef SS_USE_CAN
    RUN_TEST_GROUP(can);
#endif
#ifdef SS_USE_SUPPLY
    RUN_TEST_GROUP(supply_control);
#endif
#ifdef SS_USE_FLASH
    RUN_TEST_GROUP(s25fl);
    RUN_TEST_GROUP(flash_ctrl);
    RUN_TEST_GROUP(flash_caching);
    RUN_TEST_GROUP(flash_log);
#endif
#ifdef SS_USE_USB
    //RUN_TEST_GROUP(usb)
#endif
#ifdef SS_USE_MPU9250
    RUN_TEST_GROUP(MPU);
#endif
}

/* Enable verbose output */
int SS_run_all_tests(void) {
    // XXX: For time being, disable USB during tests.
    SS_usb_stop();

#ifdef VERBOSE_TEST_OUTPUT
    const char* args[] = {"unity", "-v"};
    int unity_code = UnityMain(2, args, tests);
#else
    int unity_code = UnityMain(0, NULL, tests);
#endif

    // XXX: For time being, reenable USB after tests.
    SS_usb_start();

    return unity_code;
}
