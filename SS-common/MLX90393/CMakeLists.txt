set(TARGET MLX90393)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_MLX90393.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_MLX90393.h)
        