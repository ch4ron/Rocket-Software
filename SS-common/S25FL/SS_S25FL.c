//==========================================
// Title:  FLASH data logging "file system" library
// Author: Andrzej Laczewski
// Date:   2019-02-24
// Ver:    1
// Organisation: AGH SPACE SYSTEMS
//==========================================

#include "SS_S25FL.h"
#include "stdio.h"
//  --------------------<

// ---------------------GLOBAL VAR-------------->>>>>>>>
uint8_t  MEASURE_ENABLE_FLAG = 0;
FLASH_SPACE_STATE_t FLASH_SPACE_STATE = EMPTY;

uint32_t global_curr_flash_page = FIRST_LOG_PAGE;
uint32_t global_flash_timestamp = 0;
//--------------------------
uint8_t DMA_TX_IN_PROGRESS = 0;
uint8_t DMA_RX_IN_PROGRESS = 0;
TIMEOUT_STATE_t TIMEOUT_STATE = OK;
Addr_section_last_written_t  global_addr_section_lw; // Contains info about current zero position in addr section
// ---------------------GLOBAL VAR--------------<<<<<<<<

uint8_t flash_bufor1[256];
uint8_t flash_bufor2[256];
uint16_t flash_bufor_inc = 0;
uint8_t *flash_bufor_pointer = flash_bufor1;
/*
 * Funkcje do obs�ugi GPIO
 */
// --------------------- EXTERNAL GPIO HANDLING-------------->>>>>>>>
void SS_S25FL_select(void)
{
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, RESET);
}
void SS_S25FL_deselect(void)
{
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, SET);
}
void SS_S25FL_reset_activ(void)
{
//	HAL_GPIO_WritePin(FLASH_RESET_GPIO_Port, FLASH_RESET_Pin, RESET);
}
void SS_S25FL_reset_deactiv(void)
{
//	HAL_GPIO_WritePin(FLASH_RESET_GPIO_Port, FLASH_RESET_Pin, SET);
}
// --------------------- EXTERNAL GPIO HANDLING--------------<<<<<<<<

/*
 * Systick handler odpowiedzilny za przesy� danych poprzez DMA/SPI do FLASH,a , Konieczne wywo�ywanie w przerwaniu systick minimum raz na 1 ms, funkcja odpowiedzialna za prze��czenie linii CS
 * inc_timestamp_handler odpowiedzialny za inkrementacje zmiennej od time-stamp'a log�w, nalezy wywolywac w przerwaniu od timera, jednostki do ustalenia (teraz jest chyba inkrementacja o 50 uS)
 */
// --------------------- INTERUPT ROUTINE HANDLERS-------------->>>>>>>>

void SS_S25FL__dma_transfer_cs_handler (void)// call inside SYSTICK interupt (every 1ms, or less if SPI bitrate is high) or somewhere else, responsible for switching CS after transmision,
{
	if( (!((HAL_DMA_GetState(&SPI_DMA_TX_STRUCT) == HAL_DMA_STATE_BUSY) || (HAL_SPI_GetState(&SPI_STRUCT) == HAL_SPI_STATE_BUSY_TX))) && (DMA_TX_IN_PROGRESS == 1) ){
		DMA_TX_IN_PROGRESS = 0;
//		osSemaphoreRelease(s25fl_semaphore);
		SS_S25FL_deselect();
	}
	if( (!((HAL_DMA_GetState(&SPI_DMA_RX_STRUCT) == HAL_DMA_STATE_BUSY) || (HAL_SPI_GetState(&SPI_STRUCT) == HAL_SPI_STATE_BUSY_RX))) && (DMA_RX_IN_PROGRESS == 1) ){
		DMA_RX_IN_PROGRESS = 0;
		SS_S25FL_deselect();
	}
}
void SS_S25FL_inc_timestamp_handler(void) // place somewhere in a timer interupt routine (called every 50 uS?)
{
	if(MEASURE_ENABLE_FLAG)
	{
		global_flash_timestamp += FLASH_TIMESTAMP_INC_FACTOR; // overflow after 71min??
	}
	else
	{
		global_flash_timestamp = 0;
	}
}



// --------------------- INTERUPT ROUTINE HANDLERS--------------<<<<<<<<


/*
 * Funkcje do konwersji adresu pomi�dzy r�nymi "rodzajami" adresowania pami�ci
 */
// ---------------------ADDRESS COVERSION FUNC-------------->>>>>>>>
uint32_t SS_S25FL_get_addr_from_page (uint32_t page_number ,uint8_t offset) // 1 000 000 pages max (256Mb flash), 256 bytes per page, max offset 255, counting pages from 0
{
	uint32_t address = page_number;
	address = (address << 8) + offset;
	return address;
}
uint32_t SS_S25FL_get_addr_from_sector (uint32_t sector_number ,uint16_t offset) //62500 sectors max (256Mb flash), 4096 bytes per sector, max offset 4095, counting sectors from 0
{
	uint32_t address = sector_number;
	address = (address << 12) + offset;
	return address;
}
uint32_t SS_S25FL_get_addr_from_halfblock (uint32_t halfblock_number ,uint16_t offset)
{
	uint32_t address = halfblock_number;
	address = (address << 15) + offset;
	return address;
}
uint32_t SS_S25FL_get_addr_from_block (uint32_t block_number ,uint16_t offset) //3906 blocks max (256Mb flash), 65536 bytes per block, max offset 65535, counting blocks from 0
{
	uint32_t address = block_number;
	address = (address << 16) + offset;
	return address;
}
//-------------------------------------
/*
 * Funkcje do sprawdzania na kt�rej stronie czy sektorze znajduje si� dany adres w danym formacie
 */
