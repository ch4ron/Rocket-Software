cmake_minimum_required(VERSION 3.11.1)

project(json-settings C)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
        ${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:
        ${CMAKE_CURRENT_SOURCE_DIR};
        ${CMAKE_CURRENT_SOURCE_DIR}/jsmn>)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/settings_json.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_json_parser.c
        ${CMAKE_CURRENT_SOURCE_DIR}/jsmn/jsmn.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_settings.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/SS_json_tests.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_json.c)
