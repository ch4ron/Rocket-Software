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

add_library(${TARGET} SS_FreeRTOS.c SS_common.c SS_common.h)

set(SS_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

if(COMPILE_DEFS MATCHES SS_USE_FIFO)
    add_subdirectory(fifo)
    target_link_libraries(${TARGET} PUBLIC fifo)
endif()
if(COMPILE_DEFS MATCHES SS_USE_DYNAMIXEL)
    add_subdirectory(dynamixel)
    target_link_libraries(${TARGET} PUBLIC dynamixel)
endif()
if(COMPILE_DEFS MATCHES SS_USE_COM)
    add_subdirectory(com)
    target_link_libraries(${TARGET} PUBLIC com)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SERVOS)
    add_subdirectory(servos)
    target_link_libraries(${TARGET} PUBLIC servos)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADS1258)
    add_subdirectory(ADS1258)
    target_link_libraries(${TARGET} PUBLIC ADS1258)
endif()
if(COMPILE_DEFS MATCHES SS_USE_ADC)
    add_subdirectory(adc)
    target_link_libraries(${TARGET} PUBLIC adc)
endif()
if(COMPILE_DEFS MATCHES SS_USE_GRAZYNA)
    add_subdirectory(grazyna)
    target_link_libraries(${TARGET} PUBLIC grazyna)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MUTEX)
    add_subdirectory(mutex)
    target_link_libraries(${TARGET} PUBLIC mutex)
endif()
if(COMPILE_DEFS MATCHES SS_USE_RELAYS)
    add_subdirectory(relays)
    target_link_libraries(${TARGET} PUBLIC relays)
endif()
if(COMPILE_DEFS MATCHES SS_USE_S25FL)
    add_subdirectory(S25FL)
    target_link_libraries(${TARGET} PUBLIC S25FL)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SEQUENCE)
    add_subdirectory(sequence)
    target_link_libraries(${TARGET} PUBLIC sequence)
endif()
if(COMPILE_DEFS MATCHES SS_USE_JSON_SETTINGS)
    add_subdirectory(json-settings)
    target_link_libraries(${TARGET} PUBLIC json-settings)
endif()
if(COMPILE_DEFS MATCHES SS_USE_SUPPLY)
    add_subdirectory(supply)
    target_link_libraries(${TARGET} PUBLIC supply)
endif()
if(COMPILE_DEFS MATCHES SS_USE_MS5X)
    add_subdirectory(MS5X)
    target_link_libraries(${TARGET} PUBLIC MS5X)
endif()
if(COMPILE_DEFS MATCHES SS_USE_CAN)
    add_subdirectory(can)
    target_link_libraries(${TARGET} PUBLIC can)
endif()


if(COMPILE_DEFS MATCHES RUN_TESTS)
    add_subdirectory(test)
    target_link_libraries(${TARGET} PUBLIC SS_test)
endif()

target_include_directories(
        ${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Mandatory
add_subdirectory(support)
target_link_libraries(${TARGET} PUBLIC support)

add_subdirectory(error)
target_link_libraries(${TARGET} PUBLIC error)

add_subdirectory(callbacks)
target_link_libraries(${TARGET} PUBLIC callbacks)
