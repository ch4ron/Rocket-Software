cmake_minimum_required(VERSION 3.11.1)

project(json-settings C)

list(APPEND SRC SS_json_parser.c jsmn/jsmn.c settings_json.c SS_settings.c)

get_directory_property(MYDEFS COMPILE_DEFINITIONS)
if(MYDEFS MATCHES RUN_TESTS)
    list(APPEND SRC test/SS_json_tests.c test/test_json.c)
endif()

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/jsmn
            ${SS_COMMON_DIR}/servos)

