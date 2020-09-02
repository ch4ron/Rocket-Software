#ifndef SI446X_H
#define SI446X_H

// ---------------->> INCLUDES:
#include "main.h"
#include "config.h"
// ----------------<< INCLUDES

// ---------------->> DEFINES:
#define SI446X_MAX_SPI_TIMEOUT 100
#define SI446X_SPI_RX_MAX_LENGTH 100

#define SI446X_EXT_OSC_FREQ  26000000UL // External oscilator frequency (VCXO/TCXO/CRYSTAL)

#define SI446X_SPI_STRUCT hspi1
// ----------------<< DEFINES


// ---------------->> OTHER:
// ----------------<< OTHER


// ---------------->> VARIABLES:
extern SPI_HandleTypeDef SI446X_SPI_STRUCT;
// ----------------<< VARIABLES


// ---------------->> FUNC:
void SI446X_init(void);
void SI446X_de_init(void);

void SI446X_carrier_wave_on(void);
void SI446X_carrier_wave_off(void);

void SI446X_PIN_shutdown_EXT_PA(void);
void SI446X_PIN_power_up_EXT_PA(void);
// ----------------<< FUNC

#endif
