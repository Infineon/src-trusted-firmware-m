/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis_compiler.h"
#include "service_api.h"
#include "tfm/tfm_core_svc.h"

__attribute__((naked))
int32_t tfm_core_get_boot_data(uint8_t major_type,
                               struct tfm_boot_data *boot_status,
                               uint32_t len)
{
#if !defined(__clang__)
    /* naked parameters, suppress compiler warnings */
    (void)major_type;
    (void)boot_status;
    (void)len;
#endif

    __ASM volatile(
        "SVC    %0\n"
        "BX     lr\n"
        : : "I" (TFM_SVC_GET_BOOT_DATA));
}
