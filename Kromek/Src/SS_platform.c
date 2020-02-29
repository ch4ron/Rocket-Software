//
// Created by maciek on 28.02.2020.
//

#include <relays/SS_relays.h>
#include "SS_platform.h"
#include "SS_servos.h"
#include "SS_Grazyna.h"


/********** SERVOS *********/

Servo servos[] = {
        {.id = 0, .tim = &htim3, .channel = TIM_CHANNEL_2, .supply = &servos1_supply},
        {.id = 1,.tim = &htim1, .channel = TIM_CHANNEL_3, .supply = &servos1_supply},
        {.id = 2,.tim = &htim1, .channel = TIM_CHANNEL_2, .supply = &servos1_supply},
        {.id = 3,.tim = &htim1, .channel = TIM_CHANNEL_1, .supply = &servos1_supply},
        {.id = 4,.tim = &htim3, .channel = TIM_CHANNEL_4, .supply = &servos2_supply},
        {.id = 5,.tim = &htim3, .channel = TIM_CHANNEL_3, .supply = &servos2_supply},
        {.id = 6,.tim = &htim8, .channel = TIM_CHANNEL_2, .supply = &servos2_supply},
        {.id = 7,.tim = &htim3, .channel = TIM_CHANNEL_1, .supply = &servos2_supply},
};

void SS_platform_servos_init() {
    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
}

/********** SUPPLY *********/

static float supply_12v_voltage_scaled(uint16_t raw, float vdd) {
    return vdd * (float) raw / 4095.0f * (4990.0f + 20000.0f) / 4990.0f;
}

static float supply_servo_voltage_scaled(uint16_t raw, float vdd) {
    return vdd * (float) raw / 4095.0f * (8450.0f + 20000.0f) / 8450.0f;
}

Supply relay_supply = {
        .ENABLE_Port = ENABLE1_GPIO_Port,
        .ENABLE_Pin= ENABLE1_Pin,
        .measurement = {
                .fun = supply_12v_voltage_scaled,
                .rankId = 4,
                .adc = 1
        }
};
Supply servos1_supply = {
        .ENABLE_Port = ENABLE2_GPIO_Port,
        .ENABLE_Pin= ENABLE2_Pin,
        .measurement = {
                .fun = supply_servo_voltage_scaled,
                .rankId = 5,
                .adc = 3
        }
};
Supply servos2_supply = {
        .ENABLE_Port = ENABLE3_GPIO_Port,
        .ENABLE_Pin= ENABLE3_Pin,
        .measurement = {
                .fun = supply_servo_voltage_scaled,
                .rankId = 3,
                .adc = 1
        }
};
Supply kozackie_servo_supply = {
        .ENABLE_Port = ENABLE4_GPIO_Port,
        .ENABLE_Pin= ENABLE4_Pin,
        .measurement = {
                .fun = supply_12v_voltage_scaled,
                .rankId = 2,
                .adc = 2
        }
};

static void SS_platform_supply_init() {
    SS_supply_init(&relay_supply);
    SS_supply_init(&kozackie_servo_supply);
    SS_supply_init(&servos1_supply);
    SS_supply_init(&servos2_supply);
}

/********** RELAYS *********/

Relay relays[] = {
        { .id = 0, .GPIO_Port = RELAY1_GPIO_Port, .Pin = RELAY1_Pin },
        { .id = 1, .GPIO_Port = RELAY2_GPIO_Port, .Pin = RELAY2_Pin },
        { .id = 2, .GPIO_Port = RELAY3_GPIO_Port, .Pin = RELAY3_Pin },
        { .id = 3, .GPIO_Port = RELAY4_GPIO_Port, .Pin = RELAY4_Pin },
        { .id = 4, .GPIO_Port = RELAY5_GPIO_Port, .Pin = RELAY5_Pin },
        { .id = 5, .GPIO_Port = RELAY6_GPIO_Port, .Pin = RELAY6_Pin },
        { .id = 6, .GPIO_Port = RELAY7_GPIO_Port, .Pin = RELAY7_Pin },
        { .id = 7, .GPIO_Port = RELAY8_GPIO_Port, .Pin = RELAY8_Pin },
        { .id = 8, .GPIO_Port = RELAY9_GPIO_Port, .Pin = RELAY9_Pin },
};

static void SS_platform_relays_init() {
    SS_relays_init(relays, sizeof(relays)/sizeof(relays[0]));
}

/********** MAIN INIT *********/

void SS_platform_init() {
    SS_platform_servos_init();
    SS_platform_supply_init();
    SS_platform_relays_init();
    SS_grazyna_init(&huart2);
}
