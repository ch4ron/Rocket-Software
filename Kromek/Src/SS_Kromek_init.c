//
// Created by maciek on 28.02.2020.
//

#include "SS_servos.h"

static void SS_Kromek_servos_init() {
    Servo servos[8] = {
            { .tim = &htim3, .channel = TIM_CHANNEL_2, .supply = &servos1_supply },
            { .tim = &htim1, .channel = TIM_CHANNEL_3, .supply = &servos1_supply },
            { .tim = &htim1, .channel = TIM_CHANNEL_2, .supply = &servos1_supply },
            { .tim = &htim1, .channel = TIM_CHANNEL_1, .supply = &servos1_supply },
            { .tim = &htim3, .channel = TIM_CHANNEL_4, .supply = &servos2_supply },
            { .tim = &htim3, .channel = TIM_CHANNEL_3, .supply = &servos2_supply },
            { .tim = &htim8, .channel = TIM_CHANNEL_2, .supply = &servos2_supply },
            { .tim = &htim3, .channel = TIM_CHANNEL_1, .supply = &servos2_supply },
    };
    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
}

void SS_Kromek_init() {
    SS_Kromek_servos_init();
}