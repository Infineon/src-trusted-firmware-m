/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include "cy_p64_psacrypto.h"
#include "tfm_attest_hal.h"
#include "tfm_memory_utils.h"
#include "tfm_plat_boot_seed.h"
#include "tfm_plat_device_id.h"
#include "utils/cy_policy.h"


#ifndef CY_ATTEST_DETAILS_FROM_POLICY
/* Verification service URL for initial attestation token */
static const char verification_service_url[] = "www.trustedfirmware.org";

/* Profile definition document for initial attestation token */
static const char attestation_profile_definition[] = "PSA_IOT_PROFILE_1";

static const uint8_t implementation_id[] = {
    170, 170, 170, 170, 170, 170, 170, 170,
    187, 187, 187, 187, 187, 187, 187, 187,
    204, 204, 204, 204, 204, 204, 204, 204,
    221, 221, 221, 221, 221, 221, 221, 221};
#endif

#define CY_SROM_OPCODE_GET_SILICON_ID      (0x00000100UL)    /* The SROM API opcode for Get Silicon ID operation */
#define CY_SROM_LC_OFFSET_Pos              (20UL)            /* A fuse byte offset position in an opcode */
#define CY_SROM_LC_DATA_Msk                (0xFUL)           /* The mask for extracting data from the SROM API return value */

typedef enum cy_en_lifecycle {
    CY_LC_VIRGIN = 0U,
    CY_LC_NORMAL = 1U,
    CY_LC_SECURE_WITH_DEBUG = 2U,
    CY_LC_SECURE = 3U,
    CY_LC_RMA = 4U
} cy_en_lifecycle_t;

enum tfm_security_lifecycle_t tfm_attest_hal_get_security_lifecycle(void)
{
    enum tfm_security_lifecycle_t tfm_lifecycle = TFM_SLC_UNKNOWN;
    cy_p64_error_codes_t status = CY_P64_INVALID;

    /* Prepare opcode before calling the SROM API */
    uint32_t opcode = CY_SROM_OPCODE_GET_SILICON_ID | CY_P64_SYSCALL_DIRECT_PARAMS;

    status = cy_p64_syscall(&opcode);

    /* The result value of the SROM API call is returned to the status variable */
    if ((status & CY_P64_SYSCALL_MASK) == CY_P64_SUCCESS) {
        cy_en_lifecycle_t my_lc = (cy_en_lifecycle_t)((status >> CY_SROM_LC_OFFSET_Pos) & CY_SROM_LC_DATA_Msk);

        switch (my_lc) {
            case CY_LC_VIRGIN:
                tfm_lifecycle = TFM_SLC_ASSEMBLY_AND_TEST;
                break;
            case CY_LC_NORMAL:
                tfm_lifecycle = TFM_SLC_PSA_ROT_PROVISIONING;
                break;
            case CY_LC_SECURE_WITH_DEBUG:
                tfm_lifecycle = TFM_SLC_RECOVERABLE_PSA_ROT_DEBUG;
                break;
            case CY_LC_SECURE:
                tfm_lifecycle = TFM_SLC_SECURED;
                break;
            case CY_LC_RMA:
                tfm_lifecycle = TFM_SLC_DECOMMISSIONED;
                break;
            default:
                break;
        }
    }

    return tfm_lifecycle;
}

const char *
tfm_attest_hal_get_verification_service(uint32_t *size)
{
#ifdef CY_ATTEST_DETAILS_FROM_POLICY
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket = NULL;
    const cy_p64_cJSON *json;
    const char *url;
    enum tfm_plat_err_t err;

    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return NULL;
    }

    /* Get verification service url from the policy*/
    json = cy_p64_find_json_item(CY_ATT_DATA_URL_PATH, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_string(json, &url);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        return NULL;
    }
#else
    const char *url = verification_service_url;
#endif

    if (size) {
        *size = strlen(url);
    }
    return url;
}

const char *
tfm_attest_hal_get_profile_definition(uint32_t *size)
{
#ifdef CY_ATTEST_DETAILS_FROM_POLICY
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket = NULL;
    const cy_p64_cJSON *json;
    const char *profile;
    enum tfm_plat_err_t err;

    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return NULL;
    }

    /* Get profile definition from the policy*/
    json = cy_p64_find_json_item(CY_ATT_DATA_PROFILE_PATH, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_string(json, &profile);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        return NULL;
    }
#else
    const char *profile = attestation_profile_definition;
#endif

    if (size) {
        *size = strlen(profile);
    }
    return profile;
}

/*
 * For the sake of TF-M test (e.g.: ConfigRegressionIPCTfmLevel2.cmake),
 * using fixed testing values when SERVICES_TEST_S is defined
 */
#if !defined(SERVICES_TEST_S)
static bool boot_seed_generated_flag = false;
static uint8_t boot_seed[BOOT_SEED_SIZE] = {0};
enum tfm_plat_err_t tfm_plat_get_boot_seed(uint32_t size, uint8_t *buf)
{
    cy_p64_psa_status_t psa_rc;

    if ((size > BOOT_SEED_SIZE) || (!buf)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }
    /* Do not generate boot seed again */
    if (!boot_seed_generated_flag) {
        psa_rc = cy_p64_psa_generate_random(boot_seed, size);
        if (psa_rc != CY_P64_PSA_SUCCESS) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        boot_seed_generated_flag = true;
    }

    tfm_memcpy(buf, boot_seed, size);

    return TFM_PLAT_ERR_SUCCESS;
}
#endif  /* !SERVICES_TEST_S */

enum tfm_plat_err_t tfm_plat_get_implementation_id(uint32_t *size,
                                                   uint8_t  *buf)
{
#ifdef CY_ATTEST_DETAILS_FROM_POLICY
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket;
    const cy_p64_cJSON *json;
    uint32_t olen;
    enum tfm_plat_err_t err;

    /* Make sure the pointers are valid */
    if ((!size) || (!buf)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Get implementation ID from the policy*/
    json = cy_p64_find_json_item(CY_ATT_IMPL_ID_PATH, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_array_uint8(json, buf, *size, &olen);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    *size = olen;

    return TFM_PLAT_ERR_SUCCESS;
#else
    const uint8_t *p_impl_id = implementation_id;
    uint32_t impl_id_size = sizeof(implementation_id);

    /* Make sure the pointers are valid */
    if ((!size) || (!buf)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    if (*size < impl_id_size) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    *size = impl_id_size;

    for (uint32_t i = impl_id_size; i > 0; i--) {
        *buf = *p_impl_id;
        buf++;
        p_impl_id++;
    }

    return TFM_PLAT_ERR_SUCCESS;
#endif /* CY_ATTEST_DETAILS_FROM_POLICY */
}

enum tfm_plat_err_t tfm_plat_get_hw_version(uint32_t *size, uint8_t *buf)
{
#ifdef CY_HW_VERSION_FROM_POLICY
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket;
    const cy_p64_cJSON *json;
    const char *hw_version;
    enum tfm_plat_err_t err;
#else
    const char hw_version[] = "060456527282910010";
#endif
    uint32_t hw_version_len;

    /* Make sure the pointers are valid */
    if ((!size) || (!buf)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

#ifdef CY_HW_VERSION_FROM_POLICY
    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Get HW version from the policy*/
    json = cy_p64_find_json_item(CY_ATT_HW_VERSION_PATH, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_string(json, &hw_version);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
#endif

    hw_version_len = (uint32_t)strlen(hw_version);

    if(*size < hw_version_len) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    *size = hw_version_len;
    tfm_memcpy(buf, hw_version, *size);

    return TFM_PLAT_ERR_SUCCESS;
}
