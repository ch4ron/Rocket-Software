cmake_minimum_required(VERSION 3.11)

set(TARGET sequence_test)

add_executable(sequence_test
  SS_sequence_test.c)

target_include_directories(
  ${TARGET}
  PRIVATE
  ../
  ../../log/)

target_link_libraries(${TARGET} hal_driver FreeRTOS ThrowTheSwitch)
add_test(NAME SequenceTest COMMAND ${TARGET})
