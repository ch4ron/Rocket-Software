cmake_minimum_required(VERSION 3.11)

add_library(ThrowTheSwitch
  Unity/extras/fixture/src/unity_fixture.c
  Unity/extras/memory/src/unity_memory.c)

add_subdirectory(Unity unity)

file(GLOB INCLUDE_DIRS
  Unity/src
  Unity/extras/fixture/src
  Unity/extras/memory/src)

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

if(COMPILE_DEFS MATCHES SS_USE_MOCK)
    set(INCLUDE_DIRS ${INCLUDE_DIRS} CMock/src)
    target_sources(ThrowTheSwitch CMock/src/cmock.c)
endif()

include_directories("${INCLUDE_DIRS}")

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)

target_link_libraries(ThrowTheSwitch unity)
