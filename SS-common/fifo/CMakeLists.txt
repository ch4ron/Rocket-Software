cmake_minimum_required(VERSION 3.11.1)

set(TARGET fifo)

add_library(${TARGET} INTERFACE)
target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_fifo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_fifo.h)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_fifo_tests.c)

add_library(${TARGET}_test SS_fifo_tests.c)
