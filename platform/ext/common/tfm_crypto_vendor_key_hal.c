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
#include "cmsis.h"

#include "psa/crypto.h"
#include "tfm_crypto_vendor_key_hal_api.h"

bool tfm_crypto_vendor_key_hal_is_valid_key_id(psa_key_id_t id)
{
    (void) id;

    return false;
}

psa_key_id_t tfm_crypto_vendor_key_hal_get_device_key_id()
{
    return (psa_key_id_t)NULL;
}

void tfm_crypto_vendor_key_hal_register_mbedcrypto_device_key_id(
    psa_key_handle_t id)
{}

bool tfm_crypto_vendor_key_hal_is_device_key_id(psa_key_id_t id)
{
    (void) id;

    return false;
}

psa_status_t tfm_crypto_vendor_key_hal_get_key_metadata(
                                           psa_key_id_t id,
                                           psa_key_usage_t *usage,
                                           psa_algorithm_t *alg);
{
    (void) id;
    (void) usage;
    (void) alg;

    return PSA_ERROR_DOES_NOT_EXIST;
}

psa_status_t tfm_crypto_vendor_key_hal_get_key_data(
                                       psa_key_id_t id,
                                       psa_key_type_t *key_type,
                                       size_t *key_bits,
                                       uint8_t *data,
                                       size_t *data_length)
{
    (void) id;
    (void) key_type;
    (void) key_bits;
    (void) data;
    (void) data_length;

    return PSA_ERROR_DOES_NOT_EXIST;
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
    (void) id;
    (void) alg;
    (void) hash;
    (void) hash_length;
    (void) signature;
    (void) signature_size;
    (void) signature_length;

    return PSA_ERROR_DOES_NOT_EXIST;
}

psa_status_t tfm_crypto_vendor_key_hal_verify_hash(
                                   psa_key_id_t id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length)

{
    (void) id;
    (void) alg;
    (void) hash;
    (void) hash_length;
    (void) signature;
    (void) signature_length;

    return PSA_ERROR_DOES_NOT_EXIST;
}

psa_status_t tfm_crypto_vendor_key_export_public_key(
    psa_key_id_t key_id,
    uint8_t *data,
    size_t data_size,
    size_t *data_length)
{
    (void) key_id;
    (void) data;
    (void) data_size;
    (void) data_length;

    return PSA_ERROR_DOES_NOT_EXIST;
}
