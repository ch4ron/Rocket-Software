cmake_minimum_required(VERSION 3.11.1)

set(TARGET common)

find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
)

if(CLANG_TIDY_EXE)
    # set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

add_library(${TARGET} INTERFACE)

target_sources(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_FreeRTOS.c
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_common.h)

set(SS_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

if(COMPILE_DEFS MATCHES SS_USE_FIFO)
    add_subdirectory(fifo)
    target_link_libraries(${TARGET} INTERFACE fifo)
endif()
if(COMPILE_DEFS MATCHES SS_USE_DYNAMIXEL)
    add_subdirectory(dynamixel)
    target_link_libraries(${TARGET} INTERFACE dynamixel)
endif()
if(COMPILE_DEFS MATCHES SS_USE_COM)
    add_subdirectory(com)
    target_link_libraries(${TARGET} INTERFACE com)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SERVOS)
    add_subdirectory(servos)
    target_link_libraries(${TARGET} INTERFACE servos)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADS1258)
    add_subdirectory(ADS1258)
    target_link_libraries(${TARGET} INTERFACE ADS1258)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADC)
    add_subdirectory(adc)
    target_link_libraries(${TARGET} INTERFACE adc)
endif()
if(COMPILE_DEFS MATCHES SS_USE_GRAZYNA)
    add_subdirectory(grazyna)
    target_link_libraries(${TARGET} INTERFACE grazyna)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MUTEX)
    add_subdirectory(mutex)
    target_link_libraries(${TARGET} INTERFACE mutex)
endif()
if(COMPILE_DEFS MATCHES SS_USE_RELAYS)
    add_subdirectory(relays)
    target_link_libraries(${TARGET} INTERFACE relays)
endif()
if(COMPILE_DEFS MATCHES SS_USE_S25FL)
    add_subdirectory(S25FL)
    target_link_libraries(${TARGET} INTERFACE S25FL)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SEQUENCE)
    add_subdirectory(sequence)
    target_link_libraries(${TARGET} INTERFACE sequence)
endif()
if(COMPILE_DEFS MATCHES SS_USE_JSON_SETTINGS)
    add_subdirectory(json-settings)
    target_link_libraries(${TARGET} INTERFACE json-settings)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SUPPLY)
    add_subdirectory(supply)
    target_link_libraries(${TARGET} INTERFACE supply)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MS5X)
    add_subdirectory(MS5X)
    target_link_libraries(${TARGET} INTERFACE MS5X)
endif()
if(COMPILE_DEFS MATCHES SS_USE_CAN)
    add_subdirectory(can)
    target_link_libraries(${TARGET} INTERFACE can)
endif()


if(COMPILE_DEFS MATCHES SS_RUN_TESTS)
    add_subdirectory(test)
    target_link_libraries(${TARGET} INTERFACE SS_test)
endif()

target_include_directories(
        ${TARGET} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Mandatory
add_subdirectory(support)
target_link_libraries(${TARGET} INTERFACE support)

add_subdirectory(error)
target_link_libraries(${TARGET} INTERFACE error)

add_subdirectory(callbacks)
target_link_libraries(${TARGET} INTERFACE callbacks)
