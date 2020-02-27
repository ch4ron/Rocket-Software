//
// Created by maciek on 26.02.2020.
//

#ifdef SS_USE_ADC
#include "SS_adc.h"
#endif
#ifdef SS_USE_ADS1258
#include "SS_ADS1258.h"
#endif
#ifdef SS_USE_DYNAMIXEL
#include "SS_dynamixel.h"
#endif
#ifdef SS_USE_SUPPLY
#include "SS_supply.h"
#endif
#ifdef SS_USE_SERVOS
#include "SS_servos.h"
#endif
#ifdef SS_USE_GRAZYNA
#include "SS_Grazyna_com.h"
#endif

void SS_init() {
#ifdef SS_USE_ADC
//    SS_adc_init();
#endif
#ifdef SS_USE_ADS1258
//  SS_measurements_init();
#endif
#ifdef SS_USE_DYNAMIXEL

#endif
#ifdef SS_USE_SUPPLY
//    SS_supply_init();
#endif
#ifdef SS_USE_SERVOS
//  SS_servos_init();
#endif
#ifdef SS_USE_GRAZYNA
    SS_grazyna_init();
#endif

//  SS_supply_init();
//  SS_adc_init();
//  SS_servos_init();
//    SS_grazyna_init();
//  SS_measurements_init();
//    printf("Elon!\r\n");
//  SS_settings_read_json(settings_json);
}