
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    main.c
    main1.c
    scheduler.c
    board.c
    settings.c
    radio.c
    dcs.c
    functions.c
    frequencies.c
    font.c
    audio.c
    misc.c
    "version.c"
    bitmaps.c
    app/app.c
    app/fm.c
    app/dtmf.c
    app/uart.c
    app/generic.c
    app/scanner.c
    app/menu.c
    app/action.c
    app/main.c
    app/aircopy.c
    ui/ui.c
    ui/rssi.c
    ui/welcome.c
    ui/main.c
    ui/menu.c
    ui/scanner.c
    ui/helper.c
    ui/lock.c
    ui/status.c
    ui/battery.c
    ui/inputbox.c
    ui/fmradio.c
    ui/aircopy.c
    helper/battery.c
    helper/boot.c
    driver/systick.c
    driver/system.c
    driver/uart.c
    driver/bk4819.c
    driver/bk1080.c
    driver/st7565.c
    driver/eeprom.c
    driver/i2c.c
    driver/backlight.c
    driver/crc.c
    driver/keyboard.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    .
)

# Symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    "ENABLE_UART"
    "ENABLE_FMRADIO"
    "ENABLE_ALARM"
    "ENABLE_NOAA"
    "ENABLE_AIRCOPY"
    "ENABLE_TX1750"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    Core
    printf
)

target_compile_definitions(printf INTERFACE 
    "PRINTF_DISABLE_SUPPORT_FLOAT"
    "PRINTF_DISABLE_SUPPORT_LONG_LONG"
    "PRINTF_DISABLE_SUPPORT_PTRDIFF_T"
    "printf=printf_"
    "sprintf=sprintf_"
    "snprintf=snprintf_"
    "vsnprintf=vsnprintf_"
    "vprintf=vprintf_"
)
