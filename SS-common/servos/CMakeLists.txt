set(TARGET servos)

add_library(${TARGET} INTERFACE)

target_include_directories(
    ${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_non_test_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_servos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_servos.c)

target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_servos_com.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_servos_com.c)

target_test_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_servos_test.c)
