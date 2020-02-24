set(ENV{LIBRARIES} ../SS-common/fifo)
include(../CMakeLists.txt)
PROJECT(Kromek_v2 C CXX ASM)
cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE SOURCES "Drivers/*.*" "Inc/*.*" "Libraries/SS-common_ADS1258/*.*" "../SS-common/*.*" "Src/*.*" "Test/*.*" "Unity/extras/fixture/src/*.*" "../Unity/extras/memory/src/*.*" "../Unity/src/*.*" "startup/*.*")

include_libraries(Inc
                    ../SS-common/JSON-settings/test
                    ../SS-common/JSON-settings
                    ../SS-common/JSON-settings/jsmn
                    ../Unity/src
                    ../Unity/extras/fixture/src
                    Test
                    ../Unity/extras/memory/src
                    Drivers/STM32F4xx_HAL_Driver/Inc
                    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
                    Drivers/CMSIS/Device/ST/STM32F4xx/Include
                    Drivers/CMSIS/Include
                    ../SS-common/adc
                    ../SS-common/com
                    ../SS-common/dynamixel
        ../SS-common/ADS1258
        ../SS-common/grazyna
                    ../SS-common/mutex
                    ../SS-common/relays
                    ../SS-common/S25FL
                    ../SS-common/sequence
                    ../SS-common/servos
                    ../SS-common/supply
                    ../SS-common/support
        ../SS-common/fifo
                    )

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F446ZETx_FLASH.ld)

set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

#build(${PROJECT_NAME} ${SOURCES})
#add_subdirectory(../SS-common/fifo fifo)
#target_link_libraries(${PROJECT_NAME}.elf fifo)
