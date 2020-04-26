cmake_minimum_required(VERSION 3.11.1)

set(TARGET supply)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_supply.c)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_supply_tests.c)

if(NOT TARGET adc)
    message(FATAL_ERROR "adc module required")
endif()
