/**
  * SS_log.h
  *
  *  Created on: May 2, 2020
  *      Author: Maciek
 **/

#ifndef SS_LOG_H
#define SS_LOG_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "printf.h"
#include "assert.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void SS_log_init(UART_HandleTypeDef *huart);
void SS_log_buf_put(char data);
void SS_log_buf_flush(void);
void SS_log_tx_isr(UART_HandleTypeDef *huart);
void SS_print_bytes(char *data, uint16_t size);

void _SS_print(const char *format, ...);
void _SS_print_fromISR(const char *format, ...);
#ifdef SS_DISABLE_PRINT
#define SS_print(format, ...)
#define SS_println(format, ...)
#define SS_error(format, ...)
#define SS_print_fromISR(format, ...)
#define SS_println_fromISR(format, ...)
#define SS_error_fromISR(format, ...)
#else
#define SS_print(format, ...) _SS_print(format, ##__VA_ARGS__)
#define SS_println(format, ...) _SS_print(format "\r\n", ##__VA_ARGS__)
#define SS_error(format, ...) _SS_print("\x01b[41mERROR\x01b[0m@ " format "\r\n", ##__VA_ARGS__)
#define SS_print_fromISR(format, ...) _SS_print_fromISR(format, ##__VA_ARGS__)
#define SS_println_fromISR(format, ...) _SS_print_fromISR(format "\r\n", ##__VA_ARGS__)
#define SS_error_fromISR(format, ...) _SS_print_fromISR("\x01b[41mERROR\x01b[0m@ " format "\r\n", ##__VA_ARGS__)
#endif

bool SS_print_no_flush_start(void);
void SS_print_no_flush_end(void);
void SS_print_no_flush(const char *format, ...);

#endif //SS_LOG_H
