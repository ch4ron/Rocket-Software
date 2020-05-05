set(TARGET test_runner)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/unity_config.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_tests.c)
