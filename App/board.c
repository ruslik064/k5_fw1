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
 *
 */

#include <string.h>
#include "app/dtmf.h"

#if defined(ENABLE_FMRADIO)
#include "app/fm.h"
#endif

#include "board.h"
#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_adc.h"

#if defined(ENABLE_FMRADIO)
#include "driver/bk1080.h"
#endif

#include "driver/bk4819.h"

#if defined(ENABLE_AIRCOPY) || defined(ENABLE_UART)
#include "driver/crc.h"
#endif

#include "driver/eeprom.h"
#include "driver/gpio.h"
#include "driver/system.h"
#include "driver/st7565.h"
#include "frequencies.h"
#include "helper/battery.h"
#include "misc.h"
#include "settings.h"

#if defined(ENABLE_OVERLAY)
#include "driver/flash.h"
#include "sram-overlay.h"
#endif

static const uint32_t gDefaultFrequencyTable[5] = {
    14502500,
    14552500,
    43477500,
    43502500,
    43697500,
};

#if defined(ENABLE_OVERLAY)
void BOARD_FLASH_Init(void)
{
    FLASH_Init(FLASH_READ_MODE_1_CYCLE);
    FLASH_ConfigureTrimValues();
    SYSTEM_ConfigureClocks();
    overlay_FLASH_MainClock = 48000000;
    overlay_FLASH_ClockMultiplier = 48;
    FLASH_Init(FLASH_READ_MODE_2_CYCLE);
}
#endif

void BOARD_GPIO_Init(void)
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA | LL_IOP_GRP1_PERIPH_GPIOB | LL_IOP_GRP1_PERIPH_GPIOF);

    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

    // ------------------

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15   //
                          | LL_GPIO_PIN_13 //
                          | LL_GPIO_PIN_12 //
                          | LL_GPIO_PIN_11 //
                          | LL_GPIO_PIN_10 //
                          | LL_GPIO_PIN_9  //
                          | LL_GPIO_PIN_8  //
                          | LL_GPIO_PIN_7  //
                          | LL_GPIO_PIN_6  //
                          | LL_GPIO_PIN_5  //
        ;

    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ------------------

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7   //
                          | LL_GPIO_PIN_6 //
                          | LL_GPIO_PIN_5 //
                          | LL_GPIO_PIN_4 //
                          | LL_GPIO_PIN_3 //
                          | LL_GPIO_PIN_0 //
        ;

    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // ------------------

    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1   //
                          | LL_GPIO_PIN_0 //
        ;

    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ------------------

    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1   //
                          | LL_GPIO_PIN_0 //
        ;

    LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    // ------------------

    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9 | LL_GPIO_PIN_10);

#if defined(ENABLE_FMRADIO)
    // GPIO_SetBit(&GPIOB->DATA, GPIOB_PIN_BK1080);
    // TODO: 
#endif
}

void BOARD_ADC_Init(void)
{

    LL_ADC_Reset(ADC1);
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

    LL_ADC_StartCalibration(ADC1);
    while (LL_ADC_IsCalibrationOnGoing(ADC1))
    {
        SYSTEM_DelayMs(10);
    }

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);

    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

    LL_ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_AUTOWAIT;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_28CYCLES_5);
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
}

void BOARD_ADC_GetBatteryInfo(uint16_t *pVoltage, uint16_t *pCurrent)
{
    static const uint32_t CHANNELS[2] = {LL_ADC_CHANNEL_4, LL_ADC_CHANNEL_9};

    uint16_t Data[2];
    for (int i = 0; i < 2; i++)
    {
        LL_ADC_REG_SetSequencerChannels(ADC1, CHANNELS[i]);
        LL_ADC_Enable(ADC1);
        SYSTEM_DelayMs(1);
        LL_ADC_REG_StartConversion(ADC1);
        while (!LL_ADC_IsActiveFlag_EOC(ADC1))
            ;
        LL_ADC_ClearFlag_EOC(ADC1);
        Data[i] = LL_ADC_REG_ReadConversionData12(ADC1);
    }

    *pVoltage = Data[0];
    *pCurrent = Data[1];
}

void BOARD_Init(void)
{
    BOARD_GPIO_Init();
    BOARD_ADC_Init();
    ST7565_Init();
#if defined(ENABLE_FMRADIO)
    BK1080_Init(0, false);
#endif
#if defined(ENABLE_AIRCOPY) || defined(ENABLE_UART)
    CRC_Init();
#endif
}

