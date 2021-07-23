#ifndef SS_FREERTOS_H
#define SS_FREERTOS_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_run_tests_task(void *pvParameters);
void SS_FreeRTOS_init(void);

#endif  // SS_COMMON_H