uint32_t SS_S25FL_get_sector_num_from_page(uint32_t page_num) // check in which sector given page is
{
	uint32_t sector_num = 0;
	int64_t curr_page;

	for (curr_page = page_num; curr_page >= 0 ; (curr_page = curr_page - 1024))
	{
		sector_num++;
	}

	return sector_num - 1; // we start from zero
}
uint32_t SS_S25FL_get_page_num_from_addr(uint32_t addr) // check in which page given addr is
{
	uint32_t page_num = 0;
	int64_t curr_addr;

	for (curr_addr = addr; curr_addr >= 0 ; (curr_addr = curr_addr - 256) )
	{
		page_num ++;
	}

	return page_num - 1; // we start from zero
}
// ---------------------ADDRESS COVERSION FUNC--------------<<<<<<<<

/*
 * Funkcje do sprawdzania zaj�to�ci peryferi�w (DMA i SPI) odpowiedzialnych za przesy� danych do flash,a
 * w celu zapobiegania wywo�aniu funkcji w trakcie pracy DMA lub standardowego wysy�ania przez SPI bez wykorzystania DMA ,
 * wait_4_spi_bus mo�e op�zniac wywo�anie danej fukcji czekajac na zakonczenie transferu DMA/SPI a� do przekroczenia maks czasu MAX_HAL_TIMEOUT,
 * Po przekroczeniu tego czasu dana funkcja zwraca b��d (nie odczeka�a si� na zako�czenie transferu)
 */
// --------------------- DATA TRANSFER STATE HANDLING FUNC-------------->>>>>>>>
B_STATE_t SS_S25FL_get_dma_transfer_state (DATA_TRANSFER_DIR_t x)  // 1 = BUSY , return dma transfer state based on global flags
{
	if(x == TX)
	{
		if(DMA_TX_IN_PROGRESS == 1) return B_BUSY;
		else return B_IDLE;
	}
	if(x == RX)
	{
		if(DMA_RX_IN_PROGRESS == 1) return B_BUSY;
		else return B_IDLE;
	}
	return B_SUCCESS;
}
B_STATE_t SS_S25FL_get_dma_spi_state (void)
{
	if( (HAL_DMA_GetState(&SPI_DMA_RX_STRUCT) == HAL_DMA_STATE_BUSY) || (HAL_DMA_GetState(&SPI_DMA_TX_STRUCT) == HAL_DMA_STATE_BUSY) || (HAL_SPI_GetState(&SPI_STRUCT) == HAL_SPI_STATE_BUSY_RX) || (HAL_SPI_GetState(&SPI_STRUCT) == HAL_SPI_STATE_BUSY_TX) || (SS_S25FL_get_dma_transfer_state(TX) == B_BUSY) || (SS_S25FL_get_dma_transfer_state(RX) == B_BUSY) )
	{
		return B_BUSY;
	}
	else
	{
		return B_IDLE;
	}
}
TIMEOUT_STATE_t SS_S25FL_wait_4_spi_bus(void)
{
	uint32_t tickstart = HAL_GetTick();
	while((SS_S25FL_get_dma_spi_state() == B_BUSY))
	{
		if((HAL_GetTick() - tickstart) > MAX_SPI_DMA_TIMEOUT)
		{
			TIMEOUT_STATE = ERR;
			return ERR;
		}
	}
	return OK;
}

// --------------------- DATA TRANSFER STATE HANDLING FUNC--------------<<<<<<<<

/*
 * Funkcje do obs�ugi podstawowych funkcjonalno�ci FLASH,a
 * Tylko do u�ytku wen�trznego (nie zabezbieczone przed wywo�anie w trakcie transferu DMA)
 */
// --------------------- BASIC FLASH HANDLING FUNC-------------->>>>>>>>
void SS_S25FL_reset_init(void)
{
	TIMEOUT_STATE = OK; // init vars

	SS_S25FL_reset_activ();
	HAL_Delay(10);
	SS_S25FL_reset_deactiv();
	HAL_Delay(50);
}
uint8_t SS_S25FL_read_status(void) // unprotected (this func is not protected against calling during DMA transmission) , only for internal use
{
	uint8_t reg = 0x05;
	SS_S25FL_select();
	HAL_SPI_Transmit(&SPI_STRUCT, &reg, 1, MAX_HAL_TIMEOUT);
	HAL_SPI_Receive(&SPI_STRUCT, &reg, 1, MAX_HAL_TIMEOUT);
	SS_S25FL_deselect();
	return reg;
}
B_STATE_t SS_S25FL_check_write_progress(void)// unprotected (this func is not protected against calling during DMA transmission) , only for internal use
{
	if( SS_S25FL_read_status() & 0x01 ) return B_BUSY;
	else return B_IDLE;
}
TIMEOUT_STATE_t SS_S25FL_wait_4_flash(uint32_t timeout)
{
	uint32_t tickstart = HAL_GetTick();
	while((SS_S25FL_check_write_progress() == B_BUSY))
	{
		if((HAL_GetTick()- tickstart) > timeout)
		{
			TIMEOUT_STATE = ERR;
			return ERR;
		}
	}
	return OK;
}
uint8_t SS_S25FL_get_id(void)// unprotected (this func is not protected against calling during DMA transmission) , only for internal use
{
	uint8_t dummy [3], ID = 0x9F;
	SS_S25FL_select();
	HAL_SPI_Transmit(&SPI_STRUCT, &ID, 1, MAX_HAL_TIMEOUT);
	HAL_SPI_Receive(&SPI_STRUCT, dummy, 2, MAX_HAL_TIMEOUT);
	HAL_SPI_Receive(&SPI_STRUCT, &ID, 1, MAX_HAL_TIMEOUT);
	SS_S25FL_deselect();
	return ID;
}
ERR_FLAG_t SS_S25FL_write_enable (void)// unprotected (this func is not protected against calling during DMA transmission) , only for internal use
{
	uint8_t reg = 0x06;
	SS_S25FL_select();
	HAL_SPI_Transmit(&SPI_STRUCT, &reg, 1, MAX_HAL_TIMEOUT);
	SS_S25FL_deselect();
	if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) return B_ERROR;
	else return B_SUCCESS;
	//while(SS_S25FL_check_write_progress() == B_BUSY);
}
// --------------------- BASIC FLASH HANDLING FUNC--------------<<<<<<<<

