cmake_minimum_required(VERSION 3.11)

include(../rocket_software.cmake)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F446RETx_FLASH.ld)
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

project(Czapla C CXX ASM)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

# DEFINE YOUR LIBRARIES HERE
add_compile_definitions(SS_USE_COM)
add_compile_definitions(SS_USE_CAN)
add_compile_definitions(SS_USE_GRAZYNA)
add_compile_definitions(SS_USE_FIFO)

create_target()
