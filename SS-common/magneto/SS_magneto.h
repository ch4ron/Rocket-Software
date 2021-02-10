/**
  * SS_magneto.h
  *
  *  Created on: Jan 30, 2021
  *      Author: Wojtas5
 **/

#ifndef SS_MAGNETO_H
#define SS_MAGNETO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_MLX90393.h"

/* ==================================================================== */
/* ============================== Macros ============================== */
/* ==================================================================== */

//TODO Some addresses are duplicate, fix this
#define MLX_ADDR_1      ((uint16_t)(0x11u << 1))
#define MLX_ADDR_2      ((uint16_t)(0x0Du << 1))
#define MLX_ADDR_3      ((uint16_t)(0x0Eu << 1))
#define MLX_ADDR_4      ((uint16_t)(0x0Fu << 1))
#define MLX_ADDR_5_nan  ((uint16_t)(0x10u << 1))
#define MLX_ADDR_5      ((uint16_t)(0x11u << 1))
#define MLX_ADDR_6      ((uint16_t)(0x12u << 1))
#define MLX_ADDR_7      ((uint16_t)(0x13u << 1))
#define MLX_ADDR_8      ((uint16_t)(0x14u << 1))
#define MLX_ADDR_9      ((uint16_t)(0x15u << 1))
#define MLX_ADDR_10     ((uint16_t)(0x16u << 1))
#define MLX_ADDR_11     ((uint16_t)(0x16u << 1))

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_magneto_handler_task(void *pvParameters);
void SS_magneto_init(MLX_HandleType *mlx);
void SS_magneto_calculatePistonPosition();

#ifdef __cplusplus
}
#endif
#endif /* SS_MAGNETO_H */
