/***************************************
|-- AUTHOR: ANDRZEJ LACZEWSKI
|-- WEBSITE: WWW.BYTECHLAB.COM
|-- PROJECT: MAGELLAN-1 NANO SATELITE
|-- MODULE: SI446X func
|-- DESCRIPTION:
.................
***************************************/
#include "si446x.h"
#include "uart_debug.h"

// ------------------------------------------->> GPIO HANDLERS:
void SI446X_PIN_set_CS(void)
{
	HAL_GPIO_WritePin(P_RADIO_CS_GPIO_Port, P_RADIO_CS_Pin, GPIO_PIN_SET);
}

void SI446X_PIN_clear_CS(void)
{
	HAL_GPIO_WritePin(P_RADIO_CS_GPIO_Port, P_RADIO_CS_Pin, GPIO_PIN_RESET);
}

void SI446X_PIN_shutdown(void)
{
	HAL_GPIO_WritePin(P_RADIO_SDN_GPIO_Port, P_RADIO_SDN_Pin, GPIO_PIN_SET);
}

void SI446X_PIN_power_up(void)
{
	HAL_GPIO_WritePin(P_RADIO_SDN_GPIO_Port, P_RADIO_SDN_Pin, GPIO_PIN_RESET);
}

void SI446X_PIN_shutdown_TCXO(void)
{
	HAL_GPIO_WritePin(P_RADIO_TCXO_PWR_GPIO_Port,P_RADIO_TCXO_PWR_Pin, GPIO_PIN_SET);
}

void SI446X_PIN_power_up_TCXO(void)
{
	HAL_GPIO_WritePin(P_RADIO_TCXO_PWR_GPIO_Port,P_RADIO_TCXO_PWR_Pin, GPIO_PIN_RESET);
}

void SI446X_PIN_shutdown_EXT_PA(void)
{
	HAL_GPIO_WritePin(P_RADIO_PA_PWR_GPIO_Port,P_RADIO_PA_PWR_Pin, GPIO_PIN_RESET);
}

void SI446X_PIN_power_up_EXT_PA(void)
{
	HAL_GPIO_WritePin(P_RADIO_PA_PWR_GPIO_Port,P_RADIO_PA_PWR_Pin, GPIO_PIN_SET);
}
// -------------------------------------------<< GPIO HANDLERS

// ------------------------------------------->> SPI READ WRITE FUNC:

// +++++++++++ OK +++++++++++
// wirte
// TODO : WHILE TIMEOUT
void SI446X_write(uint8_t* txData, uint16_t txDataLength, uint32_t timeout)
{
	uint8_t reqACK_response[2];
	uint8_t read_CTS_cmd[2] = {0x44,0x00};

	// SPI transfer
	SI446X_PIN_clear_CS();
	HAL_SPI_Transmit(&SI446X_SPI_STRUCT,txData,txDataLength,timeout);
	SI446X_PIN_set_CS();

	// Reqest ACK by Si4464
	reqACK_response[1] = 0x00;

	while (reqACK_response[1] != 0xFF)
	{
		SI446X_PIN_clear_CS();
		HAL_SPI_TransmitReceive(&SI446X_SPI_STRUCT, read_CTS_cmd, reqACK_response, 2, timeout);
		SI446X_PIN_set_CS();
	}
}

// +++++++++++ OK +++++++++++
// read without including CTS flag in reply frame
// TODO : WHILE TIMEOUT
void SI446X_read(uint8_t* txData, uint16_t txDataLength, uint8_t* rxData, uint16_t rxDataLength, uint32_t timeout)
{
	uint8_t reqACK_response[2];
	uint8_t read_CTS_cmd[2] = {0x44,0x00};

	uint8_t NOP_cmd[SI446X_SPI_RX_MAX_LENGTH] = {0x00}; // This will init all elements of array to 0x00 NOP CMD !

	// SPI transfer
	SI446X_PIN_clear_CS();
	HAL_SPI_Transmit(&SI446X_SPI_STRUCT,txData,txDataLength,timeout);
	SI446X_PIN_set_CS();

	// Reqest ACK by Si4464
	reqACK_response[1] = 0x00;

	while (1)
	{
		SI446X_PIN_clear_CS();
		HAL_SPI_TransmitReceive(&SI446X_SPI_STRUCT, read_CTS_cmd, reqACK_response, 2, timeout);

		if (reqACK_response[1] == 0xFF)
		{
			HAL_SPI_TransmitReceive(&SI446X_SPI_STRUCT, NOP_cmd, rxData, rxDataLength, timeout);
			SI446X_PIN_set_CS();
			break;
		}
		else
		{
			SI446X_PIN_set_CS();
			continue;
		}
	}
}
// -------------------------------------------<< SPI READ WRITE FUNC

