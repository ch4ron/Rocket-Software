/*
 * SS_mutex.c
 *
 *  Created on: Jan 3, 2020
 *      Author: maciek
 */

#include "SS_mutex.h"

bool SS_mutex_lock(Mutex *mutex) {
    uint32_t status = 0;
    status = __LDREXW(&mutex->flag);
    if(status == 0) {
        status =__STREXW(1, &mutex->flag);
    }
    __DMB();        // Do not start any other memory access
    // until memory barrier is completed
    return (bool) !status;
}

void SS_mutex_unlock(Mutex *mutex) {
    __DMB(); // Ensure memory operations completed before releasing lock
     mutex->flag = 0;
}
