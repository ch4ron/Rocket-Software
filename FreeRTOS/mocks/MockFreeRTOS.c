#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "MockFreeRTOS.h"


static struct MockFreeRTOSInstance
{
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

