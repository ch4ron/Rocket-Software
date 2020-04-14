#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "Mockmessage_buffer.h"


static struct Mockmessage_bufferInstance
{
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

