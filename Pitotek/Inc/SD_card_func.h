/*
 * SD_card_func.h
 *
 *  Created on: 18.04.2019
 *      Author: Andrzej
 */

#ifndef SD_CARD_FUNC_H_
#define SD_CARD_FUNC_H_

#include <stdio.h>
#include <stdint.h>
#include "fatfs.h"

#define SD_CARD_ERROR_LED_ON  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,GPIO_PIN_SET)
#define SD_CARD_ERROR_LED_OFF  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,GPIO_PIN_RESET)

typedef enum SD_file_type_of_op {INIT,MOUNT,READ,WRITE,CLOSE,OPEN,UNMOUNT} SD_file_type_of_op;

void SD_CARD_init (void);
void SD_CARD_send_file_debug (FRESULT file_op_result,SD_file_type_of_op operation);

char SD_current_file_path[50];


#endif /* SD_CARD_FUNC_H_ */
