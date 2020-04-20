/**
  * SS_com_tests.c
  *
  *  Created on: Apr 15, 2020
  *      Author: maciek
 **/

#include "SS_com.c"
#include "unity_fixture.h"

/* ==================================================================== */
/* ============================== Setup =============================== */
/* ==================================================================== */

TEST_GROUP(com);

TEST_GROUP_RUNNER(com) {
    RUN_TEST_CASE(com, whatever);
}

TEST_SETUP(com) {}
TEST_TEAR_DOWN(com) {}

/* ==================================================================== */
/* ============================== Tests =============================== */
/* ==================================================================== */

TEST(com, com_message_received) {
}

void tests(void) {
    RUN_TEST_GROUP(com);
}

int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, tests);
}
