#include "Mockstack_macros.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct Mockstack_macrosInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void Mockstack_macros_Verify(void)
{
}

void Mockstack_macros_Init(void)
{
  Mockstack_macros_Destroy();
}

void Mockstack_macros_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

