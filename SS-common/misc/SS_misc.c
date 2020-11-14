/*
 * SS_misc.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_misc.h"

/* ==================================================================== */
/* ======================== Private datatypes ========================= */
/* ==================================================================== */

typedef struct {
    bool r;
    bool g;
    bool b;
} Led;

/* ==================================================================== */
/* ========================== Private macros ========================== */
/* ==================================================================== */

#define SS_led_generate_source(name)                                                           \
    static Led name;                                                                           \
    void __attribute__((weak)) SS_platform_set_##name##_led(bool red, bool green, bool blue) { \
        (void) red;                                                                            \
        (void) green;                                                                          \
        (void) blue;                                                                           \
    }                                                                                          \
    void SS_led_set_##name(bool red, bool green, bool blue) {                                  \
        name.r = red;                                                                          \
        name.g = green;                                                                        \
        name.b = blue;                                                                         \
        SS_platform_set_##name##_led(red, green, blue);                                        \
    }                                                                                          \
    void SS_led_toggle_##name(bool red, bool green, bool blue) {                               \
        if(red) name.r = !name.r;                                                              \
        if(green) name.g = !name.g;                                                            \
        if(blue) name.b = !name.b;                                                             \
        SS_platform_set_##name##_led(name.r, name.g, name.b);                                  \
    }                                                                                          \

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

SS_led_generate_source(mem)
SS_led_generate_source(adc)
SS_led_generate_source(com)
SS_led_generate_source(meas)

void SS_led_toggle_all(bool red, bool green, bool blue) {
    SS_led_toggle_adc(red, green, blue);
    SS_led_toggle_mem(red, green, blue);
    SS_led_toggle_com(red, green, blue);
    SS_led_toggle_meas(red, green, blue);
}

void SS_led_set_all(bool red, bool green, bool blue) {
    SS_led_set_adc(red, green, blue);
    SS_led_set_mem(red, green, blue);
    SS_led_set_com(red, green, blue);
    SS_led_set_meas(red, green, blue);
}

void __attribute__((weak)) SS_platform_toggle_loop_led() {}

void SS_buzzer_start_count(uint16_t hertz_100, uint16_t beeps,uint8_t gap)
{
    BUZZER_TIM_REGISTER->ARR = 1000 *100/hertz_100 - 1;
    SS_set_beep_number(beeps);
    SS_set_beep_gap(gap);
    HAL_TIM_Base_Start_IT(&BUZZER_TIM);
}

/*                                                      These were unused in old Repo and i consider deleting it
void SS_buzzer_start(uint16_t hertz_100)
{
    BUZZER_TIM_REGISTER->ARR = 1000 *100/hertz_100 - 1;
    set_gap(2);
    HAL_TIM_Base_Start_IT(&BUZZER_TIM);
}
void SS_buzzer_stop(void)
{
    HAL_TIM_Base_Stop_IT(&BUZZER_TIM);
}

void SS_buzzer_start_gap_count(uint16_t hertz_100, uint8_t beeps, uint8_t gap)
{
    BUZZER_TIM_REGISTER->ARR = 1000 *100/hertz_100 - 1;
    set_beeps(beeps);
    set_gap(gap*2);
    HAL_TIM_Base_Start_IT(&BUZZER_TIM);
}
void SS_buzzer_start_gap(uint16_t hertz_100, uint8_t gap)
{
    BUZZER_TIM_REGISTER->ARR = 1000 *100/hertz_100 - 1;
    set_beeps(255);
    set_gap(gap*2);
    HAL_TIM_Base_Start_IT(&BUZZER_TIM);
}*/