void BOARD_EEPROM_Init(void)
{
    uint8_t Data[16];
    uint8_t i;

    memset(Data, 0, sizeof(Data));

    // 0E70..0E77
    EEPROM_ReadBuffer(0x0E70, Data, 8);
    gEeprom.CHAN_1_CALL = IS_MR_CHANNEL(Data[0]) ? Data[0] : MR_CHANNEL_FIRST;
    gEeprom.SQUELCH_LEVEL = (Data[1] < 10) ? Data[1] : 4;
    gEeprom.TX_TIMEOUT_TIMER = (Data[2] < 11) ? Data[2] : 2;
    gEeprom.NOAA_AUTO_SCAN = (Data[3] < 2) ? Data[3] : true;
    gEeprom.KEY_LOCK = (Data[4] < 2) ? Data[4] : false;
    gEeprom.VOX_SWITCH = (Data[5] < 2) ? Data[5] : false;
    gEeprom.VOX_LEVEL = (Data[6] < 10) ? Data[6] : 5;
    gEeprom.MIC_SENSITIVITY = (Data[7] < 5) ? Data[7] : 2;

    // 0E78..0E7F
    EEPROM_ReadBuffer(0x0E78, Data, 8);
    gEeprom.CHANNEL_DISPLAY_MODE = (Data[1] < 3) ? Data[1] : MDF_FREQUENCY;
    gEeprom.CROSS_BAND_RX_TX = (Data[2] < 3) ? Data[2] : CROSS_BAND_OFF;
    gEeprom.BATTERY_SAVE = (Data[3] < 5) ? Data[3] : 4;
    gEeprom.DUAL_WATCH = (Data[4] < 3) ? Data[4] : DUAL_WATCH_CHAN_A;
    gEeprom.BACKLIGHT = (Data[5] <= BACKLIGHT_TIMEOUT_MAX) ? Data[5] : BACKLIGHT_TIMEOUT_MAX;
    gEeprom.TAIL_NOTE_ELIMINATION = (Data[6] < 2) ? Data[6] : true;
    gEeprom.VFO_OPEN = (Data[7] < 2) ? Data[7] : true;

    // 0E80..0E87
    EEPROM_ReadBuffer(0x0E80, Data, 8);
    gEeprom.ScreenChannel[0] = IS_VALID_CHANNEL(Data[0]) ? Data[0] : (FREQ_CHANNEL_FIRST + BAND6_400MHz);
    gEeprom.ScreenChannel[1] = IS_VALID_CHANNEL(Data[3]) ? Data[3] : (FREQ_CHANNEL_FIRST + BAND6_400MHz);
    gEeprom.MrChannel[0] = IS_MR_CHANNEL(Data[1]) ? Data[1] : MR_CHANNEL_FIRST;
    gEeprom.MrChannel[1] = IS_MR_CHANNEL(Data[4]) ? Data[4] : MR_CHANNEL_FIRST;
    gEeprom.FreqChannel[0] = IS_FREQ_CHANNEL(Data[2]) ? Data[2] : (FREQ_CHANNEL_FIRST + BAND6_400MHz);
    gEeprom.FreqChannel[1] = IS_FREQ_CHANNEL(Data[5]) ? Data[5] : (FREQ_CHANNEL_FIRST + BAND6_400MHz);
#if defined(ENABLE_NOAA)
    gEeprom.NoaaChannel[0] = IS_NOAA_CHANNEL(Data[6]) ? Data[6] : NOAA_CHANNEL_FIRST;
    gEeprom.NoaaChannel[1] = IS_NOAA_CHANNEL(Data[7]) ? Data[7] : NOAA_CHANNEL_FIRST;
#endif

#if defined(ENABLE_FMRADIO)
    // 0E88..0E8F
    struct
    {
        uint16_t SelectedFrequency;
        uint8_t SelectedChannel;
        uint8_t IsMrMode;
        uint8_t Padding[8];
    } FM;

    EEPROM_ReadBuffer(0x0E88, &FM, 8);
    gEeprom.FM_LowerLimit = 760;
    gEeprom.FM_UpperLimit = 1080;
    if (FM.SelectedFrequency < gEeprom.FM_LowerLimit || FM.SelectedFrequency > gEeprom.FM_UpperLimit)
    {
        gEeprom.FM_SelectedFrequency = 760;
    }
    else
    {
        gEeprom.FM_SelectedFrequency = FM.SelectedFrequency;
    }

    gEeprom.FM_SelectedChannel = FM.SelectedChannel;
    gEeprom.FM_IsMrMode = (FM.IsMrMode < 2) ? FM.IsMrMode : false;

    // 0E40..0E67
    EEPROM_ReadBuffer(0x0E40, gFM_Channels, sizeof(gFM_Channels));
    FM_ConfigureChannelState();
#endif

    // 0E90..0E97
    EEPROM_ReadBuffer(0x0E90, Data, 8);
    gEeprom.BEEP_CONTROL = (Data[0] < 2) ? Data[0] : true;
    gEeprom.KEY_1_SHORT_PRESS_ACTION = (Data[1] < 9) ? Data[1] : 3;
    gEeprom.KEY_1_LONG_PRESS_ACTION = (Data[2] < 9) ? Data[2] : 8;
    gEeprom.KEY_2_SHORT_PRESS_ACTION = (Data[3] < 9) ? Data[3] : 1;
    gEeprom.KEY_2_LONG_PRESS_ACTION = (Data[4] < 9) ? Data[4] : 6;
    gEeprom.SCAN_RESUME_MODE = (Data[5] < 3) ? Data[5] : SCAN_RESUME_CO;
    gEeprom.AUTO_KEYPAD_LOCK = (Data[6] < 2) ? Data[6] : true;
    gEeprom.POWER_ON_DISPLAY_MODE = (Data[7] < 3) ? Data[7] : POWER_ON_DISPLAY_MODE_VOLTAGE;

    // 0E98..0E9F
    EEPROM_ReadBuffer(0x0E98, Data, 8);
    memcpy(&gEeprom.POWER_ON_PASSWORD, Data, 4);

    // 0EA0..0EA7
    EEPROM_ReadBuffer(0x0EA0, Data, 8);
    gEeprom.VOICE_PROMPT = (Data[0] < 3) ? Data[0] : VOICE_PROMPT_CHINESE;

    // 0EA8..0EAF
    EEPROM_ReadBuffer(0x0EA8, Data, 8);
#if defined(ENABLE_ALARM)
    gEeprom.ALARM_MODE = (Data[0] < 2) ? Data[0] : true;
#endif
    gEeprom.ROGER = (Data[1] < 3) ? Data[1] : ROGER_MODE_OFF;
    gEeprom.REPEATER_TAIL_TONE_ELIMINATION = (Data[2] < 11) ? Data[2] : 0;
    gEeprom.TX_VFO = (Data[3] < 2) ? Data[3] : 0;

    // 0ED0..0ED7
    EEPROM_ReadBuffer(0x0ED0, Data, 8);
    gEeprom.DTMF_SIDE_TONE = (Data[0] < 2) ? Data[0] : true;
    gEeprom.DTMF_SEPARATE_CODE = DTMF_ValidateCodes((char *)(Data + 1), 1) ? Data[1] : '*';
    gEeprom.DTMF_GROUP_CALL_CODE = DTMF_ValidateCodes((char *)(Data + 2), 1) ? Data[2] : '#';
    gEeprom.DTMF_DECODE_RESPONSE = (Data[3] < 4) ? Data[3] : 0;
    gEeprom.DTMF_AUTO_RESET_TIME = (Data[4] < 61) ? Data[4] : 5;
    gEeprom.DTMF_PRELOAD_TIME = (Data[5] < 101) ? Data[5] * 10 : 300;
    gEeprom.DTMF_FIRST_CODE_PERSIST_TIME = (Data[6] < 101) ? Data[6] * 10 : 100;
    gEeprom.DTMF_HASH_CODE_PERSIST_TIME = (Data[7] < 101) ? Data[7] * 10 : 100;

    // 0ED8..0EDF
    EEPROM_ReadBuffer(0x0ED8, Data, 8);
    gEeprom.DTMF_CODE_PERSIST_TIME = (Data[0] < 101) ? Data[0] * 10 : 100;
    gEeprom.DTMF_CODE_INTERVAL_TIME = (Data[1] < 101) ? Data[1] * 10 : 100;
    gEeprom.PERMIT_REMOTE_KILL = (Data[2] < 2) ? Data[2] : true;

    // 0EE0..0EE7
    EEPROM_ReadBuffer(0x0EE0, Data, 8);
    if (DTMF_ValidateCodes((char *)Data, 8))
    {
        memcpy(gEeprom.ANI_DTMF_ID, Data, 8);
    }
    else
    {
        // Original firmware overflows into the next string
        memcpy(gEeprom.ANI_DTMF_ID, "123\0\0\0\0", 8);
    }

    // 0EE8..0EEF
    EEPROM_ReadBuffer(0x0EE8, Data, 8);
    if (DTMF_ValidateCodes((char *)Data, 8))
    {
        memcpy(gEeprom.KILL_CODE, Data, 8);
    }
    else
    {
        memcpy(gEeprom.KILL_CODE, "ABCD9\0\0", 8);
    }

    // 0EF0..0EF7
    EEPROM_ReadBuffer(0x0EF0, Data, 8);
    if (DTMF_ValidateCodes((char *)Data, 8))
    {
        memcpy(gEeprom.REVIVE_CODE, Data, 8);
    }
    else
    {
        memcpy(gEeprom.REVIVE_CODE, "9DCBA\0\0", 8);
    }

    // 0EF8..0F07
    EEPROM_ReadBuffer(0x0EF8, Data, 16);
    if (DTMF_ValidateCodes((char *)Data, 16))
    {
        memcpy(gEeprom.DTMF_UP_CODE, Data, 16);
    }
    else
    {
        memcpy(gEeprom.DTMF_UP_CODE, "12345\0\0\0\0\0\0\0\0\0\0", 16);
    }

    // 0F08..0F17
    EEPROM_ReadBuffer(0x0F08, Data, 16);
    if (DTMF_ValidateCodes((char *)Data, 16))
    {
        memcpy(gEeprom.DTMF_DOWN_CODE, Data, 16);
    }
    else
    {
        memcpy(gEeprom.DTMF_DOWN_CODE, "54321\0\0\0\0\0\0\0\0\0\0", 16);
    }

    // 0F18..0F1F
    EEPROM_ReadBuffer(0x0F18, Data, 8);

    gEeprom.SCAN_LIST_DEFAULT = (Data[0] < 2) ? Data[0] : false;

    for (i = 0; i < 2; i++)
    {
        uint8_t j = (i * 3) + 1;
        gEeprom.SCAN_LIST_ENABLED[i] = (Data[j] < 2) ? Data[j] : false;
        gEeprom.SCANLIST_PRIORITY_CH1[i] = Data[j + 1];
        gEeprom.SCANLIST_PRIORITY_CH2[i] = Data[j + 2];
    }

    // 0F40..0F47
    EEPROM_ReadBuffer(0x0F40, Data, 8);
    gSetting_F_LOCK = (Data[0] < 6) ? Data[0] : F_LOCK_OFF;

    gSetting_350TX = (Data[1] < 2) ? Data[1] : true;
    gSetting_KILLED = (Data[2] < 2) ? Data[2] : false;
    gSetting_200TX = (Data[3] < 2) ? Data[3] : false;
    gSetting_500TX = (Data[4] < 2) ? Data[4] : false;
    gSetting_350EN = (Data[5] < 2) ? Data[5] : true;
    gSetting_ScrambleEnable = (Data[6] < 2) ? Data[6] : true;

    if (!gEeprom.VFO_OPEN)
    {
        gEeprom.ScreenChannel[0] = gEeprom.MrChannel[0];
        gEeprom.ScreenChannel[1] = gEeprom.MrChannel[1];
    }

    // 0D60..0E27
    EEPROM_ReadBuffer(0x0D60, gMR_ChannelAttributes, sizeof(gMR_ChannelAttributes));

    // 0F30..0F3F
    EEPROM_ReadBuffer(0x0F30, gCustomAesKey, sizeof(gCustomAesKey));

    bHasCustomAesKey = false;
    for (i = 0; i < 4; i++)
    {
        if (gCustomAesKey[i] != 0xFFFFFFFFU)
        {
            bHasCustomAesKey = true;
            break;
        }
    }

}

