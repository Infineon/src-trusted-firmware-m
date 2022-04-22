#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
# Allow linking to things 'upwards' in the directory tree (in this case tfm_partition_crypto)
cmake_policy(SET CMP0079 NEW)
# Allow relative paths
cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED PSOC6_CRYPTO_PATH)
    set(PSOC6_CRYPTO_PATH ../../../../platform/ext/target/cypress/psoc64/libs/cy-mbedtls-acceleration CACHE PATH "Path to cy-mbedtls-acceleration lib")
endif()

############################ Crypto Service ####################################

target_sources(crypto_service_crypto_hw
    PRIVATE
        crypto_hw.c
        hw_poll.c
)

target_include_directories(crypto_service_crypto_hw
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(crypto_service_crypto_hw
    PUBLIC
        psoc6_crypto_service
    PRIVATE
        crypto_service_mbedcrypto
)

target_link_libraries(platform_s
    PRIVATE
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:crypto_service_crypto_hw>
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:psoc6_crypto_service>
)

set(PSOC6_CRYPTO_LIB_TARGET psoc6_crypto_service)
set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${CY_MBEDTLS_ACCELERATION_BUILD_TYPE})
add_subdirectory(${PSOC6_CRYPTO_PATH} ${CMAKE_CURRENT_BINARY_DIR}/psoc6_crypto_service)
set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

# Adding two targets as link-time dependencies of each other seems bad, but
# in reality it just means that they'll share headers and compile defs.
target_link_libraries(psoc6_crypto_service
    PRIVATE
        crypto_service_mbedcrypto
)
target_link_libraries(crypto_service_mbedcrypto
    PRIVATE
        psoc6_crypto_service
    PUBLIC
        crypto_service_crypto_hw
)

target_compile_definitions(crypto_service_mbedcrypto
    PUBLIC
        CRYPTO_HW_ACCELERATOR
        MBEDTLS_ECDH_LEGACY_CONTEXT
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:CRYPTO_HW_ACCELERATOR_OTP_ENABLED>
)

target_compile_options(crypto_service_mbedcrypto
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wno-unused-parameter>
        $<$<C_COMPILER_ID:ARMClang>:-Wno-unused-parameter>
)
