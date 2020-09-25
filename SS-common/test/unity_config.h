#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#include "SS_log.h"
#include "usart.h"

/* TODO Add a buffer, current implementacion is inefficient - adds chars to queue one by one */

#define UNITY_OUTPUT_CHAR(a) SS_log_buf_put(a)
#define UNITY_OUTPUT_FLUSH SS_log_buf_flush

#endif
