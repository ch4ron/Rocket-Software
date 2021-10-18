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
#include "SS_magneto_cfg.h"
#include "SS_MLX90393.h"
#include "i2c.h"

/* ==================================================================== */
/* ============================== Macros ============================== */
/* ==================================================================== */

#define INVALID_PISTON_POSITION ((uint16_t)0xFFFF)

#define INDEX_TO_DISTANCE(x)    ((uint16_t)(x * DISTANCE_PER_INDEX))

#define DISTANCE_OFFSET         1u

#define MAG_TO_THE_LEFT_OF(x)   (x - 1)
#define MAG_TO_THE_RIGHT_OF(x)  (x + 1)

/* ==================================================================== */
/* ======================== Private variables ========================= */
/* ==================================================================== */

static MLX_HandleType Magnetometers[MAGNETOMETERS_QUANTITY];

//TODO: Maybe change name to Mag_To_Be_Read ?
static uint8_t Last_Read_Magnetometer_Index;

/* Current position of the piston in [mm] */
static uint16_t Current_Piston_Position;

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
        SS_magneto_calculate_position_one_axis();
    }
}

void SS_magneto_init(void)
{
    /* TODO: Think about a better way of address assignement */
    uint8_t addresses[MAGNETOMETERS_QUANTITY] = {MAG_ADDR_1, MAG_ADDR_2, MAG_ADDR_3, MAG_ADDR_4, MAG_ADDR_5, MAG_ADDR_6,
                                                 MAG_ADDR_7, MAG_ADDR_8, MAG_ADDR_9, MAG_ADDR_10, MAG_ADDR_11};

    /* initialization on a driver layer */
    for (int current_magnetometer_index = 0; current_magnetometer_index < MAGNETOMETERS_QUANTITY; ++current_magnetometer_index)
    {
        Magnetometers[current_magnetometer_index].deviceAddress = addresses[current_magnetometer_index];
        Magnetometers[current_magnetometer_index].write = I2C_Transmit;
        Magnetometers[current_magnetometer_index].read = I2C_Receive;
        Magnetometers[current_magnetometer_index].measuredValues = MLX_AXIS_X;
        Magnetometers[current_magnetometer_index].settings.resolutions.x = MLX_RESOLUTION_0;
        Magnetometers[current_magnetometer_index].settings.resolutions.y = MLX_RESOLUTION_0;
        Magnetometers[current_magnetometer_index].settings.resolutions.z = MLX_RESOLUTION_0;
        Magnetometers[current_magnetometer_index].settings.hallconf = MLX_HALLCONF_0xC;
        Magnetometers[current_magnetometer_index].settings.gain = MLX_GAIN_5X;
        Magnetometers[current_magnetometer_index].settings.oversampling = MLX_OVERSAMPLING_3;
        Magnetometers[current_magnetometer_index].settings.digitalFiltering = MLX_DIGITAL_FILTER_6;
        Magnetometers[current_magnetometer_index].settings.tempCompensation = MLX_TCMP_DISABLED;
        Magnetometers[current_magnetometer_index].mode = MLX_SINGLE_MEASUREMENT_MODE;

        SS_MLX90393_init(&Magnetometers[current_magnetometer_index]);
    }

    Last_Read_Magnetometer_Index = FIRST_MAGNETOMETER_INDEX;

    Current_Piston_Position = INVALID_PISTON_POSITION;
}

