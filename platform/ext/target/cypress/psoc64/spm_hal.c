/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <string.h>

#include "tfm_spm_hal.h"
#include "tfm_hal_platform.h"
#include "tfm_memory_utils.h"

#include "device_definition.h"
#include "ps_object_defs.h"
#include "region_defs.h"
#include "utilities.h"
#include "spe_ipc_config.h"
#include "target_cfg.h"
#include "tfm_multi_core.h"
#include "tfm_platform_core_api.h"
#include "tfm_hal_its.h"
#include "tfm_hal_ps.h"
#include "log/tfm_log.h"

#include "cy_device.h"
#include "cy_device_headers.h"
#include "cy_ipc_drv.h"
#include "cy_ipc_sema.h"
#include "cy_prot.h"
#include "cy_pra.h"
#include "cy_pra_cfg.h"
#include "pc_config.h"
#include "driver_smpu.h"
#include "region.h"
#include "utils/cy_policy.h"
#include "cy_syspm.h"
#include "cy_p64_watchdog.h"
#include "static_checks.h"

#ifdef CY_SYSPM_STAT_ENABLE
#include "cy_mcwdt.h"
uint32_t g_system_deep_sleep = 0;
static uint32_t g_cm0_deep_sleep = 0;
static uint32_t g_cm0_deep_sleep_success = 0;
static uint32_t g_max_sleep_time = 0;
static uint32_t g_min_sleep_time = 0xffffffff;
static uint32_t g_sleep_time = 0;
#endif

#ifndef CY_EXTCLK_CONFIG_FROM_POLICY
/*
 * In this case, the values in the struct will need to be set appropriately.
 * Note that the pointer values should be set to point into GPIO->PRT[].
 */
const
#endif
static cy_stc_pra_extclk_policy_t ext_clk_policy = {
    .extClkEnable = false,
    .ecoEnable = false,
    .wcoEnable = false,
    .bypassEnable = false,

    /* EXTCLK Configuration */
    .extClkFreqHz = 0,
    .extClkPort = NULL,
    .extClkPinNum = 0,

    /* ECO Configuration */
    .ecoFreqHz = 0,
    .ecoLoad = 0,
    .ecoEsr = 0,
    .ecoDriveLevel = 0,
    .ecoInPort = NULL,
    .ecoOutPort = NULL,
    .ecoInPinNum = 0,
    .ecoOutPinNum = 0,

    /* WCO Configuration */
    .wcoInPort = NULL,
    .wcoOutPort = NULL,
    .wcoInPinNum = 0,
    .wcoOutPinNum = 0,
};

/* Get address of memory regions to configure MPU */
TFM_COVERITY_DEVIATE_LINE(MISRA_C_2012_Rule_8_5, "Used for all platforms")
extern struct memory_region_limits memory_regions;

static enum tfm_plat_err_t handle_boot_wdt(void);


enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
        uint32_t partition_idx,
        const struct platform_data_t *platform_data)
{
    (void) partition_idx; /* Unused parameter */
    if (!platform_data) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

uint32_t tfm_spm_hal_get_ns_VTOR(void)
{
    return memory_regions.non_secure_code_start;
}

uint32_t tfm_spm_hal_get_ns_MSP(void)
{
    return *((uint32_t *)memory_regions.non_secure_code_start);
}

uint32_t tfm_spm_hal_get_ns_entry_point(void)
{
    return *((uint32_t *)(memory_regions.non_secure_code_start+ 4));
}

static void configure_pra_extclk(void)
{
#ifdef CY_EXTCLK_CONFIG_FROM_POLICY
    enum tfm_plat_err_t err;
    cy_p64_cJSON *jsonPacket = NULL;
    const cy_p64_cJSON *json;
    uint32_t port_num;

    /* Read any extclk configuration from the policy */
    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        ERROR_MSG("Failed to get policy jwt packet.\r\n");
        tfm_core_panic();
    }

    json = cy_p64_find_json_item("extclk", jsonPacket);
    if (!json) {
        /* No extclk block, so we're done */
        return;
    }

    /* Read each possible entry in the block */
    cy_policy_get_optional_bool("extclk/extClkEnable", jsonPacket,
                                &ext_clk_policy.extClkEnable);
    cy_policy_get_optional_bool("extclk/ecoEnable", jsonPacket,
                                &ext_clk_policy.ecoEnable);
    cy_policy_get_optional_bool("extclk/wcoEnable", jsonPacket,
                                &ext_clk_policy.wcoEnable);
    cy_policy_get_optional_bool("extclk/bypassEnable", jsonPacket,
                                  &ext_clk_policy.bypassEnable);
    cy_policy_get_optional_uint32("extclk/extClkFreqHz", jsonPacket,
                                  &ext_clk_policy.extClkFreqHz);
    if (cy_policy_get_optional_uint32("extclk/extClkPort", jsonPacket,
                                      &port_num)) {
        ext_clk_policy.extClkPort = &GPIO->PRT[port_num];
    }
    cy_policy_get_optional_uint32("extclk/extClkPinNum", jsonPacket,
                                  &ext_clk_policy.extClkPinNum);
    cy_policy_get_optional_uint32("extclk/ecoFreqHz", jsonPacket,
                                  &ext_clk_policy.ecoFreqHz);
    cy_policy_get_optional_uint32("extclk/ecoLoad", jsonPacket,
                                  &ext_clk_policy.ecoLoad);
    cy_policy_get_optional_uint32("extclk/ecoEsr", jsonPacket,
                                  &ext_clk_policy.ecoEsr);
    cy_policy_get_optional_uint32("extclk/ecoDriveLevel", jsonPacket,
                                  &ext_clk_policy.ecoDriveLevel);
    if (cy_policy_get_optional_uint32("extclk/ecoInPort", jsonPacket,
                                      &port_num)) {
        ext_clk_policy.ecoInPort = &GPIO->PRT[port_num];
    }
    if (cy_policy_get_optional_uint32("extclk/ecoOutPort", jsonPacket,
                                      &port_num)) {
        ext_clk_policy.ecoOutPort = &GPIO->PRT[port_num];
    }
    cy_policy_get_optional_uint32("extclk/ecoInPinNum", jsonPacket,
                                  &ext_clk_policy.ecoInPinNum);
    cy_policy_get_optional_uint32("extclk/ecoOutPinNum", jsonPacket,
                                  &ext_clk_policy.ecoOutPinNum);
    if (cy_policy_get_optional_uint32("extclk/wcoInPort", jsonPacket,
                                      &port_num)) {
        ext_clk_policy.wcoInPort = &GPIO->PRT[port_num];
    }
    if (cy_policy_get_optional_uint32("extclk/wcoOutPort", jsonPacket,
                                      &port_num)) {
        ext_clk_policy.wcoOutPort = &GPIO->PRT[port_num];
    }
    cy_policy_get_optional_uint32("extclk/wcoInPinNum", jsonPacket,
                                  &ext_clk_policy.wcoInPinNum);
    cy_policy_get_optional_uint32("extclk/wcoOutPinNum", jsonPacket,
                                  &ext_clk_policy.wcoOutPinNum);
#endif /* CY_EXTCLK_CONFIG_FROM_POLICY */

    /* Inform PRA about the extclk configuration */
    extClkPolicyPtr = &ext_clk_policy;
    Cy_PRA_UpdateExtClockRegIndex();
}

void tfm_spm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    smpu_print_config();

    /* Reset boot watchdog */
    handle_boot_wdt();

    /* Parse any external clock settings in the policy */
    configure_pra_extclk();

    LOG_MSG("Starting Cortex-M4 at 0x%X\r\n", start_addr);

    while(!Cy_SCB_UART_IsTxComplete(SCB5))
    {
        /* Wait until UART transmission complete */
    }

    Cy_SysEnableCM4(start_addr);
}

