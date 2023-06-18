#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)
set(CMAKE_CURRENT_SOURCE_DIR    ${CMAKE_CURRENT_LIST_DIR})
set(CY_PSOC64_DIR               ${CMAKE_CURRENT_SOURCE_DIR})

#========================= Libraries ==========================================#

set(CY_CORE_LIB_TARGET cy_core_lib)
# Note: the `psoc6pdl` will resolve CY_PSOC6PDL_LIB_PATH
set(PSOC6PDL_LIB_S_TARGET   psoc6pdl_lib_s)
set(PSOC6PDL_LIB_NS_TARGET  psoc6pdl_lib_ns)
set(P64_UTILS_LIB_S_TARGET  p64_utils_lib_s)
set(P64_UTILS_LIB_NS_TARGET p64_utils_lib_ns)

add_subdirectory(libs/core-lib)
add_subdirectory(libs/cmsis)
add_subdirectory(libs/psoc6pdl)
add_subdirectory(libs/p64_utils)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
)
target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${TEST_PSA_API}>:PSA_API_TEST_ENABLED>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_POLICY_CONCEPT>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_FLASH_LAYOUT_FROM_POLICY>
)

#========================= Platform common defs ===============================#

target_sources(tfm_s
    PRIVATE
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_psoc64_s.s>
        $<$<C_COMPILER_ID:ARMClang>:${CY_PSOC6PDL_LIB_PATH}/drivers/source/TOOLCHAIN_ARM/cy_syslib_ext.s>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_psoc64_s.S>
        $<$<C_COMPILER_ID:GNU>:${CY_PSOC6PDL_LIB_PATH}/drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.S>
        $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_psoc64_s.s>
        $<$<C_COMPILER_ID:IAR>:${CY_PSOC6PDL_LIB_PATH}/drivers/source/TOOLCHAIN_IAR/cy_syslib_ext.s>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_SOURCE_DIR}/platform/ext/common/iar/tfm_common_s.icf>
)
target_compile_definitions(tfm_s_scatter
    PRIVATE
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_psoc64_ns.s>
            $<$<C_COMPILER_ID:ARMClang>:${CY_PSOC6PDL_LIB_PATH}/drivers/source/TOOLCHAIN_ARM/cy_syslib_ext.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_psoc64_ns.S>
            $<$<C_COMPILER_ID:GNU>:${CY_PSOC6PDL_LIB_PATH}/drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.S>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_psoc64_ns.s>
            $<$<C_COMPILER_ID:IAR>:${CY_PSOC6PDL_LIB_PATH}/drivers/source/TOOLCHAIN_IAR/cy_syslib_ext.s>
    )
    target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/psoc6_ns.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/psoc6_ns.ld>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/psoc6_ns.icf>
    )
    # TODO Remove DOMAIN_NS to meet new cmake structure
    # TODO The PSoC L2 test probably also needs DOMAIN_NS=1 set
    # When pre-processing the NS linker file, we also need DOMAIN_NS=1 set
    target_compile_definitions(tfm_ns_scatter
        PRIVATE
            DOMAIN_NS=1
            $<$<BOOL:${TEST_NS}>:TEST_NS>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            # CM4 without FP support uses CM3 libs
            CMSIS_5_RTX_CM3
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        mailbox
        Device/Config
        CMSIS_Driver/Config
        utils
)

