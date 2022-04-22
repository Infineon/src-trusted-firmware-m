
/*******************************************************************************
* File Name: hw_poll.c
*
* Description:
* mbed TLS entropy poll callback function for Cypress TRNG.
*
********************************************************************************
* \copyright
* Copyright (c) 2018-2022 Cypress Semiconductor Corporation (an Infineon company)
* or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
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
*******************************************************************************/

#include <limits.h>

#include "cy_crypto_core_trng.h"
#include "crypto_common.h"

/* from psoc6hal: cyhal_trng_impl.h */
#define CYHAL_GARO31_INITSTATE          (0x04c11db7UL)
#define CYHAL_FIRO31_INITSTATE          (0x04c11db7UL)

int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen )
{
    (void)data; /* unused parameter */

    cy_cmgr_crypto_hw_t crypto_obj = CY_CMGR_CRYPTO_OBJ_INIT;
    cy_en_crypto_status_t status;
    uint32_t rand_num;

    if (!output || (len <= 0)) {
        return -1;
    }

    /* Reserve the crypto hardware for the operation */
    if (!cy_hw_crypto_reserve(&crypto_obj, CY_CMGR_CRYPTO_TRNG)) {
        return -1;
    }

    /* Cy_Crypto_Core_Trng() could generate maximum 32-bit data */
    status = Cy_Crypto_Core_Trng(crypto_obj.base,
        CYHAL_GARO31_INITSTATE, CYHAL_FIRO31_INITSTATE,
        sizeof(rand_num) * CHAR_BIT, &rand_num);

    /* Realease the crypto hardware */
    cy_hw_crypto_release(&crypto_obj);

    if (status != CY_CRYPTO_SUCCESS) {
        return -1;
    }

    len = (sizeof(rand_num) > len) ? len : sizeof(rand_num);
    mbedtls_memcpy(output, &rand_num, len);
    if (olen) {
        *olen = len;
    }

    return 0;
}