void tfm_spm_hal_wait_for_ns_cpu_ready(void)
{
    uint32_t data;
    cy_en_ipcdrv_status_t status;

    Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(IPC_RX_INTR_STRUCT),
                                0, IPC_RX_INT_MASK);

    status = Cy_IPC_Drv_SendMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(IPC_TX_CHAN),
                                 IPC_TX_NOTIFY_MASK, IPC_SYNC_MAGIC);

    if (status != CY_IPC_DRV_SUCCESS) {
        ERROR_MSG("Failed to send IPC message.\r\n");
        tfm_core_panic();
    }

    while (1)
    {
        status = (cy_en_ipcdrv_status_t) Cy_IPC_Drv_GetInterruptStatusMasked(
                        Cy_IPC_Drv_GetIntrBaseAddr(IPC_RX_INTR_STRUCT));
        status >>= CY_IPC_NOTIFY_SHIFT;
        if (status & IPC_RX_INT_MASK) {
            Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(
                                      IPC_RX_INTR_STRUCT),
                                      0, IPC_RX_INT_MASK);

            status = Cy_IPC_Drv_ReadMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(
                                            IPC_RX_CHAN),
                                            &data);
            if (status == CY_IPC_DRV_SUCCESS) {
                Cy_IPC_Drv_ReleaseNotify(Cy_IPC_Drv_GetIpcBaseAddress(IPC_RX_CHAN),
                                         IPC_RX_RELEASE_MASK);
                if (data == ~IPC_SYNC_MAGIC) {
                    LOG_MSG("\n\rCores sync success.\r\n");
                    break;
                }
            }
        }
    }
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line,
                                                        uint32_t priority)
{
    uint32_t quantized_priority = priority >> (8U - __NVIC_PRIO_BITS);
    NVIC_SetPriority(irq_line, quantized_priority);

    return TFM_PLAT_ERR_SUCCESS;
}

void tfm_spm_hal_get_mem_security_attr(const void *p, size_t s,
                                       struct security_attr_info_t *p_attr)
{
    /* System reserved memory (SysCall) */
    if (check_address_range(p, s,
                            SYS_RAM_START, SYS_RAM_LIMIT) == TFM_SUCCESS) {
        p_attr->is_valid = true;
        p_attr->is_secure = true;
        return;
    }

    /* Otherwise rely on SPM */
    tfm_get_mem_region_security_attr(p, s, p_attr);
}

void tfm_spm_hal_get_secure_access_attr(const void *p, size_t s,
                                        struct mem_attr_info_t *p_attr)
{
    uint32_t pc;
    struct mem_attr_info_t smpu_attr;

    SMPU_Get_Access_Rules(p, s, CY_PROT_SPM_DEFAULT, &smpu_attr);

    tfm_get_secure_mem_region_attr(p, s, p_attr);

    pc = Cy_Prot_GetActivePC(CPUSS_MS_ID_CM0);
    /* Check whether the current active PC is configured as the expected one .*/
    if (pc == CY_PROT_SPM_DEFAULT) {
        p_attr->is_mpu_enabled = true;
    } else {
        p_attr->is_mpu_enabled = false;
    }

    /* System reserved memory (SysCall) */
    if (check_address_range(p, s,
                            SYS_RAM_START,
                            SYS_RAM_LIMIT) == TFM_SUCCESS) {
        p_attr->is_valid = true;
        p_attr->is_priv_rd_allow = true;
        p_attr->is_priv_wr_allow = false;
        p_attr->is_unpriv_rd_allow = false;
        p_attr->is_unpriv_wr_allow = false;
        p_attr->is_xn = true;
    }

    combine_mem_attr(p_attr, &smpu_attr);
}

void tfm_spm_hal_get_ns_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    struct mem_attr_info_t smpu_attr;

    SMPU_Get_Access_Rules(p, s, CY_PROT_HOST_DEFAULT, &smpu_attr);

    tfm_get_ns_mem_region_attr(p, s, p_attr);

    combine_mem_attr(p_attr, &smpu_attr);
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void)
{
    return nvic_interrupt_enable();
}

void tfm_spm_hal_clear_pending_irq(IRQn_Type irq_line)
{
    NVIC_ClearPendingIRQ(irq_line);
}

