//
// Created by maciek on 28.02.2020.
//

#include <can.h>
#include "SS_platform.h"

#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif
#include "SS_can.h"
#include "usart.h"
#ifdef SS_USE_COM
#include "SS_com.h"
#endif
#include "SS_console.h"
#include "SS_log.h"
#ifdef SS_USE_LORA
#include "SS_rfm23.h"
#endif
#include "SS_it.h"

/*********** LED **********/

void SS_platform_set_com_led(bool r, bool g, bool b) {
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, b);
}

void SS_platform_toggle_loop_led() {
    /* HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); */
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

}



/********** MAIN INIT *********/

void SS_platform_init() {

#ifdef SS_USE_COM
    SS_com_init(COM_KROMEK_ID);
#endif
#ifdef SS_USE_CAN
    SS_can_init(&hcan1,COM_CZAPLA_ID);
    SS_can_init(&hcan2, COM_CZAPLA_ID);
#endif
#ifdef SS_USE_GRAZYNA
    SS_grazyna_init(&huart2);
    SS_log_init(&huart2);
    SS_console_init(&huart2);
#else
    SS_log_init(&huart2);
    SS_console_init(&huart2);
#endif

}
