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

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

/* Function pointers for communication interface */
typedef uint8_t (*MLX_Write)(uint16_t deviceAddress, uint8_t *writeData, uint8_t writeLen);
typedef uint8_t (*MLX_Read)(uint16_t deviceAddress, uint8_t *readData, uint8_t readLen);

typedef enum
{
    MLX_OK = 0u,
    MLX_ERROR,
    MLX_PRE_CONDITION,
    MLX_CMD_REJECTED,
    MLX_WRITE_ERROR,
    MLX_READ_ERROR
} MLX_StatusType;

typedef enum
{
    MLX_EXIT_MODE,
    MLX_BURST_MODE,
    MLX_SINGLE_MEASUREMENT_MODE,
    MLX_WAKEUP_ON_CHANGE_MODE
} MLX_ModeType;

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
} 
MLX_Resolutions;

typedef struct
{
    MLX_Resolutions resolutions;
    uint16_t hallconf;
    uint16_t gain;
    uint16_t oversampling;
    uint16_t digitalFiltering;
    uint16_t burstDatarateMs;
    uint16_t tempCompensation;
} 
MLX_Settings;

typedef struct
{
    float x;
    float y;
    float z;
} 
MLX_ConvertedValues;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} 
MLX_RawValues;

typedef struct
{
    uint16_t deviceAddress;
    uint8_t measuredValues;
    MLX_ModeType mode;
    MLX_StatusType status;
    MLX_Settings settings;
    MLX_Write write;
    MLX_Read read;
} 
MLX_HandleType;

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

#define MLX_STATUS_BYTES_TO_READ(x) ((2 * x) + 2)

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
#define MLX_HALLCONF_0x0              ((uint16_t)0x00u)
#define MLX_HALLCONF_0xC              ((uint16_t)0x0Cu)

#define MLX_GAIN_5X                   ((uint16_t)0x00u)
#define MLX_GAIN_4X                   ((uint16_t)0x01u)
#define MLX_GAIN_3X                   ((uint16_t)0x02u)
#define MLX_GAIN_2_5X                 ((uint16_t)0x03u)
#define MLX_GAIN_2X                   ((uint16_t)0x04u)
#define MLX_GAIN_1_67X                ((uint16_t)0x05u)
#define MLX_GAIN_1_33X                ((uint16_t)0x06u)
#define MLX_GAIN_1X                   ((uint16_t)0x07u)

#define MLX_DATARATE_MS_MAX           ((uint16_t)MLX_REG_BURST_DATARATE_OFFSET * MLX_MS_TO_DATARATE_RATIO)
#define MLX_MS_TO_DATARATE_RATIO      ((uint16_t)20u)

#define MLX_TCMP_DISABLED             ((uint16_t)0x00u)
#define MLX_TCMP_ENABLED              ((uint16_t)0x01u)

#define MLX_OVERSAMPLING_0            ((uint16_t)0x00u)
#define MLX_OVERSAMPLING_1            ((uint16_t)0x01u)
#define MLX_OVERSAMPLING_2            ((uint16_t)0x02u)
#define MLX_OVERSAMPLING_3            ((uint16_t)0x03u)

#define MLX_DIGITAL_FILTER_0          ((uint16_t)0x00u) 
#define MLX_DIGITAL_FILTER_1          ((uint16_t)0x01u)
#define MLX_DIGITAL_FILTER_2          ((uint16_t)0x02u)
#define MLX_DIGITAL_FILTER_3          ((uint16_t)0x03u)
#define MLX_DIGITAL_FILTER_4          ((uint16_t)0x04u)
#define MLX_DIGITAL_FILTER_5          ((uint16_t)0x05u)
#define MLX_DIGITAL_FILTER_6          ((uint16_t)0x06u)
#define MLX_DIGITAL_FILTER_7          ((uint16_t)0x07u)

#define MLX_RESOLUTION_0              ((uint8_t)0x00u)
#define MLX_RESOLUTION_1              ((uint8_t)0x01u)
#define MLX_RESOLUTION_2              ((uint8_t)0x02u)
#define MLX_RESOLUTION_3              ((uint8_t)0x03u)

/* Macros used in mlx90393_sensitivity_lookup table */
#define MLX_LOOKUP_HALLCONF_0x0       ((uint8_t)0x00u)
#define MLX_LOOKUP_HALLCONF_0xC       ((uint8_t)0x01u)
#define MLX_LOOKUP_AXIS_XY            ((uint8_t)0x00u)
#define MLX_LOOKUP_AXIS_Z             ((uint8_t)0x01u)

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
MLX_StatusType SS_MLX90393_getRawData(MLX_HandleType *mlx, MLX_RawValues *rawData);
MLX_StatusType SS_MLX90393_getConvertedData(MLX_HandleType *mlx, MLX_ConvertedValues *convertedData);
float SS_MLX90393_getConversionTimeMs(MLX_HandleType *mlx);
uint8_t SS_MLX90393_getConversionTimeMsInt(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_resetDevice(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_setMode(MLX_HandleType *mlx);
MLX_StatusType SS_MLX90393_cmdReadMeasurement(MLX_HandleType *mlx, int16_t *readData, uint8_t readLen);
MLX_StatusType SS_MLX90393_cmdReadRegister(MLX_HandleType *mlx, uint8_t regAddress, uint16_t *regData);
MLX_StatusType SS_MLX90393_cmdWriteRegister(MLX_HandleType *mlx, uint8_t regAddress, uint16_t regData);
MLX_StatusType SS_MLX90393_checkStatus(MLX_HandleType *mlx, uint8_t *status);
MLX_StatusType SS_MLX90393_handleError(MLX_HandleType *mlx);

#ifdef __cplusplus
}
#endif
#endif /* SS_MLX90393_H */
