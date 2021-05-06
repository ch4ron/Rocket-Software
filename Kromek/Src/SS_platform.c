//
// Created by maciek on 28.02.2020.
//

#include "SS_platform.h"
#include "SS_relays.h"
#include "SS_servos.h"
#include "SS_ADS1258.h"
#include "SS_measurements.h"
#include "SS_relays.h"
#include "SS_grazyna.h"
#include "flash/SS_flash.h"

#ifdef SS_USE_FLASH
#include "SS_s25fl.h"
#include "SS_flash_caching.h"
#include "SS_flash_ctrl.h"
#include "SS_flash_log.h"
#endif
#ifdef SS_USE_IGNITER
#include "SS_igniter.h"
#endif
#include "SS_adc.h"
#include "SS_can.h"
#include "SS_console.h"
#include "SS_dynamixel.h"
#include "SS_log.h"
#include "adc.h"
#include "can.h"
#include "spi.h"
#include "usart.h"
#include "quadspi.h"
#include "usart.h"

/*********** LED **********/

void SS_platform_set_mem_led(bool red, bool green, bool blue) {
    HAL_GPIO_WritePin(MEM_RED_GPIO_Port, MEM_RED_Pin, !red);
    HAL_GPIO_WritePin(MEM_GREEN_GPIO_Port, MEM_GREEN_Pin, !green);
    HAL_GPIO_WritePin(MEM_BLUE_GPIO_Port, MEM_BLUE_Pin, !blue);
}

void SS_platform_set_com_led(bool red, bool green, bool blue) {
    HAL_GPIO_WritePin(COM_RED_GPIO_Port, COM_RED_Pin, !red);
    HAL_GPIO_WritePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin, !green);
    HAL_GPIO_WritePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin, !blue);
}

void SS_platform_set_adc_led(bool red, bool green, bool blue) {
    HAL_GPIO_WritePin(ADC_RED_GPIO_Port, ADC_RED_Pin, !red);
    HAL_GPIO_WritePin(ADC_GREEN_GPIO_Port, ADC_GREEN_Pin, !green);
    HAL_GPIO_WritePin(ADC_BLUE_GPIO_Port, ADC_BLUE_Pin, !blue);
}

void SS_platform_toggle_loop_led() {
    HAL_GPIO_TogglePin(XBEE_TX_LED_GPIO_Port, XBEE_TX_LED_Pin);
}

/********** SERVOS *********/

Servo servos[] = {
    {.id = 0, .tim = &htim3, .channel = TIM_CHANNEL_2, .supply = &servos1_supply},
    {.id = 1, .tim = &htim1, .channel = TIM_CHANNEL_3, .supply = &servos1_supply},
    {.id = 2, .tim = &htim1, .channel = TIM_CHANNEL_2, .supply = &servos1_supply},
    {.id = 3, .tim = &htim1, .channel = TIM_CHANNEL_1, .supply = &servos1_supply},
    {.id = 4, .tim = &htim3, .channel = TIM_CHANNEL_4, .supply = &servos2_supply},
    {.id = 5, .tim = &htim3, .channel = TIM_CHANNEL_3, .supply = &servos2_supply},
    {.id = 6, .tim = &htim8, .channel = TIM_CHANNEL_2, .supply = &servos2_supply},
    {.id = 7, .tim = &htim3, .channel = TIM_CHANNEL_1, .supply = &servos2_supply},
};

void SS_platform_servos_init(void) {
    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
}

/********** ADC *********/

Adc adc1, adc2, adc3;

static void
SS_platform_adc_init() {
#if defined(SS_USE_ADC)
    SS_adc_init(&adc1, ADC1, 2);
    SS_adc_init(&adc2, ADC2, 1);
    SS_adc_init(&adc3, ADC3, 1);
#endif
}

/********** SUPPLY *********/

static float supply_12v_voltage_scaled(float voltage) {
    return voltage * (4990.0f + 20000.0f) / 4990.0f;
}

static float supply_servo_voltage_scaled(float voltage) {
    return voltage * (8450.0f + 20000.0f) / 8450.0f;
}

Supply relay_supply = {
    .ENABLE_Port = ENABLE1_GPIO_Port,
    .ENABLE_Pin = ENABLE1_Pin,
    .measurement = {
        .scale_fun = supply_12v_voltage_scaled,
        .channel = 12,
        .adc = &adc1}};

Supply servos1_supply = {
    .ENABLE_Port = ENABLE2_GPIO_Port,
    .ENABLE_Pin = ENABLE2_Pin,
    .measurement = {
        .scale_fun = supply_servo_voltage_scaled,
        .channel = 13,
        .adc = &adc3}};

Supply servos2_supply = {
    .ENABLE_Port = ENABLE3_GPIO_Port,
    .ENABLE_Pin = ENABLE3_Pin,
    .measurement = {
        .scale_fun = supply_servo_voltage_scaled,
        .channel = 11,
        .adc = &adc1}};

