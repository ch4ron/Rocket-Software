#ifndef SS_GRAZYNA_HAL_H
#define SS_GRAZYNA_HAL_H

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */
#include "SS_grazyna.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"

/* ==================================================================== */
/* ==================== Global function prototypes ==================== */
/* ==================================================================== */

/* huart is of type UART_HandleTypedef */
void SS_grazyna_init_hal(void *huart);
void SS_grazyna_receive_hal(uint8_t *data, uint16_t length);
void SS_grazyna_transmit_hal(uint8_t *data, uint16_t length);
uint32_t SS_grazyna_crc_hal(uint32_t *data, uint32_t len);

#endif /* SS_GRAZYNA_HAL_H */