void BOARD_EEPROM_LoadCalibration(void)
{
    uint8_t Mic;

    EEPROM_ReadBuffer(0x1EC0, gEEPROM_RSSI_CALIB[3], 8);
    memcpy(gEEPROM_RSSI_CALIB[4], gEEPROM_RSSI_CALIB[3], 8);
    memcpy(gEEPROM_RSSI_CALIB[5], gEEPROM_RSSI_CALIB[3], 8);
    memcpy(gEEPROM_RSSI_CALIB[6], gEEPROM_RSSI_CALIB[3], 8);

    EEPROM_ReadBuffer(0x1EC8, gEEPROM_RSSI_CALIB[0], 8);
    memcpy(gEEPROM_RSSI_CALIB[1], gEEPROM_RSSI_CALIB[0], 8);
    memcpy(gEEPROM_RSSI_CALIB[2], gEEPROM_RSSI_CALIB[0], 8);

    EEPROM_ReadBuffer(0x1F40, gBatteryCalibration, 12);
    if (gBatteryCalibration[0] >= 5000)
    {
        gBatteryCalibration[0] = 1900;
        gBatteryCalibration[1] = 2000;
    }
    gBatteryCalibration[5] = 2300;

    EEPROM_ReadBuffer(0x1F50 + (gEeprom.VOX_LEVEL * 2), &gEeprom.VOX1_THRESHOLD, 2);
    EEPROM_ReadBuffer(0x1F68 + (gEeprom.VOX_LEVEL * 2), &gEeprom.VOX0_THRESHOLD, 2);

    EEPROM_ReadBuffer(0x1F80 + gEeprom.MIC_SENSITIVITY, &Mic, 1);
    gEeprom.MIC_SENSITIVITY_TUNING = (Mic < 32) ? Mic : 15;

    struct
    {
        int16_t BK4819_XtalFreqLow;
        uint16_t EEPROM_1F8A;
        uint16_t EEPROM_1F8C;
        uint8_t VOLUME_GAIN;
        uint8_t DAC_GAIN;
    } Misc;

    EEPROM_ReadBuffer(0x1F88, &Misc, 8);
    gEeprom.BK4819_XTAL_FREQ_LOW = (Misc.BK4819_XtalFreqLow + 1000 < 2000) ? Misc.BK4819_XtalFreqLow : 0;

    gEEPROM_1F8A = Misc.EEPROM_1F8A & 0x01FF;
    gEEPROM_1F8C = Misc.EEPROM_1F8C & 0x01FF;

    gEeprom.VOLUME_GAIN = (Misc.VOLUME_GAIN < 64) ? Misc.VOLUME_GAIN : 58;
    gEeprom.DAC_GAIN = (Misc.DAC_GAIN < 16) ? Misc.DAC_GAIN : 8;

    BK4819_WriteRegister(BK4819_REG_3B, gEeprom.BK4819_XTAL_FREQ_LOW + 22656);
}

