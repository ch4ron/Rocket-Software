cmake_minimum_required(VERSION 3.11.1)

project(common C)
# TODO - a workaround for including unity without modifying its CMakeLists.txt, find a better way
file(GLOB_RECURSE UNITY_SOURCES
        ../Unity/extras/fixture/src/unity_fixture.c
        ../Unity/extras/memory/src/unity_memory.c)

add_library(${PROJECT_NAME}  ${UNITY_SOURCES})

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ../Unity/src
        ../Unity/extras/fixture/src
        ../Unity/extras/memory/src)

set(SS_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

if(COMPILE_DEFS MATCHES SS_USE_FIFO)
    add_subdirectory(fifo)
    target_link_libraries(${PROJECT_NAME} PUBLIC fifo)
endif()
if(COMPILE_DEFS MATCHES SS_USE_DYNAMIXEL)
    add_subdirectory(dynamixel)
    target_link_libraries(${PROJECT_NAME} PUBLIC dynamixel)
endif()
if(COMPILE_DEFS MATCHES SS_USE_COM)
    add_subdirectory(com)
    target_link_libraries(${PROJECT_NAME} PUBLIC com)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SERVOS)
    add_subdirectory(servos)
    target_link_libraries(${PROJECT_NAME} PUBLIC servos)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADS1258)
    add_subdirectory(ADS1258)
    target_link_libraries(${PROJECT_NAME} PUBLIC ADS1258)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADC)
    add_subdirectory(adc)
    target_link_libraries(${PROJECT_NAME} PUBLIC adc)
endif()
if(COMPILE_DEFS MATCHES SS_USE_GRAZYNA)
    add_subdirectory(grazyna)
    target_link_libraries(${PROJECT_NAME} PUBLIC grazyna)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SUPPLY)
    add_subdirectory(supply)
    target_link_libraries(${PROJECT_NAME} PUBLIC supply)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MUTEX)
    add_subdirectory(mutex)
    target_link_libraries(${PROJECT_NAME} PUBLIC mutex)
endif()
if(COMPILE_DEFS MATCHES SS_USE_RELAYS)
    add_subdirectory(relays)
    target_link_libraries(${PROJECT_NAME} PUBLIC relays)
endif()
if(COMPILE_DEFS MATCHES SS_USE_S25FL)
    add_subdirectory(s25fl)
    target_link_libraries(${PROJECT_NAME} PUBLIC s25fl)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SEQUENCE)
    add_subdirectory(sequence)
    target_link_libraries(${PROJECT_NAME} PUBLIC sequence)
endif()
if(COMPILE_DEFS MATCHES SS_USE_JSON_SETTINGS)
    add_subdirectory(json-settings)
    target_link_libraries(${PROJECT_NAME} PUBLIC json-settings)
endif()

target_include_directories(
        ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE SS_common.c)

# Mandatory
add_subdirectory(support)
target_link_libraries(${PROJECT_NAME} PUBLIC support)

add_subdirectory(callbacks)
target_link_libraries(${PROJECT_NAME} PUBLIC callbacks)

add_subdirectory(../Unity unity)
target_link_libraries(${PROJECT_NAME} PUBLIC unity)

