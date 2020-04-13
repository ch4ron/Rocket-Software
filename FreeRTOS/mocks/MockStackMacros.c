#include "MockStackMacros.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct MockStackMacrosInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void MockStackMacros_Verify(void)
{
}

void MockStackMacros_Init(void)
{
  MockStackMacros_Destroy();
}

void MockStackMacros_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

