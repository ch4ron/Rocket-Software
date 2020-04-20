cmake_minimum_required(VERSION 3.11)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

enable_testing()
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

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DUSE_HAL_DRIVER -DSTM32F446xx)
add_compile_options(-Wall)
add_link_options("-u_printf_float")

project(RocketSoftware)

if(${VERBOSE_TEST_OUTPUT})
    add_compile_definitions(VERBOSE_TEST_OUTPUT UNITY_OUTPUT_COLOR)
endif()
if(${CMAKE_BUILD_TYPE} and ${CMAKE_BUILD_TYPE} STREQUAL Test)
    add_compile_definitions(SS_RUN_TESTS)
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

macro(create_target)
  # Needs to be added after adding compile definitions
  add_subdirectory(${ROOT_DIR}/SS-common common)
  
  target_link_libraries(${PROJECT_NAME}.elf ThrowTheSwitch FreeRTOS common hal_driver)
  set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

  set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
  set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

  add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
          COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
          COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
          COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")

  add_custom_target(${PROJECT_NAME}_flash
    COMMAND openocd -c 'tcl_port disabled' -s openocd/scripts -c 'gdb_port 3333' -c 'telnet_port 4444' -f
    ${ROOT_DIR}/st_nucleo_f4.cfg -c 'program ${PROJECT_NAME}.elf' -c reset -c shutdown)

  add_custom_target(${PROJECT_NAME}_gdb
    COMMAND openocd -c 'tcl_port disabled' -s openocd/scripts -c 'gdb_port 3333' -c 'telnet_port 4444' -f
    ${ROOT_DIR}/st_nucleo_f4.cfg -c 'program ${PROJECT_NAME}.elf' -c init;reset init;)
endmacro()


add_subdirectory(External ThrowTheSwitch)
add_subdirectory(FreeRTOS FreeRTOS)
add_subdirectory(Drivers hal_driver)
add_subdirectory(Kromek)

