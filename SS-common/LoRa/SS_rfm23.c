/*
 * rfm23.c
 *
 *  Created on: Feb 11, 2019
 *      Author: matt
 */

#include "SS_rfm23.h"
#include "usart.h"
#include "crc.h"
uint8_t RF_BUFF[PACKET_LENGTH];

void write(uint8_t a, uint8_t b)
{
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, 0);
	uint8_t data[2];
	data[0] = a | 0b10000000;
	data[1] = b;
	HAL_SPI_Transmit(&hspi1, data, 2, 100);
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, 1);

}
uint8_t read(uint8_t a)
{
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, 0);
	uint8_t rec[2];
	uint8_t add[2];
	add[0] = a;
	add[1] = 0;
	HAL_SPI_TransmitReceive(&hspi1, add, rec, 2, 100);
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, 1);
	return rec[1];
}

void init_RFM22(void)
{
	HAL_Delay(100);
	read(0x03);				// Read Interrupt status1 register
	read(0x04);
	write(0x05, 0x06);		// Enable packet sent and packet received interrupt
	write(0x06, 0x00);		// Disable all interrupts

	write(0x0C, 0xF2);		// GPIO0 is TX State
	write(0x0B, 0xF5);		// GPIO1 is	RX State

	write(0x0E, 0x00);		// GPIO port use default value

	write(0x1C, 0x9A);
	write(0x1D, 0x40);

	write(0x20, 0x3C);
	write(0x21, 0x02);
	write(0x22, 0x22);
	write(0x23, 0x22);
	write(0x24, 0x07);
	write(0x25, 0xFF);
	write(0x2A, 0x48);
	write(0x2C, 0x28);
	write(0x2D, 0x0C);
	write(0x2E, 0x28);

	write(0x30, 0xaf);		//Baicheva
	write(0x32, 0x08);		// no broadcast, check for header 3
	write(0x33, 0x12);// Header 3 used for head length, fixed packet length, synchronize word length 3, 2,
	write(0x34, 0x08);		// 64 nibble = 32 byte preamble
	write(0x35, 0x2A);		// 0x35 need to detect 20bit preamble
	write(0x36, 0x2D);		// synchronize word1
	write(0x37, 0xD4);		// synchronize word2
	write(0x38, 0x00);
	write(0x39, 0x00);
	write(0x3A, 0xA0);		//tx header
	write(0x3B, 0x00);
	write(0x3C, 0x00);
	write(0x3D, 0x00);
	write(0x3E, PACKET_LENGTH);		// set packet length to x bytes
	write(0x3F, 0xA0);		//rx header
	write(0x40, 0x00);
	write(0x41, 0x00);
	write(0x42, 0x00);
	write(0x43, 0xFF);		// check all bits
	write(0x44, 0xFF);		// Check all bits
	write(0x45, 0xFF);		// check all bits
	write(0x46, 0xFF);		// Check all bits

	write(0x6D, 0x1F);		// MAX POWAAAAAAH! and LNA
	write(0x6E, 0x19);
	write(0x6F, 0x9A);

	write(0x70, 0x0C);
	write(0x71, 0x23);
	write(0x72, 0x50);

	write(0x75, 0x73);//868.5
	write(0x76, 0x6A);
	write(0x77, 0x40);

//	write(0x75, 0x75);//902.5
//	write(0x76, 0x1F);
//	write(0x77, 0x40);

}

void to_rx_mode(void)
{

	write(0x08, 0x03);
	write(0x08, 0x00);

	write(0x07, 5);

}

void RFM23_send(uint8_t * tx_buf)
{

	write(0x07, 0x01);	// To ready mode

	write(0x08, 0x03);	// FIFO reset
	write(0x08, 0x00);	// Clear FIFO

	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, 0);
	uint8_t addr = 0xff;
	HAL_SPI_Transmit(&hspi1, &addr, 1, 100);
	HAL_SPI_Transmit(&hspi1, tx_buf, PACKET_LENGTH, 100);
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, 1);

	//read_inerrupt(); //nie wiem czy to jest potrzebne wgl

	write(0x07, 9);	// Start TX

}

uint8_t read_inerrupt(void)
{
	uint8_t interrupt1;
	uint8_t interrupt2;
	interrupt1 = read(0x04);
	interrupt2 = read(0x03);
	//printf("0x03=%#04x, 0x04=%#04x\r\n",interrupt2,interrupt1);
	return interrupt2;
}

/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
 if(GPIO_Pin == LORA_NIRQ_Pin)
 {
 uint8_t interrupt = read_inerrupt();
 if(((interrupt&0x02)==0x02)){
 HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port,SPI1_NSS_Pin,0);
 uint8_t addr = 0x7f;
 HAL_SPI_Transmit(&hspi1, &addr,1, 100);
 HAL_SPI_TransmitReceive(&hspi1, &addr,RF_BUFF, PACKET_LENGTH, 100);
 HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port,SPI1_NSS_Pin,1);


 USART_WriteData(RF_BUFF, 10);


 }

 }
 }*/

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		to_rx_mode();
		//printf("dupe\r\n");
	}

}


void SS_LoRa_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == LORA_NIRQ_Pin)
    {
        uint8_t interrupt = read_inerrupt();
        if(((interrupt&0x02)==0x02)){
            HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port,SPI1_NSS_Pin,0);
            uint8_t addr = 0x7f;
            HAL_SPI_Transmit(&hspi1, &addr,1, 100);
            HAL_SPI_TransmitReceive_DMA(&hspi1, &addr,RF_BUFF, PACKET_LENGTH);
            HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port,SPI1_NSS_Pin,1);

            /*
            USART_WriteData(RF_BUFF, 10);
            SS_grazyna_handle_rx_cplt(RF_BUFF);
            */

        }

    }
}
