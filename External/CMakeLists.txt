cmake_minimum_required(VERSION 3.11)

add_library(littlefs INTERFACE)

target_sources(littlefs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/littlefs/lfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/littlefs/lfs_util.c)

target_include_directories(littlefs INTERFACE littlefs)

add_library(ThrowTheSwitch INTERFACE)

target_sources(ThrowTheSwitch INTERFACE
 ${CMAKE_CURRENT_SOURCE_DIR}/Unity/extras/fixture/src/unity_fixture.c
 ${CMAKE_CURRENT_SOURCE_DIR}/Unity/src/unity.c
 ${CMAKE_CURRENT_SOURCE_DIR}/Unity/extras/memory/src/unity_memory.c)

add_subdirectory(Unity)

file(GLOB THROWTHESWITCH_INCLUDE_DIRS
  Unity/src
  Unity/extras/fixture/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  Unity/extras/memory/src)

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

if(COMPILE_DEFS MATCHES SS_USE_MOCK)
    set(THROWTHESWITCH_INCLUDE_DIRS ${THROWTHESWITCH_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/CMock/src)
    target_sources(ThrowTheSwitch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CMock/src/cmock.c)
endif()

target_include_directories(ThrowTheSwitch INTERFACE "${THROWTHESWITCH_INCLUDE_DIRS}")

target_link_libraries(ThrowTheSwitch INTERFACE)
