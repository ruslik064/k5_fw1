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

add_library(App INTERFACE) 

target_include_directories(App INTERFACE .)

target_sources(App INTERFACE
    main.c
    board.c
    scheduler.c
    settings.c
    "version.c"
    functions.c
    frequencies.c
    dcs.c
    font.c
    bitmaps.c
    audio.c
    misc.c
    radio.c
    ui/aircopy.c
    ui/battery.c
    ui/fmradio.c
    ui/helper.c
    ui/inputbox.c
    ui/lock.c
    ui/main.c
    ui/menu.c
    ui/rssi.c
    ui/scanner.c
    ui/status.c
    ui/ui.c
    ui/welcome.c
    helper/battery.c
    helper/boot.c
    app/action.c
    app/aircopy.c
    app/app.c
    app/dtmf.c
    app/fm.c
    app/generic.c
    app/main.c
    app/menu.c
    app/scanner.c
    app/uart.c
)

target_compile_definitions(App INTERFACE
    "ENABLE_AIRCOPY"
    "ENABLE_ALARM"
    "ENABLE_FMRADIO"
    "ENABLE_NOAA"
    "ENABLE_TX1750"
    "ENABLE_UART"
)

target_link_libraries(App INTERFACE K5_Driver printf)

target_compile_definitions(App INTERFACE 
    "PRINTF_DISABLE_SUPPORT_FLOAT"
    "PRINTF_DISABLE_SUPPORT_LONG_LONG"
    "PRINTF_DISABLE_SUPPORT_PTRDIFF_T"
    "printf=printf_"
    "sprintf=sprintf_"
    "snprintf=snprintf_"
    "vsnprintf=vsnprintf_"
    "vprintf=vprintf_"
)
