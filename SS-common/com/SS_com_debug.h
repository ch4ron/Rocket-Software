/*
 * SS_com_debug.h
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#ifndef SS_COM_DEBUG_H_
#define SS_COM_DEBUG_H_

#define SS_DEBUG_ENABLED

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com.h"

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

#ifdef SS_DEBUG_ENABLED

void _SS_com_print_message_received(ComFrame *frame);
void _SS_com_print_message_sent(ComFrame *frame);
void _SS_com_print_message_error(ComFrame *frame, char *error);

#define SS_com_print_message_received(frame) _SS_com_print_message_received(frame)
#define SS_com_print_message_sent(frame) _SS_com_print_message_sent(frame)
#define SS_com_print_message_error(frame) _SS_com_print_message_error(frame)
#else
#define SS_com_print_message_received(frame)
#define SS_com_print_message_sent(frame)
#define SS_com_print_message_error(frame)
#endif

#endif /* SS_COM_DEBUG_H_ */
