set(TARGET SCD30)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_SCD30.h
        )

target_non_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_SCD30.c
        )

#target_test_sources(${TARGET} INTERFACE
# ${CMAKE_CURRENT_SOURCE_DIR}/SS_MPU9250_tests.c)
