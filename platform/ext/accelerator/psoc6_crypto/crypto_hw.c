/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto_hw.h"


/*
 * \brief Initialize the H.W. crypto acceleration engine
 */

int crypto_hw_accelerator_init(void)
{
    /*
     * cy-mbedtls-acceleration
     *  AES:   CYHAL_CRYPTO_COMMON
     *  ECDSA: CYHAL_CRYPTO_VU
     *  ECP:   CYHAL_CRYPTO_VU
     *  SHA:   CYHAL_CRYPTO_COMMON
     *
     * Each algorithm will call cy_hw_crypto_reserve() with the
     * base address of the crypto component that it uses.
     *
     */

    /* Nothing to do here */
    return 0;
}
