cmake_minimum_required(VERSION 3.11.1)

project(ADS1258 C)

list(APPEND SRC SS_ADS1258.c SS_measurements.c)

get_directory_property(MYDEFS COMPILE_DEFINITIONS)
if(MYDEFS MATCHES RUN_TESTS)
    list(APPEND SRC SS_ADS1258_unit_tests.c SS_measurements_tests.c)
endif()

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
