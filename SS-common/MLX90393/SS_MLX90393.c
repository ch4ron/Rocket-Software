/**
  * SS_MLX90393.c
  *
  *  Created on: Nov 27, 2020
  *      Author: Wojtas5
 **/

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_MLX90393.h"

/* ==================================================================== */
/* ========================= Local datatypes ========================== */
/* ==================================================================== */

/* Lookup table to convert raw values to uT based on [HALLCONF][GAIN_SEL][RES][AXIS] */
static const float mlx90393_sensitivity_lookup[2][8][4][2] =
{
    /* HALLCONF = 0x0 */
    {
        /* GAIN_SEL = 0, 5x gain */
        {{0.787, 1.267}, {1.573, 2.534}, {3.146, 5.068}, {6.292, 10.137}},
        /* GAIN_SEL = 1, 4x gain */
        {{0.629, 1.014}, {1.258, 2.027}, {2.517, 4.055}, {5.034, 8.109}},
        /* GAIN_SEL = 2, 3x gain */
        {{0.472, 0.760}, {0.944, 1.521}, {1.888, 3.041}, {3.775, 6.082}},
        /* GAIN_SEL = 3, 2.5x gain */
        {{0.393, 0.634}, {0.787, 1.267}, {1.573, 2.534}, {3.146, 5.068}},
        /* GAIN_SEL = 4, 2x gain */
        {{0.315, 0.507}, {0.629, 1.014}, {1.258, 2.027}, {2.517, 4.055}},
        /* GAIN_SEL = 5, 1.667x gain */
        {{0.262, 0.422}, {0.524, 0.845}, {1.049, 1.689}, {2.097, 3.379}},
        /* GAIN_SEL = 6, 1.333x gain */
        {{0.210, 0.338}, {0.419, 0.676}, {0.839, 1.352}, {1.678, 2.703}},
        /* GAIN_SEL = 7, 1x gain */
        {{0.157, 0.253}, {0.315, 0.507}, {0.629, 1.014}, {1.258, 2.027}},
    },

    /* HALLCONF = 0xC (default) */
    {
        /* GAIN_SEL = 0, 5x gain */
        {{0.751, 1.210}, {1.502, 2.420}, {3.004, 4.840}, {6.009, 9.680}},
        /* GAIN_SEL = 1, 4x gain */
        {{0.601, 0.968}, {1.202, 1.936}, {2.403, 3.872}, {4.840, 7.744}},
        /* GAIN_SEL = 2, 3x gain */
        {{0.451, 0.726}, {0.901, 1.452}, {1.803, 2.904}, {3.605, 5.808}},
        /* GAIN_SEL = 3, 2.5x gain */
        {{0.376, 0.605}, {0.751, 1.210}, {1.502, 2.420}, {3.004, 4.840}},
        /* GAIN_SEL = 4, 2x gain */
        {{0.300, 0.484}, {0.601, 0.968}, {1.202, 1.936}, {2.403, 3.872}},
        /* GAIN_SEL = 5, 1.667x gain */
        {{0.250, 0.403}, {0.501, 0.807}, {1.001, 1.613}, {2.003, 3.227}},
        /* GAIN_SEL = 6, 1.333x gain */
        {{0.200, 0.323}, {0.401, 0.645}, {0.801, 1.291}, {1.602, 2.581}},
        /* GAIN_SEL = 7, 1x gain */
        {{0.150, 0.242}, {0.300, 0.484}, {0.601, 0.968}, {1.202, 1.936}},
    }
};

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static MLX_StatusType SS_MLX90393_transceive(MLX_HandleType *mlx, uint8_t *writeData, uint8_t writeLen, uint8_t *readData, uint8_t readLen);

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */
/* TODO */
/* Implement function for reading status - DONE
 * Use this function in SS_MLX90393_handleError to properly handle ERROR and CMD_REJECTED statuses - DONE
 * Write e-mail to Melexis and ask when exactly ERROR status bit is set - DONE
 * Implement MLX initialization using Method 2 to investigate error repairing feature of MLX90393
 * Choose proper method for SS_MLX90393_init function, based on investigation
 * Move new functions to proper places in code - DONE

   Less important:
 * Check other TODOs in code
 */