Supply dynamixel_supply = {
    .ENABLE_Port = ENABLE4_GPIO_Port,
    .ENABLE_Pin = ENABLE4_Pin,
    .measurement = {
        .scale_fun = supply_12v_voltage_scaled,
        .channel = 5,
        .adc = &adc2}};

static void SS_platform_supply_init() {
    SS_supply_init(&relay_supply);
    SS_supply_init(&servos1_supply);
    SS_supply_init(&servos2_supply);
    SS_supply_init(&dynamixel_supply);
}

/********** RELAYS *********/

Relay relays[] = {
    {.id = 0, .GPIO_Port = RELAY1_GPIO_Port, .Pin = RELAY1_Pin},
    {.id = 1, .GPIO_Port = RELAY2_GPIO_Port, .Pin = RELAY2_Pin},
    {.id = 2, .GPIO_Port = RELAY3_GPIO_Port, .Pin = RELAY3_Pin},
    {.id = 3, .GPIO_Port = RELAY4_GPIO_Port, .Pin = RELAY4_Pin},
    {.id = 4, .GPIO_Port = RELAY5_GPIO_Port, .Pin = RELAY5_Pin},
    {.id = 5, .GPIO_Port = RELAY6_GPIO_Port, .Pin = RELAY6_Pin},
    {.id = 6, .GPIO_Port = RELAY7_GPIO_Port, .Pin = RELAY7_Pin},
    {.id = 7, .GPIO_Port = RELAY8_GPIO_Port, .Pin = RELAY8_Pin},
    {.id = 8, .GPIO_Port = RELAY9_GPIO_Port, .Pin = RELAY9_Pin},
};

static void SS_platform_relays_init() {
    SS_relays_init(relays, sizeof(relays) / sizeof(relays[0]));
}

/********** ADS1258 *********/

//Measurement measurements[] = {
//        {.channel_id = STATUS_CHID_REF,
//            .a_coefficient = 0.251004016064257028112449799196787148f,
//            .b_coefficient = 0.75f},
//};

Measurement measurements[] = {
    {.channel_id = STATUS_CHID_REF,
            .a_coefficient = 0.251004016064257028112449799196787148f,
            .b_coefficient = 0.75f},
    {.channel_id = STATUS_CHID_DIFF0,
            .a_coefficient = 0.251004016064257028112449799196787148f,
            .b_coefficient = 0.75f},
    {.channel_id = STATUS_CHID_DIFF1,
            .a_coefficient = 0.251004016064257028112449799196787148f,
            .b_coefficient = 0.75f},
    {.channel_id = STATUS_CHID_DIFF2,
            .a_coefficient = 95688.70933f,
            .b_coefficient = 14168.46902f},
    {.channel_id = STATUS_CHID_DIFF3,
            .a_coefficient = 1.0f,
            .b_coefficient = 0.0f},
    {.channel_id = STATUS_CHID_DIFF4,
            .a_coefficient = 0.251004016064257028112449799196787148f,
            .b_coefficient = 0.75f},
    {.channel_id = STATUS_CHID_DIFF5,
            .a_coefficient = 22.906f,
            .b_coefficient = -25.011f},
    {.channel_id = STATUS_CHID_DIFF6,
            .a_coefficient = 0.251004016064257028112449799196787148f,
            .b_coefficient = 0.75f},
    {.channel_id = STATUS_CHID_DIFF7,
            .a_coefficient = 0.251004016064257028112449799196787148f,
            .b_coefficient = 0.75f},
};

static void
SS_platform_ADS1258_init() {
    SS_ADS1258_measurements_init(measurements, sizeof(measurements) / sizeof(measurements[0]));
    SS_ADS1258_init(&hspi2);
}

#ifdef SS_USE_DYNAMIXEL
Dynamixel dynamixel = {
    .id = 0x01,
    .opened_position = 4095,
    .closed_position = 0,
    .huart = &huart1,
    .DE_Port = RS485_DE_GPIO_Port,
    .DE_Pin = RS485_DE_Pin,
    .supply = &dynamixel_supply};

#endif


/********** MAIN INIT *********/

void SS_platform_init() {
    SS_log_init(&huart5);
    SS_console_init(&huart5);
    SS_platform_adc_init();
    SS_platform_servos_init();
    SS_platform_supply_init();
    SS_platform_relays_init();
    SS_platform_ADS1258_init();
    SS_can_init(&hcan1, COM_KROMEK_ID);
    SS_grazyna_init(&huart2);
    SS_igniter_init(RELAY3_GPIO_Port, RELAY3_Pin);
#ifdef SS_USE_DYNAMIXEL
    SS_dynamixel_init(&dynamixel);
#endif
#ifdef SS_USE_FLASH
    SS_flash_ctrl_init();
    assert(SS_s25fl_init(FLASH_RESET_GPIO_Port, FLASH_RESET_Pin, 64*1024*1024, 256*1024, 512, true, 4, 1) == S25FL_STATUS_OK);
    /* assert(SS_flash_init(&hqspi, FLASH_RESET_GPIO_Port, FLASH_RESET_Pin) == FLASH_STATUS_OK); */
    HAL_Delay(100);
#endif
    SS_enable_supply(&relay_supply);
}
