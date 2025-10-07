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
/* Copyright 2023 Dual Tachyon
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
#include "driver/backlight.h"
#include "driver/gpio.h"
#include "settings.h"

static inline void _TurnOn_force(void)
{
    LL_GPIO_SetOutputPin(GPIO_PORT_BACKLIGHT, GPIO_PIN_BACKLIGHT);
}
static inline void _TurnOff_force(void)
{
    LL_GPIO_ResetOutputPin(GPIO_PORT_BACKLIGHT, GPIO_PIN_BACKLIGHT);
}

uint8_t gBacklightCountdown;

void BACKLIGHT_TurnOn(void)
{
    if (gEeprom.BACKLIGHT)
    {
        _TurnOn_force();
        gBacklightCountdown = 1 + (gEeprom.BACKLIGHT * 2);
    }
}

void BACKLIGHT_TurnOn_force(void)
{
    _TurnOn_force();
}

void BACKLIGHT_TurnOff_force(void)
{
    _TurnOff_force();
}
