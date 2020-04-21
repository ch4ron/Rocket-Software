//
// Created by maciek on 28.02.2020.
//

#include "SS_platform.h"

#include "SS_can.h"
#include "SS_common.h"
#include "can.h"
#include "usart.h"

/********** PRINTF *********/

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart6, (uint8_t *) ptr, (uint16_t) len, 1000);
    return len;
}

/********** ADC *********/

//static void SS_platform_adc_init() {
//    ADC_HandleTypeDef *adc[] = {
//            &hadc1, &hadc2, &hadc3
//    };
//    SS_adc_init(adc, sizeof(adc)/sizeof(adc[0]));
//     TODO Adc
//}

/********** SUPPLY *********/

//static void SS_platform_supply_init() {
//     TODO Supply
//}

void SS_com_transmit(ComFrame *frame) {
    if(frame->destination == COM_GRAZYNA_ID && SS_grazyna_is_enabled()) {
        SS_grazyna_transmit(frame);
    } else if(frame->destination == COM_KROMEK_ID || frame->destination == COM_GRAZYNA_ID) {
        SS_can_ext_transmit(frame);
    } else {
        SS_can_transmit(frame);
    }
}

/********** MAIN INIT *********/

void SS_platform_init() {
//    SS_platform_adc_init();
//    SS_platform_supply_init();
    SS_com_init(COM_PAUEK_ID);
    SS_can_init(&hcan2, COM_PAUEK_ID);
    SS_can_ext_init(&hcan1);
    SS_grazyna_init(&huart1);
}
