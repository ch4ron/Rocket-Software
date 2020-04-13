#include "Mockprojdefs.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct MockprojdefsInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void Mockprojdefs_Verify(void)
{
}

void Mockprojdefs_Init(void)
{
  Mockprojdefs_Destroy();
}

void Mockprojdefs_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

