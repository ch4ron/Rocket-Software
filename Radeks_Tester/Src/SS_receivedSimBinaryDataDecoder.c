//
// Created by Piotrek on 2021-01-19.
//

#include "SS_receivedSimBinaryDataDecoder.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN_DATA_IN_BUF 200

static CircularBuffer CbuffMS56Temp;
static CircularBuffer CbuffMS56Pres;
static CircularBuffer CbuffMS58Temp;
static CircularBuffer CbuffMS58Pres;
/* HELPERS */
void putToBuf(CircularBuffer Cbuf ,uint32_t value)
{
    CircularBuffer_Put(Cbuf, value);
}

uint32_t getFromBuf(CircularBuffer Cbuf)
{
   return CircularBuffer_Get(Cbuf);
}

int getCbufCount(CircularBuffer Cbuf)
{
    return CircularBuffer_GetCount(Cbuf);
}


/* UTIL FUNCTIONS */

void SS_BinDataDecoder_init()
{
    CbuffMS56Temp = CircularBuffer_Create(1500);
    CbuffMS56Pres = CircularBuffer_Create(1500);
    CbuffMS58Temp = CircularBuffer_Create(1500);
    CbuffMS58Pres = CircularBuffer_Create(1500);

}

void SS_BinDataDecoder_deInit(void)
{
    CircularBuffer_Destroy(CbuffMS56Temp);
    CircularBuffer_Destroy(CbuffMS56Pres);
    CircularBuffer_Destroy(CbuffMS58Temp);
    CircularBuffer_Destroy(CbuffMS58Pres);
}

uint8_t SS_BinDataDecoder_decode(uint8_t * Buf)
{
    uint32_t rows = NUMBER_OF_DATA_ROWS;
    uint32_t value;
    for(uint32_t i = 0; i < 16*rows; i += 16)
    {
        value = (Buf[i]<<24) + (Buf[i+1]<<16) + (Buf[i+2]<<8) + (Buf[i+3]);
        putToBuf(CbuffMS56Temp, value);
        value = (Buf[i+4]<<24) + (Buf[i+4+1]<<16) + (Buf[i+4+2]<<8) + (Buf[i+4+3]);
        putToBuf(CbuffMS56Pres, value);
        value = (Buf[i+8]<<24) + (Buf[i+8+1]<<16) + (Buf[i+8+2]<<8) + (Buf[i+8+3]);
        putToBuf(CbuffMS58Temp, value);
        value = (Buf[i+12]<<24) + (Buf[i+12+1]<<16) + (Buf[i+12+2]<<8) + (Buf[i+12+3]);
        putToBuf(CbuffMS58Pres, value);
    }
}

bool SS_BinDataDecoder_isNumberOfDataBelowMin()
{
    if (getCbufCount(CbuffMS56Pres) < MIN_DATA_IN_BUF)
        return true;
    else
        return false;
}

uint32_t SS_BinDataDecoder_GetMS56Temp()
{
    return getFromBuf(CbuffMS56Temp);
}

uint32_t SS_BinDataDecoder_GetMS56Pres()
{
    return getFromBuf(CbuffMS56Pres);
}
uint32_t SS_BinDataDecoder_GetMS58Temp()
{
    return getFromBuf(CbuffMS58Temp);
}
uint32_t SS_BinDataDecoder_GetMS58Pres()
{
    return getFromBuf(CbuffMS58Pres);
}