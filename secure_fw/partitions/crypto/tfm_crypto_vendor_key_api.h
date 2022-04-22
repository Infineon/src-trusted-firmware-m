/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_VENDOR_KEY_API_H__
#define __TFM_CRYPTO_VENDOR_KEY_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "tfm_crypto_defs.h"


/**
 * \brief Checks vendor key is valid and return the vendor key ID if valid
 *
 * \param[in]  key_id          Key identifier
 *
 * \return Return boolean to indicate if it's a valid key
 */
bool tfm_crypto_vendor_key_is_valid_key(psa_key_id_t key_id);

/**
 * \brief Checks vendor key is a valid device key and return the vendor key ID if valid
 *
 * \param[in]  key_id          Key identifier
 *
 * \return Return boolean to indicate if it's a valid device key
 */
bool tfm_crypto_vendor_key_is_valid_device_key(psa_key_id_t key_id);

/**
 * \brief Open vendor key with specified vendor key ID
 *        It will call platform specific HAL API to check
 *        if the vendor key ID is supported, then import the vendor key
 *        Note: for the vendor private key, which can not be imported,
 *              a dummy key is imported, the sign and verify function
 *              will be done by platform specific implementation
 *
 * \param[in]  encoded_key  The persistent identifier for the key
 * \param[out] key_id       Key identifier
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_vendor_key_open_key(mbedtls_svc_key_id_t encoded_key,
                                            psa_key_id_t *key_id);

/**
 * \brief Sign a hash or short message with a private key specified by key id.
 * Note: it will call platform specific implementation to perform the signing
 *
 * \param key_id                vendor key id
 *                              It must be an asymmetric key pair.
 * \param alg                   A signature algorithm that is compatible with
 *                              the type of \p id.
 * \param[in] hash              The hash or message to sign.
 * \param hash_length           Size of the \p hash buffer in bytes.
 * \param[out] signature        Buffer where the signature is to be written.
 * \param signature_size        Size of the \p signature buffer in bytes.
 * \param[out] signature_length On success, the number of bytes
 *                              that make up the returned signature value.
 *
 * \retval #PSA_SUCCESS if success, otherwise #PSA_ERROR_*
 */
 psa_status_t tfm_crypto_vendor_key_sign_hash(
                                   psa_key_id_t key_id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   uint8_t *signature,
                                   size_t signature_size,
                                   size_t *signature_length);


/**
 * \brief Verify the signature a hash or short message using a public key
 *        specified by the key id.
 *
 * Note: it will call platform specific implementation to perform the
 *       verification
 *
 * \param key_id            vendor key id
 *                          It must be a public key or an asymmetric key pair.
 * \param alg               A signature algorithm that is compatible with
 *                          the type of \p handle.
 * \param[in] hash          The hash or message whose signature is to be
 *                          verified.
 * \param hash_length       Size of the \p hash buffer in bytes.
 * \param[in] signature     Buffer containing the signature to verify.
 * \param signature_length  Size of the \p signature buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 *         The signature is valid.
 *         otherwise, #PSA_ERROR_*
 */
psa_status_t tfm_crypto_vendor_key_verify_hash(
                                   psa_key_id_t key_id,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length);


/**
 * \brief Export public key specified by the key id.
 *
 * Note: it will call platform specific implementation
 *
 * \param[in]  key_id       vendor key id
 *                          It must be a public key or an asymmetric key pair.
 * \param[out] data         Buffer where the public key to be stored
 * \param[in]  data_size    Size of buffer
 * \param[out] data_length  Actual size used in the provided buffer
 *
 * \retval #PSA_SUCCESS     Public key is exported in the buffer
 *         otherwise        #PSA_ERROR_*
 */
psa_status_t tfm_crypto_vendor_key_export_public_key(
    psa_key_id_t key_id,
    uint8_t *data,
    size_t data_size,
    size_t *data_length);


#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_VENDOR_KEY_API_H__ */
