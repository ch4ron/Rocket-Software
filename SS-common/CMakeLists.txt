cmake_minimum_required(VERSION 3.11.1)


find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
)

if(CLANG_TIDY_EXE)
    # set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

add_library(common INTERFACE)

target_sources(common INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_FreeRTOS.c
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/SS_common.h)

set(SS_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})


target_include_directories(
        common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(fifo)
add_subdirectory(dynamixel)
add_subdirectory(com)
add_subdirectory(servos)
add_subdirectory(ADS1258)
add_subdirectory(adc)
add_subdirectory(grazyna)
add_subdirectory(mutex)
add_subdirectory(relays)
add_subdirectory(S25FL)
add_subdirectory(sequence)
add_subdirectory(json-settings)
add_subdirectory(supply)
add_subdirectory(MS5X)
add_subdirectory(can)
add_subdirectory(test)
add_subdirectory(support)
add_subdirectory(error)
add_subdirectory(callbacks)


function(target_link_common TARGET)
get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)
if(COMPILE_DEFS MATCHES SS_USE_FIFO)
    target_link_libraries(${TARGET} fifo)
endif()
if(COMPILE_DEFS MATCHES SS_USE_DYNAMIXEL)
    target_link_libraries(${TARGET} dynamixel)
endif()
if(COMPILE_DEFS MATCHES SS_USE_COM)
    target_link_libraries(${TARGET} com)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SERVOS)
    target_link_libraries(${TARGET} servos)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADS1258)
    target_link_libraries(${TARGET} ADS1258)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADC)
    target_link_libraries(${TARGET} adc)
endif()
if(COMPILE_DEFS MATCHES SS_USE_GRAZYNA)
    target_link_libraries(${TARGET} grazyna)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MUTEX)
    target_link_libraries(${TARGET} mutex)
endif()
if(COMPILE_DEFS MATCHES SS_USE_RELAYS)
    target_link_libraries(${TARGET} relays)
endif()
if(COMPILE_DEFS MATCHES SS_USE_S25FL)
    target_link_libraries(${TARGET} S25FL)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SEQUENCE)
    target_link_libraries(${TARGET} sequence)
endif()
if(COMPILE_DEFS MATCHES SS_USE_JSON_SETTINGS)
    target_link_libraries(${TARGET} json-settings)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SUPPLY)
    target_link_libraries(${TARGET} supply)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MS5X)
    target_link_libraries(${TARGET} MS5X)
endif()
if(COMPILE_DEFS MATCHES SS_USE_CAN)
    target_link_libraries(${TARGET} can)
endif()
if(COMPILE_DEFS MATCHES SS_RUN_TESTS)
    target_link_libraries(${TARGET} SS_test)
endif()

target_link_libraries(${TARGET} common)

# Mandatory
target_link_libraries(${TARGET} support)

target_link_libraries(${TARGET} error)

target_link_libraries(${TARGET} callbacks)
endfunction()
