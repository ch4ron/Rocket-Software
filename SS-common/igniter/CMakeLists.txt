set(TARGET igniter)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_igniter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_igniter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_igniter_com.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_igniter_com.c)
