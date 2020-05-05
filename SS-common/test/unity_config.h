#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#include "SS_log.h"
#include "usart.h"

/* TODO Add a buffer, current implementacion is inefficient - adds chars to queue one by one */

#define UNITY_OUTPUT_CHAR(a) { uint8_t c = a; SS_print_bytes(&c, 1); }

#endif
