/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "psa/crypto.h"
#include "tfm_crypto_vendor_key_hal_api.h"
#include "tfm_memory_utils.h"

#include "crypto_keys.h"
#include "cy_p64_psacrypto.h"
#include "cy_p64_psa_mappings.h"
#include "cy_p64_syscalls.h"
#include "cy_p64_psacrypto_values.h"
#include "static_checks.h"

#define FB_PSOC64_KEY_ID_2_SLOT(id)     (cy_p64_key_slot_t)(id - PSA_KEY_ID_VENDOR_MIN + 1)
#define FB_PSOC64_KEY_SLOT_2_ID(slot)   (PSA_KEY_ID_VENDOR_MIN + (slot) - 1)


/*
 * Key ID mapping between MBedCrypto & FB
 * mbedcrypto_key_id is in the first field as it's the key to be searched
 */
typedef struct key_id_mapping_table_s {
    psa_key_id_t    mbedcrypto_key_id;
    psa_key_id_t    fb_key_id;
    bool            in_use;
} key_id_mapping_table_t;

static key_id_mapping_table_t key_id_mapping_table[CY_P64_KEY_SLOT_STATIC_MAX];

static psa_status_t remap_to_psa_error_code (cy_p64_psa_status_t code)
{
    switch (code) {
        case CY_P64_PSA_SUCCESS:
            return PSA_SUCCESS;
        case CY_P64_PSA_ERROR_NOT_SUPPORTED:
            return PSA_ERROR_NOT_SUPPORTED;
        case CY_P64_PSA_ERROR_NOT_PERMITTED:
            return PSA_ERROR_NOT_PERMITTED;
        case CY_P64_PSA_ERROR_BUFFER_TOO_SMALL:
            return PSA_ERROR_BUFFER_TOO_SMALL;
        case CY_P64_PSA_ERROR_ALREADY_EXISTS:
            return PSA_ERROR_ALREADY_EXISTS;
        case CY_P64_PSA_ERROR_DOES_NOT_EXIST:
            return PSA_ERROR_DOES_NOT_EXIST;
        case CY_P64_PSA_ERROR_BAD_STATE:
            return PSA_ERROR_BAD_STATE;
        case CY_P64_PSA_ERROR_INVALID_ARGUMENT:
            return PSA_ERROR_INVALID_ARGUMENT;
        case CY_P64_PSA_ERROR_INSUFFICIENT_MEMORY:
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        case CY_P64_PSA_ERROR_INSUFFICIENT_STORAGE:
            return PSA_ERROR_INSUFFICIENT_STORAGE;
        case CY_P64_PSA_ERROR_COMMUNICATION_FAILURE:
            return PSA_ERROR_COMMUNICATION_FAILURE;
        case CY_P64_PSA_ERROR_STORAGE_FAILURE:
            return PSA_ERROR_STORAGE_FAILURE;
        case CY_P64_PSA_ERROR_HARDWARE_FAILURE:
            return PSA_ERROR_HARDWARE_FAILURE;
        case CY_P64_PSA_ERROR_CORRUPTION_DETECTED:
            return PSA_ERROR_CORRUPTION_DETECTED;
        case CY_P64_PSA_ERROR_INSUFFICIENT_ENTROPY:
            return PSA_ERROR_INSUFFICIENT_ENTROPY;
        case CY_P64_PSA_ERROR_INVALID_SIGNATURE:
            return PSA_ERROR_INVALID_SIGNATURE;
        case CY_P64_PSA_ERROR_INVALID_PADDING:
            return PSA_ERROR_INVALID_PADDING;
        case CY_P64_PSA_ERROR_INSUFFICIENT_DATA:
            return PSA_ERROR_INSUFFICIENT_DATA;
        case CY_P64_PSA_ERROR_INVALID_HANDLE:
            return PSA_ERROR_INVALID_HANDLE;
        case CY_P64_PSA_ERROR_GENERIC_ERROR:
        default:
            return PSA_ERROR_GENERIC_ERROR;
    }
}

