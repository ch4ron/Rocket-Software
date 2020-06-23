cmake_minimum_required(VERSION 3.11)


set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446VETx_FLASH.ld)
set(MCU STM32F446VE)

project(staszek C ASM)

file(GLOB_RECURSE SOURCES
  "Inc/*.*"
  "Src/*.*"
  "startup/*.*")

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_include_directories(${PROJECT_NAME}.elf PRIVATE Inc)


# DEFINE YOUR LIBRARIES HERE
#add_compile_definitions(SS_USE_DYNAMIXEL)
add_compile_definitions(SS_USE_SERVOS)
add_compile_definitions(SS_USE_COM)
add_compile_definitions(SS_USE_ADS1258)
#add_compile_definitions(SS_USE_ADC)
# add_compile_definitions(SS_USE_SEQUENCE)
add_compile_definitions(SS_USE_CAN)
add_compile_definitions(SS_USE_JSON_SETTINGS)
# add_compile_definitions(SS_USE_MS5X)
# add_compile_definitions(SS_USE_S25FL)
add_compile_definitions(SS_COM_DEBUG)
add_compile_definitions(SS_USE_MPU9250)

create_target(${PROJECT_NAME})
