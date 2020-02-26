#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)


if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    message(Debug)
    add_definitions(-DSIMULATE -DRUN_TESTS)
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL Test)
    message(Debug)
    add_definitions(-DSIMULATE -DRUN_TESTS)
endif()

macro(include_libraries)
    foreach(arg IN ITEMS ${ARGN})
        include_directories(../SS-common/${arg})
        file(GLOB_RECURSE SOURCES ${SOURCES} "../SS-common/${arg}/*.*")
        include_directories(${arg})
    endforeach()
endmacro()
