cmake_minimum_required(VERSION 3.11.1)

project(sequence C)

list(APPEND SRC SS_sequence.c)

get_directory_property(MYDEFS COMPILE_DEFINITIONS)
if(MYDEFS MATCHES RUN_TESTS)
    list(APPEND SRC SS_sequence_test.c)
endif()

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
