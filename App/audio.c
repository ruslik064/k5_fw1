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

#if defined(ENABLE_FMRADIO)
#include "app/fm.h"
#endif

#include "audio.h"
#include "py32f0xx_ll_gpio.h"

#if defined(ENABLE_FMRADIO)
#include "driver/bk1080.h"
#endif

#include "driver/bk4819.h"
#include "driver/gpio.h"
#include "driver/system.h"
#include "driver/systick.h"
#include "functions.h"
#include "misc.h"
#include "settings.h"
#include "ui/ui.h"

#define _AMP_PORT GPIOB
#define _AMP_PIN LL_GPIO_PIN_7

// PA7
#define _VOICE_PORT_0 GPIOA
#define _VOICE_PIN_0 LL_GPIO_PIN_7

// PB0
#define _VOICE_PORT_1 GPIOB
#define _VOICE_PIN_1 LL_GPIO_PIN_0

static const uint8_t VoiceClipLengthChinese[58] = {
    0x32,
    0x32,
    0x32,
    0x37,
    0x37,
    0x32,
    0x32,
    0x32,
    0x32,
    0x37,
    0x37,
    0x32,
    0x64,
    0x64,
    0x64,
    0x64,
    0x64,
    0x69,
    0x64,
    0x69,
    0x5A,
    0x5F,
    0x5F,
    0x64,
    0x64,
    0x69,
    0x64,
    0x64,
    0x69,
    0x69,
    0x69,
    0x64,
    0x64,
    0x6E,
    0x69,
    0x5F,
    0x64,
    0x64,
    0x64,
    0x69,
    0x69,
    0x69,
    0x64,
    0x69,
    0x64,
    0x64,
    0x55,
    0x5F,
    0x5A,
    0x4B,
    0x4B,
    0x46,
    0x46,
    0x69,
    0x64,
    0x6E,
    0x5A,
    0x64,
};

static const uint8_t VoiceClipLengthEnglish[76] = {
    0x50,
    0x32,
    0x2D,
    0x2D,
    0x2D,
    0x37,
    0x37,
    0x37,
    0x32,
    0x32,
    0x3C,
    0x37,
    0x46,
    0x46,
    0x4B,
    0x82,
    0x82,
    0x6E,
    0x82,
    0x46,
    0x96,
    0x64,
    0x46,
    0x6E,
    0x78,
    0x6E,
    0x87,
    0x64,
    0x96,
    0x96,
    0x46,
    0x9B,
    0x91,
    0x82,
    0x82,
    0x73,
    0x78,
    0x64,
    0x82,
    0x6E,
    0x78,
    0x82,
    0x87,
    0x6E,
    0x55,
    0x78,
    0x64,
    0x69,
    0x9B,
    0x5A,
    0x50,
    0x3C,
    0x32,
    0x55,
    0x64,
    0x64,
    0x50,
    0x46,
    0x46,
    0x46,
    0x4B,
    0x4B,
    0x50,
    0x50,
    0x55,
    0x4B,
    0x4B,
    0x32,
    0x32,
    0x32,
    0x32,
    0x37,
    0x41,
    0x32,
    0x3C,
    0x37,
};

VOICE_ID_t gVoiceID[8];
uint8_t gVoiceReadIndex;
uint8_t gVoiceWriteIndex;
volatile uint16_t gCountdownToPlayNextVoice;
volatile bool gFlagPlayQueuedVoice;
VOICE_ID_t gAnotherVoiceID = VOICE_ID_INVALID;
BEEP_Type_t gBeepToPlay;

