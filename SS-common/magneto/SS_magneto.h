/**
  * SS_magneto.h
  *
  *  Created on: Jan 30, 2021
  *      Author: Wojtas5
 **/

#ifndef SS_MAGNETO_H
#define SS_MAGNETO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_magneto_cfg.h"

/* ==================================================================== */
/* ============================== Macros ============================== */
/* ==================================================================== */

/* ==================================================================== */
/* ========================= Extern variables ========================= */
/* ==================================================================== */

extern int16_t const Magnetometers_Distance_Lookup[MAGNETOMETERS_QUANTITY][VALUES_QUANTITY];

/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

void SS_magneto_handler_task(void *pvParameters);
void SS_magneto_init(void);
void SS_magneto_calculate_position_one_axis(void);
uint16_t SS_magneto_get_current_piston_position(void);

#ifdef __cplusplus
}
#endif
#endif /* SS_MAGNETO_H */
