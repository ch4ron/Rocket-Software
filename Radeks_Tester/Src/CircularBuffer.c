#include "CircularBuffer.h"
#include <stdlib.h>

#include <stdint.h>

typedef struct CircularBufferStruct
{
    int count;
    int index;
    int outdex;
    int capacity;
    uint32_t * values;
} CircularBufferStruct ;

enum {BUFFER_GUARD = 10000};

CircularBuffer CircularBuffer_Create(int capacity)
{
    CircularBuffer self = calloc(1, sizeof(CircularBufferStruct));
    self->capacity = capacity;
    self->values = calloc(capacity + 1, sizeof(uint32_t));
    self->values[capacity] = BUFFER_GUARD;
    return self;
}

void CircularBuffer_Destroy(CircularBuffer self)
{
    free(self->values);
    free(self);
}

int CircularBuffer_VerifyIntegrity(CircularBuffer self)
{
    return self->values[self->capacity] == BUFFER_GUARD;
}

int CircularBuffer_IsEmpty(CircularBuffer self)
{
    return self->count == 0;
}

int CircularBuffer_IsFull(CircularBuffer self)
{
    return self->count == self->capacity;
}

uint32_t CircularBuffer_Put(CircularBuffer self, uint32_t value)
{
    if (self->count >= self->capacity)
        return 0;

    self->count++;
    self->values[self->index++] = value;
    if (self->index >= self->capacity)
        self->index = 0;

    return 1;
}

uint32_t CircularBuffer_Get(CircularBuffer self)
{
    int value;
    if (self->count <= 0)
        return 0;

    value = self->values[self->outdex++];
    self->count--;
    if (self->outdex >= self->capacity)
        self->outdex = 0;

    return value;
}

int CircularBuffer_GetCount(CircularBuffer self)
{
    return self->count;
}

int CircularBuffer_Capacity(CircularBuffer self)
{
    return self->capacity;
}