cmake_minimum_required(VERSION 3.11)

include(../rocket_software.cmake)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F446ZETx_FLASH.ld)
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(Kromek C CXX ASM)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

# DEFINE YOUR LIBRARIES HERE
#add_compile_definitions(SS_USE_DYNAMIXEL)
add_compile_definitions(SS_USE_SERVOS)
add_compile_definitions(SS_USE_COM)
add_compile_definitions(SS_USE_ADS1258)
add_compile_definitions(SS_USE_GRAZYNA)
add_compile_definitions(SS_USE_SUPPLY)
add_compile_definitions(SS_USE_FIFO)
add_compile_definitions(SS_USE_ADC)
add_compile_definitions(SS_USE_RELAYS)
add_compile_definitions(SS_USE_SEQUENCE)
add_compile_definitions(SS_USE_JSON_SETTINGS)
add_compile_definitions(SS_USE_S25FL)
add_compile_definitions(SS_USE_CAN)
# add_compile_definitions(SS_USE_MOCK)

create_target()
