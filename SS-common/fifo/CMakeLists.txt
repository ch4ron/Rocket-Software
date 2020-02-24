cmake_minimum_required(VERSION 3.11.1)

project(fifo)

set(SOURCES SS_fifo.c)
set(FOO ${FOO} " " -DSIMULATE PARENT_SCOPE)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})