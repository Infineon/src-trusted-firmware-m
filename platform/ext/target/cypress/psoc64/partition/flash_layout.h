/*
 * Copyright (c) 2017-2021 Arm Limited. All rights reserved.
 * Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* Flash layout:
 *
 * With CY_FLASH_LAYOUT_FROM_POLICY:
 *  0x1000_0000 Secure image primary (320K)
 *  0x100n_0000 Flash partitioned by provisioning data
 *              Provisioning data specifies how this block is partitioned
 *              between Non-secure image primary, Secure image secondary,
 *              Internal Trusted Storage, Secure Storage, non-volatile
 *              counters, and unused.
 *                1504K at 0x1005_0000 for PSA API test and regression test configs
 *                1568K at 0x1004_0000 otherwise
 *  0x101c_8000 Reserved (224 KB)
 *  0x101f_ffff End of Flash
 * Without CY_FLASH_LAYOUT_FROM_POLICY:
 *  0x1000_0000 Secure     image primary (320 KB)
 *  0x1005_0000 Non-secure image primary (1136 KB)
 *  0x1016_c000 Secure     image secondary (320 KB)
 *  0x101b_c000 Internal Trusted Storage Area (16 KB)
 *  0x101c_0000 Protected Storage Area (24 KB)
 *  0x101c_6000 Unused area (7 KB)
 *  0x101c_7c00 Non-volatile counters area (1 KB)
 *  0x101c_8000 Reserved (224 KB)
 *  0x101f_ffff End of Flash
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_base_address.h to access flash related defines. To resolve
 * this some of the values are redefined here with different names, these are
 * marked with comment.
 */

#ifdef BL2
#error "BL2 configuration is not supported"
#endif /* BL2 */

/* Even though TFM BL2 is excluded from the build,
 * CY BL built externally is used and it needs offsets for header and trailer
 * to be taken in account.
 * */
#define CYBL_HEADER_SIZE                (0x400)
#define CYBL_TRAILER_SIZE               (0x400)

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x200)      /* 512 B */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0x00200000) /* 2 MB */
#define FLASH_BASE_ADDRESS              (0x10000000U) /* same as FLASH0_BASE */

/* The location of S partition */
#define SECURE_IMAGE_OFFSET             0x0
/* The size of S partition */
#define FLASH_S_PARTITION_SIZE          0x50000      /* 320 KB */

/* This block is needed by the secure code when not using policy
 * to specify the flash layout, and always by the non-secure (test)
 * code (so the layout here needs to match that specified in the policy)
 */
#if !defined(CY_FLASH_LAYOUT_FROM_POLICY) || (DOMAIN_NS == 1)

#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE
/* The size of NS partition */
#define FLASH_NS_PARTITION_SIZE         0x11c000     /* 1136 KB */
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* NS primary comes after S primary */
#define NON_SECURE_IMAGE_OFFSET           (SECURE_IMAGE_OFFSET + \
                                           FLASH_S_PARTITION_SIZE)
#define NS_IMAGE_PRIMARY_PARTITION_OFFSET NON_SECURE_IMAGE_OFFSET

/* Data comes after S secondary, which is after NS primary */
#define FLASH_DATA_AREA_OFFSET          (NON_SECURE_IMAGE_OFFSET + \
                                         FLASH_NS_PARTITION_SIZE + \
                                         FLASH_S_PARTITION_SIZE)

/* Internal Trusted Storage Area */
#define FLASH_ITS_AREA_OFFSET           FLASH_DATA_AREA_OFFSET
#define FLASH_ITS_AREA_SIZE             (0x4000)    /* 16 KB */

/* Protected Storage Area */
#define FLASH_PS_AREA_OFFSET            (FLASH_ITS_AREA_OFFSET + \
                                         FLASH_ITS_AREA_SIZE)
#define FLASH_PS_AREA_SIZE              (0x6000)   /* 24 KB */

/* Unused Area */
#define FLASH_UNUSED_AREA_OFFSET        (FLASH_PS_AREA_OFFSET + \
                                         FLASH_PS_AREA_SIZE)
#define FLASH_UNUSED_AREA_SIZE          (0x1c00)   /* 7 KB */

