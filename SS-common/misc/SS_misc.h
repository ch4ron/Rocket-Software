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
#include "stdint.h"
#include "tim.h"

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */
#define BUZZER_TIM htim10
#define BUZZER_TIM_REGISTER TIM10
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
SS_led_generate_header(meas)



void SS_set_beep_number(uint16_t beeps);
void SS_set_beep_gap(uint8_t gap);

void SS_platform_toggle_loop_led();
void SS_led_set_all(bool red, bool green, bool blue);
void SS_led_toggle_all(bool red, bool green, bool blue);
void SS_buzzer_start_count(uint16_t hertz_100, uint16_t beeps,uint8_t gap);


/*
void SS_buzzer_start(uint16_t hertz_100);                                      These were unused in old Repo and i consider deleting it
oid SS_buzzer_start_gap_count(uint16_t hertz_100, uint8_t beeps, uint8_t gap);
void SS_buzzer_start_gap(uint16_t hertz_100, uint8_t gap);
*/
#endif
