Enabling FreeRTOS for a new board
=================================

Set TimeBase Source as a different timer than SysTick

Set "Pendable request for system service" and "System tick timer" preemption priority to 15
Ensure all other interrupt priorities for interrupt service routines that make
calls to FreeRTOS API functions are set to 5 or more (value of
configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY). Generally it will include all
routines except (the following should be left with priority 0):
 - Non maskable interrupt
 - Hard fault interrupt
 - Memory management fault
 - Pre-fetch fault, memory access fault
 - Undefined instruction or illegal state
 - System service call via SWI instruction
 - Debug monitor

Disable the generation of the following HAL interrupt handlers in CubeMX (In tab NVIC/Code generation):

    - System service call via SWI instruction
    - Pendable request for system service
    - System tick timer
