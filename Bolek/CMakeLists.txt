cmake_minimum_required(VERSION 3.11)


set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446VETx_FLASH.ld)
set(MCU STM32F446VE)

project(bolek C ASM)

file(GLOB_RECURSE SOURCES
        "Inc/*.*"
        "Src/*.*"
        "startup/*.*")

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_include_directories(${PROJECT_NAME}.elf PRIVATE Inc)


# DEFINE YOUR LIBRARIES HERE
add_compile_definitions(SS_USE_MPU9250)
add_compile_definitions(SS_USE_SCD30)
add_compile_definitions(SS_USE_MS5X)
add_compile_definitions(SS_USE_FLASH)
#add_compile_definitions(SS_USE_USB)
add_compile_definitions(DEBUG)

create_target(${PROJECT_NAME})
