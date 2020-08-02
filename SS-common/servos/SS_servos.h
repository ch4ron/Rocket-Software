/*
 * SS_servos.h
 *
 *  Created on: 25.02.2018
 *      Author: Tomasz
 */

#ifndef SS_SERVOS_H_
#define SS_SERVOS_H_

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com.h"
#include "SS_json_parser.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

#ifdef SS_USE_SUPPLY
#include "SS_supply.h"
#endif

/* ==================================================================== */
/* ============================= Macros =============================== */
/* ==================================================================== */

#define SERVO_TIMEOUT 1500
#define MAX_SERVO_COUNT 8

/* ==================================================================== */
/* ============================ Datatypes ============================= */
/* ==================================================================== */

typedef struct {
    uint8_t id;
    uint16_t position;
    uint16_t closed_position;
    uint16_t opened_position;
    __IO uint32_t *pointer;
    TIM_HandleTypeDef *tim;
    uint32_t channel;
#ifdef SS_USE_SUPPLY
    Supply *supply;
#endif
    uint32_t timeout;
    jsmntok_t *tok;
} Servo;

typedef struct {
    uint32_t MIN_PULSE_WIDTH;  //us
    uint32_t MAX_PULSE_WIDTH;  //us
    uint32_t SERVO_FREQUENCY;  //Hz
    uint16_t SERVO_RANGE;
    uint8_t servo_count;
} ServosConfig;


/* ==================================================================== */
/* ==================== Public function prototypes ==================== */
/* ==================================================================== */

/* position range 0 - 1000 */
int8_t SS_servo_set_position(Servo *servo, uint16_t position);
int8_t SS_servo_open(Servo *servo);
int8_t SS_servo_close(Servo *servo);
int8_t SS_servo_set_closed_position(Servo *servo, uint16_t position);
int8_t SS_servo_set_opened_position(Servo *servo, uint16_t position);
void SS_servos_init(Servo *servos_array, uint8_t count);
void SS_servos_deinit(void);
int8_t SS_servo_disable(Servo *servo);
void SS_servos_set_range(uint32_t value);
uint32_t SS_servos_get_range(void);
int8_t SS_servo_check_position(Servo *servo, uint16_t position);
Servo *SS_servo_get(uint8_t id);

#endif /* SS_SERVOS_H_ */
