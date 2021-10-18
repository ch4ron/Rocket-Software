/**
  * SS_magneto_cfg.h
  *
  *  Created on: Oct 15, 2021
  *      Author: Wojtas5
 **/

#ifndef SS_MAGNETO_CFG_H
#define SS_MAGNETO_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "stdint.h"

/* ==================================================================== */
/* ============================== Macros ============================== */
/* ==================================================================== */
/* TODO: 
- Change the names of macros to be more readable 
- Rearrange the macros
- Some macros related to magneto data can be moved to SS_magneto_data.h
- Maybe add some short comments 
 */

// #define USE_SECONG_MAG_CORRELATION

#define MAGNETOMETERS_QUANTITY    11u
#define VALUES_QUANTITY           20u

#define FIRST_MAGNETOMETER_INDEX  0u

#define MAG_VALUE_FIRST_INDEX     0u
#define MAG_VALUE_INVALID_INDEX   ((uint8_t)0xFF)

/* Idle value of a magnetometer after magnet pass */
#define MAGX_IDLE_VALUE           0u

/* Magnetometer addresses are indexed ascending starting from the bottom of the tank */
#define MAG_ADDR_1      ((uint16_t)(0x0Cu << 1))
#define MAG_ADDR_2      ((uint16_t)(0x0Du << 1))
#define MAG_ADDR_3      ((uint16_t)(0x0Eu << 1))
#define MAG_ADDR_4      ((uint16_t)(0x0Fu << 1))
#define MAG_ADDR_5      ((uint16_t)(0x11u << 1))
#define MAG_ADDR_6      ((uint16_t)(0x12u << 1))
#define MAG_ADDR_7      ((uint16_t)(0x13u << 1))
#define MAG_ADDR_8      ((uint16_t)(0x14u << 1))
#define MAG_ADDR_9      ((uint16_t)(0x15u << 1))
#define MAG_ADDR_10     ((uint16_t)(0x16u << 1))
#define MAG_ADDR_11     ((uint16_t)(0x17u << 1))

/* This value indicates distance per index in magneto LUT in [mm] */
#define DISTANCE_PER_INDEX      6.51

#ifdef __cplusplus
}
#endif
#endif /* SS_MAGNETO_CFG_H */
