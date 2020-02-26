cmake_minimum_required(VERSION 3.11.1)

project(com C)

list(APPEND SRC SS_com.c SS_com_debug.c SS_com_protocol.c)

get_directory_property(MYDEFS COMPILE_DEFINITIONS)
if(MYDEFS MATCHES RUN_TESTS)
    list(APPEND SRC SS_com_tests.c)
endif()

add_library(${PROJECT_NAME} ${SRC})

if(MYDEFS MATCHES RUN_TESTS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SS_COMMON_DIR}/grazyna)
endif()
target_include_directories(${PROJECT_NAME}
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${SS_COMMON_DIR}/supply
                                   ${SS_COMMON_DIR}/servos)
