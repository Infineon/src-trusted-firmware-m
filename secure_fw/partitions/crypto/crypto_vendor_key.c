/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

/* FixMe: Use PSA_ERROR_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */
#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_crypto_vendor_key_api.h"
#include "tfm_crypto_vendor_key_hal_api.h"

#define ECC256_KEY_SIZE (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(256) + 3)

/*!
 * \defgroup public Public functions
 *
 */

static bool tfm_crypto_vendor_key_is_valid(
    psa_key_id_t key_id,
    bool is_device_key)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    /* unused parameters */
    (void)key_id;
    (void)is_device_key;

    return false;
#else
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t encoded_key;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return false;
    }

    status = psa_get_key_attributes(encoded_key, &attributes);
    if (status == PSA_SUCCESS) {
        psa_reset_key_attributes(&attributes);
    }

    psa_key_id_t fb_key_id =
        tfm_crypto_vendor_key_hal_key_id_mapping_mbedcrypto_2_fb(key_id);

    /* Device key needs special care */
    if (is_device_key) {
        return tfm_crypto_vendor_key_hal_is_device_key_id(fb_key_id);
    } else {
        return tfm_crypto_vendor_key_hal_is_valid_key_id(fb_key_id);
    }
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

bool tfm_crypto_vendor_key_is_valid_key(
    psa_key_id_t key_id)
{
    return tfm_crypto_vendor_key_is_valid(key_id, false);
}

bool tfm_crypto_vendor_key_is_valid_device_key(
    psa_key_id_t key_id)
{
    return tfm_crypto_vendor_key_is_valid(key_id, true);
}

psa_status_t tfm_crypto_vendor_key_open_key(
    mbedtls_svc_key_id_t encoded_key,
    psa_key_id_t *key)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    /* unused parameters */
    (void)encoded_key;
    (void)key;

    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    uint32_t index;
    psa_key_id_t key_id = MBEDTLS_SVC_KEY_ID_GET_KEY_ID(encoded_key);
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_usage_t usage;
    psa_algorithm_t alg;
    psa_key_type_t key_type;
    size_t key_bits;

    uint8_t key_data[ECC256_KEY_SIZE];
    size_t key_length = sizeof(key_data);

    /* Check if vendor key id is valid */
    if (!tfm_crypto_vendor_key_hal_is_valid_key_id(key_id)) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Get free handle owner index */
    status = tfm_crypto_check_key_storage(&index);

    /* Get vendor key usage and algorithm */
    if (status == PSA_SUCCESS) {
        status = tfm_crypto_vendor_key_hal_get_key_metadata(key_id, &usage, &alg);
    }

    /* Get vendor key */
    if (status == PSA_SUCCESS) {
        status = tfm_crypto_vendor_key_hal_get_key_data(key_id, &key_type, &key_bits,
                                                        key_data, &key_length);
    }

    /* Import vendor provisioned key */
    if (status == PSA_SUCCESS) {
        /* Setup key policy */
        psa_set_key_usage_flags(&attributes, usage);
        psa_set_key_algorithm(&attributes, alg);
        psa_set_key_type(&attributes, key_type);
        psa_set_key_bits(&attributes, key_bits);
        /* Vendor key has to be VOLATILE
         * Note: Non-Volatile key is a persistent key only for Non-Vendor key.
         *       i.e.: VENDOR_KEY_ID is not allowed.
         */
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        /* Need to set Owner ID, not Key ID (Key ID == 0 for Volatile key) */
#ifdef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
        mbedtls_set_key_owner_id(
            &attributes,
            MBEDTLS_SVC_KEY_ID_GET_OWNER_ID( encoded_key )
        );
#endif

        /* `encoded_key` is updated in this function */
        status = psa_import_key(&attributes, key_data, key_length, &encoded_key);
        *key = MBEDTLS_SVC_KEY_ID_GET_KEY_ID(encoded_key);
    }

    if (status == PSA_SUCCESS) {
        status = tfm_crypto_set_key_storage(index, *key);
    }

    if (status == PSA_SUCCESS) {
        status = tfm_crypto_vendor_key_hal_add_key_id_mapping(
            *key, key_id);
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_vendor_key_sign_hash(
                                   psa_key_id_t key_id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   uint8_t *signature,
                                   size_t signature_size,
                                   size_t *signature_length)
{
    psa_key_id_t fb_key_id =
        tfm_crypto_vendor_key_hal_key_id_mapping_mbedcrypto_2_fb(key_id);

    return tfm_crypto_vendor_key_hal_sign_hash(
        fb_key_id, alg, hash, hash_length,
        signature, signature_size, signature_length);
}


psa_status_t tfm_crypto_vendor_key_verify_hash(
                                   psa_key_id_t key_id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length)
{
    psa_key_id_t fb_key_id =
        tfm_crypto_vendor_key_hal_key_id_mapping_mbedcrypto_2_fb(key_id);

    return tfm_crypto_vendor_key_hal_verify_hash(
        fb_key_id, alg, hash, hash_length,
        signature, signature_length);
}

psa_status_t tfm_crypto_vendor_key_export_public_key(
    psa_key_id_t key_id,
    uint8_t *data,
    size_t data_size,
    size_t *data_length)
{
    psa_key_id_t fb_key_id =
        tfm_crypto_vendor_key_hal_key_id_mapping_mbedcrypto_2_fb(key_id);

    return tfm_crypto_vendor_key_hal_export_public_key(
        fb_key_id, data, data_size, data_length);
}


/*!@}*/
