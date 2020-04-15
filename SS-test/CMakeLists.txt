cmake_minimum_required(VERSION 3.11)

project(SS-test)


add_compile_definitions(SS_USE_MOCK)


# Enable flags for code coverage
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

# set binary_dir to be passed to python script
set(COVERAGE_DATA ${CMAKE_BINARY_DIR})

macro(generate_coverage_data TARGET SOURCE_DIR)
  # List with tested files data, used by python script
  set(COVERAGE_DATA ${COVERAGE_DATA} ${TARGET}
    ${SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endmacro()

enable_testing()

add_subdirectory(../External ThrowTheSwitch)
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

add_custom_target(coverage
  COMMAND make
  COMMAND ctest
  COMMAND python ${CMAKE_SOURCE_DIR}/coverage.py ${COVERAGE_DATA})

add_custom_target(verbose COMMAND ctest -V)
