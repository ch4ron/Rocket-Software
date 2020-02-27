cmake_minimum_required(VERSION 3.11.1)

project(support C)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
        ${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_it.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_Kromek_support.c)

include_directories(
        ${SS_COMMON_DIR}/ADS1258
        ${SS_COMMON_DIR}/dynamixel)
