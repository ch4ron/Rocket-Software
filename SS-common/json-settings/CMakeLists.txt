set(TARGET json-settings)

add_library(${TARGET} INTERFACE)

target_include_directories(
        ${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/jsmn
        ${CMAKE_CURRENT_SOURCE_DIR}/tests)

target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/settings_json.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_json_parser.c
        ${CMAKE_CURRENT_SOURCE_DIR}/jsmn/jsmn.c
        ${CMAKE_CURRENT_SOURCE_DIR}/SS_settings.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_json.h)

target_test_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_json.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_json.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/SS_json_tests.c)
