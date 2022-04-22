#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Set to not download submodules if that option is available
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    cmake_policy(SET CMP0097 NEW)
endif()

if ("${CY_MBEDTLS_ACCELERATION_PATH}" STREQUAL "DOWNLOAD")
    find_package(Git)
    if (CY_MBEDTLS_ACCELERATION_PATCH_DIR)
        file(GLOB PATCH_FILES "${CY_MBEDTLS_ACCELERATION_PATCH_DIR}/*.patch")
    endif()

    if (PATCH_FILES)
        FetchContent_Declare(cy_mbedtls_acceleration
            GIT_REPOSITORY ${CY_MBEDTLS_ACCELERATION_GIT_REMOTE}
            GIT_TAG ${CY_MBEDTLS_ACCELERATION_VERSION}
            GIT_SHALLOW FALSE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
            PATCH_COMMAND ${GIT_EXECUTABLE} apply ${PATCH_FILES}
        )
    else()
        FetchContent_Declare(cy_mbedtls_acceleration
            GIT_REPOSITORY ${CY_MBEDTLS_ACCELERATION_GIT_REMOTE}
            GIT_TAG ${CY_MBEDTLS_ACCELERATION_VERSION}
            GIT_SHALLOW FALSE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
        )
    endif()

    FetchContent_GetProperties(cy_mbedtls_acceleration)
    if(NOT cy_mbedtls_acceleration_POPULATED)
        FetchContent_Populate(cy_mbedtls_acceleration)
        set(CY_MBEDTLS_ACCELERATION_PATH ${cy_mbedtls_acceleration_SOURCE_DIR} CACHE PATH "Path to cy-mbedtls-acceleration (or DOWNLOAD to get automatically" FORCE)
    endif()
elseif(CY_MBEDTLS_ACCELERATION_PATCH_DIR)
    # Apply patches for existing sources
    find_package(Git)
    file(GLOB PATCH_FILES "${CY_MBEDTLS_ACCELERATION_PATCH_DIR}/*.patch")

    if (PATCH_FILES)
        include("../../../../../../../tools/patch.cmake")
        patch_src("${CY_MBEDTLS_ACCELERATION_PATH}" "${PATCH_FILES}")
    endif()
endif()

#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress PSoC6 MCUs acceleration for MbedTLS library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED PSOC6_CRYPTO_LIB_TARGET)
    set(PSOC6_CRYPTO_LIB_TARGET cy-mbedtls-acceleration)
endif()

set(CMAKE_PSOC64_DIR        ${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM})
set(CY_CORE_LIB_TARGET      cy_core_lib)
set(CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR
                            ${CY_MBEDTLS_ACCELERATION_PATH}/mbedtls_MXCRYPTO)

add_library(${PSOC6_CRYPTO_LIB_TARGET} STATIC)

########################## LIB #################################################

target_sources(${PSOC6_CRYPTO_LIB_TARGET}
    PRIVATE
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/aes_alt.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/crypto_common.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/ecdsa_alt.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/ecp_alt.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/ecp_curves_alt.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/sha1_alt.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/sha256_alt.c
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}/sha512_alt.c
)

target_include_directories(${PSOC6_CRYPTO_LIB_TARGET}
    INTERFACE
        ${CMAKE_CY_MBEDTLS_ACCELERATION_SRC_DIR}
        ${CY_PSOC6PDL_LIB_PATH}/devices/COMPONENT_CAT1A/include
        ${CY_PSOC6PDL_LIB_PATH}/drivers/include
        ${CMAKE_SOURCE_DIR}/platform/ext/cmsis
        ${CMAKE_PSOC64_DIR}/Device/Include
)

target_link_libraries(${PSOC6_CRYPTO_LIB_TARGET}
    PUBLIC
        ${CY_CORE_LIB_TARGET}
    PRIVATE
        ${PSOC6PDL_LIB_S_TARGET}
)
