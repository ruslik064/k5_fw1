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

add_library(K5_Driver_v1 INTERFACE) 

target_include_directories(K5_Driver_v1 INTERFACE .)

target_sources(K5_Driver_v1 INTERFACE
    device.c
    board.c
    adc.c
    aes.c
    crc.c
    gpio.c
    spi.c
    # "system.c"
    uart.c
    st7565.c
    adc.c
)

target_link_libraries(K5_Driver_v1 INTERFACE K5_Driver DP32_BSP)
target_compile_definitions(K5_Driver_v1 INTERFACE "K5_V1")
