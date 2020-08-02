cmake_minimum_required(VERSION 3.11)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446VCTx_FLASH.ld)
set(MCU STM32F446VE)

project(usb-msc-test C ASM)

file(GLOB_RECURSE SOURCES
	Src/*.*
	Startup/*.*
	Test/*.*
    Middlewares/*.*
    USB_DEVICE/*.*)

# XXX.
add_definitions(-DDEBUG)

add_compile_definitions(SS_RUN_TESTS_FROM_CONSOLE)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    Inc
    Test
    USB_DEVICE/App
    USB_DEVICE/Target
    Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc)


# DEFINE YOUR LIBRARIES HERE
#add_compile_definitions(SS_USE_DYNAMIXEL)
#add_compile_definitions(SS_USE_SERVOS)
#add_compile_definitions(SS_USE_COM)
#add_compile_definitions(SS_USE_ADS1258)
#add_compile_definitions(SS_USE_FIFO)
#add_compile_definitions(SS_USE_ADC)
#add_compile_definitions(SS_USE_SEQUENCE)
#add_compile_definitions(SS_USE_CAN)
#add_compile_definitions(SS_USE_JSON_SETTINGS)
#add_compile_definitions(SS_USE_MS5X)
add_compile_definitions(SS_USE_FLASH)
add_compile_definitions(SS_USE_USB)

create_target(${PROJECT_NAME})
