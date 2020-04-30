cmake_minimum_required(VERSION 3.11)

set(TARGET dynamixel_test)
# find_file(COM SS_com.h ../../com)
# find_file(GRAZYNA_HAL SS_grazyna_hal.h ../)

# add_custom_command(OUTPUT
#   ${MOCK_DIRECTORY}/MockSS_grazyna_hal.c
#   ${MOCK_DIRECTORY}/MockSS_grazyna_hal.h
#   ${MOCK_DIRECTORY}/MockSS_com.c
#   ${MOCK_DIRECTORY}/MockSS_com.h
#   COMMAND ${CMOCK_COMMAND}
#   ${COM}
#   ${GRAZYNA_HAL}
#   DEPENDS
#   ${COM}
#   ${GRAZYNA_HAL})

add_executable(${TARGET}
  ../SS_dynamixel.h
  ../../error/SS_error.c
  SS_dynamixel_tests.c)

target_include_directories(
  ${TARGET}
  PRIVATE
  ../
  ../../error
  ${MOCK_DIRECTORY})

target_link_libraries(${TARGET} hal_driver FreeRTOS ThrowTheSwitch)
add_test(NAME DynamixelTest COMMAND ${TARGET})
