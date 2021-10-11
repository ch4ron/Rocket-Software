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

#define INVALID_PISTON_POSITION ((uint16_t)0xFFFF)

#define INDEX_TO_DISTANCE(x)    ((uint16_t)x * 6.51)

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static MLX_HandleType Magnetometers[MAGNETOMETERS_QUANTITY];

static uint8_t Last_Read_Magnetometer_Index;

/* Current position of the piston in [mm] */
static uint16_t Current_Piston_Position;

/* TODO: This values are for concept only, move or remove later */
#define VALUE_QUANTITY  20u
#define DISTANCE_OFFSET 1u
#define MAG_TO_THE_LEFT_OF(x)   (x - 1)
#define MAG_TO_THE_RIGHT_OF(x)  (x + 1)
#define MAGX_IDLE_VALUE         0u
static const int16_t Magnetometers_Distance_Lookup[MAGNETOMETERS_QUANTITY][VALUE_QUANTITY] = 
{
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1},
    {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 900, -256, -128, -64, -32, -16, -8, -4, -2, -1}
};

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */
/* TODO: 
 * Implement algorithm for calculating piston position
 * Add error handling and logging
 * Think about diving specific functionality into layers
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

    /* initialization on a driver layer */
    for (int i = 0; i < MAGNETOMETERS_QUANTITY; ++i)
    {
        Magnetometers[i].deviceAddress = addresses[i];
        Magnetometers[i].write = I2C_Transmit;
        Magnetometers[i].read = I2C_Receive;
        Magnetometers[i].measuredValues = MLX_AXIS_X;
        Magnetometers[i].settings.resolutions.x = MLX_RESOLUTION_0;
        Magnetometers[i].settings.resolutions.y = MLX_RESOLUTION_0;
        Magnetometers[i].settings.resolutions.z = MLX_RESOLUTION_0;
        Magnetometers[i].settings.hallconf = MLX_HALLCONF_0xC;
        Magnetometers[i].settings.gain = MLX_GAIN_5X;
        Magnetometers[i].settings.oversampling = MLX_OVERSAMPLING_3;
        Magnetometers[i].settings.digitalFiltering = MLX_DIGITAL_FILTER_6;
        Magnetometers[i].settings.tempCompensation = MLX_TCMP_DISABLED;
        Magnetometers[i].mode = MLX_SINGLE_MEASUREMENT_MODE;

        SS_MLX90393_init(&Magnetometers[i]);
    }

    Last_Read_Magnetometer_Index = 0;

    Current_Piston_Position = INVALID_PISTON_POSITION;
}

void SS_magneto_calculate_piston_position(void)
{
    MLX_StatusType retValue = MLX_ERROR;
    MLX_RawValues rawData = {0};

    /* TODO: We are only receiving values for X axis, check if everything works correctly, when
             measuring just one axis */
    retValue = SS_MLX90393_getRawData(&Magnetometers[Last_Read_Magnetometer_Index], &rawData);

    if (MLX_OK == retValue)
    {
        //search for distance
        uint8_t current_magnetometer_index = Last_Read_Magnetometer_Index;
        uint16_t searched_distance = INVALID_PISTON_POSITION;

        while(current_magnetometer_index != MAGNETOMETERS_QUANTITY)
        {
            /* TODO: Each mag. has different idle values, moreover each mag. has different idle values before
                     magnet has passed and after, think about handling all of those value (maybe add an separate array with such values) */
            if (MAGX_IDLE_VALUE != rawData.x) //each magnetometer has different idle value
            {
                uint8_t value_index = ((rawData.x >= 0u) ? 9u : 10u);

                while(value_index != 0 || value_index != VALUE_QUANTITY)
                {
                    if (Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] >= (rawData.x - DISTANCE_OFFSET) &&
                        Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] <= (rawData.x + DISTANCE_OFFSET))
                    {
                        //index found
                        Last_Read_Magnetometer_Index = current_magnetometer_index;
                        searched_distance = INDEX_TO_DISTANCE(value_index);

                        break;
                    }
                    else
                    {
                        if ((rawData.x > Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] &&
                             rawData.x < Magnetometers_Distance_Lookup[current_magnetometer_index][value_index + 1]) ||
                            (rawData.x < Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] &&
                             rawData.x > Magnetometers_Distance_Lookup[current_magnetometer_index][value_index + 1]))
                        {
                            //index found but needs to be calculated
                            /* TODO: Measured value lays between two Lookup table points, here we need to approximate/interpolate
                                     the real distance*/
                            //searched_distance = approximate_distance(INDEX_TO_DISTANCE(value_index), Magnetometers_Distance_Lookup[current_magnetometer_index][value_index],
                            //                                         INDEX_TO_DISTANCE(value_index + 1), Magnetometers_Distance_Lookup[current_magnetometer_index][value_index + 1]);
                            break;
                        }
                    }

                    if (rawData.x >= 0u)
                    {
                        --value_index;
                    }
                    else
                    {
                        ++value_index;
                    }
                }
                /* TODO: After finding the distance we can still check next magnetometer to make sure it is correct, decide
                         whether this approach is beneficial */

                if (INVALID_PISTON_POSITION != searched_distance)
                {
                    Current_Piston_Position = searched_distance;

                    break;
                }
            }

            if (0 == Last_Read_Magnetometer_Index)
            {
                ++current_magnetometer_index;
            }
            else
            {
                if (current_magnetometer_index == Last_Read_Magnetometer_Index)
                {
                    current_magnetometer_index = MAG_TO_THE_LEFT_OF(Last_Read_Magnetometer_Index); //check mag. on the left
                }
                else if (current_magnetometer_index == Last_Read_Magnetometer_Index - 1)
                {
                    current_magnetometer_index = MAG_TO_THE_RIGHT_OF(Last_Read_Magnetometer_Index); //check mag. on the right
                }
                else
                {
                    // We have checked on the right and left from last read mag. - start from the beginning
                    Last_Read_Magnetometer_Index = 0; //TODO: Maybe change name to Mag_To_Be_Read ?
                    current_magnetometer_index = 0;
                }
            }
        }
    }
    else
    {
        /* TODO: At this point error emerged or measurement is not ready yet, think what to do with that */
    }
}

uint16_t SS_magneto_get_current_piston_position(void)
{
    return Current_Piston_Position;
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */
