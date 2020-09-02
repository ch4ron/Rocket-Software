#ifndef AFSK_H
#define AFSK_H

// ---------------->> INCLUDES:
#include "main.h"
#include "config.h"
#include "si446x.h"
// ----------------<< INCLUDES

// ---------------->> DEFINES:
// Nazewnictwo:MODU�_PERYFERIUM_NAZWA
#define AFSK_PWM_TIMER htim2           // TIM1
#define AFSK_SINE_SAMPLE_TIMER htim1   // TIM2
#define AFSK_BAUDRATE_TIMER htim3      // TIM3
// ----------------<< DEFINES


// ---------------->> OTHER:
// ----------------<< OTHER


// ---------------->> VARIABLES:
extern TIM_HandleTypeDef AFSK_PWM_TIMER;
extern TIM_HandleTypeDef AFSK_SINE_SAMPLE_TIMER;
extern TIM_HandleTypeDef AFSK_BAUDRATE_TIMER;
// ----------------<< VARIABLES


// ---------------->> FUNC:
void AFSK_send_frame_dat(const uint8_t *buffer, uint16_t len);
void AFSK_start_transmission(void);
uint8_t AFSK_read_state(void);
void AFSK_TIM_INTERUPT_HANDLER(TIM_HandleTypeDef *htim);
// ----------------<< FUNC

#endif