void SS_magneto_calculate_piston_position(void)
{
    MLX_StatusType retValue = MLX_ERROR;
    MLX_RawValues rawData = {0};

    /* TODO: We are only receiving values for X axis, check if everything works correctly, when
             measuring just one axis */
    retValue = SS_MLX90393_getRawData(&Magnetometers[Last_Read_Magnetometer_Index], &rawData);

void SS_magneto_calculate_position_one_axis(void)
{
    MLX_StatusType ret_value = MLX_ERROR;
    MLX_RawValues raw_mag_data = {0};

    uint16_t searched_position = INVALID_PISTON_POSITION;
    uint8_t current_magnetometer_index = Last_Read_Magnetometer_Index;
    uint8_t value_index = MAG_VALUE_INVALID_INDEX;

    while(MAGNETOMETERS_QUANTITY != current_magnetometer_index)
    {
        ret_value = SS_MLX90393_getRawData(&Magnetometers[current_magnetometer_index], &raw_mag_data);

        if (MLX_OK == ret_value)
        {
            /* TODO: Each mag. has different idle values, moreover each mag. has different idle values before
                     magnet has passed and after, think about handling all of those value (maybe add an separate array with such values) 
                     Maybe calibration will handle this situation? */
            if (MAGX_IDLE_VALUE != raw_mag_data.x)
            {
                /* TODO: A better option could be to set value_index to 0 when raw_mag_data.x >= 0u, because now in both cases
                        value_index can be incremented at the end of loop (checking raw_mag_data.x is not needed then) */
                value_index = ((raw_mag_data.x >= 0u) ? ((VALUES_QUANTITY/2) - 1) : (VALUES_QUANTITY/2));

                while(MAG_VALUE_FIRST_INDEX != value_index || VALUES_QUANTITY != value_index)
                {
                    if (Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] >= (raw_mag_data.x - DISTANCE_OFFSET) &&
                        Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] <= (raw_mag_data.x + DISTANCE_OFFSET))
                    {
                        //index found
                        Last_Read_Magnetometer_Index = current_magnetometer_index;
                        searched_position = INDEX_TO_DISTANCE(value_index);

                        break;
                    }
                    else
                    {
                        if ((raw_mag_data.x > Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] &&
                             raw_mag_data.x < Magnetometers_Distance_Lookup[current_magnetometer_index][value_index + 1]) ||
                            (raw_mag_data.x < Magnetometers_Distance_Lookup[current_magnetometer_index][value_index] &&
                             raw_mag_data.x > Magnetometers_Distance_Lookup[current_magnetometer_index][value_index + 1]))
                        {
                            //value found between two indexes, needs to be calculated
                            /* TODO: Measured value lays between two Lookup table points, here we need to approximate/interpolate
                                     the real distance*/
                            //searched_position = approximate_distance(INDEX_TO_DISTANCE(value_index), Magnetometers_Distance_Lookup[current_magnetometer_index][value_index],
                            //                                         INDEX_TO_DISTANCE(value_index + 1), Magnetometers_Distance_Lookup[current_magnetometer_index][value_index + 1]);
                            break;
                        }
                    }

                    if (raw_mag_data.x >= 0u)
                    {
                        --value_index;
                    }
                    else
                    {
                        ++value_index;
                    }
                }

                if (INVALID_PISTON_POSITION != searched_position)
                {
                    /* Position measured successfully */

#ifdef USE_SECONG_MAG_CORRELATION
                    /* TODO: After finding the distance we can still check next magnetometer to make sure it is correct, implement
                            this functionality and test this component with and without checking the next magnetometer 
                            Correlaction may be essential as there are no unique values, almost for every value measured there are
                            two distance values matching */
#endif /* USE_SECONG_MAG_CORRELATION */

                    break;
                }
            }

            if (FIRST_MAGNETOMETER_INDEX == Last_Read_Magnetometer_Index)
            {
                /* TODO: When some magnetometer has IDLE value we can move by 3 magnetometers to the right, according to X axis characteristic */
                ++current_magnetometer_index;
            }
            else
            {
                if (current_magnetometer_index == Last_Read_Magnetometer_Index)
                {
                    current_magnetometer_index = MAG_TO_THE_LEFT_OF(Last_Read_Magnetometer_Index);
                }
                else if (current_magnetometer_index == MAG_TO_THE_LEFT_OF(Last_Read_Magnetometer_Index))
                {
                    current_magnetometer_index = MAG_TO_THE_RIGHT_OF(Last_Read_Magnetometer_Index);
                }
                else if (current_magnetometer_index == MAG_TO_THE_RIGHT_OF(Last_Read_Magnetometer_Index))
                {
                    /* We have checked to the right and left from last read mag. - start from the beginning of the array */
                    Last_Read_Magnetometer_Index = FIRST_MAGNETOMETER_INDEX;
                    current_magnetometer_index = FIRST_MAGNETOMETER_INDEX;
                }
                else
                {
                    /* TODO: We shall never reach here - development error */
                }
            }
        }
        else
        {
            /* TODO: At this point error emerged or measurement is not ready yet, think what to do with that */
        }
    }
}

uint16_t SS_magneto_get_current_piston_position(void)
{
    return Current_Piston_Position;
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */
