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
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

static MLX_StatusType SS_MLX90393_transceive(uint16_t deviceAddress, uint8_t *writeData, uint8_t writeLen, uint8_t *readData, uint8_t readLen);

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
 * Reimplement SS_MLX90393_transceive function, to be more independent from i2c instance and
   its functions (or to be independent from whole interface)
 * Check other TODOs in code
 */

MLX_StatusType SS_MLX90393_init(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    
    retValue = SS_MLX90393_resetDevice(mlx->deviceAddress);

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

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_0, &regData);

    if(MLX_OK == retValue)
    {
        regData |= mlx->settings.hallconf;

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_0, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getHallconf(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_0, &regData);

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

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_0, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (mlx->settings.gain << MLX_REG_GAIN_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_0, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getGain(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_0, &regData);

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

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        switch(mlx->measuredValues)
        {
            case MLX_AXIS_X:
                regData |= (mlx->settings.resolutions.x << MLX_REG_RESOLUTION_X_SHIFT);
                break;

            case MLX_AXIS_Y:
                regData |= (mlx->settings.resolutions.y << MLX_REG_RESOLUTION_Y_SHIFT);
                break;

            case MLX_AXIS_Z:
                regData |= (mlx->settings.resolutions.z << MLX_REG_RESOLUTION_Z_SHIFT);
                break;

            case MLX_AXIS_ALL:
                regData |= (mlx->settings.resolutions.x << MLX_REG_RESOLUTION_X_SHIFT) | 
                           (mlx->settings.resolutions.y << MLX_REG_RESOLUTION_Y_SHIFT) | 
                           (mlx->settings.resolutions.z << MLX_REG_RESOLUTION_Z_SHIFT);
                break;

            default:
                return MLX_PRE_CONDITION;
        }

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getResolution(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        switch(mlx->measuredValues)
        {
            case MLX_AXIS_X:
                mlx->settings.resolutions.x = (regData &= MLX_REG_RESOLUTION_X_OFFSET) >> MLX_REG_RESOLUTION_X_SHIFT;
                break;

            case MLX_AXIS_Y:
                mlx->settings.resolutions.y = (regData &= MLX_REG_RESOLUTION_Y_OFFSET) >> MLX_REG_RESOLUTION_Y_SHIFT;
                break;

            case MLX_AXIS_Z:
                mlx->settings.resolutions.z = (regData &= MLX_REG_RESOLUTION_Z_OFFSET) >> MLX_REG_RESOLUTION_Z_SHIFT;
                break;

            case MLX_AXIS_ALL:
                mlx->settings.resolutions.x = (regData &= MLX_REG_RESOLUTION_X_OFFSET) >> MLX_REG_RESOLUTION_X_SHIFT;
                mlx->settings.resolutions.y = (regData &= MLX_REG_RESOLUTION_Y_OFFSET) >> MLX_REG_RESOLUTION_Y_SHIFT;
                mlx->settings.resolutions.z = (regData &= MLX_REG_RESOLUTION_Z_OFFSET) >> MLX_REG_RESOLUTION_Z_SHIFT;
                break;

            default:
                return MLX_PRE_CONDITION;
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setOversampling(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData |= mlx->settings.oversampling;

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getOversampling(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, &regData);

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

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (mlx->settings.digitalFiltering << MLX_REG_DIGITAL_FILTER_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getDigitalFiltering(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_2, &regData);

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

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData |= mlx->settings.burstDatarate;

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_1, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getBurstDatarate(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_BURST_DATARATE_OFFSET;

        mlx->settings.burstDatarate = regData;
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setTempCompensation(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (mlx->settings.tempCompensation << MLX_REG_TCMP_EN_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(mlx->deviceAddress, MLX_REG_ADDRESS_1, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getTempCompensation(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(mlx->deviceAddress, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_TCMP_EN_OFFSET;

        mlx->settings.tempCompensation = (regData >> MLX_REG_TCMP_EN_SHIFT);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_readAxisMeasurements(MLX_HandleType *mlx, uint8_t readLen)
{
    MLX_StatusType retValue = MLX_ERROR;
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
                mlx90393_sensitivity_lookup[mlx->settings.hallconf][mlx->settings.gain][mlx->settings.resolutions.x][MLX_LOOKUP_AXIS_XY];
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
                mlx90393_sensitivity_lookup[mlx->settings.hallconf][mlx->settings.gain][mlx->settings.resolutions.y][MLX_LOOKUP_AXIS_XY];
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
                mlx90393_sensitivity_lookup[mlx->settings.hallconf][mlx->settings.gain][mlx->settings.resolutions.z][MLX_LOOKUP_AXIS_Z];
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdStartBurstMode(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_START_BURST_MODE | mlx->measuredValues;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(mlx->deviceAddress, &cmd, sizeof(cmd), &status, sizeof(status));

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

    retValue = SS_MLX90393_transceive(mlx->deviceAddress, &cmd, sizeof(cmd), &status, sizeof(status));

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

    retValue = SS_MLX90393_transceive(mlx->deviceAddress, &cmd, sizeof(cmd), readBuffer, readLen);

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

MLX_StatusType SS_MLX90393_cmdReadRegister(uint16_t deviceAddress, uint8_t regAddress, uint16_t *regData)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd[2] = {MLX_CMD_READ_REGISTER, regAddress << 2};
    uint8_t readBuffer[3];

    retValue = SS_MLX90393_transceive(deviceAddress, cmd, sizeof(cmd), readBuffer, sizeof(readBuffer));

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

MLX_StatusType SS_MLX90393_cmdWriteRegister(uint16_t deviceAddress, uint8_t regAddress, uint16_t regData)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd[4] = {MLX_CMD_WRITE_REGISTER, (uint8_t)(regData >> 8), (uint8_t)regData, regAddress << 2};
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(deviceAddress, cmd, sizeof(cmd), &status, sizeof(status));

    if(MLX_OK == retValue)
    {
        if((MLX_STATUS_ERROR & status) != 0u)
        {
            retValue = MLX_ERROR;
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_resetDevice(uint16_t deviceAddress)
{
    MLX_StatusType retValue = MLX_ERROR;

    retValue = SS_MLX90393_cmdExitMode(deviceAddress);

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_cmdReset(deviceAddress);
    }
    
    return retValue;
}

MLX_StatusType SS_MLX90393_cmdExitMode(uint16_t deviceAddress)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_EXIT_MODE;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(deviceAddress, &cmd, sizeof(cmd), &status, sizeof(status));

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

MLX_StatusType SS_MLX90393_cmdReset(uint16_t deviceAddress)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_RESET;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(deviceAddress, &cmd, sizeof(cmd), &status, sizeof(status));

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
uint8_t SS_MLX90393_checkStatus(uint16_t deviceAddress)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_NO_OPERATION;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(deviceAddress, &cmd, sizeof(cmd), &status, sizeof(status));

    return status;
}

//TODO This function will be used/modified after receiving email from Melexis
MLX_StatusType SS_MLX90393_handleError(uint16_t deviceAddress, MLX_StatusType status)
{
    MLX_StatusType retValue = MLX_ERROR;

    switch(status)
    {
        case MLX_OK:
        // In case of pre condition, we want from upper layer to log that incorrect parameter
        // was entered, then we can track the source of the problem during debugging
        case MLX_PRE_CONDITION:
        case MLX_HAL_ERROR:
        case MLX_I2C_BUSY:
            retValue = status;
            break;

        case MLX_ERROR:
        case MLX_CMD_REJECTED:
            if((SS_MLX90393_checkStatus(deviceAddress) & MLX_STATUS_ERROR_CORRECTED) == MLX_STATUS_ERROR_CORRECTED)
            {
                retValue = MLX_OK;
            }
            else
            {
                retValue = status;
            }
            
            break;

        default:
            retValue = MLX_PRE_CONDITION;
            break;
    }

    return retValue;
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

//TODO Check if blocking versions of I2C functions are fast enough, if not, replace them with the ones
// that use interrupts
static MLX_StatusType SS_MLX90393_transceive(uint16_t deviceAddress, uint8_t *writeData, uint8_t writeLen,
                                             uint8_t *readData, uint8_t readLen)
{
    uint8_t retValue = MLX_ERROR;

    retValue = HAL_I2C_Master_Transmit(&hi2c1, deviceAddress, writeData, writeLen, I2C_DEFAULT_TIMEOUT);
    
    if(HAL_OK == retValue)
    {
        retValue = HAL_I2C_Master_Receive(&hi2c1, deviceAddress, readData, readLen, I2C_DEFAULT_TIMEOUT);
        
        if(HAL_OK == retValue)
        {
            retValue = MLX_OK;
        }
        else
        {
            // failed to receive the data
            retValue = (retValue == HAL_ERROR ? MLX_HAL_ERROR : MLX_I2C_BUSY);
        }
    }
    else
    {
        // failed to transmit the data
        retValue = (retValue == HAL_ERROR ? MLX_HAL_ERROR : MLX_I2C_BUSY); 
    }

    return (MLX_StatusType)retValue;
}