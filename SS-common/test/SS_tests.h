#ifndef SS_TESTS_H
#define SS_TESTS_H

#include "SS_common.h"

int SS_run_all_tests(void);
void SS_run_tests_task(void *pvParameters);
bool SS_get_are_tests_running(void);

#endif /* SS_TESTS_H */
