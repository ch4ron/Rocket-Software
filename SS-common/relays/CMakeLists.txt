cmake_minimum_required(VERSION 3.11.1)

project(relays C)

list(APPEND SRC SS_relays.c)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME}
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                            PRIVATE ${SS_COMMON_DIR}/supply)

