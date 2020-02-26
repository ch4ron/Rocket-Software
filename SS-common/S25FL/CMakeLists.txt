cmake_minimum_required(VERSION 3.11.1)

project(s25fl C)

list(APPEND SRC SS_S25FL.c)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