/*
 * Funkcje do kasowania r�znych "obszar�w" pami�ci,
 */
// --------------------- ERASE OPERATIONS WITHOUT DMA-------------->>>>>>>>
ERR_FLAG_t SS_S25FL_erase_sector(uint32_t address) //Erases given sector
{
	if(SS_S25FL_wait_4_spi_bus() == OK){

		uint8_t config_buff [5];

		config_buff[0] = 0xDC;
		config_buff[1] = (address>>24) & 0xff;
		config_buff[2] = (address>>16) & 0xff;
		config_buff[3] = (address>>8) & 0xff;
		config_buff[4] = (address) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;
		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);

		SS_S25FL_deselect();

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_ERASE_TIMEOUT*1000) == ERR) goto FLASH_ERROR;
#ifdef DEBUG_LVL_1
		printf("Erasing sector at address %d\r\n", address);
#endif
		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
	    return B_ERROR;
	}
}
ERR_FLAG_t SS_S25FL_erase_halfblock(uint32_t adress) ////Erases given halfblock ---->  Not implemented , not tested !!!!!!!!!
{
	if(SS_S25FL_wait_4_spi_bus() == OK){

		uint8_t config_buff [5];
		config_buff[0] = 0x53;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;
		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);

		SS_S25FL_deselect();

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_ERASE_TIMEOUT) == ERR) goto FLASH_ERROR;

		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
		return B_ERROR;
	}
}
ERR_FLAG_t SS_S25FL_erase_block(uint32_t block_num) // //Erases given block
{
	if(SS_S25FL_wait_4_spi_bus() == OK){

		uint8_t config_buff [5];
		uint32_t adress;

		adress = SS_S25FL_get_addr_from_block(block_num,0);
		config_buff[0] = 0xDC;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;
		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);

		SS_S25FL_deselect();

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_ERASE_TIMEOUT) == ERR) goto FLASH_ERROR;

		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
		return B_ERROR;
	}
}
uint8_t erasing = 0;
ERR_FLAG_t SS_S25FL_erase_full_chip(void) // Erases full FLASH ,very long command , up to 140 s !!!
{
	erasing = 1;
	printf("Erasing full chip\r\n");
	if(SS_S25FL_wait_4_spi_bus() == OK){
		uint8_t reg = 0x60;
		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;
		SS_S25FL_write_enable();
		SS_S25FL_select();
		HAL_SPI_Transmit(&SPI_STRUCT, &reg, 1, MAX_HAL_TIMEOUT);
		SS_S25FL_deselect();
		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_ERASE_TIMEOUT) == ERR) goto FLASH_ERROR;
		SS_S25FL_saving_init();
		printf("Erase done\r\n");
		erasing = 0;
		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
		printf("Flash error\r\n");
		erasing = 0;
		return B_ERROR;
	}
}

uint8_t transmit = 0;
void SS_S25FL_SystickCallback() {
	static uint32_t s25fl_tick = 0;
		if(erasing) {
			if(HAL_GetTick() - s25fl_tick > 150) {
				s25fl_tick = HAL_GetTick();
//				SS_led_MEM_toggle2(50, 0, 0, 0, 0, 50);
			}
		} else if(MEASURE_ENABLE_FLAG) {
			if(HAL_GetTick() - s25fl_tick > 150) {
				s25fl_tick = HAL_GetTick();
//				if(FLASH_SPACE_STATE == FULL)
//					SS_led_MEM_toggle(50, 0, 0);
			}

		} else if(!transmit){
//			SS_led_MEM_set(5, 5, 5);
		}

}
// --------------------- ERASE OPERATIONS WITHOUT DMA--------------<<<<<<<

/*
 * Funkcje do zapisu/odczytu FLASH,a bez wykorzystania DMA
 */
