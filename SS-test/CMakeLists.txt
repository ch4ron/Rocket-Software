cmake_minimum_required(VERSION 3.11)

project(SS-test C ASM)

add_compile_definitions(SS_USE_MOCK)

# Enable flags for code coverage
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")


add_compile_definitions(STM32F446xx)
add_compile_definitions(USE_HAL_DRIVER)

enable_testing()

add_subdirectory(../External ThrowTheSwitch)
add_subdirectory(../Drivers hal_driver)
add_subdirectory(../FreeRTOS FreeRTOS)

get_filename_component(CMOCK ../External/CMock/lib/cmock.rb ABSOLUTE)
get_filename_component(CMOCK_CONFIG cmock_config.yaml ABSOLUTE)

set(MOCK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
set(CMOCK_COMMAND ruby ${CMOCK} -o${CMOCK_CONFIG} --mock_path=${MOCK_DIRECTORY})

# Add Test targets - looks for test subdirectories in SS-common libraries
get_filename_component(COMMON_DIR ../SS-common ABSOLUTE)
file(GLOB children RELATIVE ${COMMON_DIR} ${COMMON_DIR}/*)
foreach(child ${children})
if(IS_DIRECTORY ${COMMON_DIR}/${child} AND EXISTS ${COMMON_DIR}/${child}/test)
    add_subdirectory(${COMMON_DIR}/${child}/test ${child})
endif()
endforeach()
