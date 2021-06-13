//
// Created by Piotrek on 2021-01-19.
//

#ifndef RECEIVE_SIM_DATA_DECODER_H
#define RECEIVE_SIM_DATA_DECODER_H

#include <stdint.h>
#include <stdbool.h>
#include "CircularBuffer.h"

#define NUMBER_OF_DATA_ROWS 992 // It is required that NUMBER_OF_DATA_ROWS % 16 = 0
#define BYTES_IN_ONE_ROW 16

void SS_BinDataDecoder_init();
void SS_BinDataDecoder_deInit(void);
uint8_t SS_BinDataDecoder_decode(uint8_t * Buf);
bool SS_BinDataDecoder_isNumberOfDataBelowMin();
uint32_t SS_BinDataDecoder_GetMS56Temp();
uint32_t SS_BinDataDecoder_GetMS56Pres();
uint32_t SS_BinDataDecoder_GetMS58Temp();
uint32_t SS_BinDataDecoder_GetMS58Pres();


#endif RECEIVE_SIM_DATA_DECODER_H