// ------------------------------------------->> INIT FUNC:
// +++++++++++ OK +++++++++++
// TODO : POPRAWI� wszystko i przejrzec
void SI446X_set_frequency(uint32_t tx_frequency, uint16_t frequency_deviation)
{
    // Set the output divider according to recommended ranges given in si446x datasheet  table.14 page 32
    int outdiv = 4;
    uint8_t FVCO_DIV = 0;

    if (tx_frequency < 960000000UL) { outdiv = 4;  FVCO_DIV = 0;};
    if (tx_frequency < 705000000UL) { outdiv = 6;  FVCO_DIV = 1;};
    if (tx_frequency < 470000000UL) { outdiv = 8;  FVCO_DIV = 2;};
    if (tx_frequency < 353000000UL) { outdiv = 12; FVCO_DIV = 3;};
    if (tx_frequency < 235000000UL) { outdiv = 16; FVCO_DIV = 4;};
    if (tx_frequency < 177000000UL) { outdiv = 24; FVCO_DIV = 5;};


    // Set the band parameter
    uint32_t sy_sel = 8;
   	uint8_t set_band_property_command[] = {0x11, 0x20, 0x01, 0x51, (FVCO_DIV + sy_sel)};
   	SI446X_write(set_band_property_command, 5,SI446X_MAX_SPI_TIMEOUT);
   	HAL_Delay(1);

   	// Set the PLL parameters
   	uint32_t f_pfd = 2 * SI446X_EXT_OSC_FREQ / outdiv;
   	uint32_t n = ((uint32_t)(tx_frequency / f_pfd)) - 1;
    float ratio = (float)tx_frequency / (float)f_pfd;
   	float rest  = ratio - (float)n;

   	uint32_t m = (uint32_t)(rest * 524288UL);
   	uint32_t m2 = m >> 16;
   	uint32_t m1 = (m - m2 * 0x10000) >> 8;
   	uint32_t m0 = (m - m2 * 0x10000 - (m1 << 8));

   	uint32_t channel_increment = 524288 * outdiv * frequency_deviation / (2 * SI446X_EXT_OSC_FREQ);
   	uint8_t c1 = channel_increment / 0x100;
   	uint8_t c0 = channel_increment - (0x100 * c1);

   	uint8_t set_frequency_property_command[] = {0x11, 0x40, 0x04, 0x00, n, m2, m1, m0, c1, c0}; //c1 c2?? do zmiany kanalow
   	SI446X_write(set_frequency_property_command, 10,SI446X_MAX_SPI_TIMEOUT);
   	HAL_Delay(1);

    // Set deviation for 2GFSK
    float units_per_hz = (( 0x40000 * outdiv ) / (float)SI446X_EXT_OSC_FREQ);
    uint32_t freq_dev = (uint32_t)(units_per_hz * frequency_deviation / 2.0 );

	uint8_t freq_dev_a = (freq_dev & 0x00FF0000) >> 16;
	uint8_t freq_dev_b = (freq_dev & 0x0000FF00) >> 8;
	uint8_t freq_dev_c = (freq_dev & 0x000000FF);
	uint8_t set_deviation[] = {0x11, 0x20, 0x03, 0x0a, freq_dev_a, freq_dev_b, freq_dev_c};
	SI446X_write(set_deviation, 7,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
void SI446X_set_power (uint8_t powerlevel)
{
    if(powerlevel > 0x7f) // Check if max power is reached
    {
    	powerlevel = 0x7f;
    }

    //CONFIG FRAME STRUCTURE:         [set_prop_cmd, group, num_of_props, start_prop, data..............]
    uint8_t set_power_level_command[] = {0x11, 0x22, 0x01, 0x01, powerlevel};
    SI446X_write(set_power_level_command,5,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
// TODO : research and calculate filter coeficients
void SI446X_set_filter(void)
{
	//uint8_t coeff[9] = {0x1d, 0xe5, 0xb8, 0xaa, 0xc0, 0xf5, 0x36, 0x6b, 0x7f};	// 6dB@1200 Hz, 2400 Hz
	//uint8_t coeff[9] = {0x07, 0xde, 0xbf, 0xb9, 0xd4, 0x05, 0x40, 0x6d, 0x7f};	// 3db@1200 Hz, 2400 Hz
	uint8_t coeff[9] = {0xfa, 0xe5, 0xd8, 0xde, 0xf8, 0x21, 0x4f, 0x71, 0x7f};	// LP only, 2400 Hz // ten wyglada dobrze
	//uint8_t coeff[9] = {0xd9, 0xf1, 0x0c, 0x29, 0x44, 0x5d, 0x70, 0x7c, 0x7f}; 	// LP only, 4800 Hz // ten wyglada dobrze
	//uint8_t coeff[9] = {0xd5, 0xe9, 0x03, 0x20, 0x3d, 0x58, 0x6d, 0x7a, 0x7f}; 	// LP only, 4400 Hz
	//uint8_t coeff[9] = {0x81, 0x9f, 0xc4, 0xee, 0x18, 0x3e, 0x5c, 0x70, 0x76};	// 6dB@1200Hz, 4400 Hz (bad stopband)

    //  set prop   group     numprops  startprop   data
    uint8_t init_fiter_8[] = {0x11,0x20,0x01,0x0f,coeff[8]};
    SI446X_write(init_fiter_8,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_7[] = {0x11,0x20,0x01,0x10,coeff[7]};
    SI446X_write(init_fiter_7,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_6[] = {0x11,0x20,0x01,0x11,coeff[6]};
    SI446X_write(init_fiter_6,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_5[] = {0x11,0x20,0x01,0x12,coeff[5]};
    SI446X_write(init_fiter_5,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_4[] = {0x11,0x20,0x01,0x13,coeff[4]};
    SI446X_write(init_fiter_4,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_3[] = {0x11,0x20,0x01,0x14,coeff[3]};
    SI446X_write(init_fiter_3,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_2[] = {0x11,0x20,0x01,0x15,coeff[2]};
    SI446X_write(init_fiter_2,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_1[] = {0x11,0x20,0x01,0x16,coeff[1]};
    SI446X_write(init_fiter_1,5,SI446X_MAX_SPI_TIMEOUT);

    uint8_t init_fiter_0[] = {0x11,0x20,0x01,0x17,coeff[0]};
    SI446X_write(init_fiter_0,5,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
// DEFAULT DATA RATE : 1 000 000 bps
void SI446X_set_data_rate(uint32_t data_rate)
{
	uint8_t data_rate_03 = (data_rate & 0x00FF0000) >> 16; // MSB
	uint8_t data_rate_04 = (data_rate & 0x0000FF00) >> 8;
	uint8_t data_rate_05 = (data_rate & 0x000000FF);       // LSB

    //CONFIG FRAME STRUCTURE:         [set_prop_cmd, group, num_of_props, start_prop, data..............]
	uint8_t set_data_rate_command[] = {0x11, 0x20, 0x03, 0x03, data_rate_03, data_rate_04, data_rate_05};
    SI446X_write(set_data_rate_command,7,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
void SI446X_read_part_info (void)
{
	uint8_t PART_INFO_command[] = {0x01}; // Part Info
    uint8_t PART_INFO_rcv_dat[8];

	SI446X_read(PART_INFO_command,1,PART_INFO_rcv_dat,8,SI446X_MAX_SPI_TIMEOUT);

	DEBUG_tx_length = sprintf(DEBUG_tx_buff,"INIT--->PART_INFO:%x,%x,%x,%x,%x,%x,%x,%x\r\n",PART_INFO_rcv_dat[0],PART_INFO_rcv_dat[1],PART_INFO_rcv_dat[2],PART_INFO_rcv_dat[3],PART_INFO_rcv_dat[4],PART_INFO_rcv_dat[5],PART_INFO_rcv_dat[6],PART_INFO_rcv_dat[7]);
	HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)DEBUG_tx_buff,DEBUG_tx_length,100 );
}

// +++++++++++ OK +++++++++++
// Power up radio module
// NO_PATCH, BOOT EZRADIO PRO, TCXO,
void SI446X_power_up_device(void)
{
    uint8_t xo_freq_03 = (SI446X_EXT_OSC_FREQ & 0xFF000000) >> 24; // MSB
	uint8_t xo_freq_04 = (SI446X_EXT_OSC_FREQ & 0x00FF0000) >> 16;
	uint8_t xo_freq_05 = (SI446X_EXT_OSC_FREQ & 0x0000FF00) >> 8;
	uint8_t xo_freq_06 = (SI446X_EXT_OSC_FREQ & 0x000000FF);       // LSB

    //CONFIG FRAME STRUCTURE:         [POWER_UP_cmd, BOOT_OPTIONS, XTAL_OPTIONS,XO_FREQ...... ]
    uint8_t init_command[] = {0x02, 0x01, 0x01, xo_freq_03, xo_freq_04, xo_freq_05, xo_freq_06};
    SI446X_write(init_command,7,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
// GPIO config: Set all GPIOs to INPUT, LOW drive strength, leave NIRQ and SDO unchanged
void SI446X_set_gpio_func(void)
{
	//CONFIG FRAME STRUCTURE:         [GPIO_PIN_CFG_cmd, GPIO0, GPIO1, GPIO2, GPIO3, NIRQ, SDO, GEN_CONFIG]
    uint8_t gpio_pin_cfg_command[] = {0x13, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x60};
    SI446X_write(gpio_pin_cfg_command,8,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
// Radio ready: clear all pending interrupts and get the interrupt status back
// TODO : return int status
void SI446X_read_int_status(void)
{
	//CONFIG FRAME STRUCTURE:         [CHANGE_STATE_cmd, PH_CLR_PEND, MODEM_CLR_PEND, CHIP_CLR_PEND]
    uint8_t get_int_status_command[] = {0x20, 0x00, 0x00, 0x00};
    uint8_t get_int_status_rcv_dat[8];
    SI446X_read(get_int_status_command,4,get_int_status_rcv_dat,8,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
// Set to 2GFSK, DIRECT, input:GPIO1, SYNC mode
void SI446X_set_mod_type(void)
{
	//CONFIG FRAME STRUCTURE:         [set_prop_cmd, group, num_of_props, start_prop, data..............]
	uint8_t set_modem_mod_type_command[] = {0x11, 0x20, 0x01, 0x00, 0b00101011}; // sync 2GFSK : 0b00101011  // async 2FSK: 0b10101010
    SI446X_write(set_modem_mod_type_command,5,SI446X_MAX_SPI_TIMEOUT);
}
// -------------------------------------------<< INIT FUNC

// ------------------------------------------->> CHANGE SI446X STATE FUNC:
// +++++++++++ OK +++++++++++
// Change to TX state
void SI446X_carrier_wave_on(void)
{
	//CONFIG FRAME STRUCTURE:         [CHANGE_STATE_cmd, data]
	uint8_t change_state_command[] = {0x34, 0x07};
    SI446X_write(change_state_command,2,SI446X_MAX_SPI_TIMEOUT);

    // Wait for TX to warm up
    HAL_Delay(10);
}

// +++++++++++ OK +++++++++++
// Change to READY state
void SI446X_carrier_wave_off(void)
{
	//CONFIG FRAME STRUCTURE:         [CHANGE_STATE_cmd, data]
	uint8_t change_state_command[] = {0x34, 0x03};
    SI446X_write(change_state_command,2,SI446X_MAX_SPI_TIMEOUT);
}

// +++++++++++ OK +++++++++++
// Change to TX_TUNE state
void SI446X_tune_tx(void)
{
	//CONFIG FRAME STRUCTURE:         [CHANGE_STATE_cmd, data]
    uint8_t change_state_command[] = {0x34, 0x05};
    SI446X_write(change_state_command,2,SI446X_MAX_SPI_TIMEOUT);
}
// -------------------------------------------<< CHANGE SI446X STATE FUNC

void SI446X_init(void) //Init and enable SI446X
{
	//ADD:
	//---------------------------
	// nie wiem czy trzeba ale mo�na ustawic
	//	PA
	//	PA_MODE : PA_SEL HP CORASE
	//	Group: 0x22
	//	Index: 0x00
	//---------------------------

	SI446X_PIN_set_CS();
	SI446X_PIN_shutdown();

	SI446X_PIN_power_up_TCXO();
	HAL_Delay(10);
	SI446X_PIN_power_up();
	HAL_Delay(10);

	SI446X_power_up_device();
    HAL_Delay(1);

    SI446X_read_part_info();
    HAL_Delay(1);

    SI446X_set_gpio_func();
    HAL_Delay(1);

//    SI446X_set_filter();
//    HAL_Delay(1);

    SI446X_read_int_status();
    HAL_Delay(1);

    SI446X_set_frequency(SI446x_FREQUENCY,5000);
    HAL_Delay(1);

    SI446X_set_mod_type();
    HAL_Delay(1);

    SI446X_set_power(25); // 60 == max power? // 10 by�o
    HAL_Delay(1);

    SI446X_set_data_rate(1000000);
    HAL_Delay(1);

    SI446X_tune_tx();
    HAL_Delay(1);

}

void SI446X_de_init(void) //Disable SI446X after transimission for power sawing
{


}

// --------------------------  |||||||||||||||||||||||| TRASH |||||||||||||||||||||||||  --------------------------

