/*
 * Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 * Copyright (c) 2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CRYPTO_KEYS_H
#define CRYPTO_KEYS_H

#include <stdint.h>

/* ECC key macros for SFB ES100, which is based on mbed TLS 2.20.0 */
#define FB_PSA_ECC_CURVE_SECP256R1_V1                    ((uint16_t) 0x0017)

#endif /* CRYPTO_KEYS_H */
