cmake_minimum_required(VERSION 3.11)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

#Uncomment for hardware floating point
set(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
# add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -D__FPU_PRESENT=1)

#Uncomment for software floating point
# set(FPU_FLAGS "-mfloat-abi=soft")

set(COMMON_FLAGS
    "-mcpu=cortex-m4 ${FPU_FLAGS} -mthumb -ffunction-sections -fdata-sections \
    -g -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs")

set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DUSE_HAL_DRIVER -DSTM32F446xx -DCMAKE_EXPORT_COMPILE_COMMANDS=1)
add_compile_options(-Wall)

project(RocketSoftware)

find_program(GDB arm-none-eabi-gdb)
find_program(OPENOCD openocd)
find_program(WINPTY winpty)

if(WINPTY)
    set(GDB ${WINPTY} ${GDB})
endif()

if(${VERBOSE_TEST_OUTPUT})
   add_compile_definitions(VERBOSE_TEST_OUTPUT)
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL Test)
    add_compile_definitions(UNITY_OUTPUT_COLOR)
    add_compile_definitions(SS_RUN_TESTS)
    add_compile_definitions(UNITY_INCLUDE_CONFIG_H)
endif()


set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

macro(create_target)
  get_directory_property(COMPILE_DEFS COMPILE_DEFINITIONS)
  if(COMPILE_DEFS MATCHES SS_RUN_TESTS)
    target_link_libraries(${PROJECT_NAME}.elf ThrowTheSwitch)
  endif()
  target_link_libraries(${PROJECT_NAME}.elf hal_driver FreeRTOS hal_driver)
  target_link_common(${PROJECT_NAME}.elf)
  set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

  set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
  set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

  add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")

  if(OPENOCD)
    add_custom_target(${PROJECT_NAME}_flash
        COMMAND openocd -c 'tcl_port disabled' -c 'gdb_port 3333' -c 'telnet_port 4444' -f
        ${ROOT_DIR}/st_nucleo_f4.cfg -c 'program $<TARGET_FILE:${PROJECT_NAME}.elf>' -c reset -c shutdown)
    add_dependencies(${PROJECT_NAME}_flash ${PROJECT_NAME}.elf)

    add_custom_target(${PROJECT_NAME}_gdbserver
        COMMAND ${OPENOCD} -c 'tcl_port disabled' -s openocd/scripts -c 'gdb_port 3333' -c 'telnet_port 4444' -f
			${ROOT_DIR}/st_nucleo_f4.cfg -c 'program $<TARGET_FILE:${PROJECT_NAME}.elf>' -c init;reset init;)
    add_dependencies(${PROJECT_NAME}_gdbserver ${PROJECT_NAME}.elf)

    add_custom_target(${PROJECT_NAME}_gdb
        COMMAND ${GDB} ${PROJECT_NAME}.elf
        -ex 'target extended-remote localhost:3333'
        -ex 'monitor reset halt'
        -ex 'load')
    add_dependencies(${PROJECT_NAME}_gdb ${PROJECT_NAME}.elf)
  endif()

  # Generate JLink config file and add flash target
  set(JLINK_COMMANDS "si SWD"
    "Device ${MCU}"
    "speed 4000"
    "r"
    "h"
    "loadfile ${HEX_FILE}"
    "r"
    "exit")

  set(JLINK_FILE ${CMAKE_CURRENT_BINARY_DIR}/commands.jlink)
  file(REMOVE ${JLINK_FILE})
  file(TOUCH ${JLINK_FILE})
  foreach(COMMAND ${JLINK_COMMANDS})
    file(APPEND ${JLINK_FILE} ${COMMAND})
    file(APPEND ${JLINK_FILE} "\n")
  endforeach()
  add_custom_target(${PROJECT_NAME}_jflash
    COMMAND jlink -CommandFile ${JLINK_FILE})

  add_dependencies(${PROJECT_NAME}_jflash ${PROJECT_NAME}.elf)
endmacro()

add_subdirectory(SS-common)
add_subdirectory(External)
add_subdirectory(FreeRTOS)
add_subdirectory(Drivers)
add_subdirectory(Kromek)
add_subdirectory(Staszek)
add_subdirectory(Pauek)
add_subdirectory(Test-Projects/USB-MSC)
add_subdirectory(Test-Projects/Nucleo-F446RE)
add_subdirectory(Bolek)

set(TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test)
set(TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SS-test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)

add_custom_target(test
  COMMAND ${CMAKE_COMMAND} -B${TEST_BINARY_DIR} -S${TEST_SOURCE_DIR}
  COMMAND make -C ${TEST_BINARY_DIR}
  COMMAND CTEST_OUTPUT_ON_FAILURE=1 make -C ${TEST_BINARY_DIR} test)
