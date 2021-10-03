/**
  * SS_magneto.c
  *
  *  Created on: Jan 30, 2021
  *      Author: Wojtas5
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_magneto.h"
#include "i2c.h"

/* ==================================================================== */
/* ============================== Macros ============================== */
/* ==================================================================== */

#define MAGNETOMETERS_QUANTITY 11u

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static MLX_HandleType Magnetometers[MAGNETOMETERS_QUANTITY];

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */
/* TODO: 
 * Implement algorithm for calculating piston position
 * Add error logging
 * Look for other TODOs in code
 */

void SS_magneto_handler_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        /* calculate piston position using algorithm */
        SS_magneto_calculate_piston_position();
    }
}

void SS_magneto_init(void)
{
    /* TODO: Think about a better way of address assignement */
    uint8_t addresses[MAGNETOMETERS_QUANTITY] = {MLX_ADDR_1, MLX_ADDR_2, MLX_ADDR_3, MLX_ADDR_4, MLX_ADDR_5, MLX_ADDR_6, 
                                                 MLX_ADDR_7, MLX_ADDR_8, MLX_ADDR_9, MLX_ADDR_10, MLX_ADDR_11};

    for (int i = 0; i < MAGNETOMETERS_QUANTITY; ++i)
    {
        Magnetometers[i].deviceAddress = addresses[i];
        Magnetometers[i].write = I2C_Transmit;
        Magnetometers[i].read = I2C_Receive;
        Magnetometers[i].measuredValues = MLX_AXIS_ALL;
        Magnetometers[i].settings.resolutions.x = MLX_RESOLUTION_0;
        Magnetometers[i].settings.resolutions.y = MLX_RESOLUTION_0;
        Magnetometers[i].settings.resolutions.z = MLX_RESOLUTION_0;
        Magnetometers[i].settings.hallconf = MLX_HALLCONF_0xC;
        Magnetometers[i].settings.gain = MLX_GAIN_5X;
        Magnetometers[i].settings.oversampling = MLX_OVERSAMPLING_3;
        Magnetometers[i].settings.digitalFiltering = MLX_DIGITAL_FILTER_6;
        Magnetometers[i].settings.tempCompensation = MLX_TCMP_DISABLED;
        Magnetometers[i].mode = MLX_SINGLE_MEASUREMENT_MODE;

        /* initialization on a driver layer */
        SS_MLX90393_init(&Magnetometers[i]);
    }
}

void SS_magneto_calculate_piston_position(void)
{

}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */
