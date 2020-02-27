cmake_minimum_required(VERSION 3.11.1)

project(ADS1258 C)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
        ${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_ADS1258_unit_tests.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_measurements.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_ADS1258.c)
