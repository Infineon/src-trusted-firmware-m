/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_CRYPTO_VENDOR_KEY_HAL_API_H
#define TFM_CRYPTO_VENDOR_KEY_HAL_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "tfm_api.h"
#include "tfm_crypto_defs.h"

/**
 * \brief Check if vendor key ID is valid.
 * \param[in]  id  Key identifier
 *
 * \return Return boolean to indicate if it's a valid key id
 */
bool tfm_crypto_vendor_key_hal_is_valid_key_id(psa_key_id_t id);

/**
 * \brief Get vendor device key ID.
 *
 * \return Return the device key ID
 */
psa_key_id_t tfm_crypto_vendor_key_hal_get_device_key_id();

/**
 * \brief Register MBedCrypto key id & FB key id pairs
 * \param[in]  mbedcrypto_key_id  Key identifier of MBedCrypto
 * \param[in]  fb_key_id          Key identifier of Flash Secure Boot
 *
 * \retval #PSA_SUCCESS or PSA error
 */
psa_status_t tfm_crypto_vendor_key_hal_add_key_id_mapping(
    psa_key_id_t mbedcrypto_key_id,
    psa_key_id_t fb_key_id);

/**
 * \brief Use MBedCrypto key id to find FB key id in the key id mapping table
 * \param[in]  mbedcrypto_key_id  Key identifier of MBedCrypto
 *
 * \retval FB key id or NULL
 */
psa_key_id_t tfm_crypto_vendor_key_hal_key_id_mapping_mbedcrypto_2_fb(
    psa_key_id_t mbedcrypto_key_id);

/**
 * \brief Check if it's vendor device key ID.
 * \param[in]  id  Key identifier
 *
 * \return Return boolean to indicate if it's device key id
 */
bool tfm_crypto_vendor_key_hal_is_device_key_id(psa_key_id_t id);

/**
 * \brief Get vendor key metadata if supported
 *
 * \param[in]  id  Key identifier
 * \param[out] usage Pointer to the key usage
 * \param[out] alg Pointer to the key algorithm
 * \return Return PSA_SUCCESS if success, otherwise PSA_ERROR_*
 */
psa_status_t tfm_crypto_vendor_key_hal_get_key_metadata(
                                           psa_key_id_t id,
                                           psa_key_usage_t *usage,
                                           psa_algorithm_t *alg);

/**
 * \brief Import vendor provisioned key data if supported
 *
 * \param[in]  id  Key identifier
 * \param[out] key_type Pointer to key type structure
 * \param[out] key_bits Pointer to a size_t for key bits
 * \param[out] data - pointer to key data buffer pointer
 * \param[out] data_length - pointer to key data length
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_vendor_key_hal_get_key_data(
                                       psa_key_id_t id,
                                       psa_key_type_t *key_type,
                                       size_t *key_bits,
                                       uint8_t *data,
                                       size_t *data_length);

/**
 * \brief Sign hash with vendor key
 *
 * \param[in] id                Key identifier
 * \param alg                   A signature algorithm that is compatible with
 *                              the type of key.
 * \param[in] hash              The hash or message to sign.
 * \param[in] hash_length       Size of the hash or message in bytes.
 * \param[in] signature         Buffer where the signature is to be written.
 * \param[in] signature_size    Size of the signature buffer in bytes.
 * \param[out] signature_length On success, the number of bytes
 *                              that make up the returned signature value.
 *
 * \retval #PSA_SUCCESS or PSA Crypto error
 */
psa_status_t tfm_crypto_vendor_key_hal_sign_hash(
                                   psa_key_id_t id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   uint8_t *signature,
                                   size_t signature_size,
                                   size_t *signature_length);

/**
 * \brief Verify hash with vendor key
 *
 * \param[in] id                Key identifier
 * \param alg                   A signature algorithm that is compatible with
 *                              the type of key.
 * \param[in] hash              The hash to be verified.
 * \param[in] hash_length       Size of the hash in bytes.
 * \param[in] signature         The signature to be verified.
 * \param[in] signature_length  Size of the signature in bytes.
 *
 * \retval #PSA_SUCCESS or PSA Crypto error
 */

psa_status_t tfm_crypto_vendor_key_hal_verify_hash(
                                   psa_key_id_t id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length);

/**
 * \brief Export vendor public key
 *
 * \param[in]  id           Key identifier
 * \param[out] data         Buffer where the public key to be stored
 * \param[in]  data_size    Size of buffer
 * \param[out] data_length  Actual size used in the provided buffer
 *
 * \retval #PSA_SUCCESS or PSA Crypto error
 */
psa_status_t tfm_crypto_vendor_key_hal_export_public_key(
    psa_key_id_t id,
    uint8_t *data,
    size_t data_size,
    size_t *data_length);

#ifdef __cplusplus
}
#endif

#endif /* TFM_CRYPTO_VENDOR_KEY_HAL_API_H */
