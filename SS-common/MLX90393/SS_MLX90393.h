/**
  * SS_MLX90393.c
  *
  *  Created on: Nov 27, 2020
  *      Author: Wojtas5
 **/

#ifndef SS_MLX90393_H
#define SS_MLX90393_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "stdint.h"
#include "i2c.h"

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef enum
{
    MLX_OK = 0u,
    MLX_ERROR,
    MLX_PRE_CONDITION,
    MLX_CMD_REJECTED,
    MLX_HAL_ERROR,
    MLX_I2C_BUSY
} MLX_StatusType;

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
} MLX_Resolutions;

typedef struct
{
    MLX_Resolutions resolutions;
    uint16_t hallconf;
    uint16_t gain;
    uint16_t oversampling;
    uint16_t digitalFiltering;
    uint16_t burstDatarateMs;
    uint16_t tempCompensation;
} MLX_Settings;

typedef struct
{
    float x;
    float y;
    float z;
} MLX_ConvertedValues;

typedef struct
{
    uint16_t deviceAddress; //TODO to be updated when SS_MLX90393_transceive function will be reimplemented
    
    //mlx_interface_transmit transmit; //TODO to be updated when SS_MLX90393_transceive function will be reimplemented
    
    //mlx_interface_receive receive; //TODO to be updated when SS_MLX90393_transceive function will be reimplemented

    uint8_t measuredValues;

    MLX_Settings settings;

    MLX_ConvertedValues convertedData;

    MLX_StatusType status;

} MLX_HandleType;

/* ==================================================================== */
/* ============================== Macros ============================== */
/* ==================================================================== */

/* Macros for available commands */
#define MLX_CMD_START_BURST_MODE    ((uint8_t)0x10u)
#define MLX_CMD_START_WAKEUP_MODE   ((uint8_t)0x20u)
#define MLX_CMD_START_SINGLE_MODE   ((uint8_t)0x30u)
#define MLX_CMD_READ_MEASUREMENT    ((uint8_t)0x40u)
#define MLX_CMD_READ_REGISTER       ((uint8_t)0x50u)
#define MLX_CMD_WRITE_REGISTER      ((uint8_t)0x60u)
#define MLX_CMD_EXIT_MODE           ((uint8_t)0x80u)
#define MLX_CMD_MEMORY_RECALL       ((uint8_t)0xD0u)
#define MLX_CMD_MEMORY_STORE        ((uint8_t)0xE0u)
#define MLX_CMD_RESET               ((uint8_t)0xF0u)
#define MLX_CMD_NO_OPERATION        ((uint8_t)0x00u)

#define MLX_AXIS_X                  ((uint8_t)0x02u)
#define MLX_AXIS_Y                  ((uint8_t)0x04u)
#define MLX_AXIS_Z                  ((uint8_t)0x08u)
#define MLX_AXIS_ALL                ((uint8_t)0x0Eu)
#define MLX_TEMPERATURE             ((uint8_t)0x01u)
#define MLX_MEASURE_ALL             ((uint8_t)0x0Fu)

/* Macros for status byte interpretation */
#define MLX_STATUS_DATALEN          ((uint8_t)0x03u)
#define MLX_STATUS_RESET            ((uint8_t)0x04u)
#define MLX_STATUS_ERROR_CORRECTED  ((uint8_t)0x08u)
#define MLX_STATUS_ERROR            ((uint8_t)0x10u)
#define MLX_STATUS_SINGLE_MODE      ((uint8_t)0x20u)
#define MLX_STATUS_WOC_MODE         ((uint8_t)0x40u)
#define MLX_STATUS_BURST_MODE       ((uint8_t)0x80u)
#define MLX_STATUS_ALL_MODES        ((uint8_t)0xE0u)

#define MLX_BYTES_TO_READ(x)        ((2 * x) + 2)

/* MLX90393 register addresses */
#define MLX_REG_ADDRESS_0           ((uint8_t)0x00u)
#define MLX_REG_ADDRESS_1           ((uint8_t)0x01u)
#define MLX_REG_ADDRESS_2           ((uint8_t)0x02u)

