cmake_minimum_required(VERSION 3.11)

get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)

add_library(FreeRTOS INTERFACE)

if(COMPILE_DEFS MATCHES SS_USE_MOCK)
  file(GLOB_RECURSE SOURCES "mocks/*.*" application.c)
  message(${SOURCES})
  target_include_directories(FreeRTOS INTERFACE mocks)
else()
  file(GLOB SOURCES
  application.c
  Source/croutine.c
  Source/event_groups.c
  Source/list.c
  Source/queue.c
  Source/stream_buffer.c
  Source/timers.c
  Source/tasks.c
  Source/portable/MemMang/heap_4.c
  Source/portable/GCC/ARM_CM4F/port.c)
endif()


target_include_directories(FreeRTOS INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F)

target_sources(FreeRTOS INTERFACE
  "${SOURCES}")

if(COMPILE_DEFS MATCHES SS_FREERTOS_TRACE)
    add_subdirectory(TraceRecorder)
    target_link_libraries(FreeRTOS tracerecorder)
endif()
