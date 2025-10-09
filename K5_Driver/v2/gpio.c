/* Copyright 2023 muzkr
 * https://github.com/muzkr
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

#include "driver/gpio.h"

#include "py32f0xx_ll_gpio.h"

#define _PIN_MAPPING(Port, Pin) ((0xffff & ((uint32_t)(Port))) << 16 | (0xffff & (Pin)))
#define _PIN_PORT(Pin) ((GPIO_TypeDef *)(IOPORT_BASE + ((Pin) >> 16)))
#define _PIN_PIN(Pin) (0xffff & (Pin))

static const uint32_t _PinMapping[] = {
    _PIN_MAPPING(GPIOF, LL_GPIO_PIN_0),  //    GPIO_PIN_KEYBOARD_0
    _PIN_MAPPING(GPIOF, LL_GPIO_PIN_1),  //     GPIO_PIN_KEYBOARD_1
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_0),  //     GPIO_PIN_KEYBOARD_2
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_1),  //     GPIO_PIN_KEYBOARD_3
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_5),  //     GPIO_PIN_KEYBOARD_4
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_6),  //     GPIO_PIN_KEYBOARD_5
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_7),  //     GPIO_PIN_KEYBOARD_6
    _PIN_MAPPING(GPIOB, LL_GPIO_PIN_0),  //     GPIO_PIN_KEYBOARD_7
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_8),  //     GPIO_PIN_BACKLIGHT
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_11), //     GPIO_PIN_ST7565_A0
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_13), //     GPIO_PIN_ST7565_RES
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_9),  //     GPIO_PIN_ST7565_CS
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_10), //     GPIO_PIN_ST7565_SCLK
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_12), //     GPIO_PIN_ST7565_SDA
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_13), //     GPIO_PIN_BK1080, !! Same as GPIO_PIN_ST7565_RES
    _PIN_MAPPING(GPIOB, LL_GPIO_PIN_3),  //     GPIO_PIN_BK4819_SCN,
    _PIN_MAPPING(GPIOB, LL_GPIO_PIN_4),  //     GPIO_PIN_BK4819_SCL,
    _PIN_MAPPING(GPIOB, LL_GPIO_PIN_5),  //     GPIO_PIN_BK4819_SDA,
    _PIN_MAPPING(GPIOB, LL_GPIO_PIN_6),  //     GPIO_PIN_FLASHLIGHT,
    _PIN_MAPPING(GPIOB, LL_GPIO_PIN_7),  //     GPIO_PIN_AUDIO_PATH,
    _PIN_MAPPING(GPIOA, LL_GPIO_PIN_3),  //     GPIO_PIN_PTT, !! Also used as UART RX
};

void GPIO_SetPinDir(uint32_t Pin, uint32_t Dir)
{
    Pin = _PinMapping[Pin];
    LL_GPIO_SetPinMode(_PIN_PORT(Pin), _PIN_PIN(Pin), GPIO_DIR_INPUT == Dir ? LL_GPIO_MODE_INPUT : LL_GPIO_MODE_OUTPUT);
}

void GPIO_SetOutputPin(uint32_t Pin)
{
    Pin = _PinMapping[Pin];
    LL_GPIO_SetOutputPin(_PIN_PORT(Pin), _PIN_PIN(Pin));
}

void GPIO_ResetOutputPin(uint32_t Pin)
{
    Pin = _PinMapping[Pin];
    LL_GPIO_ResetOutputPin(_PIN_PORT(Pin), _PIN_PIN(Pin));
}

void GPIO_ToggleOutputPin(uint32_t Pin)
{
    Pin = _PinMapping[Pin];
    LL_GPIO_TogglePin(_PIN_PORT(Pin), _PIN_PIN(Pin));
}

uint32_t GPIO_GetInputPin(uint32_t Pin)
{
    Pin = _PinMapping[Pin];
    return LL_GPIO_IsInputPinSet(_PIN_PORT(Pin), _PIN_PIN(Pin));
}
