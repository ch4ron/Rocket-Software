
#ifndef JEMU_SUDO_H
#define JEMU_SUDO_H

#include <stdint.h>
#define PIN_NUMBER_REG       0xa0000500
#define PIN_VALUE_REG        0xa0000504
#define APPLY_PIN_LEVEL_TASK 0xa0000000
#define EXIT_CODE_REG        0xa0000508
#define EXIT_TASK            0xa0000004


void jumper_sudo_set_pin_value(uint32_t pin_number);
void jumper_sudo_set_level_value(uint32_t pin_level);
void jumper_sudo_apply_pin_level_task();
void jumper_sudo_set_pin_level(uint32_t pin_number, uint32_t pin_level);
void jumper_sudo_set_exit_code(uint32_t exit_code);
void jumper_sudo_exit_task();
void jumper_sudo_exit_with_exit_code(uint32_t exit_code);
#endif //JEMU_SUDO_H
