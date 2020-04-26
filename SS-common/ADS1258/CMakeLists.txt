cmake_minimum_required(VERSION 3.11.1)

set(TARGET ADS1258)

add_library(${TARGET} INTERFACE)
target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_measurements.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_measurements.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_ADS1258.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_ADS1258.h)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_measurements_tests.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_ADS1258_unit_tests.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_ADS1258_unit_tests.c)
