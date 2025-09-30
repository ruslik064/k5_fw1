# /* Copyright (c) 2025 muzkr
#  * https://github.com/muzkr
#  *
#  * Licensed under the MIT License (the "License");
#  * you may not use this file except in compliance with the License.
#  * You may obtain a copy of the License at
#  *
#  *     https://mit-license.org/
#  *
#  *     Unless required by applicable law or agreed to in writing, software
#  *     distributed under the License is distributed on an "AS IS" BASIS,
#  *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  *     See the License for the specific language governing permissions and
#  *     limitations under the License.
#  *
#  */

# ----------------------------
# PY32_Drivers library
#

add_library(PY32_Drivers INTERFACE)

target_include_directories(PY32_Drivers SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/PY32F0xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_sources(PY32_Drivers INTERFACE
    Src/system_py32f0xx.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_utils.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_exti.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_gpio.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_adc.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_dma.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_crc.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_usart.c
)

# ---------------------------
# Core library  
#

add_library(Core INTERFACE)

target_include_directories(Core INTERFACE Inc)
target_compile_definitions(Core INTERFACE
	USE_FULL_LL_DRIVER 
	PY32F030x8 
    $<$<CONFIG:Debug>:DEBUG>
)

target_sources(Core INTERFACE
    Src/core_main.c
    Src/py32f0xx_it.c
    # Src/sysmem.c
    # Src/syscalls.c
    Src/startup_py32f030xx.s
)

target_link_libraries(Core INTERFACE PY32_Drivers ${TOOLCHAIN_LINK_LIBRARIES})