static psa_key_type_t cy_psa_key_type_map(cy_p64_psa_key_type_t fb_key_type)
{
    psa_key_type_t key_type = PSA_KEY_TYPE_NONE;

    if ( sizeof(cy_p64_psa_key_type_t) == sizeof(psa_key_type_t))
    {
        /* no conversion needed if the size of key type is identical */
        key_type = fb_key_type;
    } else {
        
        TFM_COVERITY_DEVIATE_LINE(MISRA_C_2012_Rule_14_3, "In the common case these types can be compatible but not equal")
        uint8_t ecc_type = CY_P64_PSA_KEY_TYPE_ECC_GET_FAMILY(fb_key_type);

        /* Only secp256r1 is valid */
        if (ecc_type == CY_P64_PSA_ECC_FAMILY_SECP_R1)
        {
            if (CY_P64_PSA_KEY_TYPE_IS_ECC_KEY_PAIR(fb_key_type))
            {
                /* Elliptic curve key pair */
                key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(FB_PSA_ECC_CURVE_SECP256R1_V1);
            }
            else if (CY_P64_PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(fb_key_type))
            {
                /* Elliptic curve public key */
                key_type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(FB_PSA_ECC_CURVE_SECP256R1_V1);
            }
        }
    }
    return key_type;
}

bool tfm_crypto_vendor_key_hal_is_valid_key_id(psa_key_id_t id)
{
    return ( (id >= PSA_KEY_ID_VENDOR_MIN) &&
             (id < (PSA_KEY_ID_VENDOR_MIN + CY_P64_KEY_SLOT_STATIC_MAX)) );
}

psa_key_id_t tfm_crypto_vendor_key_hal_get_device_key_id()
{
    return FB_PSOC64_KEY_SLOT_2_ID(CY_P64_KEY_SLOT_DEVICE_ECDSA);
}

