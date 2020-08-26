cmake_minimum_required(VERSION 3.11)

set(TARGET dynamixel_test)

add_executable(${TARGET}
  ../SS_dynamixel.h
  SS_dynamixel_tests.c)

target_include_directories(
  ${TARGET}
  PRIVATE
  ../
  ../../log
  ${MOCK_DIRECTORY})

target_link_libraries(${TARGET} hal_driver FreeRTOS ThrowTheSwitch)
add_test(NAME DynamixelTest COMMAND ${TARGET})
