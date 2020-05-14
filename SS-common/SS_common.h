//
// Created by maciek on 27.02.2020.
//

#ifndef SS_COMMON_H
#define SS_COMMON_H

#ifdef SS_USE_RELAYS
#include "SS_relays.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_ADS1258.h"
#include "SS_measurements.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos.h"
#endif
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif
#ifdef SS_USE_ADC
#include "SS_adc.h"
#include "adc.h"
#endif

void SS_init(void);
void SS_FreeRTOS_init(void);

#ifdef SS_RUN_TESTS
int SS_run_all_tests(void);
#endif

#endif  // SS_COMMON_H
