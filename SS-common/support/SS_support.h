/*
 * SS_support.h
 *
 *  Created on: Feb 6, 2020
 *      Author: maciek
 */

#ifndef SS_SUPPORT_H_
#define SS_SUPPORT_H_

#include "stdbool.h"

void SS_support_set_mem_led(bool red, bool green, bool blue);
void SS_support_set_com_led(bool red, bool green, bool blue);
void SS_support_set_adc_led(bool red, bool green, bool blue);

#endif /* SS_SUPPORT_H_ */
