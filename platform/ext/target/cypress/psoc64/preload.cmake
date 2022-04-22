#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# preload.cmake is used to set things that related to the platform that are both
# immutable and global, which is to say they should apply to any kind of project
# that uses this platform. In practice this is normally compiler definitions and
# variables related to hardware.

# Set architecture and CPU. Cypress PSoC64 runs the S code on its Cortex-m0+
set(TFM_SYSTEM_PROCESSOR cortex-m0plus)
set(TFM_SYSTEM_ARCHITECTURE armv6-m)

# Compile definitions that are global for the hardware
# Note that these do affect both S and NS builds

# Hardware platform
add_definitions(-DCYS0644ABZI_S2D44)

# Configure PDL to use RAM interrupt vectors. This is required by P64_utils
add_definitions(-DRAM_VECTORS_SUPPORT)

add_definitions(-DBOOT_DATA_AVAILABLE)

# How much SRAM to set aside for parsing the policy.
# The default value specified in p64_utils is sufficient to parse the policy
# as shipped. This value is sufficient to parse the policy as shipped with
# an additional "extclk" block.
# It may be necessary to increase this further if more entries are added to
# the policy used to provision the board.
if(DEFINED CY_P64_HEAP_DATA_SIZE)
    add_definitions(-DCY_P64_HEAP_DATA_SIZE=${CY_P64_HEAP_DATA_SIZE})
else()
    add_definitions(-DCY_P64_HEAP_DATA_SIZE=0x4800U)
endif()

# Disable default PDL IPC configuration. Use Cy_Platform_Init() in
# in psoc6_system_init_cm4.c and psoc6_system_init_cm0p.c instead.
add_definitions(-DCY_IPC_DEFAULT_CFG_DISABLE)