// --------------------- READ WRITE OPERATIONS WITHOUT DMA-------------->>>>>>>>
ERR_FLAG_t SS_S25FL_write_bytes(uint32_t adress, uint8_t *array, uint8_t length) // Write specified number of bytes to FLASH , non DMA
{
	if(SS_S25FL_wait_4_spi_bus() == OK){

		uint8_t config_buff [5];
		config_buff[0] = 0x12;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;

		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);
		HAL_SPI_Transmit(&SPI_STRUCT, array, length, MAX_HAL_TIMEOUT);

		SS_S25FL_deselect();

		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
	    return B_ERROR;
	}
}
ERR_FLAG_t SS_S25FL_write_page(uint32_t page, uint8_t *array) // Write single page to FLASH , non DMA
{
	if(SS_S25FL_wait_4_spi_bus() == OK){

		uint32_t adress = SS_S25FL_get_addr_from_page(page,0);
		uint8_t config_buff [5];
		config_buff[0] = 0x12;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;

		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);
		HAL_SPI_Transmit(&SPI_STRUCT, array, 256, MAX_HAL_TIMEOUT);

		SS_S25FL_deselect();

		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
	    return B_ERROR;
	}
}
ERR_FLAG_t SS_S25FL_read_page(uint32_t page, uint8_t *array) // Read single page from FLASH , non DMA
{
	if(SS_S25FL_wait_4_spi_bus() == OK){

		uint32_t adress = SS_S25FL_get_addr_from_page(page,0);
		uint8_t config_buff [5];
		config_buff[0] = 0x13;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;

		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);
		HAL_SPI_Receive(&SPI_STRUCT, array, 256, MAX_HAL_TIMEOUT);

		SS_S25FL_deselect();

		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
	    return B_ERROR;
	}
}
// --------------------- READ WRITE OPERATIONS WITHOUT DMA--------------<<<<<<<

/*
 * Funkcje do zapisu/odczytu FLASH,a z wykorzystaniem DMA
 */
// --------------------- READ WRITE OPERATIONS WITH DMA-------------->>>>>>>>
ERR_FLAG_t SS_S25FL_write_page_dma(uint32_t page, uint8_t *array) // Write single page to FLASH , with use of DMA
{
	static uint32_t counter = 0;
	if(HAL_GetTick() - counter > 200) {
//		SS_led_MEM_toggle2(0, 50, 0, 0, 0, 50);
		counter = HAL_GetTick();
	}
	if(SS_S25FL_wait_4_spi_bus() == OK){ // Protection against too early func call

		uint32_t adress = SS_S25FL_get_addr_from_page(page,0);
		uint8_t config_buff [5];
		config_buff[0] = 0x12;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;

		SS_S25FL_write_enable();
		SS_S25FL_select();


		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);

		HAL_SPI_Transmit_DMA(&SPI_STRUCT, array, 256);

		DMA_TX_IN_PROGRESS = 1;
#ifdef DEBUG_LVL_1
		printf("Writing page: %d, at address %d\r\n", page, adress);
#endif
		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
		return B_ERROR;
	}

}
ERR_FLAG_t SS_S25FL_read_page_dma(uint32_t page, uint8_t *array) // Read single page from FLASH , with use of DMA
{
	if(SS_S25FL_wait_4_spi_bus() == OK){ // Protection against too early func call

		uint32_t adress = SS_S25FL_get_addr_from_page(page,0);
		uint8_t config_buff [5];
		config_buff[0] = 0x13;
		config_buff[1] = (adress>>24) & 0xff;
		config_buff[2] = (adress>>16) & 0xff;
		config_buff[3] = (adress>>8) & 0xff;
		config_buff[4] = (adress) & 0xff;

		if (SS_S25FL_wait_4_flash(MAX_WAIT_4_FLASH_TIMEOUT) == ERR) goto FLASH_ERROR;

		SS_S25FL_write_enable();
		SS_S25FL_select();

		HAL_SPI_Transmit(&SPI_STRUCT, config_buff, 5, MAX_HAL_TIMEOUT);

		HAL_SPI_Receive_DMA(&SPI_STRUCT, array, 256);

		DMA_RX_IN_PROGRESS = 1;

		return B_SUCCESS;
	}
	else {
		FLASH_ERROR:
		return B_ERROR;
	}

}
// --------------------- READ WRITE OPERATIONS WITH DMA--------------<<<<<<<<

/*
 * Funkcje odpowiedzialne za obs�ug� sekcji pierwszych 500 stron w pami�ci odpowiedzialnych za odresowanie ostatniej strony na kt�rej zapisane s� logi,
 * Zapis ilo�ci zapisanych stron polega na kasowaniu kolejnych jedynek zaczynaj�c od MSB w kolejnych bajtach w tej sekcji poprzez zast�pienie ich zerami,
 * Kazde zero w tej sekcji oznacza jedna zapisan� stron� zaczynaj�c od 500,nej
 * przyk�ad MSB ->>> (adres zerowy) [0001 1111] ->>> LSB = 3 zapisane strony
 */
