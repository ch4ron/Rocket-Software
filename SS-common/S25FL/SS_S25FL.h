#include "stm32f4xx_hal.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

extern uint8_t  MEASURE_ENABLE_FLAG;
extern uint32_t global_curr_flash_page;
extern uint32_t global_flash_timestamp ;
extern uint8_t DMA_TX_IN_PROGRESS;
extern uint8_t DMA_RX_IN_PROGRESS;
extern uint32_t baro_counter;
typedef struct{
    uint8_t byte_num_of_zeroes; //from 7 to 0  MSB---> LSB
    uint32_t byte_addr;
} Addr_section_last_written_t;
typedef enum { TX,RX } DATA_TRANSFER_DIR_t;
typedef enum { ERR,OK } TIMEOUT_STATE_t;
typedef enum { B_BUSY,B_IDLE } B_STATE_t;
typedef enum { B_ERROR,B_SUCCESS } ERR_FLAG_t;
typedef enum { FULL,EMPTY,NOT_FULL } FLASH_SPACE_STATE_t;

extern FLASH_SPACE_STATE_t FLASH_SPACE_STATE;
#define TRUE 1
#define FALSE 0

//------------------USER DEFINES ( YOU CAN CHANGE IT :) )------------------------------->
//#define DEBUG_LVL_1
//#define DEBUG_LVL_2
//#define DEBUG_LVL_3

#define FLASH_TIMESTAMP_INC_FACTOR 50

#define MAX_HAL_TIMEOUT 250       //250 ms
#define MAX_SPI_DMA_TIMEOUT 250
#define MAX_WAIT_4_FLASH_TIMEOUT 1000    //1 s
#define MAX_WAIT_4_FLASH_ERASE_TIMEOUT 300000 //5 min

//#define FLASH_256MB
#define FLASH_512MB

#define HAL_SPI_INSTANCE SPI2
#define HAL_TIM_INSTANCE TIM8

extern SPI_HandleTypeDef hspi4;
extern DMA_HandleTypeDef hdma_spi4_tx;
extern DMA_HandleTypeDef hdma_spi4_rx;

#define SPI_STRUCT hspi4
#define SPI_DMA_TX_STRUCT hdma_spi4_tx
#define SPI_DMA_RX_STRUCT hdma_spi4_rx
#define UART_DEBUG_STRUCT huart5
//---------------------------------------------------------------------------------------<
//------------------------------------------------->
#if defined  FLASH_256MB

#define FLASH_SIZE_BYTES ((uint32_t) 256000000 )    //256Mb
#define FIRST_LOG_PAGE   ((uint32_t) 501 )

#elif defined FLASH_512MB

#define FLASH_SIZE_BYTES ((uint32_t) 512000000 )   //512Mb
#define FIRST_LOG_PAGE   ((uint32_t) 1001 )

#endif

#define FLASH_MAX_SIZE_OFFSET ((uint32_t) 50 ) // aby nie powzwolic na dotarcie do konca pamieci

#define FLASH_SIZE_PAGES ( (FLASH_SIZE_BYTES/256) - FLASH_MAX_SIZE_OFFSET )
//-------------------------------------------------<
//------------------------------------------------->
#define __print_debug_to_UART_1(x,y) debug_tx_length = sprintf(debug_tx_char_buff,x,y);\
HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT );

#define __print_debug_to_UART_2(x,y,z) debug_tx_length = sprintf(debug_tx_char_buff,x,y,z);\
HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT );

#define __print_debug_to_UART_3(x,y,z,m) debug_tx_length = sprintf(debug_tx_char_buff,x,y,z,m);\
HAL_UART_Transmit(&UART_DEBUG_STRUCT,(uint8_t *)debug_tx_char_buff,debug_tx_length, MAX_HAL_TIMEOUT );
//-------------------------------------------------<
// --------------------------DEBUG----------------->
#if defined DEBUG_LVL_1 || defined DEBUG_LVL_2 || defined DEBUG_LVL_3
char debug_tx_char_buff [500];
uint16_t debug_tx_length;
#endif
// ----------------------------DEBUG---------------<

void SS_S25FL_reset_init (void);
void SS_S25FL_saving_init(void);

void SS_S25FL__dma_transfer_cs_handler (void);
void SS_S25FL_inc_timestamp_handler(void);  // place somewhere in a timer interupt routine (called every 50 uS?)

void SS_S25FL_erase_only_written_pages (void);
ERR_FLAG_t SS_S25FL_erase_full_chip(void);
void SS_S25FL_start_logging(void);
void SS_S25FL_stop_logging(void);
uint32_t SS_S25FL_get_free_space_in_pages(void);
TIMEOUT_STATE_t SS_S25FL_get_timeout (void);

///--------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef DEBUG_LVL_1 // --------------------------DEBUG----------------->
void SS_S25FL_dump_page_to_uart(uint32_t page_num);
#endif

void SS_S25FL_save_variable_u32(uint8_t variable_id, uint32_t data);
void SS_S25FL_read_data_logs_to_uart(uint8_t id);
void SS_S25FL_save_3_floats(uint8_t variable_id, float data1, float data2, float data3);
void SS_S25FL_save_3x_int16_t(uint8_t variable_id, int16_t data1, int16_t data2, int16_t data3);
void SS_S25FL_save_float(uint8_t variable_id, float data);
void save_flash_bufor();
