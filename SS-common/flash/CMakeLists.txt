set(TARGET flash)

add_library(${TARGET} INTERFACE)

target_include_directories(
    ${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_s25fl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_s25fl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash.h
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_ctrl.c
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_ctrl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_lfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_lfs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_com.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_com.h
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_caching.c
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_caching.h)
)

target_test_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_s25fl_tests.c
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_ctrl_tests.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_log_tests.c
    #${CMAKE_CURRENT_SOURCE_DIR}/SS_flash_caching_tests.c)
)
