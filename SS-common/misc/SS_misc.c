/*
 * SS_misc.c
 *
 *  Created on: Dec 24, 2019
 *      Author: maciek
 */

#include "SS_misc.h"

typedef struct {
    bool r;
    bool g;
    bool b;
} Led;

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
    void SS_led_toggle_red_##name() {                                                          \
        name.r = !name.r;                                                                      \
        SS_platform_set_##name##_led(name.r, name.g, name.b);                                  \
    }                                                                                          \
    void SS_led_toggle_green_##name() {                                                        \
        name.g = !name.g;                                                                      \
        SS_platform_set_##name##_led(name.r, name.g, name.b);                                  \
    }                                                                                          \
    void SS_led_toggle_blue_##name() {                                                         \
        name.b = !name.b;                                                                      \
        SS_platform_set_##name##_led(name.r, name.g, name.b);                                  \
    }

SS_led_generate_source(mem)
SS_led_generate_source(adc)
SS_led_generate_source(com)

void __attribute__((weak)) SS_platform_toggle_loop_led() {}
