#include "MockFreeRTOS.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct MockFreeRTOSInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void MockFreeRTOS_Verify(void)
{
}

void MockFreeRTOS_Init(void)
{
  MockFreeRTOS_Destroy();
}

void MockFreeRTOS_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

