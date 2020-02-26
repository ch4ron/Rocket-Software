cmake_minimum_required(VERSION 3.11.1)

project(fifo C)

list(APPEND SRC SS_fifo.c)

get_directory_property(MYDEFS COMPILE_DEFINITIONS)
if(MYDEFS MATCHES RUN_TESTS)
    list(APPEND SRC SS_fifo_tests.c)
endif()

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#include(CTest)
#enable_testing()
#include_directories(dupa_test ${CMAKE_CURRENT_SOURCE_DIR})
#add_executable(dupa_test SS_fifo.c SS_fifo_tests.c test/SS_fifo_test_runner.c)
#set_target_properties(dupa_test PROPERTIES COMPILE_OPTIONS "${EXTLIB_COMPILE_FLAGS}")
#target_link_libraries(dupa_test unity)
#add_test(NAME ${PROJECT_NAME}_test COMMAND dupa_test)
