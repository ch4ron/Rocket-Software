#include "SS_platform.h"
#include "main.h"

#ifdef SS_USE_MS5X
#include "SS_MS5X.h"
#endif

#include "SS_common.h"
#ifdef SS_USE_FLASH
#include "SS_s25fl.h"
#include "SS_flash_caching.h"
#include "SS_flash_ctrl.h"
#endif

#include "tim.h"
#include "SS_console.h"
#include "usart.h"
#include "SS_log.h"
#include "stdbool.h"

/*********** LED **********/

void SS_platform_toggle_loop_led(void)
{
    //HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}

/********** MAIN INIT *********/

void SS_platform_init() {
    SS_log_init(&huart4);
    SS_console_init(&huart4);

    /* SS_MS56_init(&ms5607, MS56_PRESS_4096, MS56_TEMP_4096); */

#ifdef SS_USE_MPU9250
    SS_platform_init_MPU();
#endif
#ifdef SS_USE_FLASH
    assert(SS_s25fl_init(FLASH_RESET_GPIO_Port, FLASH_RESET_Pin, 64*1024*1024, 256*1024, 512, true, 4, 1) == S25FL_STATUS_OK);
    assert(SS_flash_init(&hqspi, FLASH_RESET_GPIO_Port, FLASH_RESET_Pin) == FLASH_STATUS_OK);
#endif
}
