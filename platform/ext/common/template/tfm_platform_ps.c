/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/protected_storage.h"

psa_status_t tfm_platform_ps_set(
    int32_t client_id,
    psa_storage_uid_t uid,
    uint32_t data_length,
    psa_storage_create_flags_t create_flags)
{
    (void)client_id;
    (void)uid;
    (void)data_length;
    (void)create_flags;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_platform_ps_get(
    int32_t client_id,
    psa_storage_uid_t uid,
    uint32_t data_offset,
    uint32_t data_length)
{
    (void)client_id;
    (void)uid;
    (void)data_offset;
    (void)data_length;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_platform_ps_get_info(
    int32_t client_id,
    psa_storage_uid_t uid,
    struct psa_storage_info_t *p_info)
{
    (void)client_id;
    (void)uid;
    (void)p_info;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_platform_ps_remove(
    int32_t client_id,
    psa_storage_uid_t uid)
{
    (void)client_id;
    (void)uid;

    return PSA_ERROR_NOT_SUPPORTED;
}
