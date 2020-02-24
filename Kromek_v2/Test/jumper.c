#include "jumper.h"

void jumper_sudo_set_pin_value(uint32_t pin_number) {
    *((volatile uint32_t *)PIN_NUMBER_REG) = pin_number;
}
void jumper_sudo_set_level_value(uint32_t pin_level) {
    *((volatile uint32_t *)PIN_VALUE_REG) = pin_level;
}
void jumper_sudo_apply_pin_level_task() {
    *((volatile uint32_t *)APPLY_PIN_LEVEL_TASK) = 1U;
}

void jumper_sudo_set_pin_level(uint32_t pin_number, uint32_t pin_level) {
    jumper_sudo_set_pin_value(pin_number);
    jumper_sudo_set_level_value(pin_level);
    jumper_sudo_apply_pin_level_task();
}

void jumper_sudo_set_exit_code(uint32_t exit_code) {
    *((volatile uint32_t *)EXIT_CODE_REG) = exit_code;
}
void jumper_sudo_exit_task() {
    *((volatile uint32_t *)EXIT_TASK) = 1U;
}
void jumper_sudo_exit_with_exit_code(uint32_t exit_code) {
    jumper_sudo_set_exit_code(exit_code);
    jumper_sudo_exit_task();
}