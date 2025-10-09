#    Copyright 2025 muzkr
#
#         https://github.com/muzkr
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

set(EXE_NAME "k5_v2_fw1-${VERSION_STRING}")
add_executable(${EXE_NAME})

target_include_directories(${EXE_NAME} PRIVATE Inc)

target_sources(${EXE_NAME} PRIVATE
    Src/system_py32f0xx.c
    Src/main.c
    Src/py32f0xx_it.c
    Src/startup_py32f030xx.s
)

target_compile_definitions(${EXE_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    "VERSION_STRING=\"${VERSION_STRING}\""
    "Main=main" # App/main.c
    "SystickHandler=SysTick_Handler" # App/scheduler.c
)

target_link_libraries(${EXE_NAME} App K5_Driver_v2)

set(TARGET_FLAGS "-mcpu=cortex-m0plus")
target_compile_options(${EXE_NAME} PRIVATE ${TARGET_FLAGS})
target_link_options(${EXE_NAME} PRIVATE ${TARGET_FLAGS} "-T" "${CMAKE_CURRENT_SOURCE_DIR}/py32f030x8.ld")
target_link_options(${EXE_NAME} PRIVATE 
    "-Wl,-Map=${EXE_NAME}.map" 
    "--specs=nano.specs" 
    "-Wl,--gc-sections"
)
target_link_libraries(${EXE_NAME} "m")

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${EXE_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${EXE_NAME}.map)

set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# -----------------------------------
#  Generate .hex and .bin files
#

set(TARGET_ELF "${CMAKE_BINARY_DIR}/${EXE_NAME}.elf")

# Generate .bin file
add_custom_command(
    TARGET ${EXE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET_ELF} ${CMAKE_BINARY_DIR}/${EXE_NAME}.bin
    COMMENT "Generating .bin file"
)

# Generate .hex file
add_custom_command(
    TARGET ${EXE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${TARGET_ELF} ${CMAKE_BINARY_DIR}/${EXE_NAME}.hex
    COMMENT "Generating .hex file"
)
