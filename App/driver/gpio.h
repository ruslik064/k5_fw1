/* Copyright (c) 2025 muzkr
 * https://github.com/muzkr
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://mit-license.org/
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *
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

#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#include "py32f0xx_ll_gpio.h"

// -------------------------
//  Keyboard

// Row 1: PF0
#define GPIO_PORT_KEYBOARD_0 GPIOF
#define GPIO_PIN_KEYBOARD_0 LL_GPIO_PIN_0
// Row 2: PF1
#define GPIO_PORT_KEYBOARD_1 GPIOF
#define GPIO_PIN_KEYBOARD_1 LL_GPIO_PIN_1
// Row 3: PA0
#define GPIO_PORT_KEYBOARD_2 GPIOA
#define GPIO_PIN_KEYBOARD_2 LL_GPIO_PIN_0
// Row 4: PA1
#define GPIO_PORT_KEYBOARD_3 GPIOA
#define GPIO_PIN_KEYBOARD_3 LL_GPIO_PIN_1

// Col 1: PA5
#define GPIO_PORT_KEYBOARD_4 GPIOA
#define GPIO_PIN_KEYBOARD_4 LL_GPIO_PIN_5
// Col 2: PA6
#define GPIO_PORT_KEYBOARD_5 GPIOA
#define GPIO_PIN_KEYBOARD_5 LL_GPIO_PIN_6
// Col 3: PA7
#define GPIO_PORT_KEYBOARD_6 GPIOA
#define GPIO_PIN_KEYBOARD_6 LL_GPIO_PIN_7
// Col 4: PB0
#define GPIO_PORT_KEYBOARD_7 GPIOB
#define GPIO_PIN_KEYBOARD_7 LL_GPIO_PIN_0

#define GPIO_PORT_I2C_SCL GPIO_PORT_KEYBOARD_4
#define GPIO_PIN_I2C_SCL GPIO_PIN_KEYBOARD_4
#define GPIO_PORT_I2C_SDA GPIO_PORT_KEYBOARD_5
#define GPIO_PIN_I2C_SDA GPIO_PIN_KEYBOARD_5

#define GPIO_PORT_VOICE_0 GPIO_PORT_KEYBOARD_6
#define GPIO_PIN_VOICE_0 GPIO_PIN_KEYBOARD_6
#define GPIO_PORT_VOICE_1 GPIO_PORT_KEYBOARD_7
#define GPIO_PIN_VOICE_1 GPIO_PIN_KEYBOARD_7

// --------------------------
//

// PA3
#define GPIO_PORT_PTT GPIOA
#define GPIO_PIN_PTT LL_GPIO_PIN_3

// PA8
#define GPIO_PORT_BACKLIGHT GPIOA
#define GPIO_PIN_BACKLIGHT LL_GPIO_PIN_8

// PB6
#define GPIO_PORT_FLASHLIGHT GPIOB
#define GPIO_PIN_FLASHLIGHT LL_GPIO_PIN_6

// PB7
#define GPIO_PORT_AUDIO_PATH GPIOB
#define GPIO_PIN_AUDIO_PATH LL_GPIO_PIN_7

#endif // DRIVER_GPIO_H
