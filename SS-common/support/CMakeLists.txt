cmake_minimum_required(VERSION 3.11.1)

project(support C)

set(SRC SS_Kromek_support.c SS_it.c)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
        ${SS_COMMON_DIR}/ADS1258
        ${SS_COMMON_DIR}/com
        ${SS_COMMON_DIR}/dynamixel
        ${SS_COMMON_DIR}/servos
        ${SS_COMMON_DIR}/supply
        ${SS_COMMON_DIR}/mutex
        ${SS_COMMON_DIR}/grazyna
        ${SS_COMMON_DIR}/sequence)
