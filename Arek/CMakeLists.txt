cmake_minimum_required(VERSION 3.11)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETX_FLASH.ld)
set(MCU STM32F446RE)

project(arek C ASM)

file(GLOB_RECURSE SOURCES "Inc/*.*" "Src/*.*" "Startup/*.*") # dodac foldery

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_include_directories(${PROJECT_NAME}.elf PRIVATE Inc ) # dodac foldery

#----------------------------------------------------------------------------------
add_compile_definitions(configGENERATE_RUN_TIME_STATS=0) # disable RTOS runtime stats
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}.elf PRIVATE "-O3") # Set optimization level
#----------------------------------------------------------------------------------

## DEFINE YOUR LIBRARIES HERE
#add_compile_definitions(SS_USE_SERVOS)
#add_compile_definitions(SS_USE_COM)
#add_compile_definitions(SS_USE_ADS1258)
#add_compile_definitions(SS_USE_FIFO)
##add_compile_definitions(SS_USE_ADC)
#add_compile_definitions(SS_USE_SEQUENCE)
#add_compile_definitions(SS_USE_CAN)
#add_compile_definitions(SS_USE_JSON_SETTINGS)
#add_compile_definitions(SS_USE_MS5X) ## odkomentowac gdy bedzie i wywalic plik lokalnie

create_target(${PROJECT_NAME})
