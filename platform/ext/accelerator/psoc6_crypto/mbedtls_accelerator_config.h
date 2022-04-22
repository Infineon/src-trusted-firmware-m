/**
 * \brief User Configuration
 *
 *  Included by tfm_mbedcrypto_config.h at the end, to provide
 *  user a way to override default common settings.
 */
/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MBEDTLS_ACCELERATOR_CONFIG_H
#define MBEDTLS_ACCELERATOR_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* TRNG Config */
#undef MBEDTLS_TEST_NULL_ENTROPY
#undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES

#define MBEDTLS_ENTROPY_HARDWARE_ALT


/* AES algorithm */
#define MBEDTLS_AES_C
#define MBEDTLS_AES_ALT
#define MBEDTLS_CIPHER_MODE_CBC             /* mbedtls_aes_crypt_cbc */
#define MBEDTLS_CIPHER_MODE_CFB             /* mbedtls_aes_crypt_cfb128 */
#define MBEDTLS_CIPHER_MODE_CTR             /* mbedtls_aes_crypt_ctr */
#define MBEDTLS_CIPHER_MODE_OFB             /* mbedtls_aes_crypt_ofb */
#define MBEDTLS_CIPHER_MODE_XTS             /* mbedtls_aes_crypt_xts */

/* Only NIST-P curves are currently supported */
#define MBEDTLS_ECP_C
/* temporary disabled #define MBEDTLS_ECP_ALT */
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED    /* ecp_mod_p192 */
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED    /* ecp_mod_p224 */
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED    /* ecp_mod_p256 */
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED    /* ecp_mod_p384 */
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED    /* ecp_mod_p521 */
#define MBEDTLS_ECP_DP_BP256R1_ENABLED      /* brainpoolP256r1 */
#define MBEDTLS_ECP_DP_BP384R1_ENABLED      /* brainpoolP384r1 */
#define MBEDTLS_ECP_DP_BP512R1_ENABLED      /* brainpoolP512r1 */
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED    /* ecp_mod_p192k1 */
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED    /* ecp_mod_p224k1 */
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED    /* ecp_mod_p256k1 */
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED   /* ecp_use_curve25519 */
#define MBEDTLS_ECP_DP_CURVE448_ENABLED     /* ecp_use_curve448 */

/* ECDSA algorithm */
#define MBEDTLS_ECDSA_C
/* temporary disabled #define MBEDTLS_ECDSA_SIGN_ALT */             /* mbedtls_ecdsa_sign */
/* temporary disabled #define MBEDTLS_ECDSA_VERIFY_ALT */           /* mbedtls_ecdsa_verify */
#define MBEDTLS_ECDSA_DETERMINISTIC         /* mbedtls_ecdsa_sign_det */

/* SHA algorithm */
#undef MBEDTLS_SHA1_C
#undef MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA512_C
#define MBEDTLS_SHA512_ALT


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MBEDTLS_ACCELERATOR_CONFIG_H */
