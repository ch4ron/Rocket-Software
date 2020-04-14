cmake_minimum_required(VERSION 3.11)


find_file(COM SS_com.h ../../com)
find_file(GRAZYNA_HAL SS_grazyna_hal.h ../)

add_custom_command(OUTPUT
  ${MOCK_DIRECTORY}/MockSS_grazyna_hal.c
  ${MOCK_DIRECTORY}/MockSS_grazyna_hal.h
  ${MOCK_DIRECTORY}/MockSS_com.c
  COMMAND ${CMOCK_COMMAND}
  ${COM}
  ${GRAZYNA_HAL})

add_executable(grazyna_test
  SS_grazyna_tests.c
  ${MOCK_DIRECTORY}/MockSS_grazyna_hal.c
  ${MOCK_DIRECTORY}/MockSS_grazyna_hal.h
  ${MOCK_DIRECTORY}/MockSS_com.c
  ../SS_grazyna.c)

target_include_directories(
  grazyna_test
  PRIVATE
  ../
  ../../com
  ${MOCK_DIRECTORY})

target_link_libraries(grazyna_test FreeRTOS ThrowTheSwitch)
add_test(NAME GrazynaTest COMMAND grazyna_test)
