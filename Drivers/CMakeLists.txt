#    Copyright 2025 muzkr
#
#         https://github.com/muzkr
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

# ---------------------------
#  CMSIS
#

add_library(CMSIS INTERFACE)
target_include_directories(CMSIS SYSTEM INTERFACE 
    CMSIS/Include
)


# ----------------------------
# PY32_Drivers
#

add_library(PY32_Drivers INTERFACE)

target_include_directories(PY32_Drivers SYSTEM INTERFACE
    PY32F0xx_HAL_Driver/Inc
    CMSIS/Device/PY32F0xx/Include
)

target_sources(PY32_Drivers INTERFACE
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_utils.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_exti.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_gpio.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_adc.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_dma.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_rcc.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_crc.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_usart.c
    PY32F0xx_HAL_Driver/Src/py32f0xx_ll_tim.c
)

target_compile_definitions(PY32_Drivers INTERFACE
	USE_FULL_LL_DRIVER 
	PY32F030x8 
)

target_link_libraries(PY32_Drivers INTERFACE CMSIS)

# ---------------------------
# DP32 BSP
#

add_library(DP32_BSP INTERFACE)
target_include_directories(DP32_BSP SYSTEM INTERFACE 
    ./DP32_BSP 
    CMSIS/Device/ARM/ARMCM0/Include
)
target_link_libraries(DP32_BSP INTERFACE CMSIS)
