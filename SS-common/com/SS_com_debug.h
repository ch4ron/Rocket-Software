/*
 * SS_com_debug.h
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#ifndef SS_COM_DEBUG_H_
#define SS_COM_DEBUG_H_

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com.h"

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_com_print_message_received(ComFrame *frame);
void SS_com_print_message_sent(ComFrame *frame);
void SS_com_print_message_error(ComFrame *frame, char *error);

#endif /* SS_COM_DEBUG_H_ */
