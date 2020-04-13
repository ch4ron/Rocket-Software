#include "Mockdeprecated_definitions.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmock.h"

static struct Mockdeprecated_definitionsInstance {
    unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;

void Mockdeprecated_definitions_Verify(void)
{
}

void Mockdeprecated_definitions_Init(void)
{
  Mockdeprecated_definitions_Destroy();
}

void Mockdeprecated_definitions_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
}

