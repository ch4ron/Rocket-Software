#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "Mockprojdefs.h"


static struct MockprojdefsInstance
{
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

