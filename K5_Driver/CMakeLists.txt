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

add_subdirectory(v1)
add_subdirectory(v2)

add_library(K5_Driver INTERFACE)
target_include_directories(K5_Driver INTERFACE .)
target_sources(K5_Driver INTERFACE
    driver/systick.c
    driver/keyboard.c
    driver/bk1080.c
    driver/bk4819.c
    driver/i2c.c
    driver/uart.c
    driver/system.c
    driver/eeprom.c
    driver/backlight.c
)
