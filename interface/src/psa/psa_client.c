/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "tfm/tfm_core_svc.h"
#include "psa/client.h"
#include "tfm_api.h"

__attribute__((naked))
uint32_t psa_framework_version(void)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_FRAMEWORK_VERSION));
}

__attribute__((naked))
uint32_t psa_version(uint32_t sid)
{
#if !defined(__clang__)
    /* naked parameter, suppress compiler warning */
    (void)sid;
#endif

    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_VERSION));
}

__attribute__((naked))
psa_handle_t psa_connect(uint32_t sid, uint32_t version)
{
#if !defined(__clang__)
    /* naked parameters, suppress compiler warnings */
    (void)sid;
    (void)version;
#endif

    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_CONNECT));
}

__attribute__((naked))
static psa_status_t psa_call_param_pack(psa_handle_t handle,
                                   struct tfm_control_parameter_t *ctrl_param,
                                   const psa_invec *in_vec, psa_outvec *out_vec)
{
#if !defined(__clang__)
    /* naked parameters, suppress compiler warnings */
    (void)handle;
    (void)ctrl_param;
    (void)in_vec;
    (void)out_vec;
#endif

    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_CALL));
}

psa_status_t psa_call(psa_handle_t handle,
                      int32_t type,
                      const psa_invec *in_vec,
                      size_t in_len,
                      psa_outvec *out_vec,
                      size_t out_len)
{
    struct tfm_control_parameter_t ctrl_param;
    ctrl_param.type = type;
    ctrl_param.in_len = in_len;
    ctrl_param.out_len = out_len;

    return psa_call_param_pack(handle, &ctrl_param, in_vec, out_vec);
}

__attribute__((naked))
void psa_close(psa_handle_t handle)
{
#if !defined(__clang__)
    /* naked parameter, suppress compiler warning */
    (void)handle;
#endif

    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_CLOSE));
}
