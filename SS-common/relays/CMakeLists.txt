set(TARGET relays)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_relays.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_relays.c)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_relays_tests.c)
