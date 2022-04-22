/*
 * Copyright (c) 2017-2020 ARM Limited
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
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

#include "tfm_plat_crypto_keys.h"
#include <stddef.h>
#include "tfm_crypto_vendor_key_hal_api.h"

#include "crypto_keys.h"
#include "cy_p64_psacrypto.h"
#include "cy_p64_syscalls.h"


enum tfm_plat_err_t tfm_plat_get_huk_derived_key(const uint8_t *label,
                                                 size_t label_size,
                                                 const uint8_t *context,
                                                 size_t context_size,
                                                 uint8_t *key,
                                                 size_t key_size)
{
    cy_p64_psa_key_handle_t handle;
    cy_p64_psa_status_t ret = cy_p64_keys_load_key_handle(CY_P64_KEY_SLOT_DERIVE, &handle);

    if (ret != CY_P64_PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    cy_p64_psa_key_derivation_operation_t operation =
                CY_P64_PSA_KEY_DERIVATION_OPERATION_INIT;

    /* Setup key derivation */
    ret = cy_p64_psa_key_derivation_setup(&operation,
                CY_P64_PSA_ALG_HKDF(CY_P64_PSA_ALG_SHA_256));

    /* Inject salt to key derivation */
    if (ret == CY_P64_PSA_SUCCESS) {
        ret = cy_p64_psa_key_derivation_inp_bytes(&operation,
                CY_P64_PSA_KEY_DERIVATION_INPUT_SALT, context, context_size);
    }

    /* Inject key to key derivation */
    if (ret == CY_P64_PSA_SUCCESS) {
        ret = cy_p64_psa_key_derivation_inp_key(&operation,
                CY_P64_PSA_KEY_DERIVATION_INPUT_SECRET, handle);
    }

    /* Inject info to key derivation */
    if( ret == CY_P64_PSA_SUCCESS) {
        ret = cy_p64_psa_key_derivation_inp_bytes(&operation,
                CY_P64_PSA_KEY_DERIVATION_INPUT_INFO, label, label_size);
    }

    /* Read data from key derivation operation */
    if( ret == CY_P64_PSA_SUCCESS) {
        ret = cy_p64_psa_key_derivation_out_bytes(&operation,
                key, key_size);
    }

    /* Clean up the key derivation operation object */
    (void)cy_p64_psa_key_derivation_abort(&operation);

    /* Note: cy_p64_keys_load_key_handle API doesn't require calling
        cy_p64_keys_close_key() for keys stored in flash
        (slots below FB_PSOC64_MAX_KEY_SLOTS) */
    return (ret == CY_P64_PSA_SUCCESS) ? TFM_PLAT_ERR_SUCCESS :
                                         TFM_PLAT_ERR_SYSTEM_ERR;
}

enum tfm_plat_err_t
tfm_plat_open_initial_attest_key(
    psa_key_handle_t *handle,
    psa_ecc_curve_t *curve)
{
    psa_status_t status;

    status = psa_open_key(
        tfm_crypto_vendor_key_hal_get_device_key_id(),
        handle);
    if (status != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (curve) {
        *curve = CY_P64_PSA_ECC_CURVE_SECP256R1;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t
tfm_plat_close_initial_attest_key(psa_key_handle_t handle)
{
    psa_status_t status;

    status = psa_close_key(handle);
    if (status != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