void BOARD_FactoryReset(bool bIsAll)
{
    uint8_t Template[8];
    uint16_t i;

    memset(Template, 0xFF, sizeof(Template));
    for (i = 0x0C80; i < 0x1E00; i += 8)
    {
        if (
            !(i >= 0x0EE0 && i < 0x0F18) &&             // ANI ID + DTMF codes
            !(i >= 0x0F30 && i < 0x0F50) &&             // AES KEY + F LOCK + Scramble Enable
            !(i >= 0x1C00 && i < 0x1E00) &&             // DTMF contacts
            !(i >= 0x0EB0 && i < 0x0ED0) &&             // Welcome strings
            !(i >= 0x0EA0 && i < 0x0EA8) &&             // Voice Prompt
            (bIsAll || (!(i >= 0x0D60 && i < 0x0E28) && // MR Channel Attributes
                        !(i >= 0x0F18 && i < 0x0F30) && // Scan List
                        !(i >= 0x0F50 && i < 0x1C00) && // MR Channel NAmes
                        !(i >= 0x0E40 && i < 0x0E70) && // FM Channels
                        !(i >= 0x0E88 && i < 0x0E90)))  // FM settings
        )
        {
            EEPROM_WriteBuffer(i, Template);
        }
    }
    if (bIsAll)
    {
        RADIO_InitInfo(gRxVfo, FREQ_CHANNEL_FIRST + 5, 5, 41002500);
        for (i = 0; i < 5; i++)
        {
            const uint32_t Frequency = gDefaultFrequencyTable[i];

            gRxVfo->ConfigRX.Frequency = Frequency;
            gRxVfo->ConfigTX.Frequency = Frequency;
            gRxVfo->Band = FREQUENCY_GetBand(Frequency);
            SETTINGS_SaveChannel(MR_CHANNEL_FIRST + i, 0, gRxVfo, 2);
        }
    }
}