// --------------------- ADDRESS SECTION HANDLING FUNTIONS-------------->>>>>>>
Addr_section_last_written_t SS_S25FL__addr_section__find_last_written_byte (void)
{
	uint8_t flash_buff[256];
	uint32_t page_num;
	uint16_t byte_num;
	uint8_t found_end = 0;

	uint8_t last_written_byte;
	uint8_t last_written_byte_num_of_zeroes = 0;
	uint32_t last_written_byte_addr = 0;

	Addr_section_last_written_t last_written_index;

	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("Last written page found\r\n");
	#endif// ---------------------------------------DEBUG-----------------<

	for (page_num = 0; page_num <= 500 ; page_num ++ )
	{
		#ifdef DEBUG_LVL_3 // --------------------------DEBUG----------------->
		printf("__page_loop_iter__\r\n")
		#endif// ---------------------------------------DEBUG-----------------<

		SS_S25FL_read_page(page_num, flash_buff);

		 	for (byte_num = 0; byte_num <= 255 ; byte_num ++ ) // find last unwritten byte (0xFF)
			{
				#ifdef DEBUG_LVL_3 // --------------------------DEBUG----------------->
		 		printf_1("LST UN WRI BYT: %lu\r\n",byte_num)
				#endif// ---------------------------------------DEBUG-----------------<

				if(flash_buff[byte_num] == 0xFF)
				{
					found_end = 1;
					break;
				}
				else
				{
					last_written_byte_addr ++;
				}
			}
			if (found_end == 1)
			{
				found_end = 0;
				break;
			}
		}

	if (last_written_byte_addr == 0) // handle overflow (-1)
	{
		last_written_index.byte_addr = 0;
		last_written_index.byte_num_of_zeroes = 0;
	}
	else
	{
		last_written_byte = ~flash_buff[byte_num-1]; // invert to read num of zeroes

		while(last_written_byte != 0)
		{
			if((last_written_byte) & 0b10000000)
			{
				last_written_byte_num_of_zeroes ++;
			}
			last_written_byte = last_written_byte << 1;
		}

		last_written_index.byte_addr = last_written_byte_addr-1;
		last_written_index.byte_num_of_zeroes = last_written_byte_num_of_zeroes-1; // -1 for bit shift
	}

	return last_written_index;
}
void SS_S25FL__addr_section__byte_zeroes_inc (void) // num. of zeroes overflow handling
{
	if(global_addr_section_lw.byte_num_of_zeroes < 7)
	{
		global_addr_section_lw.byte_num_of_zeroes ++;
	}
	else
	{
		global_addr_section_lw.byte_num_of_zeroes = 0;
		global_addr_section_lw.byte_addr ++;
	}
}
uint32_t SS_S25FL__addr_section__get_last_log_page_init (void) // execute once on startup to get initial page number and init global var
{
	uint32_t last_log_page;

	global_addr_section_lw = SS_S25FL__addr_section__find_last_written_byte();

	last_log_page = (FIRST_LOG_PAGE - 1) + (global_addr_section_lw.byte_addr*8) + (global_addr_section_lw.byte_num_of_zeroes + 1); // start from 501

	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("Byte_addr_get/num of zzeroes_get_init %lu,%u\r\n",global_addr_section_lw.byte_addr,global_addr_section_lw.byte_num_of_zeroes);
	#endif// ---------------------------------------DEBUG-----------------<

	if(last_log_page != FIRST_LOG_PAGE)
	{
		SS_S25FL__addr_section__byte_zeroes_inc(); // move to next pos for further saving (prevent saving twice on the same pos)

		#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
		printf("Byte_addr_get/num of zzeroes_get_init(inc zeroes) %lu,%u\r\n",global_addr_section_lw.byte_addr,global_addr_section_lw.byte_num_of_zeroes);
		#endif// ---------------------------------------DEBUG-----------------<
	}

	return last_log_page;
}
uint32_t SS_S25FL__addr_section__get_last_log_page (void) // returns last log page (written)
{
	uint32_t last_log_page;
	Addr_section_last_written_t last_written_index;

	last_written_index = SS_S25FL__addr_section__find_last_written_byte();

	last_log_page = (FIRST_LOG_PAGE - 1) + (last_written_index.byte_addr*8) + (last_written_index.byte_num_of_zeroes + 1); // start from 501 or 1001

	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("Last written page %ld\r\n",last_log_page);
	#endif// ---------------------------------------DEBUG-----------------<

	return last_log_page;
}
void SS_S25FL__addr_section__inc_last_log_page (void) // icrements addr section by one zero
{
	uint8_t write_byte = 0;

	write_byte = ~ ( 0b10000000 >> global_addr_section_lw.byte_num_of_zeroes);
	SS_S25FL_write_bytes(global_addr_section_lw.byte_addr,&write_byte,1);

	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("write_byte/addr/num of zeroes: %d,%ld,%d\r\n",write_byte,global_addr_section_lw.byte_addr,global_addr_section_lw.byte_num_of_zeroes);
	#endif// ---------------------------------------DEBUG-----------------<
	SS_S25FL__addr_section__byte_zeroes_inc();
}
// --------------------- ADDRESS SECTION HANDLING FUNTIONS--------------<<<<<<<

/*
 * erase_only_written_pages s�y�y do kasowania tylko zapisanych stron w pami�ci, najpierw odczytywana jest ilo�c zapisanych stron a potem na tej podstawie kasowane sa zapisane strony
 * w sekcji za odpowiedzialnej za przchowywanie log�w (stony od 500) , nast�pnie ksaowana jest sekcja przechowyuj�ca adres ostatniej zapisanej strony z logami (strony od 0 do 500)
 */
