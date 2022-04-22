/*
 * Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MBEDTLS_VERSION_H__
#define __TFM_MBEDTLS_VERSION_H__

/**
 * The version number x.y.z is split into three parts.
 * Major, Minor, Patchlevel
 */
#define TFM_MBEDTLS_VERSION_MAJOR  2
#define TFM_MBEDTLS_VERSION_MINOR  25
#define TFM_MBEDTLS_VERSION_PATCH  0

/**
 * The single version number has the following structure:
 *    MMNNPP00
 *    Major version | Minor version | Patch version
 */
#define TFM_MBEDTLS_VERSION_NUMBER         0x02190000
#define TFM_MBEDTLS_VERSION_STRING         "2.25.0"
#define TFM_MBEDTLS_VERSION_STRING_FULL    "mbed TLS 2.25.0"

#endif /* __TFM_MBEDTLS_VERSION_H__ */