void tfm_spm_hal_enable_irq(IRQn_Type irq_line)
{
    NVIC_EnableIRQ(irq_line);
}

void tfm_spm_hal_disable_irq(IRQn_Type irq_line)
{
    NVIC_DisableIRQ(irq_line);
}

enum irq_target_state_t tfm_spm_hal_set_irq_target_state(
                                          IRQn_Type irq_line,
                                          enum irq_target_state_t target_state)
{
    (void)irq_line;
    (void)target_state;

    return TFM_IRQ_TARGET_STATE_SECURE;
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_target_state_cfg(void)
{
    return nvic_interrupt_target_state_cfg();
}

enum tfm_plat_err_t tfm_spm_hal_enable_fault_handlers(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_system_reset_cfg(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_init_debug(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_platform_init(void)
{
    platform_init();

#if defined(CY_DEVICE_SECURE)
    /* Initialize Protected Register Access driver. */
    Cy_PRA_Init();
#endif /* defined(CY_DEVICE_SECURE) */

    /* Merge note: do NOT call mock_tfm_shared_data() */

    __enable_irq();
    stdio_init();

    return TFM_HAL_SUCCESS;
}

#ifdef CY_FLASH_LAYOUT_FROM_POLICY
enum tfm_hal_status_t
tfm_hal_its_fs_info(struct tfm_hal_its_fs_info_t *fs_info)
{
    if (!fs_info) {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    fs_info->flash_area_addr = cy_policy_get_uint32(CY_ITS_ADDR_PATH);
    fs_info->flash_area_size = cy_policy_get_uint32(CY_ITS_SIZE_PATH);
    fs_info->sectors_per_block = TFM_HAL_ITS_SECTORS_PER_BLOCK;

    return TFM_HAL_SUCCESS;
}

uint32_t tfm_hal_its_max_asset_size(void)
{
    return cy_policy_get_uint32(CY_ITS_MAX_ASSET_SIZE_PATH);
}

uint32_t tfm_hal_its_max_num_assets(void)
{
    return cy_policy_get_uint32(CY_ITS_NUM_ASSETS_PATH);
}

enum tfm_hal_status_t
tfm_hal_ps_fs_info(struct tfm_hal_ps_fs_info_t *fs_info)
{
    if (!fs_info) {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    fs_info->flash_area_addr = cy_policy_get_uint32(CY_PS_ADDR_PATH);
    fs_info->flash_area_size = cy_policy_get_uint32(CY_PS_SIZE_PATH);
    fs_info->sectors_per_block = TFM_HAL_PS_SECTORS_PER_BLOCK;

    return TFM_HAL_SUCCESS;
}

uint32_t tfm_hal_ps_max_asset_size(void)
{
    return cy_policy_get_uint32(CY_PS_MAX_ASSET_SIZE_PATH);
}

uint32_t tfm_hal_ps_max_num_assets(void)
{
    return cy_policy_get_uint32(CY_PS_NUM_ASSETS_PATH);
}

void nvc_flash_block(uint32_t *base, size_t *size)
{
    if (!base || !size) {
        ERROR_MSG("Invalid function parameters.\r\n");
        tfm_core_panic();
    }

    *base = cy_policy_get_uint32(CY_NV_COUNTERS_ADDR_PATH);
    *size = cy_policy_get_uint32(CY_NV_COUNTERS_SIZE_PATH);
}
#endif /* CY_FLASH_LAYOUT_FROM_POLICY */

void tfm_nspe_flash_block(uint32_t *base, size_t *size)
{
#ifdef CY_FLASH_LAYOUT_FROM_POLICY
    uint32_t imageStart, imageSize;
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket;
    enum tfm_plat_err_t err;
#else
    uint32_t imageStart = NS_PARTITION_START;
    uint32_t imageSize = NS_PARTITION_SIZE;
#endif

    if (!base || !size) {
        ERROR_MSG("Invalid function parameters.\r\n");
        tfm_core_panic();
    }

#ifdef CY_FLASH_LAYOUT_FROM_POLICY
    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        ERROR_MSG("Failed to get policy jwt packet.\r\n");
        tfm_core_panic();
    }

    status = cy_p64_policy_get_image_address_and_size(jsonPacket, 16, "BOOT", &imageStart, &imageSize);
    if (status != CY_P64_SUCCESS) {
        ERROR_MSG("Failed to read NSPE image provisioned data.\r\n");
        tfm_core_panic();
    }
#endif

    *base = imageStart;
    *size = imageSize;
}

enum tfm_plat_err_t tfm_spm_hal_deep_sleep( void )
{
    cy_en_syspm_status_t status = CY_SYSPM_SUCCESS;
/* Disable deep sleep if secure IRQ test is enabled as TCPWM
 * timer interrupt can't be generated in deep sleep state
 */
#ifndef TFM_ENABLE_IRQ_TEST

#ifdef CY_SYSPM_STAT_ENABLE
    uint32_t before, after;
    MCWDT_STRUCT_Type const *mcwdt_base = MCWDT_STRUCT0;

    before = _FLD2VAL(MCWDT_STRUCT_MCWDT_CNTHIGH_WDT_CTR2, MCWDT_STRUCT_MCWDT_CNTHIGH(mcwdt_base));
#endif
    /* Put CPU in deep sleep mode */
    status = Cy_SysPm_DeepSleep( CY_SYSPM_WAIT_FOR_INTERRUPT );

#ifdef CY_SYSPM_STAT_ENABLE
    after = _FLD2VAL(MCWDT_STRUCT_MCWDT_CNTHIGH_WDT_CTR2, MCWDT_STRUCT_MCWDT_CNTHIGH(mcwdt_base));
    g_sleep_time = after - before;
    if (g_sleep_time > g_max_sleep_time) {
       g_max_sleep_time = g_sleep_time;
    }

    if (g_sleep_time < g_min_sleep_time) {
       g_min_sleep_time = g_sleep_time;
    }

    g_cm0_deep_sleep++;
    if (status == CY_SYSPM_SUCCESS) {
        g_cm0_deep_sleep_success++;
    }
#endif /* CY_SYSPM_STAT_ENABLE */

#endif /* TFM_ENABLE_IRQ_TEST */
    return ( (status == CY_SYSPM_SUCCESS) ?
             TFM_PLAT_ERR_SUCCESS : TFM_PLAT_ERR_SYSTEM_ERR );
}

static enum tfm_plat_err_t handle_boot_wdt(void)
{
    bool wdt_enable = true;
    uint32_t wdt_timeout = 5000;
#ifdef CY_WDT_CONFIG_FROM_POLICY
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket;
    enum tfm_plat_err_t err;
#endif

    /* Update watchdog timer to mark successfull start up of the image */
    if (cy_p64_wdg_is_enabled()) {
        cy_p64_wdg_stop();
        cy_p64_wdg_free();
        LOG_MSG("Disabled boot watchdog\r\n");
    }

#ifdef CY_WDT_CONFIG_FROM_POLICY
    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    status = cy_p64_policy_get_image_boot_config(jsonPacket, 1, &wdt_enable,
                                                 &wdt_timeout, NULL);

    if (status != CY_P64_SUCCESS) {
        ERROR_MSG("Failed to read BNU WDT from provisioning data.\r\n");
        /* just ignore and bail out */
        return TFM_PLAT_ERR_SUCCESS;
    }
#endif

    if (wdt_enable) {
        /* Initialize watchdog timer. It should be updated in the CM4 user app
        to mark successful start up. If watchdog timer is not updated, watchdog
        will reset the board. CYBL will attempt to roll back to the working
        image.
        */
        if (wdt_timeout > cy_p64_wdg_max_timeout_ms()) {
            wdt_timeout = cy_p64_wdg_max_timeout_ms();
            LOG_MSG("Adjusted watchdog timer to %d ms\r\n", wdt_timeout);
        }
        LOG_MSG("Re-start watchdog timer for %d ms\r\n", wdt_timeout);
        cy_p64_wdg_init(&wdt_timeout);
        cy_p64_wdg_start();
    }

    return TFM_PLAT_ERR_SUCCESS;
}
