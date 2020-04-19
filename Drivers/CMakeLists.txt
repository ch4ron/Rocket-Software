cmake_minimum_required(VERSION 3.11.1)

set(TARGET hal_driver)

add_library(${TARGET} INTERFACE)

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

if(COMPILE_DEFS MATCHES SS_USE_MOCK)
  set(TO_MOCK
    stm32f4xx_hal.h
    stm32f4xx_hal_adc.h
    stm32f4xx_hal_can.h
    stm32f4xx_hal_crc.h
    stm32f4xx_hal_dma.h
    stm32f4xx_hal_def.h
    stm32f4xx_hal_exti.h
    stm32f4xx_hal_gpio.h
    stm32f4xx_hal_i2c.h
    stm32f4xx_hal_i2c_ex.h
    stm32f4xx_hal_i2s.h
    stm32f4xx_hal_i2s_ex.h
    stm32f4xx_hal_qspi.h
    stm32f4xx_hal_rcc.h
    stm32f4xx_hal_rcc_ex.h
    stm32f4xx_hal_rtc.h
    stm32f4xx_hal_rtc_ex.h
    stm32f4xx_hal_spi.h
    stm32f4xx_hal_tim.h
    stm32f4xx_hal_tim_ex.h
    stm32f4xx_hal_uart.h
    stm32f4xx_hal_usart.h)

  foreach(FILE ${TO_MOCK})
    list(APPEND MOCKS ${CMAKE_CURRENT_SOURCE_DIR}/STM32F4xx_HAL_Driver/Inc/${FILE})
    list(APPEND MOCK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/mocks/Mock${FILE})
  endforeach()

  add_custom_command(OUTPUT
    ${MOCK_SOURCES}
    COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/mock_drivers.rb ${MOCKS})

  set_source_files_properties(${MOCK_SOURCES} PROPERTIES GENERATED TRUE)
  add_custom_target(generate_mocks ALL DEPENDS
    ${MOCK_SOURCES}
  )

  add_dependencies(${TARGET} generate_mocks)

  target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})
else()
  file(GLOB_RECURSE SOURCES
    "CMSIS/*.*"
    "STM32F4xx_HAL_Driver/*.*")
  target_sources(${TARGET} INTERFACE "${SOURCES}")
endif()

target_include_directories(${TARGET} INTERFACE
    CMSIS/Device/ST/STM32F4xx/Include
    CMSIS/Include
    STM32F4xx_HAL_Driver/Inc
    STM32F4xx_HAL_Driver/Inc/Legacy)