MLX_StatusType SS_MLX90393_init(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    
    retValue = SS_MLX90393_resetDevice(mlx);

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setHallconf(mlx);
    }

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setGain(mlx);
    }

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setResolution(mlx);
    }

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setOversampling(mlx);
    }

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setDigitalFiltering(mlx);
    }

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setBurstDatarate(mlx);
    }

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setTempCompensation(mlx);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setHallconf(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_0, &regData);

    if(MLX_OK == retValue)
    {
        regData |= mlx->settings.hallconf;

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_0, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getHallconf(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_0, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_HALLCONF_OFFSET;

        mlx->settings.hallconf = regData;
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setGain(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_0, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (mlx->settings.gain << MLX_REG_GAIN_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_0, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getGain(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_0, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_GAIN_OFFSET;

        mlx->settings.gain = (regData >> MLX_REG_GAIN_SHIFT);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setResolution(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        switch(mlx->measuredValues)
        {
            case MLX_AXIS_X:
            {
                regData |= (mlx->settings.resolutions.x << MLX_REG_RESOLUTION_X_SHIFT);
                break;
            }

            case MLX_AXIS_Y:
            {
                regData |= (mlx->settings.resolutions.y << MLX_REG_RESOLUTION_Y_SHIFT);
                break;
            }

            case MLX_AXIS_Z:
            {
                regData |= (mlx->settings.resolutions.z << MLX_REG_RESOLUTION_Z_SHIFT);
                break;
            }

            case MLX_AXIS_ALL:
            {
                regData |= (mlx->settings.resolutions.x << MLX_REG_RESOLUTION_X_SHIFT) | 
                           (mlx->settings.resolutions.y << MLX_REG_RESOLUTION_Y_SHIFT) | 
                           (mlx->settings.resolutions.z << MLX_REG_RESOLUTION_Z_SHIFT);
                break;
            }

            default:
            {
                return MLX_PRE_CONDITION;
            }
        }

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getResolution(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        switch(mlx->measuredValues)
        {
            case MLX_AXIS_X:
            {
                mlx->settings.resolutions.x = (regData &= MLX_REG_RESOLUTION_X_OFFSET) >> MLX_REG_RESOLUTION_X_SHIFT;
                break;
            }

            case MLX_AXIS_Y:
            {
                mlx->settings.resolutions.y = (regData &= MLX_REG_RESOLUTION_Y_OFFSET) >> MLX_REG_RESOLUTION_Y_SHIFT;
                break;
            }

            case MLX_AXIS_Z:
            {
                mlx->settings.resolutions.z = (regData &= MLX_REG_RESOLUTION_Z_OFFSET) >> MLX_REG_RESOLUTION_Z_SHIFT;
                break;
            }

            case MLX_AXIS_ALL:
            {
                mlx->settings.resolutions.x = (regData &= MLX_REG_RESOLUTION_X_OFFSET) >> MLX_REG_RESOLUTION_X_SHIFT;
                mlx->settings.resolutions.y = (regData &= MLX_REG_RESOLUTION_Y_OFFSET) >> MLX_REG_RESOLUTION_Y_SHIFT;
                mlx->settings.resolutions.z = (regData &= MLX_REG_RESOLUTION_Z_OFFSET) >> MLX_REG_RESOLUTION_Z_SHIFT;
                break;
            }

            default:
            {
                return MLX_PRE_CONDITION;
            }
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setOversampling(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData |= mlx->settings.oversampling;

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getOversampling(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_OVERSAMPLING_OFFSET;

        mlx->settings.oversampling = regData;
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setDigitalFiltering(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (mlx->settings.digitalFiltering << MLX_REG_DIGITAL_FILTER_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getDigitalFiltering(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_OVERSAMPLING_OFFSET;

        mlx->settings.digitalFiltering = (regData >> MLX_REG_DIGITAL_FILTER_SHIFT);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setBurstDatarate(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    if(MLX_DATARATE_MS_MAX < mlx->settings.burstDatarateMs)
    {
        return MLX_PRE_CONDITION;
    }

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData |= mlx->settings.burstDatarateMs / MLX_MS_TO_DATARATE_RATIO;

        if(MLX_MS_TO_DATARATE_RATIO/2 <= mlx->settings.burstDatarateMs % MLX_MS_TO_DATARATE_RATIO)
        {
            ++regData;
        }

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_1, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getBurstDatarate(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_BURST_DATARATE_OFFSET;

        mlx->settings.burstDatarateMs = regData * MLX_MS_TO_DATARATE_RATIO;
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setTempCompensation(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (mlx->settings.tempCompensation << MLX_REG_TCMP_EN_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(mlx, MLX_REG_ADDRESS_1, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getTempCompensation(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_TCMP_EN_OFFSET;

        mlx->settings.tempCompensation = (regData >> MLX_REG_TCMP_EN_SHIFT);
    }

    return retValue;
}

//TODO Decide whether the readLen parameter is needed, maybe it should be chosen based on measuredValues?
// Maybe refactor this function to be more informative, as it is only used for Axis measurements, not the temperature
MLX_StatusType SS_MLX90393_readAxisMeasurements(MLX_HandleType *mlx, uint8_t readLen)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t hallconf = (mlx->settings.hallconf == MLX_HALLCONF_0x0) ? MLX_LOOKUP_HALLCONF_0x0 : MLX_LOOKUP_HALLCONF_0xC;
    int16_t readData[3];

    if((MLX_AXIS_ALL & mlx->measuredValues) == 0u)
    {
        return MLX_PRE_CONDITION;
    }

    retValue = SS_MLX90393_cmdReadMeasurement(mlx, readData, readLen);

    if(MLX_OK == retValue)
    {
        if((mlx->measuredValues & MLX_AXIS_X) != 0u)
        {
            if(MLX_RESOLUTION_3 == mlx->settings.resolutions.x)
            {
                readData[MLX_INDEX_X_AXIS] -= 0x4000;
            }
            else if(MLX_RESOLUTION_2 == mlx->settings.resolutions.x)
            {
                readData[MLX_INDEX_X_AXIS] -= 0x8000;
            }

            mlx->convertedData.x = (float)readData[MLX_INDEX_X_AXIS] * 
                mlx90393_sensitivity_lookup[hallconf][mlx->settings.gain][mlx->settings.resolutions.x][MLX_LOOKUP_AXIS_XY];
        }
        
        if((mlx->measuredValues & MLX_AXIS_Y) != 0u)
        {
            if(MLX_RESOLUTION_3 == mlx->settings.resolutions.y)
            {
                readData[MLX_INDEX_Y_AXIS] -= 0x4000;
            }
            else if(MLX_RESOLUTION_2 == mlx->settings.resolutions.y)
            {
                readData[MLX_INDEX_Y_AXIS] -= 0x8000;
            }

            mlx->convertedData.y = (float)readData[MLX_INDEX_Y_AXIS] * 
                mlx90393_sensitivity_lookup[hallconf][mlx->settings.gain][mlx->settings.resolutions.y][MLX_LOOKUP_AXIS_XY];
        }

        if((mlx->measuredValues & MLX_AXIS_Z) != 0u)
        {
            if(MLX_RESOLUTION_3 == mlx->settings.resolutions.z)
            {
                readData[MLX_INDEX_Z_AXIS] -= 0x4000;
            }
            else if(MLX_RESOLUTION_2 == mlx->settings.resolutions.z)
            {
                readData[MLX_INDEX_Z_AXIS] -= 0x8000;
            }

            mlx->convertedData.z = (float)readData[MLX_INDEX_Z_AXIS] * 
                mlx90393_sensitivity_lookup[hallconf][mlx->settings.gain][mlx->settings.resolutions.z][MLX_LOOKUP_AXIS_Z];
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdStartBurstMode(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_START_BURST_MODE | mlx->measuredValues;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(mlx, &cmd, sizeof(cmd), &status, sizeof(status));

    if(MLX_OK == retValue)
    {
        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
        else
        {
            if((MLX_STATUS_BURST_MODE & status) == 0u)
            {
                retValue = MLX_CMD_REJECTED;
            }
        }  
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdStartSingleMeasurementMode(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_START_SINGLE_MODE | mlx->measuredValues;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(mlx, &cmd, sizeof(cmd), &status, sizeof(status));

    if(MLX_OK == retValue)
    {
        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
        else
        {
            if((MLX_STATUS_SINGLE_MODE & status) == 0u)
            {
                retValue = MLX_CMD_REJECTED;
            }
        }  
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdReadMeasurement(MLX_HandleType *mlx, int16_t *readData, uint8_t readLen)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_READ_MEASUREMENT | mlx->measuredValues;
    uint8_t readBuffer[9];

    retValue = SS_MLX90393_transceive(mlx, &cmd, sizeof(cmd), readBuffer, readLen);

    if(MLX_OK == retValue)
    {
        uint8_t status = readBuffer[0];

        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
        else
        {
            if(MLX_BYTES_TO_READ(MLX_STATUS_DATALEN & status) == readLen)
            {
                for(int i = 0, j = 1; j < readLen; ++i, j += 2)
                {
                    readData[i] = (int16_t)((uint16_t)(readBuffer[j] << 8) | (uint16_t)(readBuffer[j + 1]));
                }
            }
            else
            {
                retValue = MLX_ERROR;
            }
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdReadRegister(MLX_HandleType *mlx, uint8_t regAddress, uint16_t *regData)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd[2] = {MLX_CMD_READ_REGISTER, regAddress << 2};
    uint8_t readBuffer[3];

    retValue = SS_MLX90393_transceive(mlx, cmd, sizeof(cmd), readBuffer, sizeof(readBuffer));

    if(MLX_OK == retValue)
    {
        uint8_t status = readBuffer[0];

        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
        else
        {
            *regData = (uint16_t)((uint16_t)(readBuffer[1] << 8) | (uint16_t)readBuffer[2]);
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdWriteRegister(MLX_HandleType *mlx, uint8_t regAddress, uint16_t regData)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd[4] = {MLX_CMD_WRITE_REGISTER, (uint8_t)(regData >> 8), (uint8_t)regData, regAddress << 2};
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(mlx, cmd, sizeof(cmd), &status, sizeof(status));

    if(MLX_OK == retValue)
    {
        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_resetDevice(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    retValue = SS_MLX90393_cmdExitMode(mlx);

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_cmdReset(mlx);
    }
    
    return retValue;
}

MLX_StatusType SS_MLX90393_cmdExitMode(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_EXIT_MODE;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(mlx, &cmd, sizeof(cmd), &status, sizeof(status));

    if(MLX_OK == retValue)
    {
        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
        else
        {
            if((MLX_STATUS_ALL_MODES & status) != 0u)
            {
                retValue = MLX_CMD_REJECTED;
            }
        }  
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdReset(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_RESET;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(mlx, &cmd, sizeof(cmd), &status, sizeof(status));

    if(MLX_OK == retValue)
    {
        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
        else
        {
            if((MLX_STATUS_RESET & status) != 0u)
            {
                retValue = MLX_CMD_REJECTED;
            }
        } 
    }

    return retValue;
}

//TODO This function will be used/modified after receiving email from Melexis
MLX_StatusType SS_MLX90393_checkStatus(MLX_HandleType *mlx, uint8_t *status)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_NO_OPERATION;

    retValue = SS_MLX90393_transceive(mlx, &cmd, sizeof(cmd), status, sizeof(status));

    return retValue;
}

//TODO This function will be used/modified after receiving email from Melexis
MLX_StatusType SS_MLX90393_handleError(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    switch(mlx->status)
    {
        case MLX_OK:
        {
            break;
        }

        // In case of pre condition, we want from upper layer to log that incorrect parameter
        // was entered, then we can track the source of the problem during debugging
        case MLX_PRE_CONDITION:
        case MLX_WRITE_ERROR:
        case MLX_READ_ERROR:
        {
            retValue = mlx->status;
            break;
        }

        case MLX_ERROR:
        case MLX_CMD_REJECTED:
        {
            uint8_t status = 0u;

            (void)SS_MLX90393_checkStatus(mlx, &status);

            if ((status & MLX_STATUS_ERROR_CORRECTED) == MLX_STATUS_ERROR_CORRECTED)
            {
                retValue = MLX_OK;
            }
            else
            {
                retValue = status;
            }
            
            break;
        }

        default:
        {
            retValue = MLX_PRE_CONDITION;
            break;
        }
    }

    return retValue;
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

//TODO Check if blocking versions of I2C functions are fast enough, if not, replace them with the ones
// that use interrupts
static MLX_StatusType SS_MLX90393_transceive(MLX_HandleType *mlx, uint8_t *writeData, uint8_t writeLen,
                                             uint8_t *readData, uint8_t readLen)
{
    MLX_StatusType retValue = MLX_ERROR;

    retValue = mlx->write(mlx->deviceAddress, writeData, writeLen);
    
    if(0u == retValue)
    {
        retValue = mlx->read(mlx->deviceAddress, readData, readLen);
        
        if(0u == retValue)
        {
            retValue = MLX_OK;
        }
        else
        {
            // failed to receive the data
            retValue = MLX_READ_ERROR;
        }
    }
    else
    {
        // failed to transmit the data
        retValue = MLX_WRITE_ERROR; 
    }

    return retValue;
}