/*
 * SS_fifo.c
 *
 *  Created on: 04.06.2019
 *      Author: tatar
 */
#include "SS_fifo.h"
#include <string.h>
/*
 *	declare struct with volatile prefix
 */
bool SS_fifo_put_data(volatile fifo *q, void *data) {

	uint8_t head_temp = q->head + 1;

	//overlap at the end
	if (head_temp == q->buff_size)
		head_temp = 0;

	//buffer full
	if (head_temp == q->tail)
		return false;
	//copy data
	memcpy((q->buffer) + head_temp * q->cell_size, data, q->cell_size);
	q->head = head_temp;
	return true;
}

bool SS_fifo_get_data(volatile fifo *q, void *data) {
	//if empty
	if (q->head == q->tail)
		return false;

	q->tail++;

	//overlap
	if (q->tail == q->buff_size)
		q->tail = 0;

	memcpy(data, (q->buffer) + q->tail * q->cell_size, q->cell_size);

	return true;

}
