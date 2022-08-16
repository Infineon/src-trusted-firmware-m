#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


################################## Tweakable ################################

# Balance between SRAM use and performance
set(NUM_MAILBOX_QUEUE_SLOT              4           CACHE BOOL      "Number of mailbox queue slots")

# Set to ON to configure TF-M from provisioning data provided in the policy file
# Set to OFF to use hard-coded values like upstream TF-M
set(CY_POLICY_CONCEPT                   ON          CACHE BOOL      "Get information from provision data (policy file)")

################################## Hardware ################################

# Only IPC model is supported for dual-core platforms
set(TFM_PSA_API                         ON          CACHE BOOL      "Use PSA api (IPC mode) instead of secure library mode" FORCE)

# dual core doesn't support NS client identification
set(TFM_NS_CLIENT_IDENTIFICATION        OFF         CACHE BOOL      "Enable NS client identification" FORCE)

# Allows crypto keys stored in the hardware accelerator block to be used for crypto operations
set(TFM_ENABLE_PS_OEM_UID               ON          CACHE BOOL      "Enable OEM UID" FORCE)

# PSoC64 has separate secure and non-secure cores
set(TFM_MULTI_CORE_TOPOLOGY             ON          CACHE BOOL      "Whether to build for a dual-cpu architecture")

set(PLATFORM_DUMMY_ATTEST_HAL           FALSE       CACHE BOOL      "Use dummy attest hal implementation. Should not be used in production.")
# PSoC64 has a full flash-based NV counters implementation
set(PLATFORM_DUMMY_NV_COUNTERS          FALSE       CACHE BOOL      "Use dummy nv counter implementation. Should not be used in production.")
set(PLATFORM_DUMMY_CRYPTO_KEYS          FALSE       CACHE BOOL      "Use dummy crypto keys. Should not be used in production.")
set(PLATFORM_DUMMY_ROTPK                FALSE       CACHE BOOL      "Use dummy root of trust public key. Dummy key is the public key for the default keys in bl2. Should not be used in production.")
set(PLATFORM_DUMMY_IAK                  FALSE       CACHE BOOL      "Use dummy initial attestation_key. Should not be used in production.")

# The PSoC64 has a hardware crypto accelerator
set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms" FORCE)
set(CRYPTO_HW_ACCELERATOR_TYPE          psoc6_crypto)
set(CY_MBEDTLS_ACCELERATION_BUILD_TYPE  MinSizeRel  CACHE STRING    "Build type of cy-mbedtls-acceleration library" FORCE)

################################## Software ################################

# PSoC64 uses Cypress bootloader
set(BL2                                 OFF         CACHE BOOL      "Whether to build BL2" FORCE)

# Note that these won't be used if CY_POLICY_CONCEPT is ON
set(PS_MAX_ASSET_SIZE                   "2008"      CACHE STRING    "The maximum asset size to be stored in the Protected Storage area")
set(ITS_MAX_ASSET_SIZE                  "2048"      CACHE STRING    "The maximum asset size to be stored in the Internal Trusted Storage area")
set(ITS_NUM_ASSETS                      "8"         CACHE STRING    "The maximum number of assets to be stored in the Internal Trusted Storage area")
# Set ITS_BUF_SIZE to ITS_MAX_ASSET_SIZE by default. It fixes very slow performance of ITS/PS.
set(ITS_BUF_SIZE                        ""          CACHE STRING    "Size of the ITS internal data transfer buffer (defaults to ITS_MAX_ASSET_SIZE if not set)")

################################## Dependencies ################################

set(CY_CORE_LIB_PATH                    "DOWNLOAD"  CACHE PATH      "Path to Cypress Core library repo (or DOWNLOAD to fetch automatically")
set(CY_CORE_LIB_GIT_REMOTE              "https://github.com/Infineon/core-lib.git"  CACHE STRING      "Cypress Core library repo URL (to fetch automatically")
set(CY_CORE_LIB_VERSION                 "release-v1.3.0" CACHE STRING  "The version of Cypress Core library to use")
set(CY_CORE_LIB_PATCH_DIR               "${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/cypress/psoc64/libs/core-lib"
    CACHE PATH      "Path to folder with Cypress Core library patches. Set empty string to skip patching.")

set(CY_PSOC6PDL_LIB_PATH                "DOWNLOAD"  CACHE PATH      "Path to PSOC6PDL repo (or DOWNLOAD to fetch automatically")
set(CY_PSOC6PDL_LIB_GIT_REMOTE          "https://github.com/Infineon/mtb-pdl-cat1.git"  CACHE STRING      "PSOC6PDL library repo URL (to fetch automatically")
set(CY_PSOC6PDL_LIB_VERSION             "release-v2.4.0" CACHE STRING  "The version of PSOC6PDL to use")
set(CY_PSOC6PDL_LIB_PATCH_DIR           "${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/cypress/psoc64/libs/psoc6pdl"
    CACHE PATH      "Path to folder with PSOC6PDL patches. Set empty string to skip patching.")

set(CY_P64_UTILS_LIB_PATH               "DOWNLOAD"  CACHE PATH      "Path to p64_utils repo (or DOWNLOAD to fetch automatically")
set(CY_P64_UTILS_LIB_GIT_REMOTE         "https://github.com/Infineon/p64_utils.git"  CACHE STRING      "p64_utils library repo URL (to fetch automatically")
set(CY_P64_UTILS_LIB_VERSION            "release-v1.0.1" CACHE STRING "The version of p64_utils to use")
set(CY_P64_UTILS_LIB_PATCH_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/cypress/psoc64/libs/p64_utils"
    CACHE PATH      "Path to folder with p64_utils patches. Set empty string to skip patching.")
