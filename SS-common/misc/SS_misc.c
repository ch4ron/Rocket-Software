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
    void __attribute__((weak)) SS_platform_set_##name##_led(bool red, bool green, bool blue) {  \
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

SS_led_generate_source(mem)
SS_led_generate_source(adc)
SS_led_generate_source(com)

void SS_led_toggle_all(bool red, bool green, bool blue) {
    SS_led_toggle_adc(red, green, blue);
    SS_led_toggle_mem(red, green, blue);
    SS_led_toggle_com(red, green, blue);
}

void SS_led_set_all(bool red, bool green, bool blue) {
    SS_led_set_adc(red, green, blue);
    SS_led_set_mem(red, green, blue);
    SS_led_set_com(red, green, blue);
}

void __attribute__((weak)) SS_platform_toggle_loop_led() {}
