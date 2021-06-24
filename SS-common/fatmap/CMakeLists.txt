set(TARGET fatmap)

add_library(${TARGET} INTERFACE)

target_include_directories(
    ${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_fatmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_fatmap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_fatmap_defs.h
)

#target_test_sources(${TARGET} INTERFACE
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_s25fl_tests.c
#)