/* Non-volatile Counters Area */
#define FLASH_NV_COUNTERS_AREA_OFFSET   (FLASH_UNUSED_AREA_OFFSET + \
                                         FLASH_UNUSED_AREA_SIZE)
#define FLASH_NV_COUNTERS_AREA_SIZE     (2 * FLASH_AREA_IMAGE_SECTOR_SIZE) /* 1 KB */

/* Reserved areas */
#define FLASH_AREA_SYSTEM_RESERVED_SIZE (0x38000) /* 224 KB */

/* Check if it fits into available Flash*/
#define FLASH_DATA_AREA_SIZE            (FLASH_ITS_AREA_SIZE + \
                                         FLASH_PS_AREA_SIZE + \
                                         FLASH_UNUSED_AREA_SIZE + \
                                         FLASH_NV_COUNTERS_AREA_SIZE + \
                                         FLASH_AREA_SYSTEM_RESERVED_SIZE)

#if (FLASH_DATA_AREA_OFFSET + FLASH_DATA_AREA_SIZE) > (FLASH_TOTAL_SIZE)
#error "Out of Flash memory"
#endif

#endif /* !defined(CY_FLASH_LAYOUT_FROM_POLICY) || (DOMAIN_NS == 1) */

/* Protected Storage (PS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M PS Integration Guide.
 */
#define TFM_HAL_PS_FLASH_DRIVER Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#ifdef CY_FLASH_LAYOUT_FROM_POLICY
/* PS base address, size, max asset size, and max number of assets all come
 * from provisioning data */
#else
/* Base address of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_ADDR    FLASH_PS_AREA_OFFSET
/* Size of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_SIZE    FLASH_PS_AREA_SIZE
#define PS_RAM_FS_SIZE                TFM_HAL_PS_FLASH_AREA_SIZE
#endif
#define TFM_HAL_PS_SECTOR_SIZE          FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_PS_SECTORS_PER_BLOCK    (4)
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_PS_PROGRAM_UNIT         (0x1)

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide. The ITS should be in the internal flash, but is
 * allocated in the external flash just for development platforms that don't
 * have internal flash available.
 */
#define TFM_HAL_ITS_FLASH_DRIVER Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#ifdef CY_FLASH_LAYOUT_FROM_POLICY
/* ITS base address, size, max asset size, and max number of assets all come
 * from provisioning data */
#else
/* Base address of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_ADDR    FLASH_ITS_AREA_OFFSET
/* Size of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_SIZE    FLASH_ITS_AREA_SIZE
#define ITS_RAM_FS_SIZE                TFM_HAL_ITS_FLASH_AREA_SIZE
#endif
#define TFM_HAL_ITS_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of physical erase sectors per logical FS block */
/* Merge node: CY's modification: 4508ca674dd1 */
#define TFM_HAL_ITS_SECTORS_PER_BLOCK   (4)
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_ITS_PROGRAM_UNIT        (0x1)
/* Decrease flash wear slightly, at the cost of increased ITS service memory */
#define ITS_MAX_BLOCK_DATA_COPY (512)

/* NV Counters definitions */
#define TFM_NV_COUNTERS_FLASH_DEV       Driver_FLASH0
#ifndef CY_FLASH_LAYOUT_FROM_POLICY
#define TFM_NV_COUNTERS_AREA_SIZE    FLASH_NV_COUNTERS_AREA_SIZE
#define TFM_NV_COUNTERS_SECTOR_ADDR  FLASH_NV_COUNTERS_AREA_OFFSET
#endif
#define TFM_NV_COUNTERS_SECTOR_SIZE     FLASH_AREA_IMAGE_SECTOR_SIZE

/* Use Flash to store Code data */
/* Same as FLASH_BASE_ADDRESS */
#define S_ROM_ALIAS_BASE  (0x10000000)
#define NS_ROM_ALIAS_BASE (0x10000000)

/* Use SRAM to store RW data */
#define S_RAM_ALIAS_BASE  (0x08000000)
#define NS_RAM_ALIAS_BASE (0x08000000)

#endif /* __FLASH_LAYOUT_H__ */