void AUDIO_PlayBeep(BEEP_Type_t Beep)
{
    uint16_t ToneConfig;
    uint16_t ToneFrequency;
    uint16_t Duration;

    if (Beep != BEEP_500HZ_60MS_DOUBLE_BEEP && Beep != BEEP_440HZ_500MS && !gEeprom.BEEP_CONTROL)
    {
        return;
    }

#if defined(ENABLE_AIRCOPY)
    if (gScreenToDisplay == DISPLAY_AIRCOPY)
    {
        return;
    }
#endif
    if (gCurrentFunction == FUNCTION_RECEIVE)
    {
        return;
    }
    if (gCurrentFunction == FUNCTION_MONITOR)
    {
        return;
    }

    ToneConfig = BK4819_ReadRegister(BK4819_REG_71);

    LL_GPIO_ResetOutputPin(_AMP_PORT, _AMP_PIN);

    if (gCurrentFunction == FUNCTION_POWER_SAVE && gRxIdleMode)
    {
        BK4819_RX_TurnOn();
    }

#if defined(ENABLE_FMRADIO)
    if (gFmRadioMode)
    {
        BK1080_Mute(true);
    }
#endif
    SYSTEM_DelayMs(20);
    switch (Beep)
    {
    case BEEP_1KHZ_60MS_OPTIONAL:
        ToneFrequency = 1000;
        break;
    case BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL:
    case BEEP_500HZ_60MS_DOUBLE_BEEP:
        ToneFrequency = 500;
        break;
    default:
        ToneFrequency = 440;
        break;
    }
    BK4819_PlayTone(ToneFrequency, true);
    SYSTEM_DelayMs(2);
    LL_GPIO_SetOutputPin(_AMP_PORT, _AMP_PIN);
    SYSTEM_DelayMs(60);

    switch (Beep)
    {
    case BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL:
    case BEEP_500HZ_60MS_DOUBLE_BEEP:
        BK4819_ExitTxMute();
        SYSTEM_DelayMs(60);
        BK4819_EnterTxMute();
        SYSTEM_DelayMs(20);
        // Fallthrough
    case BEEP_1KHZ_60MS_OPTIONAL:
        BK4819_ExitTxMute();
        Duration = 60;
        break;
    case BEEP_440HZ_500MS:
    default:
        BK4819_ExitTxMute();
        Duration = 500;
        break;
    }

    SYSTEM_DelayMs(Duration);
    BK4819_EnterTxMute();
    SYSTEM_DelayMs(20);
    LL_GPIO_ResetOutputPin(_AMP_PORT, _AMP_PIN);

    gVoxResumeCountdown = 80;

    SYSTEM_DelayMs(5);
    BK4819_TurnsOffTones_TurnsOnRX();
    SYSTEM_DelayMs(5);
    BK4819_WriteRegister(BK4819_REG_71, ToneConfig);
    if (gEnableSpeaker)
    {
        LL_GPIO_SetOutputPin(_AMP_PORT, _AMP_PIN);
    }
#if defined(ENABLE_FMRADIO)
    if (gFmRadioMode)
    {
        BK1080_Mute(false);
    }
#endif
    if (gCurrentFunction == FUNCTION_POWER_SAVE && gRxIdleMode)
    {
        BK4819_Sleep();
    }
}

void AUDIO_PlayVoice(uint8_t VoiceID)
{
    uint8_t i;

    LL_GPIO_SetOutputPin(_VOICE_PORT_0, _VOICE_PIN_0);
    SYSTEM_DelayMs(20);
    LL_GPIO_ResetOutputPin(_VOICE_PORT_0, _VOICE_PIN_0);
    for (i = 0; i < 8; i++)
    {
        if ((VoiceID & 0x80U) == 0)
        {
            LL_GPIO_ResetOutputPin(_VOICE_PORT_1, _VOICE_PIN_1);
        }
        else
        {
            LL_GPIO_SetOutputPin(_VOICE_PORT_1, _VOICE_PIN_1);
        }
        SYSTICK_DelayUs(1000);
        LL_GPIO_SetOutputPin(_VOICE_PORT_0, _VOICE_PIN_0);
        SYSTICK_DelayUs(1200);
        LL_GPIO_ResetOutputPin(_VOICE_PORT_0, _VOICE_PIN_0);
        VoiceID <<= 1;
        SYSTICK_DelayUs(200);
    }
}

void AUDIO_PlaySingleVoice(bool bFlag)
{
    uint8_t VoiceID;
    uint8_t Delay;

    VoiceID = gVoiceID[0];
    if (gEeprom.VOICE_PROMPT != VOICE_PROMPT_OFF && gVoiceWriteIndex)
    {
        if (gEeprom.VOICE_PROMPT == VOICE_PROMPT_CHINESE)
        {
            // Chinese
            if (VoiceID >= sizeof(VoiceClipLengthChinese))
            {
                goto Bailout;
            }
            Delay = VoiceClipLengthChinese[VoiceID];
            VoiceID += VOICE_ID_CHI_BASE;
        }
        else
        {
            // English
            if (VoiceID >= sizeof(VoiceClipLengthEnglish))
            {
                goto Bailout;
            }
            Delay = VoiceClipLengthEnglish[VoiceID];
            VoiceID += VOICE_ID_ENG_BASE;
        }

        if (gCurrentFunction == FUNCTION_RECEIVE || gCurrentFunction == FUNCTION_MONITOR)
        {
            BK4819_SetAF(BK4819_AF_MUTE);
        }
#if defined(ENABLE_FMRADIO)
        if (gFmRadioMode)
        {
            BK1080_Mute(true);
        }
#endif
        LL_GPIO_SetOutputPin(_AMP_PORT, _AMP_PIN);
        gVoxResumeCountdown = 2000;
        SYSTEM_DelayMs(5);
        AUDIO_PlayVoice(VoiceID);
        if (gVoiceWriteIndex == 1)
        {
            Delay += 3;
        }
        if (bFlag)
        {
            SYSTEM_DelayMs(Delay * 10);
            if (gCurrentFunction == FUNCTION_RECEIVE || gCurrentFunction == FUNCTION_MONITOR)
            {
                if (gRxVfo->IsAM)
                {
                    BK4819_SetAF(BK4819_AF_AM);
                }
                else
                {
                    BK4819_SetAF(BK4819_AF_OPEN);
                }
            }
#if defined(ENABLE_FMRADIO)
            if (gFmRadioMode)
            {
                BK1080_Mute(false);
            }
#endif
            if (!gEnableSpeaker)
            {
                LL_GPIO_ResetOutputPin(_AMP_PORT, _AMP_PIN);
            }
            gVoiceWriteIndex = 0;
            gVoiceReadIndex = 0;
            gVoxResumeCountdown = 80;
            return;
        }
        gVoiceReadIndex = 1;
        gCountdownToPlayNextVoice = Delay;
        gFlagPlayQueuedVoice = false;
        return;
    }

Bailout:
    gVoiceReadIndex = 0;
    gVoiceWriteIndex = 0;
}

