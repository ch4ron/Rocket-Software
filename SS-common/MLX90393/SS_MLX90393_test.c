#include "SS_MLX90393.h"

/* Macros */
#define MLX_ADDRESS ((uint16_t)0x00u)

/* Functions */
void Handle_Driver_Error(void)
{

}

void Handle_Wrong_Values(void)
{
    
}

void test_resolution(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint8_t res_x = mlx->settings.resolutions.x;
    uint8_t res_y = mlx->settings.resolutions.y;
    uint8_t res_z = mlx->settings.resolutions.z;

    retValue = SS_MLX90393_getResolution(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (res_x != mlx->settings.resolutions.x ||
        res_y != mlx->settings.resolutions.y ||
        res_z != mlx->settings.resolutions.z)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.resolutions.x = MLX_RESOLUTION_1;
    mlx->settings.resolutions.y = MLX_RESOLUTION_2;
    mlx->settings.resolutions.z = MLX_RESOLUTION_3;
    res_x = mlx->settings.resolutions.x;
    res_y = mlx->settings.resolutions.y;
    res_z = mlx->settings.resolutions.z;

    retValue = SS_MLX90393_setResolution(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getResolution(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (res_x != mlx->settings.resolutions.x ||
        res_y != mlx->settings.resolutions.y ||
        res_z != mlx->settings.resolutions.z)
    {
        Handle_Wrong_Values();
    }
}

void test_hallconf(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t hallconf = mlx->settings.hallconf;

    retValue = SS_MLX90393_getHallconf(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (hallconf != mlx->settings.hallconf)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.hallconf = MLX_HALLCONF_0x0;
    hallconf = mlx->settings.hallconf;

    retValue = SS_MLX90393_setHallconf(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getHallconf(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (hallconf != mlx->settings.hallconf)
    {
        Handle_Wrong_Values();
    }
}

void test_gain(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t gain = mlx->settings.gain;

    retValue = SS_MLX90393_getGain(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (gain != mlx->settings.gain)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.gain = MLX_GAIN_2X;
    gain = mlx->settings.gain;

    retValue = SS_MLX90393_setGain(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getGain(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (gain != mlx->settings.gain)
    {
        Handle_Wrong_Values();
    }
}

void test_oversampling(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t oversampling = mlx->settings.gain;

    retValue = SS_MLX90393_getOversampling(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (oversampling != mlx->settings.oversampling)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.oversampling = MLX_OVERSAMPLING_3;
    oversampling = mlx->settings.oversampling;

    retValue = SS_MLX90393_setOversampling(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getOversampling(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (oversampling != mlx->settings.oversampling)
    {
        Handle_Wrong_Values();
    }
}

void test_digitalFiltering(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t digitalFiltering = mlx->settings.digitalFiltering;

    retValue = SS_MLX90393_getDigitalFiltering(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (digitalFiltering != mlx->settings.digitalFiltering)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.digitalFiltering = MLX_DIGITAL_FILTER_5;
    digitalFiltering = mlx->settings.digitalFiltering;

    retValue = SS_MLX90393_setDigitalFiltering(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getDigitalFiltering(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (digitalFiltering != mlx->settings.digitalFiltering)
    {
        Handle_Wrong_Values();
    }
}

void test_burstDatarateMs(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t burstDatarateMs = mlx->settings.burstDatarateMs;

    retValue = SS_MLX90393_getBurstDatarate(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (burstDatarateMs != mlx->settings.burstDatarateMs)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.burstDatarateMs = 200u;
    burstDatarateMs = mlx->settings.burstDatarateMs;

    retValue = SS_MLX90393_setBurstDatarate(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getBurstDatarate(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (burstDatarateMs != mlx->settings.burstDatarateMs)
    {
        Handle_Wrong_Values();
    }
}

void test_tempCompensation(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t tempCompensation = mlx->settings.tempCompensation;

    retValue = SS_MLX90393_getTempCompensation(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (tempCompensation != mlx->settings.tempCompensation)
    {
        Handle_Wrong_Values();
    }

    mlx->settings.tempCompensation = MLX_TCMP_ENABLED;
    tempCompensation = mlx->settings.tempCompensation;

    retValue = SS_MLX90393_setTempCompensation(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getTempCompensation(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (tempCompensation != mlx->settings.tempCompensation)
    {
        Handle_Wrong_Values();
    }
}

void test_resetDevice(MLX_HandleType *mlx)
{
    MLX_StatusType retValue = MLX_ERROR;

    uint16_t gain = mlx->settings.gain;
    uint16_t hallconf = mlx->settings.hallconf;

    retValue = SS_MLX90393_resetDevice(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getGain(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    retValue = SS_MLX90393_getHallconf(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    if (gain != mlx->settings.gain)
    {
        Handle_Wrong_Values();
    }

    if (hallconf != mlx->settings.hallconf)
    {
        Handle_Wrong_Values();
    }

    // restore values
    retValue = SS_MLX90393_init(mlx);
    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }
}

int main()
{
    MLX_StatusType retValue = MLX_ERROR;
    MLX_HandleType mlx;

    mlx.deviceAddress = MLX_ADDRESS;
    // mlx.write = I2C_Transmit;
    // mlx.read = I2C_Receive;
    mlx.measuredValues = MLX_AXIS_ALL;
    mlx.settings.resolutions.x = MLX_RESOLUTION_0;
    mlx.settings.resolutions.y = MLX_RESOLUTION_0;
    mlx.settings.resolutions.z = MLX_RESOLUTION_0;
    mlx.settings.hallconf = MLX_HALLCONF_0xC;
    mlx.settings.gain = MLX_GAIN_5X;
    mlx.settings.oversampling = MLX_OVERSAMPLING_0;
    mlx.settings.digitalFiltering = MLX_DIGITAL_FILTER_0;
    mlx.settings.burstDatarateMs = 100u;
    mlx.settings.tempCompensation = MLX_TCMP_DISABLED;

    retValue = SS_MLX90393_init(&mlx);

    if (MLX_OK != retValue)
    {
        Handle_Driver_Error();
    }

    test_resolution(&mlx);
    test_hallconf(&mlx);
    test_gain(&mlx);
    test_oversampling(&mlx);
    test_digitalFiltering(&mlx);
    test_burstDatarateMs(&mlx);
    test_tempCompensation(&mlx);
    test_resetDevice(&mlx);

    return 0;
}
