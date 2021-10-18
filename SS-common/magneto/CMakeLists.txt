set(TARGET magneto)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_magneto.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_magneto.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_magneto_data.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_magneto_cfg.h)
        