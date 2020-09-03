//
// Created by maciek on 26.02.2020.
//
#ifdef SS_USE_ADC
#include "SS_adc.h"
#endif
#ifdef SS_USE_SEQUENCE
#include "SS_sequence.h"
#endif

#include "SS_init.h"
#include "SS_log.h"
#include "SS_misc.h"

void SS_init(void) {
    SS_print("Elon!\r\n");
    SS_led_set_adc(0, 0, 0);
    SS_led_set_mem(0, 0, 0);
    SS_led_set_com(0, 0, 0);
#ifdef SS_USE_ADC
    SS_adc_start();
#endif
#ifdef SS_USE_SEQUENCE
    SS_sequence_init();
#endif
    SS_FreeRTOS_init();
}
