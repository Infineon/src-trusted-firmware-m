#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
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

if ("${CY_P64_UTILS_LIB_PATH}" STREQUAL "DOWNLOAD")
    find_package(Git)
    if (CY_P64_UTILS_LIB_PATCH_DIR)
        file(GLOB PATCH_FILES *.patch)
    endif()

    if (PATCH_FILES)
        FetchContent_Declare(p64_utils
            GIT_REPOSITORY ${CY_P64_UTILS_LIB_GIT_REMOTE}
            GIT_TAG ${CY_P64_UTILS_LIB_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
            PATCH_COMMAND ${GIT_EXECUTABLE} apply ${PATCH_FILES}
        )
    else()
        FetchContent_Declare(p64_utils
            GIT_REPOSITORY ${CY_P64_UTILS_LIB_GIT_REMOTE}
            GIT_TAG ${CY_P64_UTILS_LIB_VERSION}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            GIT_SUBMODULES ""
        )
    endif()

    FetchContent_GetProperties(p64_utils)
    if(NOT p64_utils_POPULATED)
        FetchContent_Populate(p64_utils)
        set(CY_P64_UTILS_LIB_PATH ${p64_utils_SOURCE_DIR} CACHE PATH "Path to p64_utils repo (or DOWNLOAD to fetch automatically" FORCE)
    endif()
elseif(CY_P64_UTILS_LIB_PATCH_DIR)
    # Apply patches for existing sources
    find_package(Git)
    file(GLOB PATCH_FILES "${CY_P64_UTILS_LIB_PATCH_DIR}/*.patch")

    if (PATCH_FILES)
        include("../../../../../../../tools/patch.cmake")
        patch_src("${CY_P64_UTILS_LIB_PATH}" "${PATCH_FILES}")
    endif()
endif()

#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress PSoC64 Utils Library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED P64_UTILS_LIB_S_TARGET)
    set(P64_UTILS_LIB_S_TARGET p64_utils_lib_s)
endif()

if (NOT DEFINED P64_UTILS_LIB_NS_TARGET)
    set(P64_UTILS_LIB_NS_TARGET p64_utils_lib_ns)
endif()

add_definitions(-DCY_P64_FREE_WIPED)

#=================================== Secure ===================================#

add_library(${P64_UTILS_LIB_S_TARGET} STATIC)

target_sources(${P64_UTILS_LIB_S_TARGET}
    PRIVATE
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_base64.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_cJSON.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_jwt_policy.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_malloc.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_syscalls.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_syscall.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_watchdog.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_image.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_psacrypto.c
)

target_include_directories(${P64_UTILS_LIB_S_TARGET}
    PUBLIC
        ${CY_P64_UTILS_LIB_PATH}
)

target_link_libraries(${P64_UTILS_LIB_S_TARGET}
    PRIVATE
        ${CY_CORE_LIB_TARGET}
        cy_cmsis_lib
        ${PSOC6PDL_LIB_S_TARGET}
)

#================================= Non-Secure =================================#

add_library(${P64_UTILS_LIB_NS_TARGET} STATIC)

target_sources(${P64_UTILS_LIB_NS_TARGET}
    PRIVATE
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_base64.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_cJSON.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_jwt_policy.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_malloc.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_syscalls.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_syscall.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_watchdog.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_image.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_psacrypto.c
)

target_include_directories(${P64_UTILS_LIB_NS_TARGET}
    PUBLIC
        ${CY_P64_UTILS_LIB_PATH}
)

target_link_libraries(${P64_UTILS_LIB_NS_TARGET}
    PRIVATE
        ${CY_CORE_LIB_TARGET}
        cy_cmsis_lib
        ${PSOC6PDL_LIB_NS_TARGET}
)
