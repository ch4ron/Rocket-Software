cmake_minimum_required(VERSION 3.11)

add_library(FreeRTOS
  application
  Source/croutine.c
  Source/event_groups.c
  Source/list.c
  Source/queue.c
  Source/stream_buffer.c
  Source/timers.c
  Source/tasks.c
  Source/portable/MemMang/heap_4.c
  Source/portable/GCC/ARM_CM4F/port.c)


file(GLOB INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  Source/include
  Source/portable/GCC/ARM_CM4F)

include_directories(${INCLUDE_DIRS})

set(INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)

add_subdirectory(TraceRecorder)

target_link_libraries(FreeRTOS tracerecorder)
