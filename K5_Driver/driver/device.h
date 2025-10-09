/* Copyright 2023 muzkr
 * https://github.com/muzkr
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

#ifndef _DRIVER_DEVICE_H
#define _DRIVER_DEVICE_H

#if defined(K5_V1)
#include "ARMCM0.h"
#include "system_ARMCM0.h"
#elif defined(K5_V2)
#include "py32f0xx.h"
#else
#error "Must define K5_Vx"
#endif

#endif // _DRIVER_DEVICE_H