// --------------------- ERASE ONLY WRITTEN PAGES FUNC-------------->>>>>>>
void SS_S25FL_erase_in_page_range (uint32_t start_page, uint32_t stop_page) // erases in given page range including these pages
{
	uint32_t curr_sector;
	uint32_t start_sector;
	uint32_t stop_sector;

	start_sector = SS_S25FL_get_sector_num_from_page(start_page);
	stop_sector = SS_S25FL_get_sector_num_from_page(stop_page);

	for (curr_sector = start_sector; curr_sector <=  stop_sector ; curr_sector++ )
	{
		SS_S25FL_erase_sector(curr_sector);
	}
}
void SS_S25FL__addr_section__erase (void) // erases addr section
{
 	#if defined FLASH_256MB
	SS_S25FL_erase_block(0);     // 0 -- 256 page
	SS_S25FL_erase_block(1);     // 256 -- 512 page
	#endif

	#if defined FLASH_512MB
	SS_S25FL_erase_block(0);   // 0-----
	SS_S25FL_erase_block(1);
	SS_S25FL_erase_block(2);
	SS_S25FL_erase_block(3);   // -----1024
	#endif

	global_addr_section_lw.byte_addr = 0;
	global_addr_section_lw.byte_num_of_zeroes = 0;

	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("------------Erased addr section ----------\r\n");
	#endif// ---------------------------------------DEBUG-----------------<
}
void SS_S25FL_erase_only_written_pages(void) // erases only written pages in addr section and log section
{
	printf("Erasing written pages\r\n");
	uint32_t last_written_page;

	erasing = 1;
	last_written_page = SS_S25FL__addr_section__get_last_log_page();
	uint32_t last_written_address = SS_S25FL_get_addr_from_page(last_written_page, 0);
	for(uint32_t i = 0; i < last_written_address; i += 262144) {
		SS_S25FL_erase_sector(i);
	}

	erasing = 0;

//	SS_S25FL_erase_in_page_range (FIRST_LOG_PAGE,last_written_page);

//	SS_S25FL__addr_section__erase();

	FLASH_SPACE_STATE = EMPTY;
	SS_S25FL_saving_init();
	printf("Erase done\r\n");
	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("------------Erased Written pages ----------\r\n");
	#endif// ---------------------------------------DEBUG-----------------<
}
// --------------------- ERASE ONLY WRITTEN PAGES FUNC--------------<<<<<<<

/*
 *  Funkcje do obs�ugi zapisu log�w
 */
// --------------------- DATA LOGGING FUNC -------------->>>>>>>
void SS_S25FL_check_flash_overflow (void)
{
	if (global_curr_flash_page >= FLASH_SIZE_PAGES )
	{
		FLASH_SPACE_STATE = FULL;
	}
	else
	{
		FLASH_SPACE_STATE = NOT_FULL;
	}
}


void SS_S25FL_saving_init(void) // var logging init function , call once on startup
{
	volatile uint32_t last_log_page = SS_S25FL__addr_section__get_last_log_page_init();

	if(last_log_page == FIRST_LOG_PAGE) // handle overwritting last page on startup
	{
		global_curr_flash_page = FIRST_LOG_PAGE;
	}
	else
	{
		global_curr_flash_page = last_log_page + 1 ;
	}

	SS_S25FL_check_flash_overflow();

	global_flash_timestamp = 0;
	MEASURE_ENABLE_FLAG = 0;

	#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
	printf("------------Logging init, first page to write: %ld ----------\r\n",global_curr_flash_page);
	#endif// ---------------------------------------DEBUG-----------------<
}
void SS_S25FL_start_logging(void) // call to start logging data to flash
{
	flash_bufor_inc = 0;
	global_flash_timestamp = 0;
	MEASURE_ENABLE_FLAG = 1;

}
void SS_S25FL_stop_logging(void) // call to stop logging data to flash
{
	MEASURE_ENABLE_FLAG = 0;
}
// --------------------- DATA LOGGING FUNC --------------<<<<<<<


// --------------------- OTHER -------------->>>>>>>
uint32_t SS_S25FL_get_free_space_in_pages(void) // call to get free space left in pages (256 bytes per page)
{
	if ( global_curr_flash_page < FLASH_SIZE_PAGES)
	{
		return  (FLASH_SIZE_PAGES - global_curr_flash_page);
	}
	else
	{
		return 0;
	}
}
TIMEOUT_STATE_t SS_S25FL_get_timeout(void) // get global timeout flag
{
	return TIMEOUT_STATE;
}
// --------------------- OTHER --------------<<<<<<<








//------------------------------ TEST FUNC-------------------------------- >>>>>>>>>>>>>>>>>>>
#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
void SS_S25FL_dump_page_to_uart(uint32_t page_num)
{
	uint16_t i;
	uint8_t page_buff[256];

	debug_tx_length = sprintf(debug_tx_char_buff, "<<---- PAGE NUM: %ld ---->>\r\n",page_num);
	HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);


	SS_S25FL_read_page(page_num, page_buff);

	for (i=0;i<=255;i++)
	{
		debug_tx_length = sprintf(debug_tx_char_buff, "%X,",page_buff[i]);
		HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
	}

	debug_tx_length = sprintf(debug_tx_char_buff, "\r\n<<---------------------->>\r\n");
	HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
}
void SS_S25FL_test_get_addr_from_page(void)
{
	uint32_t curr_page;

	for(curr_page = 0 ; curr_page <= 500 ; curr_page++)
	{
		debug_tx_length = sprintf(debug_tx_char_buff, "Address:%ld,Page num:%ld \r\n",SS_S25FL_get_addr_from_page (curr_page,0),curr_page);
		HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
	}
}
void SS_S25FL_test_get_addr_from_sector(void)
{
	uint32_t curr_sector;

	for(curr_sector = 0 ; curr_sector <= 10 ; curr_sector++)
	{
		debug_tx_length = sprintf(debug_tx_char_buff, "Address:%ld,Sector num:%ld \r\n",SS_S25FL_get_addr_from_sector(curr_sector,0),curr_sector);
		HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
	}
}
void SS_S25FL_test_get_addr_from_block(void)
{
	uint32_t curr_block;

	for(curr_block =3900 ; curr_block <= 3906 ; curr_block ++)
	{
		debug_tx_length = sprintf(debug_tx_char_buff, "Address:%ld,Block num:%ld \r\n",SS_S25FL_get_addr_from_block(curr_block,0),curr_block);
		HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
	}
}
void SS_S25FL_test_get_sector_num_from_page(void)
{
	uint32_t curr_page;
	for(curr_page = 0 ; curr_page <= 500 ; curr_page++)
	{
		debug_tx_length = sprintf(debug_tx_char_buff, "Sector num:%ld,Page num:%ld \r\n",SS_S25FL_get_sector_num_from_page(curr_page),curr_page);
		HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
	}
}
void SS_S25FL_test_get_page_num_from_addr(void)
{
	uint32_t curr_addr;
	for(curr_addr = 0 ; curr_addr <= 1000 ; curr_addr ++)
	{
		debug_tx_length = sprintf(debug_tx_char_buff, "Page num:%ld,Addr num:%ld \r\n",SS_S25FL_get_page_num_from_addr(curr_addr),curr_addr);
		HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
	}
}
//---------------------------------------------------------------------------------------
void SS_S25FL_test__addr_section__get_last_log_page_init (void)
{
	SS_S25FL__addr_section__erase ();
	uint32_t i;
	for (i=0;i<=500;i++)
	{
		SS_S25FL_dump_page_to_uart(i);
	}

	debug_tx_length = sprintf(debug_tx_char_buff, "Page num:%ld \r\n",SS_S25FL__addr_section__get_last_log_page_init());
	HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
}

