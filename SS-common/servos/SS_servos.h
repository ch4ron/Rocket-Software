	/*
 * SS_servos.h
 *
 *  Created on: 25.02.2018
 *      Author: Tomasz
 */

#ifndef SS_SERVOS_H_
#define SS_SERVOS_H_

#include "SS_com_protocol.h"
#include "stm32f4xx_hal.h"
#include "SS_supply.h"
#include "tim.h"
#include "SS_json_parser.h"

#define SERVO_TIMEOUT 1500
#define MAX_SERVO_COUNT 8

typedef struct {
    uint8_t id;
	uint16_t position;
	uint16_t closed_position;
	uint16_t opened_position;
	__IO uint32_t *pointer;
	TIM_HandleTypeDef *tim;
	uint32_t channel;
	Supply *supply;
	uint32_t timeout;
	jsmntok_t *tok;
} Servo;

typedef enum {
    COM_SERVO_OPEN,
    COM_SERVO_CLOSE,
    COM_SERVO_OPENED_POSITION,
    COM_SERVO_CLOSED_POSITION,
    COM_SERVO_POSITION,
    COM_SERVO_DISABLE,
    COM_SERVOS_RANGE,
} ComServoID;

typedef struct {
    uint32_t MIN_PULSE_WIDTH; //us
    uint32_t MAX_PULSE_WIDTH; //us
    uint32_t SERVO_FREQUENCY; //Hz
    uint16_t SERVO_RANGE;
    uint8_t servo_count;
} ServosConfig;


extern void SS_platform_servos_init();
void SS_servos_init(Servo *servos_array, uint8_t count);
void SS_servo_init(Servo *servo);
void SS_servo_set_position(Servo *servo, uint16_t value);
void SS_servo_open(Servo *servo);
void SS_servo_close(Servo *servo);
void SS_servo_disable(Servo *servo);
void SS_servo_set_closed_position(Servo *servo, uint16_t position);
void SS_servo_set_opened_position(Servo *servo, uint16_t position);
void SS_servos_SYSTICK(void);
ComStatus SS_servos_handle_grazyna_service(ComFrameContent *frame);
ComStatus SS_servos_handle_grazyna_request(ComFrameContent *frame);
void SS_servos_read_json(char *json, jsmntok_t **tok);

#endif /* SS_SERVOS_H_ */
