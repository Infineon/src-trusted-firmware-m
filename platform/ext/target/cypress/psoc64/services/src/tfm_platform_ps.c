/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cy_p64_syscalls.h"
#include "cy_p64_psacrypto.h"
#include "psa/protected_storage.h"
#include "tfm_platform_ps.h"
#include "tfm_ps_defs.h"
#include "tfm_ps_req_mngr.h"
#include "static_checks.h"

#define CY_FB_CERTIFICATE_POLICY_ID_MIN 0x200
#define CY_FB_CERTIFICATE_POLICY_ID_MAX 0x2FF

#define IS_CY_FB_CERTIFICATE_ID(x) (((x) >= CY_FB_CERTIFICATE_POLICY_ID_MIN) \
                                    && ((x) <= CY_FB_CERTIFICATE_POLICY_ID_MAX))

static cy_p64_psa_key_handle_t uid_to_cert_id(uint32_t uid)
{
    TFM_COVERITY_BLOCK(TFM_COVERITY_DEVIATE(cert_int31_c, "uid limits \
                                are checked in is_tfm_ps_oem_uid() function") \
                       TFM_COVERITY_FP(cert_int30_c, "uid limits \
                                are checked in is_tfm_ps_oem_uid() function"))
    return (CY_FB_CERTIFICATE_POLICY_ID_MIN + (uid - TFM_PS_OEM_UID_MIN));
    TFM_COVERITY_BLOCK_END(cert_int31_c cert_int30_c)
}

static psa_status_t platform_get_certificate(
    uint32_t uid,
    char **cert,
    uint32_t *cert_length)
{
    cy_p64_psa_key_handle_t fb_cert_policy_id;
    cy_p64_error_codes_t rc;

    fb_cert_policy_id = uid_to_cert_id(uid);
    if (! IS_CY_FB_CERTIFICATE_ID(fb_cert_policy_id)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    rc = cy_p64_get_provisioning_details(
        fb_cert_policy_id,
        cert,
        cert_length);

    if (rc != CY_P64_SUCCESS) {
        /* We don't expose detail information */
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_platform_ps_set(
    int32_t client_id,
    psa_storage_uid_t uid,
    uint32_t data_length,
    psa_storage_create_flags_t create_flags)
{
    (void)client_id; /* unused parameter */
    (void)uid; /* unused parameter */
    (void)data_length; /* unused parameter */
    (void)create_flags; /* unused parameter */

    return PSA_ERROR_NOT_PERMITTED;
}

psa_status_t tfm_platform_ps_get(
    int32_t client_id,
    psa_storage_uid_t uid,
    uint32_t data_offset,
    uint32_t data_size,
    size_t *p_data_length)
{
    (void)client_id; /* unused parameter */

    psa_status_t rc;
    char *cert;
    uint32_t cert_length;

    /* One should get full certificate, from the beginning, not part of it */
    if (data_offset) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* get the certificate */
    TFM_COVERITY_FP_LINE(cert_int31_c, "uid limits are checked in is_tfm_ps_oem_uid() function")
    rc = platform_get_certificate(uid, &cert, &cert_length);
    if (rc != PSA_SUCCESS) {
        return rc;
    }

    /* we have a valid certificate */

    /* sufficient buffer space ? */
    if (data_size < cert_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Copy the certificate to the output buffer */
    ps_req_mngr_write_asset_data((uint8_t *)cert, cert_length);

    if (p_data_length) {
        *p_data_length = cert_length;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_platform_ps_get_info(
    int32_t client_id,
    psa_storage_uid_t uid,
    struct psa_storage_info_t *p_info)
{
    (void)client_id; /* unused parameter */

    psa_status_t rc;
    char *cert;
    uint32_t cert_length;

    /* get the certificate */
    TFM_COVERITY_FP_LINE(cert_int31_c, "uid limits are checked in is_tfm_ps_oem_uid() function")
    rc = platform_get_certificate(uid, &cert, &cert_length);
    if (rc != PSA_SUCCESS) {
        return rc;
    }

    /* we have a valid certificate */

    /* Copy certificate info to the info struct */
    p_info->size = cert_length;
    p_info->flags = PSA_STORAGE_FLAG_WRITE_ONCE;

    return PSA_SUCCESS;
}

psa_status_t tfm_platform_ps_remove(
    int32_t client_id,
    psa_storage_uid_t uid)
{
    (void)client_id; /* unused parameter */
    (void)uid; /* unused parameter */

    return PSA_ERROR_NOT_PERMITTED;
}