void SS_S25FL__addr_section__test_1 (void)
{
	//SS_S25FL__addr_section__erase();

	debug_tx_length = sprintf(debug_tx_char_buff, "Last page num: %lu\r\n",SS_S25FL__addr_section__get_last_log_page_init());
	HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);

	for(uint16_t cnt = 1; cnt <= 4; cnt++)
	{
		SS_S25FL__addr_section__inc_last_log_page();
	}

	debug_tx_length = sprintf(debug_tx_char_buff, "After inc page num: %lu\r\n",SS_S25FL__addr_section__get_last_log_page());
	HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT);
}
void SS_S25FL_test_2 (void)
{

	//SS_S25FL__addr_section__erase();

	uint8_t write_byte = 0b10111111;
	SS_S25FL_write_bytes(0,&write_byte,1);

	HAL_Delay(10);

	uint8_t buf_rcv [256];
	SS_S25FL_read_page_dma(0,buf_rcv);

	HAL_UART_Transmit(&UART_DEBUG_STRUCT, "xxxxx",5, MAX_HAL_TIMEOUT);
	HAL_UART_Transmit(&UART_DEBUG_STRUCT, buf_rcv, 256, MAX_HAL_TIMEOUT);

}
void SS_S25FL_test_logs (void)
{

	MEASURE_ENABLE_FLAG = 1;

	uint16_t i;

	for(i=0;i<=512;i++)
	{
		SS_S25FL_save_variable_u32(1, i);;
	}

	SS_S25FL_read_data_logs_to_uart(1);

	for (i=0;i<=50;i++)
	{
		SS_S25FL_dump_page_to_uart(i);
	}

	for (i=470;i<560;i++)
	{
		SS_S25FL_dump_page_to_uart(i);
	}

}
void SS_S25FL_test_erase_in_range(void)
{
	uint8_t page_buff [256];
	uint32_t i;

	for (i = 0; i <= 255 ; i++)
	{
		page_buff[i]= 0x00;
	}

	for (i=0;i<=500;i++)
	{
		SS_S25FL_write_page(i,page_buff);
	}

	SS_S25FL_erase_in_page_range (16,96);

	for (i=0;i<=500;i++)
	{
		SS_S25FL_dump_page_to_uart(i);
	}

}
void SS_S25FL_test_1(void)
{

	//SS_S25FL_erase_full_chip();
    //HAL_Delay(200);

	/*uint8_t buf [256];


	for (uint32_t i = 0; i<256;i++){ // fill with val
		buf [i]= 0xAA;
	}
	buf [0] = 0xAF;
	buf [1] = 0xAE;
	buf [2] = 0xAD;
	buf [3] = 0xAC;
	buf [254] = 0xA9;
	buf [255] = 0xAB;

	SS_S25FL_write_page_dma(116, buf);*/

	uint8_t buf_rcv [256];
	SS_S25FL__addr_section__erase();
	HAL_Delay(10);
	SS_S25FL_read_page_dma(400,buf_rcv);

	HAL_UART_Transmit(&UART_DEBUG_STRUCT, "Test start:", 12, MAX_HAL_TIMEOUT);
	HAL_UART_Transmit(&UART_DEBUG_STRUCT, buf_rcv, 256, MAX_HAL_TIMEOUT);

	//uint8_t data;
	//data = SS_S25FL_get_id();
	//HAL_UART_Transmit(&UART_DEBUG_STRUCT, "abcd", 4, 200);
	//HAL_UART_Transmit(&UART_DEBUG_STRUCT, &data, 1, 200);

}
#endif// ---------------------------------------DEBUG-----------------<
//------------------------------ TEST FUNC-------------------------------- >>>>>>>>>>>>>>>>>>>

//------------------------------- TRASH:   [XXXXXXXXXXXXXXXXXXXX]
/*uint32_t SS_S25FL_REETURN_LAST_PAGE_NUM(void)
{
	return page;
	HAL_PWR_EnableBkUpAccess();
	uint32_t page = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
	HAL_PWR_DisableBkUpAccess();
}
void SS_S25FL_SAVE_LAST_PAGE_NUM(uint32_t page)
{
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, page);
	HAL_PWR_DisableBkUpAccess();
}*/
union fu {
	float f;
	uint32_t u;
};

