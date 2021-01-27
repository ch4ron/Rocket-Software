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
/* Implement function for reading status (probably using SS_MLX90393_cmdReadRegister as it is most neutral) - DONE
 * Use this function in SS_MLX90393_handleError to properly handle ERROR and CMD_REJECTED statuses
 * Implement MLX initialization using Method 2 to investigate error repairing feature of MLX90393
 * Choose proper method for SS_MLX90393_init function, based on investigation
 * Move new functions to proper places in code

   Less important:
 * Reimplement SS_MLX90393_transceive function, to be more independent from i2c instance and
   its functions (or to be independent from whole interface)
 * Check other TODOs in code
 */

// Parameters setup for fastest speed, as accuracy is not needed that much
// DIG = 2, OSR = 0
//TODO Gain and resolution needs to be set according to values that will be measured
MLX_StatusType SS_MLX90393_init(uint16_t deviceAddress, MLX_InitParams *params)
{
    MLX_StatusType retValue = MLX_ERROR;

    retValue = SS_MLX90393_resetDevice(deviceAddress);

    if(MLX_OK == retValue)
    {
        retValue = SS_MLX90393_setGain(deviceAddress, params->gain);

        if(MLX_OK == retValue)
        {
            SS_MLX90393_setResolution(deviceAddress, MLX_AXIS_ALL, &params->resolutions);
        }
    }
    else
    {
        return retValue;
    }
    
    // To be called
    SS_MLX90393_setResolution(deviceAddress, MLX_AXIS_ALL, &params->resolutions);
    SS_MLX90393_setOversampling(deviceAddress, params->oversampling);
    SS_MLX90393_setDigitalFiltering(deviceAddress, params->digitalFiltering);
    SS_MLX90393_setBurstDatarate(deviceAddress, params->Burstdatarate);

    /* Method 1
    // This method is useful when we are sure, that previous command was executed successfully
    retValue = SS_MLX90393_setGain(deviceAddress, params->gain);
    if(handleError(retValue))
    {
        return retValue;
    }

    retValue = SS_MLX90393_setGain(deviceAddress, params->gain);
    if(handleError(retValue))
    {
        return retValue;
    }

    retValue = SS_MLX90393_setGain(deviceAddress, params->gain);
    if(handleError(retValue))
    {
        return retValue;
    }

    retValue = SS_MLX90393_setGain(deviceAddress, params->gain);
    if(handleError(retValue))
    {
        return retValue;
    }
    */

    /* Method 2
    // Here we are making sure, that a specific value is set, after repairing an error
    // If we check that repairing an error means succesfull command execution - Method 1 can be used
    retValue = SS_MLX90393_setGain(deviceAddress, params->gain);
    if(MLX_OK != retValue)
    {
        if(!handleError(retValue))
        {
            uint16_t gain = 0u;
            SS_MLX90393_getGain(deviceAddress, &gain);

            if(params->gain != gain)
            {
                // try again
                retValue = SS_MLX90393_setGain(deviceAddress, params->gain);

                if(MLX_OK != retValue)
                {
                    return retValue;
                }
            }
        }
        else
        {
            return retValue;
        }
    }
    */
    return retValue;
}

uint8_t SS_MLX90393_checkStatus(uint16_t deviceAddress)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_NO_OPERATION;
    uint8_t status = 0u;

    retValue = SS_MLX90393_transceive(deviceAddress, &cmd, sizeof(cmd), &status, sizeof(status));

    return status;
}

