cmake_minimum_required(VERSION 3.11)

set(TARGET sequence_test)

find_file(COM SS_com.h ../../com)

add_custom_command(OUTPUT
  ${MOCK_DIRECTORY}/MockSS_com.c
  ${MOCK_DIRECTORY}/MockSS_com.h
  COMMAND ${CMOCK_COMMAND}
  ${COM}
  DEPENDS
  ${COM})


add_executable(sequence_test
  SS_sequence_test.c
  ${MOCK_DIRECTORY}/MockSS_com.h
  ${MOCK_DIRECTORY}/MockSS_com.c)

target_include_directories(
  ${TARGET}
  PRIVATE
  ../
  ../../com/
  ../../log/
  ${MOCK_DIRECTORY})

target_link_libraries(${TARGET} hal_driver FreeRTOS ThrowTheSwitch)
add_test(NAME SequenceTest COMMAND ${TARGET})
