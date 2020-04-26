cmake_minimum_required(VERSION 3.11)

set(TARGET grazyna)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})
get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

target_test_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna_tests_hal.c)

target_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna_hal.c)

target_non_test_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna.c
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna_hal.c)