/* MLX90393 register offsets */
#define MLX_REG_HALLCONF_OFFSET     ((uint16_t)0x000Fu)
#define MLX_REG_GAIN_OFFSET         ((uint16_t)0x0070u)
#define MLX_REG_GAIN_SHIFT          4u

#define MLX_REG_BURST_DATARATE_OFFSET ((uint16_t)0x003Fu)
#define MLX_REG_BURST_SEL_OFFSET      ((uint16_t)0x03C0u)
#define MLX_REG_TCMP_EN_OFFSET        ((uint16_t)0x0400u)
#define MLX_REG_TCMP_EN_SHIFT         10u
#define MLX_REG_COMM_MODE_OFFSET      ((uint16_t)0x8000u)

#define MLX_REG_OVERSAMPLING_OFFSET   ((uint16_t)0x0003u)
#define MLX_REG_DIGITAL_FILTER_OFFSET ((uint16_t)0x001Cu)
#define MLX_REG_DIGITAL_FILTER_SHIFT  2u
#define MLX_REG_RESOLUTION_X_OFFSET   ((uint16_t)0x0060u)
#define MLX_REG_RESOLUTION_X_SHIFT    5u
#define MLX_REG_RESOLUTION_Y_OFFSET   ((uint16_t)0x0180u)
#define MLX_REG_RESOLUTION_Y_SHIFT    7u
#define MLX_REG_RESOLUTION_Z_OFFSET   ((uint16_t)0x0600u)
#define MLX_REG_RESOLUTION_Z_SHIFT    9u  

/* MLX90393 register values */
#define MLX_RESOLUTION_0              ((uint8_t)0x00u)
#define MLX_RESOLUTION_1              ((uint8_t)0x01u)
#define MLX_RESOLUTION_2              ((uint8_t)0x02u)
#define MLX_RESOLUTION_3              ((uint8_t)0x03u)

/* Other macros */
#define MLX_INDEX_X_AXIS              0u
#define MLX_INDEX_Y_AXIS              1u
#define MLX_INDEX_Z_AXIS              2u

#define I2C_DEFAULT_TIMEOUT           ((uint32_t)300u)

/* Lookup table to convert raw values to uT based on [HALLCONF][GAIN_SEL][RES][AXIS] */
#define MLX_LOOKUP_HALLCONF_0x0       ((uint8_t)0x00u)
#define MLX_LOOKUP_HALLCONF_0xC       ((uint8_t)0x01u)
#define MLX_LOOKUP_AXIS_XY            ((uint8_t)0x00u)
#define MLX_LOOKUP_AXIS_Z             ((uint8_t)0x01u)

const float mlx90393_sensitivity_lookup[2][8][4][2] = 
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
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

MLX_StatusType SS_MLX90393_init(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setHallconf(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getHallconf(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setGain(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getGain(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setResolution(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getResolution(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setOversampling(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getOversampling(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setDigitalFiltering(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getDigitalFiltering(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setBurstDatarate(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getBurstDatarate(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setTempCompensation(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_getTempCompensation(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_readAxisMeasurements(MLX_HandleType *mlx, uint8_t readLen);
MLX_StatusType SS_MLX90393_cmdStartBurstMode(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_cmdStartSingleMeasurementMode(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_cmdReadMeasurement(MLX_HandleType *mlx, int16_t *readData, uint8_t readLen);
MLX_StatusType SS_MLX90393_cmdReadRegister(uint16_t deviceAddress, uint8_t regAddress, uint16_t *regData);
MLX_StatusType SS_MLX90393_cmdWriteRegister(uint16_t deviceAddress, uint8_t regAddress, uint16_t regData);
MLX_StatusType SS_MLX90393_resetDevice(uint16_t deviceAddress);
MLX_StatusType SS_MLX90393_cmdExitMode(uint16_t deviceAddress);
MLX_StatusType SS_MLX90393_cmdReset(uint16_t deviceAddress);
uint8_t SS_MLX90393_checkStatus(uint16_t deviceAddress);
MLX_StatusType SS_MLX90393_handleError(uint16_t deviceAddress, MLX_StatusType status);

#ifdef __cplusplus
}
#endif
#endif /* SS_MLX90393_H */
