/**
# Copyright (c) 2025 muzkr
#
#   https://github.com/muzkr
#
# Licensed under the MIT License (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at the root of this repository.
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.
#
*/
/* Copyright 2023 Manuel Jinger
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

#include "py32f0xx_ll_gpio.h"
#include "driver/gpio.h"
#include "driver/keyboard.h"
#include "driver/systick.h"

#define _PIN_SET(n) LL_GPIO_SetOutputPin(GPIO_PORT_KEYBOARD_##n, GPIO_PIN_KEYBOARD_##n)
#define _PIN_RESET(n) LL_GPIO_ResetOutputPin(GPIO_PORT_KEYBOARD_##n, GPIO_PIN_KEYBOARD_##n)
#define _PIN_IS_SET(n) LL_GPIO_IsInputPinSet(GPIO_PORT_KEYBOARD_##n, GPIO_PIN_KEYBOARD_##n)

KEY_Code_t gKeyReading0 = KEY_INVALID;
KEY_Code_t gKeyReading1 = KEY_INVALID;
uint16_t gDebounceCounter;
bool gWasFKeyPressed;

KEY_Code_t KEYBOARD_Poll(void)
{
    KEY_Code_t Key = KEY_INVALID;

    _PIN_SET(4);
    _PIN_SET(5);
    _PIN_SET(6);
    _PIN_SET(7);

    SYSTICK_DelayUs(1);

    // Keys connected to gnd
    if (!_PIN_IS_SET(0))
    {
        Key = KEY_SIDE1;
        goto Bye;
    }
    if (!_PIN_IS_SET(1))
    {
        Key = KEY_SIDE2;
        goto Bye;
    }
    // Original doesn't do PTT

    // First row
    _PIN_RESET(4);
    SYSTICK_DelayUs(1);

    if (!_PIN_IS_SET(0))
    {
        Key = KEY_MENU;
        goto Bye;
    }
    if (!_PIN_IS_SET(1))
    {
        Key = KEY_1;
        goto Bye;
    }
    if (!_PIN_IS_SET(2))
    {
        Key = KEY_4;
        goto Bye;
    }
    if (!_PIN_IS_SET(3))
    {
        Key = KEY_7;
        goto Bye;
    }

    // Second row
    _PIN_RESET(5);
    SYSTICK_DelayUs(1);

    _PIN_SET(4);
    SYSTICK_DelayUs(1);

    if (!_PIN_IS_SET(0))
    {
        Key = KEY_UP;
        goto Bye;
    }
    if (!_PIN_IS_SET(1))
    {
        Key = KEY_2;
        goto Bye;
    }
    if (!_PIN_IS_SET(2))
    {
        Key = KEY_5;
        goto Bye;
    }
    if (!_PIN_IS_SET(3))
    {
        Key = KEY_8;
        goto Bye;
    }

    // Third row
    _PIN_RESET(4);
    SYSTICK_DelayUs(1);

    _PIN_SET(5);
    SYSTICK_DelayUs(1);

    _PIN_SET(4);
    SYSTICK_DelayUs(1);

    _PIN_RESET(6);
    SYSTICK_DelayUs(1);

    if (!_PIN_IS_SET(0))
    {
        Key = KEY_DOWN;
        goto Bye;
    }
    if (!_PIN_IS_SET(1))
    {
        Key = KEY_3;
        goto Bye;
    }
    if (!_PIN_IS_SET(2))
    {
        Key = KEY_6;
        goto Bye;
    }
    if (!_PIN_IS_SET(3))
    {
        Key = KEY_9;
        goto Bye;
    }

    // Fourth row
    _PIN_RESET(7);
    SYSTICK_DelayUs(1);

    _PIN_SET(6);
    SYSTICK_DelayUs(1);

    if (!_PIN_IS_SET(0))
    {
        Key = KEY_EXIT;
        goto Bye;
    }
    if (!_PIN_IS_SET(1))
    {
        Key = KEY_STAR;
        goto Bye;
    }
    if (!_PIN_IS_SET(2))
    {
        Key = KEY_0;
        goto Bye;
    }
    if (!_PIN_IS_SET(3))
    {
        Key = KEY_F;
        goto Bye;
    }

Bye:

    _PIN_SET(4);
    _PIN_SET(5);
    _PIN_RESET(6);
    _PIN_SET(7);

    return Key;
}
