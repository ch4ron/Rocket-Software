//
// Created by maciek on 28.02.2020.
//

#include "SS_platform.h"
#include "SS_relays.h"
#include "SS_servos.h"
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif
#include "SS_igniter.h"
#include "usart.h"
#ifdef SS_USE_COM
#include "SS_com.h"
#endif
#include "SS_console.h"
#include "SS_log.h"

/*********** LED **********/

void SS_platform_set_com_led(bool r, bool g, bool b) {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, b);
}

void SS_platform_toggle_loop_led() {
    /* HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); */
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

}

/********** SERVOS *********/

Servo servos[] = {
    {.id = 0, .tim = &htim3, .channel = TIM_CHANNEL_1 },
    {.id = 1, .tim = &htim3, .channel = TIM_CHANNEL_2 },
    {.id = 2, .tim = &htim3, .channel = TIM_CHANNEL_3 },
    {.id = 3, .tim = &htim3, .channel = TIM_CHANNEL_4 },
};

void SS_platform_servos_init(void) {
    SS_servos_init(servos, sizeof(servos) / sizeof(servos[0]));
}


/********** RELAYS *********/

Relay relays[] = {
    {.id = 0, .GPIO_Port = RELAY1_GPIO_Port, .Pin = RELAY1_Pin},
    {.id = 1, .GPIO_Port = RELAY2_GPIO_Port, .Pin = RELAY2_Pin},
    {.id = 2, .GPIO_Port = RELAY3_GPIO_Port, .Pin = RELAY3_Pin},
    {.id = 3, .GPIO_Port = RELAY4_GPIO_Port, .Pin = RELAY4_Pin},
};

static void SS_platform_relays_init() {
    SS_relays_init(relays, sizeof(relays) / sizeof(relays[0]));
}


/********** MAIN INIT *********/

void SS_platform_init() {
    SS_platform_servos_init();
    SS_platform_relays_init();
#ifdef SS_USE_COM
    SS_com_init(COM_KROMEK_ID);
#endif
#ifdef SS_USE_GRAZYNA
    SS_grazyna_init(&huart2);
    SS_log_init(&huart1);
    SS_console_init(&huart1);
#else
    SS_log_init(&huart2);
    SS_console_init(&huart2);
#endif
    SS_igniter_init(RELAY2_GPIO_Port, RELAY2_Pin);
}
