cmake_minimum_required(VERSION 3.11.1)

project(S25FL C)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
        ${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_ctrl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_s25fl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test_flash_ctrl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test_s25fl.c)
