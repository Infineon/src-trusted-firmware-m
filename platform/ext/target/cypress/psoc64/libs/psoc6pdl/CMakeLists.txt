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

if ("${CY_PSOC6PDL_LIB_PATH}" STREQUAL "DOWNLOAD")
    find_package(Git)
    if (CY_PSOC6PDL_LIB_PATCH_DIR)
        file(GLOB PATCH_FILES *.patch)
    endif()

    if (PATCH_FILES)
        FetchContent_Declare(psoc6pdl
            GIT_REPOSITORY ${CY_PSOC6PDL_LIB_GIT_REMOTE}
            GIT_TAG ${CY_PSOC6PDL_LIB_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
            PATCH_COMMAND ${GIT_EXECUTABLE} apply ${PATCH_FILES}
        )
    else()
        FetchContent_Declare(psoc6pdl
            GIT_REPOSITORY ${CY_PSOC6PDL_LIB_GIT_REMOTE}
            GIT_TAG ${CY_PSOC6PDL_LIB_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
        )
    endif()

    FetchContent_GetProperties(psoc6pdl)
    if(NOT psoc6pdl_POPULATED)
        FetchContent_Populate(psoc6pdl)
        set(CY_PSOC6PDL_LIB_PATH ${psoc6pdl_SOURCE_DIR} CACHE PATH "Path to psoc6pdl (or DOWNLOAD to get automatically" FORCE)
    endif()
elseif(CY_PSOC6PDL_LIB_PATCH_DIR)
    # Apply patches for existing sources
    find_package(Git)
    file(GLOB PATCH_FILES "${CY_PSOC6PDL_LIB_PATCH_DIR}/*.patch")

    if (PATCH_FILES)
        include("../../../../../../../tools/patch.cmake")
        patch_src("${CY_PSOC6PDL_LIB_PATH}" "${PATCH_FILES}")
    endif()
endif()

#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress PSoC 6 Peripheral Driver Library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED PSOC6PDL_LIB_S_TARGET)
    set(PSOC6PDL_LIB_S_TARGET psoc6pdl_lib_s)
endif()
if (NOT DEFINED PSOC6PDL_LIB_NS_TARGET)
    set(PSOC6PDL_LIB_NS_TARGET psoc6pdl_lib_ns)
endif()

set(CMAKE_PSOC64_DIR        ${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM})

add_subdirectory(psoc6pdl_s_lib)
add_subdirectory(psoc6pdl_ns_lib)

# This directory is treated as part of PDL, but isn't part of the PDL repo
add_library(cy_pdl_platform INTERFACE)

target_include_directories(cy_pdl_platform
    INTERFACE
        ${CMAKE_PSOC64_DIR}/Device/Include
)
