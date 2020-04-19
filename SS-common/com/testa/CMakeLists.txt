cmake_minimum_required(VERSION 3.11)

find_file(ERROR SS_error.h ../../error)
find_file(CAN SS_can.h ../../can)

add_custom_command(OUTPUT
  ${MOCK_DIRECTORY}/MockSS_can.c
  ${MOCK_DIRECTORY}/MockSS_error.c
  COMMAND ${CMOCK_COMMAND}
  ${CAN}
  ${ERROR})

add_executable(com_test
  SS_com_tests.c
  ../SS_com.h
  ${MOCK_DIRECTORY}/MockSS_can.c
  ${MOCK_DIRECTORY}/MockSS_error.c)

target_include_directories(com_test PRIVATE
  ../
  ../../can
  ../../error
  ${MOCK_DIRECTORY})

target_link_libraries(com_test FreeRTOS ThrowTheSwitch)
add_test(NAME ComTest COMMAND com_test)
