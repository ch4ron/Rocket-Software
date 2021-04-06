set(TARGET SCD30)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_SCD30.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sensirion_arch_config.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sensirion_common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sensirion_i2c.h

        )

target_non_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_SCD30.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sensirion_hw_i2c_implementation.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sensirion_common.c
        )

#target_test_sources(${TARGET} INTERFACE
# ${CMAKE_CURRENT_SOURCE_DIR}/SS_MPU9250_tests.c)
