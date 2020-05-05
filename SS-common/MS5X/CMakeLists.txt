set(TARGET MS5X)

add_library(${TARGET} INTERFACE)
target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_MS5X.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_MS5X.c)
