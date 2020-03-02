//
// Created by maciek on 29.02.2020.
//

#include "stdarg.h"
#include "stdio.h"

void SS_error(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    printf("ERROR@ ");
    vprintf(format, arg);
    printf("\r\n");
    va_end(arg);
}