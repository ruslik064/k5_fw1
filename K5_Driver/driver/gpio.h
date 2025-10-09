/* Copyright 2025 muzkr https://github.com/muzkr
 * Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#include <stdint.h>

enum GPIO_PINS
{
    GPIO_PIN_KEYBOARD_0 = 0U,
    GPIO_PIN_KEYBOARD_1,
    GPIO_PIN_KEYBOARD_2,
    GPIO_PIN_KEYBOARD_3,
    GPIO_PIN_KEYBOARD_4, // Shared with I2C!
    GPIO_PIN_KEYBOARD_5, // Shared with I2C!
    GPIO_PIN_KEYBOARD_6, // Shared with voice chip!
    GPIO_PIN_KEYBOARD_7, // Shared with voice chip!
    GPIO_PIN_BACKLIGHT,
    GPIO_PIN_ST7565_A0,
    GPIO_PIN_ST7565_RES,
    GPIO_PIN_ST7565_CS,
    GPIO_PIN_ST7565_SCLK,
    GPIO_PIN_ST7565_SDA,
    GPIO_PIN_BK1080,
    GPIO_PIN_BK4819_SCN,
    GPIO_PIN_BK4819_SCL,
    GPIO_PIN_BK4819_SDA,
    GPIO_PIN_FLASHLIGHT,
    GPIO_PIN_AUDIO_PATH,
    GPIO_PIN_PTT,
    _GPIO_PIN_LAST,

    GPIO_PIN_I2C_SCL = GPIO_PIN_KEYBOARD_4, // Shared with keyboard!
    GPIO_PIN_I2C_SDA = GPIO_PIN_KEYBOARD_5, // Shared with keyboard!
    GPIO_PIN_VOICE_0 = GPIO_PIN_KEYBOARD_6, // Shared with keyboard!
    GPIO_PIN_VOICE_1 = GPIO_PIN_KEYBOARD_7, // Shared with keyboard!
};

#define GPIO_DIR_INPUT 0U
#define GPIO_DIR_OUTPUT 1U

void GPIO_SetPinDir(uint32_t Pin, uint32_t Dir);
void GPIO_SetOutputPin(uint32_t Pin);
void GPIO_ResetOutputPin(uint32_t Pin);
void GPIO_ToggleOutputPin(uint32_t Pin);
uint32_t GPIO_GetInputPin(uint32_t Pin);

static inline void GPIO_SetAudioPath() { GPIO_SetOutputPin(GPIO_PIN_AUDIO_PATH); }
static inline void GPIO_ResetAudioPath() { GPIO_ResetOutputPin(GPIO_PIN_AUDIO_PATH); }

static inline void GPIO_SetBacklight() { GPIO_SetOutputPin(GPIO_PIN_BACKLIGHT); }
static inline void GPIO_ResetBacklight() { GPIO_ResetOutputPin(GPIO_PIN_BACKLIGHT); }

#endif
