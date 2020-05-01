set(TARGET com)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_com_feed.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_com_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_com.c)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_com_tests.c)
