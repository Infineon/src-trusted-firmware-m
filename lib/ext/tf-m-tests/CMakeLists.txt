#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
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

if ("${TFM_TEST_REPO_PATH}" STREQUAL "DOWNLOAD")
    find_package(Git)
    if (TFM_TEST_REPO_APPLY_PATCHES)
        file(GLOB PATCH_FILES *.patch)
    endif()

    if (PATCH_FILES)
        FetchContent_Declare(tfm_test_repo
                GIT_REPOSITORY ${TFM_TEST_REPO_GIT_REMOTE}
                GIT_TAG ${TFM_TEST_REPO_VERSION}
                GIT_PROGRESS TRUE
                PATCH_COMMAND ${GIT_EXECUTABLE} apply ${PATCH_FILES}
        )
    else()
        FetchContent_Declare(tfm_test_repo
            GIT_REPOSITORY ${TFM_TEST_REPO_GIT_REMOTE}
            GIT_TAG ${TFM_TEST_REPO_VERSION}
            GIT_PROGRESS TRUE
        )
    endif()

    FetchContent_GetProperties(tfm_test_repo)
    if(NOT tfm_test_repo_POPULATED)
        FetchContent_Populate(tfm_test_repo)
        set(TFM_TEST_REPO_PATH ${tfm_test_repo_SOURCE_DIR} CACHE PATH "Path to TFM-TEST repo (or DOWNLOAD to fetch automatically" FORCE)
    endif()
endif()

if ("${CMSIS_5_PATH}" STREQUAL DOWNLOAD)
    set(CMSIS_5_PATH ${TFM_TEST_REPO_PATH}/CMSIS CACHE PATH "Path to CMSIS_5 (or DOWNLOAD to fetch automatically" FORCE)
endif()

if (NOT TFM_APP_PATH)
    set(TFM_APP_PATH ${TFM_TEST_REPO_PATH}/app CACHE PATH "Path to TFM NS app" FORCE)
endif()

if (NOT TFM_TEST_PATH)
    set(TFM_TEST_PATH ${TFM_TEST_REPO_PATH}/test CACHE PATH "Path to TFM tests" FORCE)
endif()

if (NOT TFM_NS_LOG_PATH)
    set(TFM_NS_LOG_PATH ${TFM_TEST_REPO_PATH}/log CACHE PATH "Path to NS log" FORCE)
endif()
