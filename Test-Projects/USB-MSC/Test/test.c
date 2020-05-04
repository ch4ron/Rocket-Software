/*
 * test.c
 *
 *  Created on: Feb 25, 2020
 *      Author: Mikolaj Wielgus
 */

#include "unity_fixture.h"
#include "test.h"

static void run_all_tests(void);

int test(void)
{
	int argc = 2;
	const char *argv[] = {"", "-v"};
	return UnityMain(argc, argv, run_all_tests);
}

static void run_all_tests(void)
{
	RUN_TEST_GROUP(s25fl);
	RUN_TEST_GROUP(flash_ctrl);
	RUN_TEST_GROUP(flash_caching);
}
