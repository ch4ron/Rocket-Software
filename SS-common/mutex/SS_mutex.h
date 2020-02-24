/*
 * SS_mutex.h
 *
 *  Created on: Jan 3, 2020
 *      Author: maciek
 */

#ifndef SS_MUTEX_H_
#define SS_MUTEX_H_

#include "stdbool.h"
#include "stm32f4xx_hal.h"

typedef struct {
    volatile uint32_t flag;
} Mutex;

bool SS_mutex_lock(Mutex *mutex);
void SS_mutex_unlock(Mutex *mutex);


#endif /* SS_MUTEX_H_ */
