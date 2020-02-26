cmake_minimum_required(VERSION 3.11.1)

project(dynamixel C)

list(APPEND SRCC SS_dynamixel.c)

get_directory_property(MYDEFS COMPILE_DEFINITIONS)
if(MYDEFS MATCHES RUN_TESTS)
    list(APPEND SRCC SS_dynamixel_tests.c)
endif()

add_library(${PROJECT_NAME} ${SRCC})
target_include_directories(${PROJECT_NAME}
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                            PRIVATE ${SS_COMMON_DIR}/fifo
                                    ${SS_COMMON_DIR}/supply)

add_subdirectory(${SS_COMMON_DIR}/mutex mutex)
target_link_libraries(${PROJECT_NAME} mutex)
