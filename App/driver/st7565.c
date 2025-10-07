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

#include <stdint.h>
#include <string.h>
#include "py32f0xx_ll_gpio.h"
#include "driver/gpio.h"
#include "driver/st7565.h"
#include "driver/system.h"
#include "misc.h"

#define _PORT_LCD GPIOA
#define _PIN_CS LL_GPIO_PIN_9
#define _PIN_SCLK LL_GPIO_PIN_10
#define _PIN_A0 LL_GPIO_PIN_11
#define _PIN_SDA LL_GPIO_PIN_12
#define _PIN_RSTB LL_GPIO_PIN_13

#define _CS_ASSERT() LL_GPIO_ResetOutputPin(_PORT_LCD, _PIN_CS)
#define _CS_RELEASE() LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_CS)

uint8_t gStatusLine[128];
uint8_t gFrameBuffer[7][128];

void ST7565_DrawLine(uint8_t Column, uint8_t Line, uint16_t Size, const uint8_t *pBitmap, bool bIsClearMode)
{
    uint16_t i;

    _CS_ASSERT();
    ST7565_SelectColumnAndLine(Column + 4U, Line);
    LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_A0);

    if (!bIsClearMode)
    {
        for (i = 0; i < Size; i++)
        {
            ST7565_WriteByte(pBitmap[i]);
        }
    }
    else
    {
        for (i = 0; i < Size; i++)
        {
            ST7565_WriteByte(0);
        }
    }

    _CS_RELEASE();
}

void ST7565_BlitFullScreen(void)
{
    uint8_t Line;
    uint8_t Column;

    _CS_ASSERT();
    ST7565_WriteByte(0x40);

    for (Line = 0; Line < ARRAY_SIZE(gFrameBuffer); Line++)
    {
        ST7565_SelectColumnAndLine(4U, Line + 1U);
        LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_A0);

        for (Column = 0; Column < ARRAY_SIZE(gFrameBuffer[0]); Column++)
        {
            ST7565_WriteByte(gFrameBuffer[Line][Column]);
        }
    }

    // SYSTEM_DelayMs(20);
    _CS_RELEASE();
}

void ST7565_BlitStatusLine(void)
{
    uint8_t i;

    _CS_ASSERT();

    ST7565_WriteByte(0x40);
    ST7565_SelectColumnAndLine(4, 0);
    LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_A0);

    for (i = 0; i < ARRAY_SIZE(gStatusLine); i++)
    {
        ST7565_WriteByte(gStatusLine[i]);
    }

    _CS_RELEASE();
}

void ST7565_FillScreen(uint8_t Value)
{
    memset(gStatusLine, Value, sizeof(gStatusLine));
    memset(gFrameBuffer, Value, sizeof(gFrameBuffer));
    ST7565_BlitStatusLine();
    ST7565_BlitFullScreen();
}

void ST7565_Init(void)
{
    ST7565_HardwareReset();

    _CS_ASSERT();

    ST7565_WriteByte(0xE2);
    SYSTEM_DelayMs(0x78);
    ST7565_WriteByte(0xA2);
    ST7565_WriteByte(0xC0);
    ST7565_WriteByte(0xA1);
    ST7565_WriteByte(0xA6);
    ST7565_WriteByte(0xA4);
    ST7565_WriteByte(0x24);
    ST7565_WriteByte(0x81);
    ST7565_WriteByte(0x1F);
    ST7565_WriteByte(0x2B);
    SYSTEM_DelayMs(1);
    ST7565_WriteByte(0x2E);
    SYSTEM_DelayMs(1);
    ST7565_WriteByte(0x2F);
    ST7565_WriteByte(0x2F);
    ST7565_WriteByte(0x2F);
    ST7565_WriteByte(0x2F);
    SYSTEM_DelayMs(0x28);
    ST7565_WriteByte(0x40);
    ST7565_WriteByte(0xAF);

    _CS_RELEASE();

    ST7565_FillScreen(0x00);
}

void ST7565_HardwareReset(void)
{
    LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_RSTB);
    SYSTEM_DelayMs(1);
    LL_GPIO_ResetOutputPin(_PORT_LCD, _PIN_RSTB);
    SYSTEM_DelayMs(20);
    LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_RSTB);
    SYSTEM_DelayMs(120);
}

void ST7565_SelectColumnAndLine(uint8_t Column, uint8_t Line)
{
    LL_GPIO_ResetOutputPin(_PORT_LCD, _PIN_A0);

    ST7565_WriteByte(Line + 0xB0);
    ST7565_WriteByte(((Column >> 4) & 0x0F) | 0x10);
    ST7565_WriteByte((Column >> 0) & 0x0F);
}

void ST7565_WriteByte(uint8_t Value)
{
    LL_GPIO_ResetOutputPin(_PORT_LCD, _PIN_SCLK);

    for (int i = 0; i < 8; i++)
    {
        if (0x80 & Value)
        {
            LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_SDA);
        }
        else
        {
            LL_GPIO_ResetOutputPin(_PORT_LCD, _PIN_SDA);
        }

        SYSTEM_DelayBusy(1);

        LL_GPIO_SetOutputPin(_PORT_LCD, _PIN_SCLK);

        SYSTEM_DelayBusy(1);

        Value <<= 1;
        LL_GPIO_ResetOutputPin(_PORT_LCD, _PIN_SCLK);
    }
}
