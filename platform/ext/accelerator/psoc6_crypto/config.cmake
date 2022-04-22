#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

################################## Dependencies ################################

set(CY_MBEDTLS_ACCELERATION_PATH        "DOWNLOAD"  CACHE PATH      "Path to cy-mbedtls-acceleration (or DOWNLOAD to fetch automatically")
set(CY_MBEDTLS_ACCELERATION_VERSION     "release-v1.3.0" CACHE STRING "The version of cy-mbedtls-acceleration to use")
set(CY_MBEDTLS_ACCELERATION_GIT_REMOTE  "https://github.com/Infineon/cy-mbedtls-acceleration.git"
    CACHE STRING "The URL to retrieve cy-mbedtls-acceleration from.")
set(CY_MBEDTLS_ACCELERATION_PATCH_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/cypress/psoc64/libs/cy-mbedtls-acceleration"
    CACHE PATH      "Path to folder with cy-mbedtls-acceleration patches. Set empty string to skip patching.")

############ Override defaults for cy-mbedtls-acceleration #####################

set(CRYPTO_ENGINE_BUF_SIZE              0x8000      CACHE STRING    "Heap size for the crypto backend" FORCE)
set(CRYPTO_IOVEC_BUFFER_SIZE            7168        CACHE STRING    "Default size of the internal scratch buffer used for PSA FF IOVec allocations" FORCE)
