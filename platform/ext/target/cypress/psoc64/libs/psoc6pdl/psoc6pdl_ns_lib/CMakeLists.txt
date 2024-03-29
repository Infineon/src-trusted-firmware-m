#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress PSoC 6 Peripheral Driver Library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED PSOC6PDL_LIB_NS_TARGET)
    set(PSOC6PDL_LIB_NS_TARGET psoc6pdl_lib_ns)
endif()

########################## PSOC6PDL_LIB_NS #####################################

if (TFM_MULTI_CORE_TOPOLOGY)
    include(${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/preload_ns.cmake)
    tfm_toolchain_reload_compiler()
    # The platform target is created in this directory/file so that it has the
    # same settings as the main ns target.
    add_library(${PSOC6PDL_LIB_NS_TARGET} STATIC)
endif()

target_sources(${PSOC6PDL_LIB_NS_TARGET}
    PRIVATE
        ${CY_PSOC6PDL_LIB_PATH}/devices/COMPONENT_CAT1A/source/cy_device.c

        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_flash.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_gpio.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_ipc_drv.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_ipc_pipe.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_ipc_sema.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_pra.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_pra_cfg.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_prot.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_scb_common.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_scb_uart.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_sysclk.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_sysint.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_syslib.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_syspm.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_systick.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_tcpwm_counter.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_trigmux.c
        ${CY_PSOC6PDL_LIB_PATH}/drivers/source/cy_wdt.c
)

target_include_directories(${PSOC6PDL_LIB_NS_TARGET}
    PUBLIC
        ${CY_PSOC6PDL_LIB_PATH}/devices/COMPONENT_CAT1A/include
        ${CY_PSOC6PDL_LIB_PATH}/drivers/include
)

target_link_libraries(${PSOC6PDL_LIB_NS_TARGET}
    PUBLIC
        cy_pdl_platform
        cy_cmsis_lib
    PRIVATE
        ${CY_CORE_LIB_TARGET}
)
