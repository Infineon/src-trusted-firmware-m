/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_platform_system.h"
#include "platform_description.h"

void tfm_platform_hal_system_reset(void)
{
    /* Reset the system */
    NVIC_SystemReset();
}

enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec  *in_vec,
                                               psa_outvec *out_vec)
{
    (void)request; /* unused parameter */
    (void)in_vec; /* unused parameter */
    (void)out_vec; /* unused parameter */

    /* SCC is configured as non-secure through PPC,
     * so this function is not needed on this platform
     */
    return TFM_PLATFORM_ERR_NOT_SUPPORTED;
}

