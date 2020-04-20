cmake_minimum_required(VERSION 3.11)

add_library(tracerecorder
  trcKernelPort.c
  trcSnapshotRecorder.c
  trcStreamingRecorder.c
  streamports/Jlink_RTT/SEGGER_RTT.c
  streamports/Jlink_RTT/trcStreamingPort.c)

target_include_directories(tracerecorder PUBLIC
  config
  include
  streamports/Jlink_RTT/include)
