set(TARGET callbacks)

add_library(${TARGET} INTERFACE)

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_it.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_it.c)

target_include_directories(${TARGET} INTERFACE
        ${SS_COMMON_DIR}/callbacks
        ${SS_COMMON_DIR}/ADS1258
        ${SS_COMMON_DIR}/dynamixel)
