#include "Mockmessage_buffer.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct Mockmessage_bufferInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void Mockmessage_buffer_Verify(void)
{
}

void Mockmessage_buffer_Init(void)
{
  Mockmessage_buffer_Destroy();
}

void Mockmessage_buffer_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

