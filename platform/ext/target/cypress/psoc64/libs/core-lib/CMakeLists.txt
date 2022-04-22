#-------------------------------------------------------------------------------
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

if ("${CY_CORE_LIB_PATH}" STREQUAL "DOWNLOAD")
    find_package(Git)
    if (CY_CORE_LIB_PATCH_DIR)
        file(GLOB PATCH_FILES *.patch)
    endif()

    if (PATCH_FILES)
        FetchContent_Declare(cy_core_lib
            GIT_REPOSITORY ${CY_CORE_LIB_GIT_REMOTE}
            GIT_TAG ${CY_P64_CORE_LIB_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
            PATCH_COMMAND ${GIT_EXECUTABLE} apply ${PATCH_FILES}
        )
    else()
        FetchContent_Declare(cy_core_lib
            GIT_REPOSITORY ${CY_CORE_LIB_GIT_REMOTE}
            GIT_TAG ${CY_CORE_LIB_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
        )
    endif()

    FetchContent_GetProperties(cy_core_lib)
    if(NOT cy_core_lib_POPULATED)
        FetchContent_Populate(cy_core_lib)
        set(CY_CORE_LIB_PATH ${cy_core_lib_SOURCE_DIR} CACHE PATH "Path to Cypress Core library repo (or DOWNLOAD to fetch automatically" FORCE)
    endif()
elseif(CY_CORE_LIB_PATCH_DIR)
    # Apply patches for existing sources
    find_package(Git)
    file(GLOB PATCH_FILES "${CY_CORE_LIB_PATCH_DIR}/*.patch")

    if (PATCH_FILES)
        include("../../../../../../../tools/patch.cmake")
        patch_src("${CY_CORE_LIB_PATH}" "${PATCH_FILES}")
    endif()
endif()

#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress Core Library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED CY_CORE_LIB_TARGET)
    set(CY_CORE_LIB_TARGET cy_core_lib)
endif()

add_library(${CY_CORE_LIB_TARGET} INTERFACE)

target_include_directories(${CY_CORE_LIB_TARGET}
    INTERFACE
        ${CY_CORE_LIB_PATH}/include
)
