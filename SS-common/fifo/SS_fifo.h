#ifndef SS_FIFO_H_
#define SS_FIFO_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef struct {
	uint8_t * const buffer;
	uint8_t head;
	uint8_t tail;
	const uint8_t buff_size;
	const uint8_t cell_size;
} Fifo;

bool SS_fifo_put_data(volatile Fifo*q, void *data);
bool SS_fifo_get_data(volatile Fifo*q, void *data);

#define FIFO_INIT(name, size, bufor_t) \
   uint8_t name ## _fifo_array[size][sizeof(bufor_t)]; \
   volatile Fifo name ## _fifo = { \
        .buffer = (uint8_t*) name ## _fifo_array, \
        .head = 0, \
        .tail = 0, \
        .buff_size = size, \
        .cell_size = sizeof(bufor_t) \
   };


#endif /* SS_FIFO_H_ */