union iu {
	int16_t i;
	uint16_t u;
};
void SS_S25FL_save_3x_int16_t(uint8_t variable_id, int16_t data1, int16_t data2, int16_t data3) {
	uint32_t flash_time;
//	if(data3 < 100) printf("%d\r\n", data3);
	if (MEASURE_ENABLE_FLAG) {
		flash_time = global_flash_timestamp / 100; // divide to get 0.1ms per 1?
		flash_bufor_pointer[flash_bufor_inc++] = variable_id;
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time >> 16);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time >> 8);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time);
		save_flash_bufor();

		union iu iu1 = { .i = data1 };
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (iu1.u >> 8);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (iu1.u);
		save_flash_bufor();

		union iu iu2 = { .i = data2 };
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (iu2.u >> 8);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (iu2.u);
		save_flash_bufor();

		union iu iu3 = { .i = data3 };
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (iu3.u >> 8);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (iu3.u);
		save_flash_bufor();

	}
}
//void SS_S25FL_save_float(uint8_t variable_id, float data) {
//	union fu fu = { .f = data };
//	SS_S25FL_save_variable_u32(variable_id, fu.u);
//}
//void SS_S25FL_save_3_floats(uint8_t variable_id, float data1, float data2, float data3) {
//	union fu fu1 = { .f = data1 };
//	union fu fu2 = { .f = data2 };
//	union fu fu3 = { .f = data3 };
//	uint32_t flash_time;
//	if (MEASURE_ENABLE_FLAG) {
//		flash_time = global_flash_timestamp / 100; // divide to get 0.1ms per 1?
//
//		flash_bufor_pointer[flash_bufor_inc++] = variable_id;
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time >> 16);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time >> 8);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time);
//		save_flash_bufor();
//
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu1.u >> 24);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu1.u >> 16);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu1.u >> 8);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu1.u);
//		save_flash_bufor();
//
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu2.u >> 24);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu2.u >> 16);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu2.u >> 8);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu2.u);
//		save_flash_bufor();
//
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu3.u >> 24);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu3.u >> 16);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu3.u >> 8);
//		save_flash_bufor();
//		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (fu3.u);
//		save_flash_bufor();
//	}
//}

void save_flash_bufor() {
	SS_S25FL_check_flash_overflow();
	if(FLASH_SPACE_STATE != FULL) {
		if (flash_bufor_inc == 256) {
			SS_S25FL__addr_section__inc_last_log_page();
			if (flash_bufor_pointer == flash_bufor1) {
//					SS_led_MEM_set(0, 0, 80);
					SS_S25FL_write_page_dma(global_curr_flash_page, flash_bufor1);
//				}
				flash_bufor_pointer = flash_bufor2;
			} else {
//				if(osSemaphoreAcquire(s25fl_semaphore, 1) == osOK) {
//					SS_led_MEM_set(0, 50, 0);
					SS_S25FL_write_page_dma(global_curr_flash_page, flash_bufor2);
//				}
				flash_bufor_pointer = flash_bufor1;
			}
			global_curr_flash_page++;
			flash_bufor_inc = 0;
		}
	}
}

uint32_t baro_counter = 0;

void SS_S25FL_save_variable_u32(uint8_t variable_id, uint32_t data) {
	uint32_t flash_time;
	if (MEASURE_ENABLE_FLAG) {
		flash_time = global_flash_timestamp / 100; // divide to get 0.1ms per 1?
		flash_bufor_pointer[flash_bufor_inc++] = variable_id;
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time >> 16);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time >> 8);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (flash_time);
		save_flash_bufor();

		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (data >> 24);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (data >> 16);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (data >> 8);
		save_flash_bufor();
		flash_bufor_pointer[flash_bufor_inc++] = (uint8_t) (data);
		save_flash_bufor();
		baro_counter++;
	}
}


void SS_S25FL_read_data_logs_to_uart(uint8_t id)
	{
	transmit = 1;
	uint32_t check_page = FIRST_LOG_PAGE;
	uint8_t check_array[256];
	uint32_t num =  global_curr_flash_page - FIRST_LOG_PAGE;
	uint8_t data[4];
	data[0] = (uint8_t) (num >> 24);
	data[1] = (uint8_t) (num >> 16);
	data[2] = (uint8_t) (num >> 8);
	data[3] = (uint8_t) (num);
	HAL_UART_Transmit(&huart5, data, 4, 1000);
	for(uint32_t page = 0; page < num; page++) {
		SS_S25FL_read_page(check_page + page, check_array);
		HAL_UART_Transmit(&huart5, check_array, 256, 1000);
//		SS_led_MEM_toggle(0, 0, 50);
	}
	transmit = 0;

}

void SS_S25FL_find_erased() {
       uint32_t check_page = FIRST_LOG_PAGE;
       uint8_t check_array[256];
       for(uint32_t page = 0;; page++) {
               SS_S25FL_read_page(check_page + page, check_array);
               for(uint16_t i = 0; i < 256; i++){
                       if(check_array[i] != 0xff) {
//                             HAL_UART_Transmit(&huart1, check_array, 256, 1000);
                               printf("page: %lu, i: %d\r\n", page, i);
                               return;
                       }
               }
//               SS_led_MEM_toggle(0, 0, 50);
       }
}

