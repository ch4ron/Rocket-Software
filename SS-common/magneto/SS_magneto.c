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

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

#define MLX_MODULES_QUANTITY 11u

void SS_magneto_handler_task(void *pvParameters)
{
    // initialize an array of 11 MLX_HandleType structs
    MLX_HandleType mlx[MLX_MODULES_QUANTITY];

    // init magneto module
    SS_magneto_init(&mlx);

    while(1)
    {
        // calculate piston position using algorithm
        SS_magneto_calculatePistonPosition();
    }
}

void SS_magneto_init(MLX_HandleType *mlx)
{
    uint8_t addresses = {MLX_ADDR_1, MLX_ADDR_2, MLX_ADDR_3, MLX_ADDR_4, MLX_ADDR_5, MLX_ADDR_6, 
                         MLX_ADDR_7, MLX_ADDR_8, MLX_ADDR_9. MLX_ADDR_10, MLX_ADDR_11};

    for(int i = 0; i < MLX_MODULES_QUANTITY; ++i)
    {
        //TODO These values are not configured properly yet, determine what configuration we need for
        // our use case
        mlx[i].deviceAddress = addresses[i];
        mlx[i].measuredValues = MLX_AXIS_ALL;
        mlx[i].settings.resolutions.x = MLX_RESOLUTION_0;
        mlx[i].settings.resolutions.y = MLX_RESOLUTION_0;
        mlx[i].settings.resolutions.z = MLX_RESOLUTION_0;
        mlx[i].settings.hallconf = MLX_HALLCONF_0xC;
        mlx[i].settings.gain = MLX_GAIN_5X;
        mlx[i].settings.oversampling = MLX_OVERSAMPLING_0;
        mlx[i].settings.digitalFiltering = MLX_DIGITAL_FILTER_0;
        mlx[i].settings.burstDatarateMs = 100u;
        mlx[i].settings.tempCompensation = MLX_TCMP_DISABLED;

        // initialization on a driver layer
        SS_MLX90393_init(&mlx[i]);
    }
}

void SS_magneto_calculatePistonPosition()
{

}
