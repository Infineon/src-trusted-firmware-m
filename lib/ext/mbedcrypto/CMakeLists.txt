#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
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

if ("${MBEDCRYPTO_PATH}" STREQUAL "DOWNLOAD")
    find_package(Git)
    if (MBEDCRYPTO_PATCH_DIR)
        file(GLOB PATCH_FILES "${MBEDCRYPTO_PATCH_DIR}/*.patch")
    endif()

    if (PATCH_FILES)
        FetchContent_Declare(mbedcrypto
            GIT_REPOSITORY ${MBEDCRYPTO_GIT_REMOTE}
            GIT_TAG ${MBEDCRYPTO_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
            PATCH_COMMAND ${GIT_EXECUTABLE} apply ${PATCH_FILES}
        )
    else()
        FetchContent_Declare(mbedcrypto
            GIT_REPOSITORY ${MBEDCRYPTO_GIT_REMOTE}
            GIT_TAG ${MBEDCRYPTO_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
        )
    endif()

    FetchContent_GetProperties(mbedcrypto)
    if(NOT mbedcrypto_POPULATED)
        FetchContent_Populate(mbedcrypto)
        set(MBEDCRYPTO_PATH ${mbedcrypto_SOURCE_DIR} CACHE PATH "Path to mbed-crypto (or DOWNLOAD to get automatically" FORCE)
    endif()
elseif(MBEDCRYPTO_PATCH_DIR)
    # Apply patches for existing sources
    find_package(Git)
    file(GLOB PATCH_FILES "${MBEDCRYPTO_PATCH_DIR}/*.patch")

    if (PATCH_FILES)
        include("../../../tools/patch.cmake")
        patch_src("${MBEDCRYPTO_PATH}" "${PATCH_FILES}")
    endif()
endif()
