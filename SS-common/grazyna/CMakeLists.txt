cmake_minimum_required(VERSION 3.11.1)

project(grazyna C)

list(APPEND SRC SS_grazyna_com.c)

#get_directory_property(MYDEFS COMPILE_DEFINITIONS)
#if(MYDEFS MATCHES RUN_TESTS)
#    list(APPEND SRC SS_grazyna_com.c)
#endif()

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME}
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                            PRIVATE ${SS_COMMON_DIR}/com
                                    ${SS_COMMON_DIR}/fifo)