void AUDIO_SetVoiceID(uint8_t Index, VOICE_ID_t VoiceID)
{
    if (Index >= ARRAY_SIZE(gVoiceID))
    {
        return;
    }
    if (Index == 0)
    {
        gVoiceWriteIndex = 0;
        gVoiceReadIndex = 0;
    }

    gVoiceID[Index] = VoiceID;
    gVoiceWriteIndex++;
}

uint8_t AUDIO_SetDigitVoice(uint8_t Index, uint16_t Value)
{
    uint16_t Remainder;
    uint8_t Result;
    uint8_t Count;

    if (Index == 0)
    {
        gVoiceWriteIndex = 0;
        gVoiceReadIndex = 0;
    }

    Count = 0;
    Result = Value / 1000U;
    Remainder = Value % 1000U;
    if (Remainder < 100U)
    {
        if (Remainder < 10U)
        {
            goto Skip;
        }
    }
    else
    {
        Result = Remainder / 100U;
        gVoiceID[gVoiceWriteIndex++] = (VOICE_ID_t)Result;
        Count++;
        Remainder -= Result * 100U;
    }
    Result = Remainder / 10U;
    gVoiceID[gVoiceWriteIndex++] = (VOICE_ID_t)Result;
    Count++;
    Remainder -= Result * 10U;

Skip:
    gVoiceID[gVoiceWriteIndex++] = (VOICE_ID_t)Remainder;

    return Count + 1U;
}

void AUDIO_PlayQueuedVoice(void)
{
    uint8_t VoiceID;
    uint8_t Delay;
    bool Skip;

    Skip = false;
    if (gVoiceReadIndex != gVoiceWriteIndex && gEeprom.VOICE_PROMPT != VOICE_PROMPT_OFF)
    {
        VoiceID = gVoiceID[gVoiceReadIndex];
        if (gEeprom.VOICE_PROMPT == VOICE_PROMPT_CHINESE)
        {
            if (VoiceID < ARRAY_SIZE(VoiceClipLengthChinese))
            {
                Delay = VoiceClipLengthChinese[VoiceID];
                VoiceID += VOICE_ID_CHI_BASE;
            }
            else
            {
                Skip = true;
            }
        }
        else
        {
            if (VoiceID < ARRAY_SIZE(VoiceClipLengthEnglish))
            {
                Delay = VoiceClipLengthEnglish[VoiceID];
                VoiceID += VOICE_ID_ENG_BASE;
            }
            else
            {
                Skip = true;
            }
        }
        gVoiceReadIndex++;
        if (!Skip)
        {
            if (gVoiceReadIndex == gVoiceWriteIndex)
            {
                Delay += 3;
            }
            AUDIO_PlayVoice(VoiceID);
            gCountdownToPlayNextVoice = Delay;
            gFlagPlayQueuedVoice = false;
            gVoxResumeCountdown = 2000;
            return;
        }
    }

    if (gCurrentFunction == FUNCTION_RECEIVE || gCurrentFunction == FUNCTION_MONITOR)
    {
        if (gRxVfo->IsAM)
        {
            BK4819_SetAF(BK4819_AF_AM);
        }
        else
        {
            BK4819_SetAF(BK4819_AF_OPEN);
        }
    }
#if defined(ENABLE_FMRADIO)
    if (gFmRadioMode)
    {
        BK1080_Mute(false);
    }
#endif
    if (!gEnableSpeaker)
    {
        LL_GPIO_ResetOutputPin(_AMP_PORT, _AMP_PIN);
    }
    gVoxResumeCountdown = 80;
    gVoiceWriteIndex = 0;
    gVoiceReadIndex = 0;
}
