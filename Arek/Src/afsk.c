/***************************************
|-- AUTHOR: ANDRZEJ LACZEWSKI
|-- WEBSITE: WWW.BYTECHLAB.COM
|-- PROJECT: MAGELLAN-1 NANO SATELITE
|-- MODULE: AFSK func
|-- DESCRIPTION:
.................
***************************************/
#include "afsk.h"

// ------------------------------------------->> GLOBAL VARIABLES:
// TODO : STATIC KEYWORD !!!
volatile uint8_t AFSK_trig_frame_tx = 0;

volatile uint8_t AFSK_current_byte;
volatile uint32_t AFSK_packet_pos;

volatile uint32_t AFSK_packet_size = 0;
volatile const uint8_t *AFSK_packet;


// AFSK SINE LOOK UP TABLE ,max amplitude : 127, num of points: 128
volatile const uint8_t AFSK_sine_table[128] =
{
	64,67,70,73,76,79,82,85,88,91,93,96,99,101,104,106,
	108,111,113,115,116,118,120,121,122,123,124,125,126,126,127,127,
	127,127,127,126,126,125,124,123,122,121,120,118,116,115,113,111,
	108,106,104,101,99,96,93,91,88,85,82,79,76,73,70,67,
	64,60,57,54,51,48,45,42,39,36,34,31,28,26,23,21,
	19,16,14,12,11,9,7,6,5,4,3,2,1,1,0,0,
	0,0,0,1,1,2,3,4,5,6,7,9,11,12,14,16,
	19,21,23,26,28,31,34,36,39,42,45,48,51,54,57,60
};
volatile uint16_t AFSK_sine_cnt = 0;
// -------------------------------------------<< GLOBAL VARIABLES

// TODO:
//- Check timers frequency exactly

// ------------------------------------------->> TIMERS:
void AFSK_timers_start()
{
	HAL_TIM_PWM_Start(&AFSK_PWM_TIMER, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&AFSK_SINE_SAMPLE_TIMER);
	HAL_TIM_Base_Start_IT(&AFSK_BAUDRATE_TIMER);
}

void AFSK_timers_stop()
{
	HAL_TIM_PWM_Stop(&AFSK_PWM_TIMER, TIM_CHANNEL_2);
	HAL_TIM_Base_Stop_IT(&AFSK_SINE_SAMPLE_TIMER);
	HAL_TIM_Base_Stop_IT(&AFSK_BAUDRATE_TIMER);
}

void AFSK_output_bit(void);

void AFSK_TIM_INTERUPT_HANDLER(TIM_HandleTypeDef *htim)
{
	 // AFSK Sine generation interupt routine
	 if(htim->Instance == TIM1)
	 {
		  TIM2->CCR2 = AFSK_sine_table[AFSK_sine_cnt];

		  if(AFSK_sine_cnt < 127)
		  {
			  AFSK_sine_cnt ++;
		  }
		  else
		  {
			  AFSK_sine_cnt = 0;
		  }
	 }

	 // Bit output interupt routine (at baudrate)
 	 if(htim->Instance == TIM3)
	 {
 		AFSK_output_bit();
	 }
}
// -------------------------------------------<< TIMERS

void AFSK_output_bit(void)
{
	if (AFSK_trig_frame_tx)
	{
		if (AFSK_packet_pos == AFSK_packet_size) // End of packet transmittion
		{
			AFSK_timers_stop();
			SI446X_carrier_wave_off();
			SI446X_PIN_shutdown_EXT_PA();
			AFSK_trig_frame_tx = 0;
			HAL_GPIO_WritePin(LED_IND_GPIO_Port, LED_IND_Pin,GPIO_PIN_RESET);
			return;
		}

		if ((AFSK_packet_pos & 7) == 0) // Load up next byte
		{
			AFSK_current_byte = AFSK_packet[AFSK_packet_pos >> 3]; // divide by 8
		}
		else
		{
			AFSK_current_byte = AFSK_current_byte / 2;  // ">>1" forces int conversion
		}

		if((AFSK_current_byte & 1) == 0) // NRZI
		{
			TIM1->ARR ^= (416 ^ 208);
		}

		AFSK_packet_pos++;
	}
}

void AFSK_send_frame_dat(const uint8_t *buffer, uint16_t len) // transfer frame data
{
	AFSK_packet_size = len;
	AFSK_packet = buffer;
}

void AFSK_start_transmission(void)
{
	HAL_GPIO_WritePin(LED_IND_GPIO_Port, LED_IND_Pin,GPIO_PIN_SET);

	AFSK_packet_pos = 0;
	AFSK_trig_frame_tx = 1;

    // Wake up and configure radio
	SI446X_init();
    HAL_Delay(10);

	// Key the radio
    SI446X_PIN_power_up_EXT_PA();

    HAL_Delay(1);
    SI446X_carrier_wave_on();

	// Start transmission
    AFSK_timers_start();

    TIM1->ARR = 416; // 1200 TONE NRZI INIT
}


uint8_t AFSK_read_state(void)
{
	return AFSK_trig_frame_tx;
}


// --------------------------  |||||||||||||||||||||||| TRASH |||||||||||||||||||||||||  --------------------------



//volatile uint8_t APRS_sample_frame [1300]= {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,1,0,1,1,0,1,0,1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,1,0,0,1,0,1,0,1,0,0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,1,0,0,1,1,0,1,1,1,0,0,1,0,1,0,1,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,1,1,0,1,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,0,1,0,1,0,1,0,1,1,0,1,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,1,1,0,1,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,0,1,0,0,0,1,0,1,0,1,0,1,1,0,0,0,1,0,1,1,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,1,1,0,0,1,1,0,0,1,0,0,0,1,1,1,0,1,1,1,1,0,1,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1};
//size = 1282
//volatile uint8_t APRS_sample_frame_bytes [200] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xD4,0xCA,0x36,0x37,0x95,0x6A,0x75,0x2D,0x33,0x23,0x23,0x3B,0x95,0x8A,0x30,0xDB,0xD2,0x2C,0x8B,0x6A,0x8B,0x30,0xDB,0xD2,0x2C,0x89,0x6A,0x74,0x54,0x05,0x43,0x99,0x89,0x7B,0x79,0x20,0xB0,0x03,0x02,0x02};
//size = 161


//volatile uint8_t tx_trigger = 0;
//void AFSK_output_sample_bit(void)
//{
//	static uint16_t arr_bit_cnt;
//	if (tx_trigger == 1)
//	{
//		if(APRS_sample_frame[arr_bit_cnt] == 1)
//		{
//			TIM2->ARR = 416;
//		}
//		else
//		{
//			TIM2->ARR = 208;
//		}
//
//		if (arr_bit_cnt < 1282)
//		{
//			arr_bit_cnt++;
//		}
//		else
//		{
//			tx_trigger = 0;
//			arr_bit_cnt = 0;
//			SI446X_carrier_wave_off();
//		}
//	}
//
//}
