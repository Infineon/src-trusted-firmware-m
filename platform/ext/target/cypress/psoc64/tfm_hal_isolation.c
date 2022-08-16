/*
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cy_device.h"
#include "target_cfg.h"
#include "tfm_api.h"
#include "tfm_hal_defs.h"
#include "tfm_multi_core.h"
#include "tfm_plat_defs.h"
#include "cy_utils.h"
#include "cy_p64_syscalls.h"
#include "cy_p64_image.h"
#include "utils/cy_policy.h"
#include "region_defs.h"
#include "cy_syslib.h"
#include "tfm_spm_hal.h"
#include "static_checks.h"


static void boot_tasks_handler(void)
{
    bool set_img_ok = true;
    cy_p64_error_codes_t status;
#ifdef CY_IMG_OK_CONFIG_FROM_POLICY
    cy_p64_cJSON *jsonPacket;
    enum tfm_plat_err_t err;
#endif

#ifdef CY_HW_SETTINGS_FROM_POLICY
    /* populate hw_settings structure from policy*/
    cy_get_policy_hw_settings(&hw_settings);
#else
    hw_settings.uart_base = SCB5_BASE;
    hw_settings.uart_enabled = true;
    hw_settings.cm4_ap_enabled = true;
    hw_settings.debug_window = 100;
#endif

    if (hw_settings.cm4_ap_enabled) {
        /* Try to enable CM4 DAP */
        if (cy_p64_access_port_control(CY_P64_CM4_AP, CY_P64_AP_EN) == CY_P64_SUCCESS) {
            /* The delay is required after Access port was enabled for
            * debugger/programmer to connect and set TEST BIT */
            Cy_SysLib_Delay(hw_settings.debug_window);
        }
    }

    if(CY_P64_IS_TEST_MODE_SET)
    {
        Cy_SysEnableCM4(CY_P64_CM4_ROM_LOOP_ADDR);
        Cy_SysLib_Delay(1);
        CPUSS->CM4_VECTOR_TABLE_BASE = tfm_spm_hal_get_ns_VTOR();
        status = cy_p64_acquire_response();
        if(status == CY_P64_SUCCESS)
        {
            TFM_COVERITY_DEVIATE_LINE(MISRA_C_2012_Rule_2_2, "Used for tests")        
            cy_p64_acquire_test_bit_loop();
        }
        Cy_SysDisableCM4();
    }

#ifdef CY_IMG_OK_CONFIG_FROM_POLICY
    /* Read the setting from policy */
    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        ERROR_MSG("Failed to get policy jwt packet.\r\n");
        tfm_core_panic();
    }

    status = cy_p64_policy_get_image_boot_config(jsonPacket, 1, NULL,
                                                 NULL, &set_img_ok);
#else
    status = CY_P64_SUCCESS;
#endif
    if (status == CY_P64_SUCCESS) {
        if (set_img_ok) {
            /* Write Image OK flag to the slot trailer */
            cy_p64_confirm_image((FLASH_BASE_ADDRESS + SECURE_IMAGE_OFFSET),
                                      FLASH_S_PARTITION_SIZE);
        }

    }
}

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(void)
{
    Cy_PDL_Init(CY_DEVICE_CFG);

    boot_tasks_handler();

    if (smpu_init_cfg() != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    if (ppu_init_cfg() != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    if (bus_masters_cfg() != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_memory_has_access(uintptr_t base,
                                                size_t size,
                                                uint32_t attr)
{
    enum tfm_status_e status;

    status = tfm_has_access_to_region((const void *)base, size, attr);
    if (status != TFM_SUCCESS) {
         return TFM_HAL_ERROR_MEM_FAULT;
    }

    return TFM_HAL_SUCCESS;
}
