set(TARGET sequence)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_sequence.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_sequence.h)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_sequence_test.c)
