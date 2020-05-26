/*
 * SS_misc.h
 *
 *  Created on: Feb 6, 2020
 *      Author: maciek
 */

#ifndef SS_MISC_H
#define SS_MISC_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "stdbool.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

#define SS_led_generate_header(name)                                    \
    void SS_platform_set_##name##_led(bool red, bool green, bool blue); \
    void SS_led_set_##name(bool red, bool green, bool blue);            \
    void SS_led_toggle_##name(bool red, bool green, bool blue);         \

/* ==================================================================== */
/* ==================== Global function prototypes ==================== */
/* ==================================================================== */

SS_led_generate_header(mem)
SS_led_generate_header(adc)
SS_led_generate_header(com)

void SS_platform_toggle_loop_led();
void SS_led_set_all(bool red, bool green, bool blue);
void SS_led_toggle_all(bool red, bool green, bool blue);

#endif
