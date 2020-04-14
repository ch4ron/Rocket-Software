cmake_minimum_required(VERSION 3.11)

project(grazyna)

add_library(grazyna INTERFACE)

target_include_directories(
        grazyna INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(grazyna INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna_hal.c)

if(COMPILE_DEFS MATCHES SS_RUN_TESTS)
    target_sources(grazyna INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/SS_grazyna_tests.c)
endif()
