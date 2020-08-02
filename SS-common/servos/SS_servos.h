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

typedef enum {
    COM_SERVO_OPEN = 0x01,
    COM_SERVO_CLOSE,
    COM_SERVO_OPENED_POSITION,
    COM_SERVO_CLOSED_POSITION,
    COM_SERVO_POSITION,
    COM_SERVO_DISABLE,
    COM_SERVOS_RANGE,
} ComServoID;

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

/* Defined by the application */
void SS_platform_servos_init();

void SS_servos_init(Servo *servos_array, uint8_t count);
void SS_servo_init(Servo *servo);
void SS_servos_deinit();
int8_t SS_servo_set_position(Servo *servo, uint16_t position);
void SS_servo_open(Servo *servo);
void SS_servo_close(Servo *servo);
void SS_servo_disable(Servo *servo);
int8_t SS_servo_set_closed_position(Servo *servo, uint16_t position);
int8_t SS_servo_set_opened_position(Servo *servo, uint16_t position);
void SS_servos_sequence(uint8_t id, ComServoID operation, int16_t value, int16_t time);
void SS_servos_SYSTICK(void);

ComStatus SS_servos_com_service(ComFrame *frame);
ComStatus SS_servos_com_sequence_validate(ComFrame *frame);
ComStatus SS_servos_com_request(ComFrame *frame);
void SS_servos_read_json(char *json, jsmntok_t **tok);

#endif /* SS_SERVOS_H_ */
