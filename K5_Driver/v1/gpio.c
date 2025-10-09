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

#include "driver/gpio.h"
#include "bsp/dp32g030/gpio.h"
#include "bsp/dp32g030/portcon.h"

#define _PIN_MAPPING(Bank, PinNum) (((0xffff & (uint32_t)(Bank)) << 16) | (0xffff & (PinNum)))
#define _GPIO_BANK(Pin) ((volatile GPIO_Bank_t *)(GPIOA_BASE_ADDR | (0xffff & ((Pin) >> 16))))
#define _PIN_NUM(Pin) (0xffff & (Pin))

static const uint32_t _PinMappings[] = {
    _PIN_MAPPING(GPIOA_BASE_ADDR, 3),  // GPIO_PIN_KEYBOARD_0
    _PIN_MAPPING(GPIOA_BASE_ADDR, 4),  // GPIO_PIN_KEYBOARD_1
    _PIN_MAPPING(GPIOA_BASE_ADDR, 5),  // GPIO_PIN_KEYBOARD_2
    _PIN_MAPPING(GPIOA_BASE_ADDR, 6),  // GPIO_PIN_KEYBOARD_3
    _PIN_MAPPING(GPIOA_BASE_ADDR, 10), // GPIO_PIN_KEYBOARD_4
    _PIN_MAPPING(GPIOA_BASE_ADDR, 11), // GPIO_PIN_KEYBOARD_5
    _PIN_MAPPING(GPIOA_BASE_ADDR, 12), // GPIO_PIN_KEYBOARD_6
    _PIN_MAPPING(GPIOA_BASE_ADDR, 13), // GPIO_PIN_KEYBOARD_7
    _PIN_MAPPING(GPIOB_BASE_ADDR, 6),  // GPIO_PIN_BACKLIGHT
    _PIN_MAPPING(GPIOB_BASE_ADDR, 9),  // GPIO_PIN_ST7565_A0
    _PIN_MAPPING(GPIOB_BASE_ADDR, 11), // GPIO_PIN_ST7565_RES
    0,                                 // GPIO_PIN_ST7565_CS, !! Not used
    0,                                 // GPIO_PIN_ST7565_SCLK, !! Not used
    0,                                 // GPIO_PIN_ST7565_SDA, !! Not used
    _PIN_MAPPING(GPIOB_BASE_ADDR, 15), // GPIO_PIN_BK1080
    _PIN_MAPPING(GPIOC_BASE_ADDR, 0),  // GPIO_PIN_BK4819_SCN
    _PIN_MAPPING(GPIOC_BASE_ADDR, 1),  // GPIO_PIN_BK4819_SCL
    _PIN_MAPPING(GPIOC_BASE_ADDR, 2),  // GPIO_PIN_BK4819_SDA
    _PIN_MAPPING(GPIOC_BASE_ADDR, 3),  // GPIO_PIN_FLASHLIGHT
    _PIN_MAPPING(GPIOC_BASE_ADDR, 4),  // GPIO_PIN_AUDIO_PATH
    _PIN_MAPPING(GPIOC_BASE_ADDR, 5),  // GPIO_PIN_PTT
};

static inline void GPIO_ClearBit(volatile uint32_t *pReg, uint8_t Bit)
{
    *pReg &= ~(1U << Bit);
}

static inline uint8_t GPIO_CheckBit(volatile uint32_t *pReg, uint8_t Bit)
{
    return (*pReg >> Bit) & 1U;
}

static inline void GPIO_FlipBit(volatile uint32_t *pReg, uint8_t Bit)
{
    *pReg ^= 1U << Bit;
}

static inline void GPIO_SetBit(volatile uint32_t *pReg, uint8_t Bit)
{
    *pReg |= 1U << Bit;
}

void GPIO_SetPinDir(uint32_t Pin, uint32_t Dir)
{
    Pin = _PinMappings[Pin];
    volatile GPIO_Bank_t *Bank = _GPIO_BANK(Pin);
    uint32_t PinNum = _PIN_NUM(Pin);
    uint32_t PortNum = (((uint32_t)Bank) - GPIOA_BASE_ADDR) / 0x800U;
    volatile uint32_t *PORTCON_IE = (uint32_t *)(PORTCON_PORTA_IE_ADDR + 4U * PortNum);

    if (GPIO_DIR_INPUT == Dir)
    {
        // PC2:
        // PORTCON_PORTC_IE = (PORTCON_PORTC_IE & ~PORTCON_PORTC_IE_C2_MASK) | PORTCON_PORTC_IE_C2_BITS_ENABLE;
        // GPIOC->DIR = (GPIOC->DIR & ~GPIO_DIR_2_MASK) | GPIO_DIR_2_BITS_INPUT;
        GPIO_SetBit(PORTCON_IE, PinNum);
        GPIO_ClearBit(&Bank->DIR, PinNum);
    }
    else
    {
        // PC2:
        // PORTCON_PORTC_IE = (PORTCON_PORTC_IE & ~PORTCON_PORTC_IE_C2_MASK) | PORTCON_PORTC_IE_C2_BITS_DISABLE;
        // GPIOC->DIR = (GPIOC->DIR & ~GPIO_DIR_2_MASK) | GPIO_DIR_2_BITS_OUTPUT;
        GPIO_ClearBit(PORTCON_IE, PinNum);
        GPIO_SetBit(&Bank->DIR, PinNum);
    }
}

void GPIO_SetOutputPin(uint32_t Pin)
{
    Pin = _PinMappings[Pin];
    GPIO_SetBit(&_GPIO_BANK(Pin)->DATA, _PIN_NUM(Pin));
}

void GPIO_ResetOutputPin(uint32_t Pin)
{
    Pin = _PinMappings[Pin];
    GPIO_ClearBit(&_GPIO_BANK(Pin)->DATA, _PIN_NUM(Pin));
}

void GPIO_ToggleOutputPin(uint32_t Pin)
{
    Pin = _PinMappings[Pin];
    GPIO_FlipBit(&_GPIO_BANK(Pin)->DATA, _PIN_NUM(Pin));
}

uint32_t GPIO_GetInputPin(uint32_t Pin)
{
    Pin = _PinMappings[Pin];
    return GPIO_CheckBit(&_GPIO_BANK(Pin)->DATA, _PIN_NUM(Pin));
}
