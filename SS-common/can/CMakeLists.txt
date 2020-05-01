set(TARGET can)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_can.h)

target_non_test_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_can.c
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_can.h)

target_test_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_can_tests.c)
