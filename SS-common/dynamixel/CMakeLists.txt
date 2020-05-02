set(TARGET dynamixel)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel_com.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel_com.h)

target_test_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_dynamixel_tests.c)
