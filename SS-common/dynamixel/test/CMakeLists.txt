cmake_minimum_required(VERSION 3.11)

set(TARGET dynamixel_test)

find_file(LOG SS_log.h ../../log)

add_custom_command(OUTPUT
  ${MOCK_DIRECTORY}/MockSS_log.c
  ${MOCK_DIRECTORY}/MockSS_log.h
  COMMAND ${CMOCK_COMMAND}
  ${LOG}
  DEPENDS
  ${LOG})

add_executable(${TARGET}
  ../SS_dynamixel.h
  ../../log/SS_log.h
  ${MOCK_DIRECTORY}/MockSS_log.c
  ${MOCK_DIRECTORY}/MockSS_log.h
  SS_dynamixel_tests.c)

target_include_directories(
  ${TARGET}
  PRIVATE
  ../
  ../../log
  ${MOCK_DIRECTORY})

target_link_libraries(${TARGET} hal_driver FreeRTOS ThrowTheSwitch)
add_test(NAME DynamixelTest COMMAND ${TARGET})
