project(callbacks C)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_it.c)

include_directories(
        ${SS_COMMON_DIR}/ADS1258
        ${SS_COMMON_DIR}/dynamixel)
