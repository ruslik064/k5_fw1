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
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/systick.h"

#define _PORT_I2C GPIO_PORT_I2C_SCL
#define _PIN_SCL GPIO_PIN_I2C_SCL
#define _PIN_SDA GPIO_PIN_I2C_SDA

#define _SET_SCL() LL_GPIO_SetOutputPin(_PORT_I2C, _PIN_SCL)
#define _RESET_SCL() LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SCL)

#define _SET_SDA() LL_GPIO_SetOutputPin(_PORT_I2C, _PIN_SDA)
#define _RESET_SDA() LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SDA)

void I2C_Start(void)
{
    _SET_SDA();
    SYSTICK_DelayUs(1);
    _SET_SCL();
    SYSTICK_DelayUs(1);
    _RESET_SDA();
    SYSTICK_DelayUs(1);
    _RESET_SCL();
    SYSTICK_DelayUs(1);
}

void I2C_Stop(void)
{
    _RESET_SDA();
    SYSTICK_DelayUs(1);
    _RESET_SCL();
    SYSTICK_DelayUs(1);
    _SET_SCL();
    SYSTICK_DelayUs(1);
    _SET_SDA();
    SYSTICK_DelayUs(1);
}

uint8_t I2C_Read(bool bFinal)
{
    uint8_t i, Data;

    LL_GPIO_SetPinMode(_PORT_I2C, _PIN_SDA, LL_GPIO_MODE_INPUT);

    Data = 0;
    for (i = 0; i < 8; i++)
    {
        LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SCL);
        SYSTICK_DelayUs(1);
        LL_GPIO_SetOutputPin(_PORT_I2C, _PIN_SCL);
        SYSTICK_DelayUs(1);
        Data <<= 1;
        SYSTICK_DelayUs(1);

        if (LL_GPIO_IsInputPinSet(_PORT_I2C, _PIN_SDA))
        {
            Data |= 1U;
        }
        LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SCL);
        SYSTICK_DelayUs(1);
    }

    LL_GPIO_SetPinMode(_PORT_I2C, _PIN_SDA, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SCL);
    SYSTICK_DelayUs(1);
    if (bFinal)
    {
        LL_GPIO_SetOutputPin(_PORT_I2C, _PIN_SDA);
    }
    else
    {
        LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SDA);
    }
    SYSTICK_DelayUs(1);
    LL_GPIO_SetOutputPin(_PORT_I2C, _PIN_SCL);
    SYSTICK_DelayUs(1);
    LL_GPIO_ResetOutputPin(_PORT_I2C, _PIN_SCL);
    SYSTICK_DelayUs(1);

    return Data;
}

int I2C_Write(uint8_t Data)
{
    uint8_t i;
    int ret = -1;

    _RESET_SCL();
    SYSTICK_DelayUs(1);

    for (i = 0; i < 8; i++)
    {
        if ((Data & 0x80) == 0)
        {
            _RESET_SDA();
        }
        else
        {
            _SET_SDA();
        }
        Data <<= 1;
        SYSTICK_DelayUs(1);
        _SET_SCL();
        SYSTICK_DelayUs(1);
        _RESET_SCL();
        SYSTICK_DelayUs(1);
    }

    LL_GPIO_SetPinMode(_PORT_I2C, _PIN_SDA, LL_GPIO_MODE_INPUT);
    _SET_SDA();
    SYSTICK_DelayUs(1);
    _SET_SCL();
    SYSTICK_DelayUs(1);

    for (i = 0; i < 255; i++)
    {
        if (!LL_GPIO_IsInputPinSet(_PORT_I2C, _PIN_SDA))
        {
            ret = 0;
            break;
        }
    }

    _RESET_SCL();
    SYSTICK_DelayUs(1);
    LL_GPIO_SetPinMode(_PORT_I2C, _PIN_SDA, LL_GPIO_MODE_OUTPUT);
    _RESET_SDA();

    return ret;
}

int I2C_ReadBuffer(void *pBuffer, uint8_t Size)
{
    uint8_t *pData = (uint8_t *)pBuffer;
    uint8_t i;

    if (Size == 1)
    {
        *pData = I2C_Read(true);
        return 1;
    }

    for (i = 0; i < Size - 1; i++)
    {
        SYSTICK_DelayUs(1);
        pData[i] = I2C_Read(false);
    }

    SYSTICK_DelayUs(1);
    pData[i++] = I2C_Read(true);

    return Size;
}

int I2C_WriteBuffer(const void *pBuffer, uint8_t Size)
{
    const uint8_t *pData = (const uint8_t *)pBuffer;
    uint8_t i;

    for (i = 0; i < Size; i++)
    {
        if (I2C_Write(*pData++) < 0)
        {
            return -1;
        }
    }

    return 0;
}
