#ifndef i2c_H
#define i2c_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  ******************************************************************************
  * File Name          : i2c.h
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "main.h"

/* ==================================================================== */
/* ==================== Exported object definitions =================== */
/* ==================================================================== */

extern I2C_HandleTypeDef hi2c1;

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void MX_I2C1_Init(void);
uint8_t I2C_Transmit(uint16_t deviceAddress, uint8_t *writeData, uint8_t writeLen);
uint8_t I2C_Receive(uint16_t deviceAddress, uint8_t *readData, uint8_t readLen);

#ifdef __cplusplus
}
#endif

#endif /*i2c_H */
