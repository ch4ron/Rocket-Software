cmake_minimum_required(VERSION 3.11.1)

project(dynamixel C)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
        ${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:
        ${CMAKE_CURRENT_SOURCE_DIR};
        ${SS_COMMON_DIR}/mutex>)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel_tests.c)

if(NOT TARGET fifo)
    add_subdirectory(../fifo fifo)
    target_link_libraries(${PROJECT_NAME} INTERFACE fifo)
endif()

if(NOT TARGET mutex)
    message(FATAL_ERROR "fifo module required")
endif()
