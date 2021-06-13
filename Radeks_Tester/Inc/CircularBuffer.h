#ifndef D_CircularBuffer_H
#define D_CircularBuffer_H

#include <stdint.h>

typedef struct CircularBufferStruct * CircularBuffer;

CircularBuffer CircularBuffer_Create(int capacity);
void CircularBuffer_Destroy(CircularBuffer);
int CircularBuffer_IsEmpty(CircularBuffer);
int CircularBuffer_IsFull(CircularBuffer);
uint32_t CircularBuffer_Put(CircularBuffer, uint32_t);
uint32_t CircularBuffer_Get(CircularBuffer);
int CircularBuffer_GetCount(CircularBuffer);
int CircularBuffer_Capacity(CircularBuffer);
int CircularBuffer_VerifyIntegrity(CircularBuffer);
#endif  /* D_CircularBuffer_H */
