/*
 * SS_servos.c
 *
 *  Created on: 25.02.2018
 *      Author: Tomasz
 */

#include "SS_servos.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"

#define SERVO_RESOLUTION 1000
#define MAX_SERVO_COUNT 8

ServosConfig servos_config = {
        .MIN_PULSE_WIDTH = 1000,
        .MAX_PULSE_WIDTH = 2000,
        .SERVO_FREQUENCY = 300,
        .SERVO_RANGE = 1000
};

Servo servos[MAX_SERVO_COUNT];

static void SS_servo_init(Servo *servo) {
    uint32_t freq;
    if (servo->tim->Instance == TIM8 || servo->tim->Instance == TIM1 || servo->tim->Instance == TIM11 || servo->tim->Instance == TIM10 || servo->tim->Instance == TIM9) {
        freq = HAL_RCC_GetPCLK2Freq() * 2;
    } else {
        freq = HAL_RCC_GetPCLK1Freq() * 2;
    }
    servo->tim->Init.Prescaler = (freq / SERVO_RESOLUTION) / servos_config.SERVO_FREQUENCY;
    servo->tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    servo->tim->Init.Period = SERVO_RESOLUTION - 1;
    servo->tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_DeInit(servo->tim);
    HAL_TIM_Base_Init(servo->tim);
    HAL_TIM_Base_Start(servo->tim);
/* Simulator does not support pwm */
#ifndef SIMULATE
    HAL_TIM_PWM_Start(servo->tim, servo->channel);
	switch (servo->channel) {
		case TIM_CHANNEL_1:
			servo->pointer = &servo->tim->Instance->CCR1;
			break;
		case TIM_CHANNEL_2:
			servo->pointer = &servo->tim->Instance->CCR2;
			break;
		case TIM_CHANNEL_3:
			servo->pointer = &servo->tim->Instance->CCR3;
			break;
		case TIM_CHANNEL_4:
			servo->pointer = &servo->tim->Instance->CCR4;
			break;
		default:
			break;
	}
#endif
    servo->opened_position = servos_config.SERVO_RANGE;
    servo->closed_position = 0;
    SS_servo_close(servo);
}

/* width in us */
void SS_servo_set_pulse_width(Servo *servo, uint16_t width) {
/* Simulator does not support pwm */
#ifndef SIMULATE
    *servo->pointer = width * servos_config.SERVO_FREQUENCY * SERVO_RESOLUTION / 1000000;
#endif
}

uint16_t SS_servo_get_width(uint16_t position) {
    return servos_config.MIN_PULSE_WIDTH + position * (servos_config.MAX_PULSE_WIDTH - servos_config.MIN_PULSE_WIDTH) / servos_config.SERVO_RANGE;
}

/* position range 0 - 1000 */
void SS_servo_set_position(Servo *servo, uint16_t position) {
    SS_enable_supply(servo->supply);
    uint16_t width = SS_servo_get_width(position);
    SS_servo_set_pulse_width(servo, width);
    servo->position = position;
#ifndef SERVOS_NO_TIMEOUT
    servo->timeout = SERVO_TIMEOUT;
    SS_supply_set_timeout(servo->supply, SERVO_TIMEOUT);
#endif
}

void SS_servo_open(Servo *servo) {
    SS_servo_set_position(servo, servo->opened_position);
}

void SS_servo_close(Servo *servo) {
    SS_servo_set_position(servo, servo->closed_position);
}

void SS_servos_reinit() {
    for(uint8_t i = 0; i < servos_config.servo_count; i++) {
        SS_servo_init(&servos[i]);
    }
}

void SS_servos_init(Servo *servos_array, uint8_t count) {
    servos_config.servo_count = count;
    memcpy(servos, servos_array, count * sizeof(Servo));
    SS_servos_reinit();
}

void SS_servo_disable(Servo *servo) {
    *servo->pointer = 0;
}

void SS_servo_set_closed_position(Servo *servo, uint16_t position) {
    servo->closed_position = position;
}

void SS_servo_set_opened_position(Servo *servo, uint16_t position) {
    servo->opened_position = position;
}

void SS_servo_SYSTICK(Servo *servo) {
    if(servo->timeout == 1)
        SS_servo_disable(servo);
    if(servo->timeout > 0)
        servo->timeout--;
}

void SS_servos_SYSTICK() {
#ifndef SERVOS_NO_TIMEOUT
    for(uint8_t i = 0; i < servos_config.servo_count; i++) {
        SS_servo_SYSTICK(&servos[i]);
    }
#endif
}

static bool SS_servos_check_id(uint8_t id) {
    if(id >= servos_config.servo_count) {
#ifndef RUN_TESTS
        printf("Servo id: %d is too high, the board supports only %d servos\r\n", id, servos_config.servo_count);
#endif
        return false;
    }
    return true;
}

ComStatus SS_servos_handle_grazyna_service(ComFrameContent *frame) {
    if(!SS_servos_check_id(frame->id)) {
        return COM_ERROR;
    }
    ComServoID msgID = frame->message_type;
    Servo *servo = &servos[frame->id];
    uint32_t value = frame->payload;
    switch(msgID) {
        case COM_SERVO_OPEN:
            SS_servo_open(servo);
            break;
        case COM_SERVO_CLOSE:
            SS_servo_close(servo);
            break;
        case COM_SERVO_OPENED_POSITION:
            SS_servo_set_opened_position(servo, value);
            break;
        case COM_SERVO_CLOSED_POSITION:
            SS_servo_set_closed_position(servo, value);
            break;
        case COM_SERVO_POSITION:
            SS_servo_set_position(servo, value);
            break;
        case COM_SERVO_DISABLE:
            SS_servo_disable(servo);
            break;
        case COM_SERVOS_RANGE:
            servos_config.SERVO_RANGE = value;
            SS_servos_reinit();
            break;
        default:
            printf("Unhandled Grazyna servo service: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

ComStatus SS_servos_handle_grazyna_request(ComFrameContent *frame) {
    if(!SS_servos_check_id(frame->id)) {
        return COM_ERROR;
    }
    ComServoID msgID = frame->message_type;
    Servo *servo = &servos[frame->id];
    switch(msgID) {
        case COM_SERVO_OPENED_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &servo->opened_position);
            break;
        case COM_SERVO_CLOSED_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &servo->closed_position);
            break;
        case COM_SERVO_POSITION:
            SS_com_add_payload_to_frame(frame, UINT16, &servo->position);
            break;
        case COM_SERVOS_RANGE:
            SS_com_add_payload_to_frame(frame, UINT16, &servos_config.SERVO_RANGE);
            break;
        default:
            printf("Unhandled Grazyna servo request: %d\r\n", msgID);
            return COM_ERROR;
    }
    return COM_OK;
}

void SS_servos_read_json(char *json, jsmntok_t **tok) {
    for(uint8_t i = 0; i < servos_config.servo_count; i++) {
        int id, opened_pos, closed_pos;
        JsonData data[] = {
                {
                        .name = "id",
                        .type = JSON_INT,
                        .data = &id
                },
                {
                        .name = "closedPos",
                        .type = JSON_INT,
                        .data = &closed_pos
                },
                {
                        .name = "openedPos",
                        .type = JSON_INT,
                        .data = &opened_pos
                },
        };
        SS_json_parse_data(data, sizeof(data)/sizeof(data[0]), json, tok[i]);
        servos[id].opened_position = opened_pos;
        servos[id].closed_position = closed_pos;
        servos[id].tok = tok[i];
    }
}