psa_status_t tfm_crypto_vendor_key_hal_add_key_id_mapping(
    psa_key_id_t mbedcrypto_key_id,
    psa_key_id_t fb_key_id)
{
    uint32_t slot_idx;

    for (slot_idx = 0; slot_idx < CY_P64_KEY_SLOT_STATIC_MAX; slot_idx++) {
        key_id_mapping_table_t *slot = &key_id_mapping_table[slot_idx];
        if ( ! slot->in_use ) {
            slot->mbedcrypto_key_id = mbedcrypto_key_id;
            slot->fb_key_id         = fb_key_id;
            slot->in_use            = true;

            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_INSUFFICIENT_MEMORY;
}

psa_key_id_t tfm_crypto_vendor_key_hal_key_id_mapping_mbedcrypto_2_fb(
    psa_key_id_t mbedcrypto_key_id)
{
    uint32_t slot_idx;

    for (slot_idx = 0; slot_idx < CY_P64_KEY_SLOT_STATIC_MAX; slot_idx++) {
        key_id_mapping_table_t *slot = &key_id_mapping_table[slot_idx];
        if (slot->in_use && slot->mbedcrypto_key_id == mbedcrypto_key_id) {
            return slot->fb_key_id;
        }
    }

    return (psa_key_id_t)NULL;
}

bool tfm_crypto_vendor_key_hal_is_device_key_id(psa_key_id_t id)
{
    return (id == FB_PSOC64_KEY_SLOT_2_ID(CY_P64_KEY_SLOT_DEVICE_ECDSA));
}

static psa_status_t get_fb_key_slot(psa_key_id_t id, cy_p64_key_slot_t *fb_slot)
{
    if (tfm_crypto_vendor_key_hal_is_valid_key_id(id)) {
        *fb_slot = FB_PSOC64_KEY_ID_2_SLOT(id);
        return PSA_SUCCESS;
    }
    return PSA_ERROR_DOES_NOT_EXIST;
}

psa_status_t tfm_crypto_vendor_key_hal_get_key_metadata(
                                           psa_key_id_t id,
                                           psa_key_usage_t *usage,
                                           psa_algorithm_t *alg)
{
    cy_p64_key_slot_t fb_slot;
    psa_status_t status = get_fb_key_slot(id, &fb_slot);
    cy_p64_psa_status_t ret;
    cy_p64_psa_key_handle_t fb_handle;

    if (status == PSA_SUCCESS) {
        ret = cy_p64_keys_load_key_handle(fb_slot, &fb_handle);
        if (ret == CY_P64_PSA_SUCCESS) {
            cy_p64_psa_key_attributes_t attributes = CY_P64_PSA_KEY_ATTRIBUTES_INIT;
            ret = cy_p64_psa_get_key_attributes(fb_handle, &attributes);
            if (ret == CY_P64_PSA_SUCCESS) {
                *usage = cy_p64_psa_map_key_usage_cy2psa(
                    cy_p64_psa_get_key_usage_flags(&attributes) );
                *alg   = cy_p64_psa_map_alg_cy2psa(
                    cy_p64_psa_get_key_algorithm(&attributes) );
            }
        /* Note: here and below, cy_p64_keys_load_key_handle API doesn't
           require calling cy_p64_keys_close_key() for keys stored in flash
           (slots below CY_P64_KEY_SLOT_STATIC_MAX) */
        }
        status = remap_to_psa_error_code(ret);
    }
    return status;
}

psa_status_t tfm_crypto_vendor_key_hal_get_key_data(
                                       psa_key_id_t id,
                                       psa_key_type_t *key_type,
                                       size_t *key_bits,
                                       uint8_t *data,
                                       size_t *data_length)
{
    cy_p64_key_slot_t fb_slot;
    psa_status_t status = get_fb_key_slot(id, &fb_slot);
    cy_p64_psa_status_t ret;
    cy_p64_psa_key_handle_t fb_handle;

    if (status == PSA_SUCCESS) {
        ret = cy_p64_keys_load_key_handle(fb_slot, &fb_handle);
        if (ret == CY_P64_PSA_SUCCESS) {
            cy_p64_psa_key_attributes_t attributes = CY_P64_PSA_KEY_ATTRIBUTES_INIT;
            ret = cy_p64_psa_get_key_attributes(fb_handle, &attributes);
            if (ret == CY_P64_PSA_SUCCESS) {
                *key_type = cy_psa_key_type_map(
                                cy_p64_psa_get_key_type(&attributes));
                *key_bits = cy_p64_psa_get_key_bits(&attributes);

                /* Key type is equal to PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP256R1) for
                 * CY_P64_KEY_SLOT_DEVICE_ECDSA, but this function returns public key, so we should
                 * change key_type. It depends on the current version of TF-M that imports the key,
                 * but can only share the public part.
                 */
                if (fb_slot == CY_P64_KEY_SLOT_DEVICE_ECDSA) {
                    TFM_COVERITY_DEVIATE_LINE(cert_int31_c, "The key_type variable contains only static 16-bit values")
                    *key_type = PSA_KEY_TYPE_ECC_PUBLIC_KEY((*key_type) & PSA_KEY_TYPE_ECC_CURVE_MASK);
                }
                ret = cy_p64_psa_export_public_key(fb_handle, data, *data_length, data_length);
            }
        }
        status = remap_to_psa_error_code(ret);
    }
    return status;
}

psa_status_t tfm_crypto_vendor_key_hal_sign_hash(
                                   psa_key_id_t id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   uint8_t *signature,
                                   size_t signature_size,
                                   size_t *signature_length)
{
    cy_p64_psa_key_handle_t fb_handle;
    cy_p64_key_slot_t fb_slot;
    psa_status_t status = get_fb_key_slot(id, &fb_slot);
    cy_p64_psa_status_t ret;

    if( status == PSA_SUCCESS) {
        ret = cy_p64_keys_load_key_handle(fb_slot, &fb_handle);
        if (ret == CY_P64_PSA_SUCCESS) {
            ret = cy_p64_psa_sign_hash(fb_handle,
                                       cy_p64_psa_map_alg_psa2cy(alg),
                                       hash,
                                       hash_length,
                                       signature,
                                       signature_size,
                                       signature_length);
        }
        status = remap_to_psa_error_code(ret);
    }
    return status;
}

psa_status_t tfm_crypto_vendor_key_hal_verify_hash(
                                   psa_key_id_t id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length)
{
    cy_p64_psa_key_handle_t fb_handle;
    cy_p64_key_slot_t fb_slot = 0;
    psa_status_t status = get_fb_key_slot(id, &fb_slot);
    cy_p64_psa_status_t ret;

    if( status == PSA_SUCCESS) {
        ret = cy_p64_keys_load_key_handle(fb_slot, &fb_handle);
        if (ret == CY_P64_PSA_SUCCESS) {
            ret = cy_p64_psa_verify_hash(fb_handle,
                                              cy_p64_psa_map_alg_psa2cy(alg),
                                              hash,
                                              hash_length,
                                              signature,
                                              signature_length);
        }
        status = remap_to_psa_error_code(ret);
    }
    return status;
}

psa_status_t tfm_crypto_vendor_key_hal_export_public_key(
                                   psa_key_id_t id,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length)
{
    cy_p64_psa_key_handle_t fb_handle;
    cy_p64_key_slot_t fb_slot = 0;
    psa_status_t status = get_fb_key_slot(id, &fb_slot);
    cy_p64_psa_status_t ret;

    if (status == PSA_SUCCESS) {
        ret = cy_p64_keys_load_key_handle(fb_slot, &fb_handle);
        if (ret == CY_P64_PSA_SUCCESS) {
            ret = cy_p64_psa_export_public_key(
                fb_handle,
                data,
                data_size,
                data_length);
        }
        status = remap_to_psa_error_code(ret);
    }
    return status;
}
