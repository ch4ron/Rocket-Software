#include "Mocksemphr.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct MocksemphrInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void Mocksemphr_Verify(void)
{
}

void Mocksemphr_Init(void)
{
  Mocksemphr_Destroy();
}

void Mocksemphr_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