MLX_StatusType SS_MLX90393_handleError(uint16_t deviceAddress, MLX_StatusType status)
{
    MLX_StatusType retValue = MLX_ERROR;

    switch(status)
    {
        case MLX_OK:
        // In case of pre condition, we want from upper layer to log that incorrect parameter
        // was entered, after one debugging session problem will be solved
        case MLX_PRE_CONDITION:
            retValue = status;
            break;

        case MLX_ERROR:
        case MLX_CMD_REJECTED:
            if((SS_MLX90393_checkStatus(deviceAddress) & MLX_STATUS_ERROR_CORRECTED) == MLX_STATUS_ERROR_CORRECTED)
            {
                retValue = MLX_OK;
            }

            break;

        default:
            retValue = MLX_PRE_CONDITION;
            break;
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setGain(uint16_t deviceAddress, uint16_t gain)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (gain << MLX_REG_GAIN_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(deviceAddress, MLX_REG_ADDRESS_1, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getGain(uint16_t deviceAddress, uint16_t *gain)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_1, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_GAIN_OFFSET;

        *gain = (regData >> MLX_REG_GAIN_SHIFT);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setResolution(uint16_t deviceAddress, uint8_t axis, MLX_Resolutions *resolutions)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_3, &regData);

    if(MLX_OK == retValue)
    {
        switch(axis)
        {
            case MLX_AXIS_X:
                regData |= (resolutions->res_x << MLX_REG_RESOLUTION_X_SHIFT);
                break;

            case MLX_AXIS_Y:
                regData |= (resolutions->res_y << MLX_REG_RESOLUTION_Y_SHIFT);
                break;

            case MLX_AXIS_Z:
                regData |= (resolutions->res_z << MLX_REG_RESOLUTION_Z_SHIFT);
                break;

            case MLX_AXIS_ALL:
                regData |= (resolutions->res_x << MLX_REG_RESOLUTION_X_SHIFT) | 
                           (resolutions->res_y << MLX_REG_RESOLUTION_Y_SHIFT) | 
                           (resolutions->res_z << MLX_REG_RESOLUTION_Z_SHIFT);
                break;

            default:
                return MLX_PRE_CONDITION;
        }

        retValue = SS_MLX90393_cmdWriteRegister(deviceAddress, MLX_REG_ADDRESS_3, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getResolution(uint16_t deviceAddress, uint8_t axis, MLX_Resolutions *resolutions)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_3, &regData);

    if(MLX_OK == retValue)
    {
        switch(axis)
        {
            case MLX_AXIS_X:
                resolutions->res_x = (regData &= MLX_REG_RESOLUTION_X_OFFSET) >> MLX_REG_RESOLUTION_X_SHIFT;
                break;

            case MLX_AXIS_Y:
                resolutions->res_y = (regData &= MLX_REG_RESOLUTION_Y_OFFSET) >> MLX_REG_RESOLUTION_Y_SHIFT;
                break;

            case MLX_AXIS_Z:
                resolutions->res_z = (regData &= MLX_REG_RESOLUTION_Z_OFFSET) >> MLX_REG_RESOLUTION_Z_SHIFT;
                break;

            case MLX_AXIS_ALL:
                resolutions->res_x = (regData &= MLX_REG_RESOLUTION_X_OFFSET) >> MLX_REG_RESOLUTION_X_SHIFT;
                resolutions->res_y = (regData &= MLX_REG_RESOLUTION_Y_OFFSET) >> MLX_REG_RESOLUTION_Y_SHIFT;
                resolutions->res_z = (regData &= MLX_REG_RESOLUTION_Z_OFFSET) >> MLX_REG_RESOLUTION_Z_SHIFT;
                break;

            default:
                return MLX_PRE_CONDITION;
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setOversampling(uint16_t deviceAddress, uint16_t oversampling)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_3, &regData);

    if(MLX_OK == retValue)
    {
        regData |= oversampling;

        retValue = SS_MLX90393_cmdWriteRegister(deviceAddress, MLX_REG_ADDRESS_3, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getOversampling(uint16_t deviceAddress, uint16_t *oversampling)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_3, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_OVERSAMPLING_OFFSET;

        *oversampling = regData;
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setDigitalFiltering(uint16_t deviceAddress, uint16_t digitalFiltering)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_3, &regData);

    if(MLX_OK == retValue)
    {
        regData |= (digitalFiltering << MLX_REG_DIGITAL_FILTER_SHIFT);

        retValue = SS_MLX90393_cmdWriteRegister(deviceAddress, MLX_REG_ADDRESS_3, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getDigitalFiltering(uint16_t deviceAddress, uint16_t *digitalFiltering)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_3, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_OVERSAMPLING_OFFSET;

        *digitalFiltering = (regData >> MLX_REG_DIGITAL_FILTER_SHIFT);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_setBurstDatarate(uint16_t deviceAddress, uint16_t datarate)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData |= datarate;

        retValue = SS_MLX90393_cmdWriteRegister(deviceAddress, MLX_REG_ADDRESS_2, regData);
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_getBurstDatarate(uint16_t deviceAddress, uint16_t *datarate)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint16_t regData = 0u;

    retValue = SS_MLX90393_cmdReadRegister(deviceAddress, MLX_REG_ADDRESS_2, &regData);

    if(MLX_OK == retValue)
    {
        regData &= MLX_REG_BURST_DATARATE_OFFSET;

        *datarate = regData;
    }

    return retValue;
}

//TODO Maybe change name to readAllAxis and simplify function?
//then measuredValues is no longer needed, MLX_AXIS_ALL will be passed instead
//TODO Find out how to pass configurable HALLCONF to lookup table, maybe use InitParams structure?
MLX_StatusType SS_MLX90393_readAxisMeasurements(uint16_t deviceAddress, uint8_t measuredValues, uint8_t readLen,
                                                MLX_ConvertedValues *convValues, MLX_Resolutions *resolutions, uint8_t gain)
{
    MLX_StatusType retValue = MLX_ERROR;
    int16_t readData[3];

    if((MLX_AXIS_ALL & measuredValues) == 0u)
    {
        return MLX_PRE_CONDITION;
    }

    retValue = SS_MLX90393_cmdReadMeasurement(deviceAddress, measuredValues, readData, readLen);

    if(MLX_OK == retValue)
    {
        if((measuredValues & MLX_AXIS_X) != 0u)
        {
            if(MLX_RESOLUTION_3 == resolutions->res_x)
            {
                readData[MLX_INDEX_X_AXIS] -= 0x4000;
            }
            else if(MLX_RESOLUTION_2 == resolutions->res_x)
            {
                readData[MLX_INDEX_X_AXIS] -= 0x8000;
            }

            convValues->x = (float)readData[MLX_INDEX_X_AXIS] * 
                mlx90393_sensitivity_lookup[MLX_LOOKUP_HALLCONF_0xC][gain][resolutions->res_x][MLX_LOOKUP_AXIS_XY];
        }
        
        if((measuredValues & MLX_AXIS_Y) != 0u)
        {
            if(MLX_RESOLUTION_3 == resolutions->res_y)
            {
                readData[MLX_INDEX_Y_AXIS] -= 0x4000;
            }
            else if(MLX_RESOLUTION_2 == resolutions->res_y)
            {
                readData[MLX_INDEX_Y_AXIS] -= 0x8000;
            }

            convValues->y = (float)readData[MLX_INDEX_Y_AXIS] * 
                mlx90393_sensitivity_lookup[MLX_LOOKUP_HALLCONF_0xC][gain][resolutions->res_y][MLX_LOOKUP_AXIS_XY];
        }

        if((measuredValues & MLX_AXIS_Z) != 0u)
        {
            if(MLX_RESOLUTION_3 == resolutions->res_z)
            {
                readData[MLX_INDEX_Z_AXIS] -= 0x4000;
            }
            else if(MLX_RESOLUTION_2 == resolutions->res_z)
            {
                readData[MLX_INDEX_Z_AXIS] -= 0x8000;
            }

            convValues->z = (float)readData[MLX_INDEX_Z_AXIS] * 
                mlx90393_sensitivity_lookup[MLX_LOOKUP_HALLCONF_0xC][gain][resolutions->res_z][MLX_LOOKUP_AXIS_Z];
        }
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdStartBurstMode(uint16_t deviceAddress, uint8_t measuredValues)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_START_BURST_MODE | measuredValues;
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
            if((MLX_STATUS_BURST_MODE & status) == 0u)
            {
                retValue = MLX_CMD_REJECTED;
            }
        }  
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdStartSingleMeasurementMode(uint16_t deviceAddress, uint8_t measuredValues)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_START_SINGLE_MODE | measuredValues;
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
            if((MLX_STATUS_SINGLE_MODE & status) == 0u)
            {
                retValue = MLX_CMD_REJECTED;
            }
        }  
    }

    return retValue;
}

MLX_StatusType SS_MLX90393_cmdReadMeasurement(uint16_t deviceAddress, uint8_t measuredValues, 
                                              int16_t *readData, uint8_t readLen)
{
    MLX_StatusType retValue = MLX_ERROR;
    uint8_t cmd = MLX_CMD_READ_MEASUREMENT | measuredValues;
    uint8_t readBuffer[9];

    retValue = SS_MLX90393_transceive(deviceAddress, &cmd, sizeof(cmd), readBuffer, readLen);

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