target_sources(platform_s
    PRIVATE
        mailbox/platform_multicore.c
        mailbox/platform_spe_mailbox.c
        CMSIS_Driver/Driver_Flash.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/psoc6_system_init_cm0p.c
        Device/Source/system_psoc6_cm0plus.c
        attest_hal.c
        crypto_keys.c
        driver_ppu.c
        driver_smpu.c
        nv_counters.c
        spm_hal.c
        target_cfg.c
        tfm_hal_isolation.c
        services/src/tfm_crypto_vendor_key_hal.c
        services/src/tfm_platform_ps.c
        services/src/tfm_platform_system.c
        $<$<BOOL:${CY_POLICY_CONCEPT}>:${CMAKE_CURRENT_SOURCE_DIR}/utils/cy_policy.c>
        utils/cy_p64_psa_mappings.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        $<$<NOT:$<BOOL:${CY_POLICY_CONCEPT}>>:${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_its.c>
        $<$<NOT:$<BOOL:${CY_POLICY_CONCEPT}>>:${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_ps.c>
)

target_compile_definitions(platform_s
    PRIVATE
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_ATTEST_DETAILS_FROM_POLICY>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_WDT_CONFIG_FROM_POLICY>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_IMG_OK_CONFIG_FROM_POLICY>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_HW_VERSION_FROM_POLICY>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_HW_SETTINGS_FROM_POLICY>
        $<$<BOOL:${CY_POLICY_CONCEPT}>:CY_EXTCLK_CONFIG_FROM_POLICY>
)

target_link_libraries(platform_s
    PUBLIC
        ${CY_CORE_LIB_TARGET}
        ${P64_UTILS_LIB_S_TARGET}
        ${PSOC6PDL_LIB_S_TARGET}
    PRIVATE
        tfm_spm
        tfm_app_rot_partition_ps
)

target_compile_definitions(tfm_app_rot_partition_ps
    PRIVATE
        $<$<BOOL:${TFM_ENABLE_PS_OEM_UID}>:TFM_ENABLE_PS_OEM_UID>
)

# GNU ld has some strange behaviour to do with weak functions, and does not deal
# well with strong functions in static libraries overriding weak functions in
# object files. For this reason, the file mailbox_ipc_intr is linked directly to
# the s binary as an object file. This issue does not affect armclang, but the
# fix is compatible.
target_sources(tfm_s
    PRIVATE
        mailbox/mailbox_ipc_intr.c
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        mailbox/platform_multicore.c
        mailbox/platform_ns_mailbox.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/psoc6_system_init_cm4.c
        Device/Source/system_psoc6_cm4.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        Device/Config
        CMSIS_Driver/Config
)

target_compile_definitions(platform_ns
    PUBLIC
        DOMAIN_NS=1
        $<$<BOOL:${TEST_S}>:TEST_S>
)

target_link_libraries(platform_ns
    PUBLIC
        ${PSOC6PDL_LIB_NS_TARGET}
    PRIVATE
        ${P64_UTILS_LIB_NS_TARGET}
        ${CY_CORE_LIB_TARGET}
)

#===================== Non-Secure application interface =======================#

# interface library for TF-M non-secure application
add_library(tfm_app_ns_interface INTERFACE)

target_compile_definitions(tfm_app_ns_interface
    INTERFACE
        RAM_VECTORS_SUPPORT
        $<$<BOOL:${TFM_PSA_API}>:TFM_PSA_API>
        $<$<BOOL:${TFM_NS_CLIENT_IDENTIFICATION}>:TFM_NS_CLIENT_IDENTIFICATION>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:TFM_MULTI_CORE_TOPOLOGY>

        # Don't define TFM_MULTI_CORE_NS_OS - it should be defined by non-secure application
        #$<$<BOOL:${TFM_MULTI_CORE_NS_OS}>:TFM_MULTI_CORE_NS_OS>
        # Don't define TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD - it should be defined by non-secure application
        #$<$<AND:$<BOOL:${TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD}>,$<BOOL:${TFM_MULTI_CORE_NS_OS}>>:TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD>
)

target_link_libraries(tfm_app_ns_interface
    INTERFACE
        platform_ns
        platform_region_defs
        tfm_partition_defs
)

########################### export sources #####################################

set(INSTALL_MEMORY_LAYOUT_DIR    ${TFM_INSTALL_PATH}/memory_layout)

install(FILES       mailbox/platform_multicore.c
                    mailbox/platform_ns_mailbox.c
                    os_wrapper/os_wrapper_stub.c
                    os_wrapper/os_wrapper_cyabs_rtos.c
                    os_wrapper/os_wrapper_cyabs_rtos.c
        DESTINATION ${INSTALL_INTERFACE_SRC_DIR})

install(FILES       mailbox/platform_multicore.h
                    mailbox/ns_ipc_config.h
                    Device/Config/device_cfg.h
                    tfm_mbedtls_version.h
        DESTINATION ${INSTALL_INTERFACE_INC_DIR})

install(FILES       partition/flash_layout.h
                    partition/region_defs.h
        DESTINATION ${INSTALL_MEMORY_LAYOUT_DIR})

if (CY_INSTALL_NS_INTERFACE)
    install(FILES       Device/Source/armclang/psoc6_ns.sct
                        Device/Source/armclang/startup_psoc64_ns.s
            DESTINATION ${TFM_INSTALL_PATH}/startup/armclang)

    install(FILES       Device/Source/gcc/psoc6_ns.ld
                        Device/Source/gcc/startup_psoc64_ns.S
            DESTINATION ${TFM_INSTALL_PATH}/startup/gcc)

    install(FILES       Device/Source/iar/psoc6_ns.icf
                        Device/Source/iar/startup_psoc64_ns.s
            DESTINATION ${TFM_INSTALL_PATH}/startup/iar)
endif()
