//
// Created by maciek on 28.02.2020.
//

#include "SS_platform.h"
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif
#include "usart.h"
#ifdef SS_USE_COM
#include "SS_com.h"
#endif
#include "SS_console.h"
#include "SS_log.h"

/*********** LED **********/

void SS_platform_set_com_led(bool r, bool g, bool b) {
    HAL_GPIO_WritePin(LOOP_LED_GPIO_Port, LOOP_LED_Pin, b);
}

void SS_platform_toggle_loop_led() {
    /* HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); */
    HAL_GPIO_TogglePin(COM_BLUE_GPIO_Port, COM_BLUE_Pin);
    HAL_GPIO_TogglePin(COM_GREEN_GPIO_Port, COM_GREEN_Pin);

}



/********** MAIN INIT *********/

void SS_platform_init() {

#ifdef SS_USE_COM
    SS_com_init(COM_CZAPLA_ID);
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
