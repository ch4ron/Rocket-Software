set(TARGET log)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/printf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/printf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_log.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_log.c)
