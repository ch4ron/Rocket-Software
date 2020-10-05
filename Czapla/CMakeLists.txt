cmake_minimum_required(VERSION 3.11)


set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETx_FLASH.ld)
set(MCU STM32F446RE)

project(czapla C ASM)

file(GLOB_RECURSE SOURCES
        "Inc/*.*"
        "Src/*.*"
        "startup/*.*")

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_include_directories(${PROJECT_NAME}.elf PRIVATE Inc)


# DEFINE YOUR LIBRARIES HERE

#add_compile_definitions(SS_USE_COM)
add_compile_definitions(SS_USE_CAN)
#add_compile_definitions(SS_COM_DEBUG)
#add_compile_definitions(SS_USE_GRAZYNA)
add_compile_definitions(SS_USE_COM)
#add_compile_definitions(SS_USE_GRAZYNA)

#add_compile_definitions(SS_USE_FLASH)


create_target(${PROJECT_NAME})
