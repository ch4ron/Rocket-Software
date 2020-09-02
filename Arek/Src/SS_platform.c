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
//#include "SS_console.h"
#include "usart.h"
#include "SS_log.h"
#include "stdbool.h"

/********** MAIN INIT *********/
void SS_platform_init()
{
    SS_log_init(&huart2);
    //SS_console_init(&huart2);
}
