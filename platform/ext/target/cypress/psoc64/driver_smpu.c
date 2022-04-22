/*
 * Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 * Copyright (c) 2021, Arm Limited. All rights reserved.
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
#include <inttypes.h>

#include "driver_smpu.h"
#include "nv_counters.h"
#include "pc_config.h"
#include "region_defs.h"
#include "RTE_Device.h"
#include "smpu_config.h"
#include "log/tfm_log.h"
#include "tfm_hal_its.h"
#include "tfm_hal_ps.h"
#include "tfm_memory_utils.h"
#include "tfm_spm_hal.h"
#include "static_checks.h"

#include "cy_prot.h"

/* Affect all 8 subregions */
#define ALL_ENABLED 0

struct smpu_resources {
    PROT_SMPU_SMPU_STRUCT_Type *smpu;
    cy_stc_smpu_cfg_t slave_config;
    cy_stc_smpu_cfg_t master_config;
};

static const char * smpu_name(const SMPU_Resources *smpu_dev)
{
    switch ((int)smpu_dev->smpu) {
    case (int)PROT_SMPU_SMPU_STRUCT0:
        return "SMPU 0";
    case (int)PROT_SMPU_SMPU_STRUCT1:
        return "SMPU 1";
    case (int)PROT_SMPU_SMPU_STRUCT2:
        return "SMPU 2";
    case (int)PROT_SMPU_SMPU_STRUCT3:
        return "SMPU 3";
    case (int)PROT_SMPU_SMPU_STRUCT4:
        return "SMPU 4";
    case (int)PROT_SMPU_SMPU_STRUCT5:
        return "SMPU 5";
    case (int)PROT_SMPU_SMPU_STRUCT6:
        return "SMPU 6";
    case (int)PROT_SMPU_SMPU_STRUCT7:
        return "SMPU 7";
    case (int)PROT_SMPU_SMPU_STRUCT8:
        return "SMPU 8";
    case (int)PROT_SMPU_SMPU_STRUCT9:
        return "SMPU 9";
    case (int)PROT_SMPU_SMPU_STRUCT10:
        return "SMPU 10";
    case (int)PROT_SMPU_SMPU_STRUCT11:
        return "SMPU 11";
    case (int)PROT_SMPU_SMPU_STRUCT12:
        return "SMPU 12";
    case (int)PROT_SMPU_SMPU_STRUCT13:
        return "SMPU 13";
    case (int)PROT_SMPU_SMPU_STRUCT14:
        return "SMPU 14";
    case (int)PROT_SMPU_SMPU_STRUCT15:
        return "SMPU 15";
    default:
        return "Unrecognised SMPU";
    }
}

static bool is_runtime(const SMPU_Resources *smpu_dev)
{
    if ((smpu_dev->slave_config.address == SMPU_DYNAMIC_BASE) &&
        (smpu_dev->slave_config.regionSize == SMPU_DYNAMIC_REGIONSIZE) &&
        (smpu_dev->slave_config.subregions == SMPU_DYNAMIC_SUBREGIONS)) {
        return true;
        }
    return false;
}

static bool is_whole_power_of_two(size_t size)
{
    TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "Possible wrap is accounted for by the algorithm")
    return ((size - 1) & size) == 0;
}

static bool is_aligned(uint32_t base, size_t size)
{
    return (base % size) == 0;
}

/* size must be a whole power of two, >= 4 */
static cy_en_prot_size_t bytes_to_regionsize(size_t size)
{
    int ret = 1;

    while (REGIONSIZE_TO_BYTES(ret) < size) {
        ret += 1;
    }
    return (cy_en_prot_size_t)ret;
}

static uint32_t round_up_to_power_of_two(uint32_t x)
{
    TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "Possible wrap is accounted for by the algorithm")
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "Possible wrap is accounted for by the algorithm")
    x++;
    return x;
}

static uint32_t round_down_to_power_of_two(uint32_t x)
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "Possible wrap is accounted for by the algorithm")
    x = x - (x >> 1);
    return x;
}

static uint32_t round_down_to_multiple(uint32_t base, size_t regionSize)
{
    TFM_COVERITY_FP_LINE(cert_int30_c, "Reviewed, wrap isn't possible")
    return (regionSize * (base / regionSize));
}

/* Maps 0..7 to CY_PROT_SUBREGION_DIS0..CY_PROT_SUBREGION_DIS7 */
static uint32_t subregion_num_to_mask(int num) {
    TFM_COVERITY_DEVIATE_LINE(cert_int31_c, "Operands are limited to 8-bit values")
    return (uint32_t)CY_PROT_SUBREGION_DIS0 << (unsigned int)num;
}

#define MIN_REGIONSIZE 256
/* The largest regionSize is actually 4GB, but that doesn't fit in a size_t */
#define MAX_REGIONSIZE (1 * 1024 * 1024 * 1024)
#define NUM_SUBREGIONS 8

static cy_en_prot_status_t calc_smpu_params(uint32_t base,
                                            size_t size,
                                            cy_stc_smpu_cfg_t *slave_config)
{
    /* Simplest case - base is a multiple of the size,
     * and size is a whole power of two, and >= 256
     */
    if (is_whole_power_of_two(size) &&
        is_aligned(base, size) &&
        (size >= MIN_REGIONSIZE)) {
        slave_config->address = (void *)base;
        slave_config->regionSize = bytes_to_regionsize(size);
        slave_config->subregions = ALL_ENABLED;

        return CY_PROT_SUCCESS;
    } else {
        /* Try to find a regionSize that could work */
        size_t low_rs = round_up_to_power_of_two(size);
        size_t high_rs = round_down_to_power_of_two(size * NUM_SUBREGIONS);
        size_t regionSize;

        if (low_rs < MIN_REGIONSIZE) {
            low_rs = MIN_REGIONSIZE;
        }

        if (size > MAX_REGIONSIZE / NUM_SUBREGIONS) {
            high_rs = MAX_REGIONSIZE;
        }

        for (regionSize = low_rs; regionSize <= high_rs; regionSize <<= 1)
        {
            const size_t sub_size = regionSize / NUM_SUBREGIONS;
            uint32_t address;
            int n;
            uint32_t mask = 0;

            /* Check whether base is aligned with sub_size */
            if (base % sub_size) {
                /*
                 * If we get here, we failed. That's because:
                 * value of base is never changed
                 * sub_size is always a power of 2 and is only getting bigger in each cycle
                 * so if base is not divisible by smaller power of 2 it is
                 * impossible that it will be divisible by bigger powers of 2
                 */
                return CY_PROT_FAILURE;
            }

            /* Calculate number of subregions needed to cover full size */
            if ((size % sub_size) != 0) {
                continue;
            }
            n = size / sub_size;
            if (n > NUM_SUBREGIONS) {
                continue;
            }

            /* Find the start address of SMPU region in which base is located */
            address = round_down_to_multiple(base, regionSize);

            /* Check whether single SMPU region covers whole region that should be protected */
            TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "The addition operation is performed on static values")
            if (address + regionSize < base + size) {
                continue;
            }

            /* Calculate the SMPU config */
            slave_config->address = (void *)address;
            slave_config->regionSize = bytes_to_regionsize(regionSize);

            /* Figure out which subregions to disable */
            for (int num = 0; num < NUM_SUBREGIONS; num += 1) {
                if (address + num * sub_size < base) {
                    /* Disable this subregion */
                    mask |= subregion_num_to_mask(num);
                } else if (address + num * sub_size >= base + size) {
                    /* Disable this subregion */
                    mask |= subregion_num_to_mask(num);
                }
            }
            TFM_COVERITY_DEVIATE_LINE(cert_int31_c, "The mask variable value is limited to 8-bit value")
            slave_config->subregions = mask;

            return CY_PROT_SUCCESS;
        }

        /* If we get here, we failed */
        return CY_PROT_FAILURE;
    }
}

static cy_en_prot_status_t get_region(const PROT_SMPU_SMPU_STRUCT_Type *smpu,
                                      uint32_t *base, size_t *size)
{
    cy_en_prot_status_t ret = CY_PROT_FAILURE;

    /* Figure out the base, size, and subregion mask to use */
    if (smpu == ITS_SMPU_STRUCT) {
        struct tfm_hal_its_fs_info_t its_fs_info;
        /* Retrieve the ITS region definition */
        if (tfm_hal_its_fs_info(&its_fs_info) == TFM_HAL_SUCCESS) {
            *base = its_fs_info.flash_area_addr;
            *size = its_fs_info.flash_area_size;
            ret = CY_PROT_SUCCESS;
        }
    } else if (smpu == NVC_SMPU_STRUCT) {
        /* Retrieve the non-volatile counters region definition */
        nvc_flash_block(base, size);
        ret = CY_PROT_SUCCESS;

    } else if (smpu == PS_SMPU_STRUCT) {
        struct tfm_hal_ps_fs_info_t ps_fs_info;
        /* Retrieve the PS region definition */
        if (tfm_hal_ps_fs_info(&ps_fs_info) == TFM_HAL_SUCCESS) {
            *base = ps_fs_info.flash_area_addr;
            *size = ps_fs_info.flash_area_size;
            ret = CY_PROT_SUCCESS;
        }
    }

    if (ret == CY_PROT_SUCCESS) {
        /* flash driver uses offsets rather than absolute addresses,
         * so we need to add the flash base address here.
         */
        TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "The addition operation is performed on static values")
        *base += FLASH_BASE_ADDRESS;
    }

    return ret;
}

static cy_en_prot_status_t populate_region(const PROT_SMPU_SMPU_STRUCT_Type *smpu,
                                           cy_stc_smpu_cfg_t *slave_config)
{
    cy_en_prot_status_t ret;
    uint32_t base;
    size_t size;

    ret = get_region(smpu, &base, &size);

    if (ret == CY_PROT_SUCCESS) {
        /* And figure out how to configure the SMPU region */
        ret = calc_smpu_params(base, size, slave_config);
    }

    return ret;
}

static void print_smpu_config(const char *name,
                              const cy_stc_smpu_cfg_t *slave_config)
{
    LOG_MSG("%s - address = %p, size = 0x%x bytes, %s subregions enabled\r\n",
           name,
           slave_config->address,
           REGIONSIZE_TO_BYTES(slave_config->regionSize),
           slave_config->subregions == ALL_ENABLED ? "all" : "some");
    if (slave_config->subregions != ALL_ENABLED) {
        LOG_MSG("\tsubregion size = 0x%x bytes\r\n",
                REGIONSIZE_TO_BYTES(slave_config->regionSize)/8);
        for (int i=0; i<8; i++) {
            LOG_MSG("\tsubregion %d %s\r\n",
                    i,
                    slave_config->subregions & (1<<i) ? "disabled" : "enabled");
        }
    }
}

static cy_en_prot_status_t SMPU_Read_Region(const PROT_SMPU_SMPU_STRUCT_Type *smpu,
                                            uint32_t *address,
                                            uint32_t *size,
                                            uint32_t *subregions,
                                            uint32_t *att0_reg)
{
    uint32_t reg = smpu->ATT0;

    /* Return a copy of ATT0 if requested */
    if (att0_reg) {
        *att0_reg = reg;
    }

    if (!_FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_ENABLED, reg)) {
        /* Disabled SMPU */
        return CY_PROT_FAILURE;
    }

    *size = REGIONSIZE_TO_BYTES(_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT0_REGION_SIZE, reg));
    reg = smpu->ADDR0;
    *subregions = _FLD2VAL(PROT_SMPU_SMPU_STRUCT_ADDR0_SUBREGION_DISABLE, reg);
    *address = _FLD2VAL(PROT_SMPU_SMPU_STRUCT_ADDR0_ADDR24, reg) << 8;
    /* Some address bits may be ignored */
    *address -= *address % *size;
    return CY_PROT_SUCCESS;
}

static void dump_smpu(const PROT_SMPU_SMPU_STRUCT_Type *smpu)
{
    uint32_t base;
    size_t size;
    uint32_t size32;
    uint32_t subregions;

    if (CY_PROT_SUCCESS == get_region(smpu, &base, &size)) {
        LOG_MSG(" base = 0x%x, size = 0x%x\r\n", base, size);
    } else {
        LOG_MSG(" Unsupported dynamic SMPU region\r\n");
    }

    if (SMPU_Read_Region(smpu, &base, &size32, &subregions, NULL) == CY_PROT_SUCCESS) {
        LOG_MSG("\tAddress = 0x%x", base);
        LOG_MSG(", size = 0x%x bytes", size32);
        LOG_MSG(", %s subregions enabled\r\n",
                subregions == ALL_ENABLED ? "all" : "some");
        if (subregions != ALL_ENABLED) {
            LOG_MSG("\tsubregion size = 0x%x bytes\r\n", size32/8);
            for (unsigned int i = 0UL; i < 8UL; i++) {
                LOG_MSG("\tsubregion %u %s\r\n",
                        i,
                        (subregions & (1UL<<i)) == 0U ? "disabled" : "enabled");
            }
        }
    } else {
        LOG_MSG("SMPU slave is disabled\r\n");
    }
}

static void set_to_deny_all_access(struct mem_attr_info_t *p_attr)
{
    p_attr->is_mpu_enabled = true;
    p_attr->is_valid = true;
    p_attr->is_xn = true;
    p_attr->is_priv_rd_allow = false;
    p_attr->is_priv_wr_allow = false;
    p_attr->is_unpriv_rd_allow = false;
    p_attr->is_unpriv_wr_allow = false;
}

static void smpu_att0_to_tfm_attr(uint32_t att0, struct mem_attr_info_t *p_attr)
{
    p_attr->is_mpu_enabled = true;
    p_attr->is_valid = true;
    /* SMPU has separate PX and UX bits. Here we ignore UX */
    p_attr->is_xn = !_FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_PX, att0);
    p_attr->is_priv_rd_allow = _FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_PR, att0);
    p_attr->is_priv_wr_allow = _FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_PW, att0);
    p_attr->is_unpriv_rd_allow = _FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_UR, att0);
    p_attr->is_unpriv_wr_allow = _FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_UW, att0);
}

/*
 * For the given Protection Context, check whether the specified
 * SMPU controls access to the specified memory range for the specified PC.
 * If so, set *p_attr accordingly.
 */
enum smpu_coverage_t {
    NOT_COVERED,
    PARTLY_COVERED,
    FULLY_COVERED
};
static enum smpu_coverage_t SMPU_Region_Coverage(const void *p, size_t s,
                                                 enum cy_en_prot_pc_t pc,
                                                 PROT_SMPU_SMPU_STRUCT_Type *smpu,
                                                 struct mem_attr_info_t *p_attr)
{
    bool pc_mismatch = false;
    uint32_t address;
    uint32_t size;
    uintptr_t limit;
    uint32_t subregions;
    uint32_t att0;
    cy_en_prot_status_t status = SMPU_Read_Region(smpu,
                                                  &address,
                                                  &size,
                                                  &subregions,
                                                  &att0);
    if (status != CY_PROT_SUCCESS) {
        /* Disabled SMPU */
        return NOT_COVERED;
    }

    /* Check PC */
    if ((_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT0_PC_MASK_15_TO_1, att0)
        & (CY_PROT_PCMASK1 << (pc - CY_PROT_PC1))) == 0U) {
        /* pc does not match the mask */
        if (_FLD2BOOL(PROT_SMPU_SMPU_STRUCT_ATT0_PC_MATCH, att0)) {
            /* This SMPU neither allows nor denies access for this PC */
            return NOT_COVERED;
        }
        /* In this case we need to check whether the address range is covered */
        pc_mismatch = true;
    }

    /* Adjust address and size to allow for subregions */
    if (subregions != ALL_ENABLED) {
        const uint32_t subregion_size = size / NUM_SUBREGIONS;
        uint32_t mask = 0;

        /* Check for disabled subregions that shift the protected base address up */
        for (int i = 0; i < NUM_SUBREGIONS; i++) {
            if (subregions & subregion_num_to_mask(i)) {
                address += subregion_size;
                size -= subregion_size;
                mask |= subregion_num_to_mask(i);
            } else {
                break;
            }
        }

        if (!size) {
            /* All subregions are disabled, so this SMPU is effectively disabled */
            return NOT_COVERED;
        }

        /* Check for disabled subregions that lower the top of the protected range */
        for (int i = NUM_SUBREGIONS - 1; i >= 0; i--) {
            if (subregions & subregion_num_to_mask(i)) {
                size -= subregion_size;
                mask |= subregion_num_to_mask(i);
            } else {
                break;
            }
        }

        /* It is possible to specify a subregion mask that creates multiple
         * discontiguous covered regions. Handling that would a significant
         * amount of code, and it's unclear whether there is a use case for
         * such SMPU configurations, so here we detect that situation and
         * panic.
         */
        if (mask != subregions) {
            tfm_core_panic();
        }
    }

    limit = address + size - 1;

    /* Now check the address range */
    if (check_address_range(p, s, address, limit) == TFM_SUCCESS) {
        if (pc_mismatch) {
            /* Access denied - PC doesn't match */
            set_to_deny_all_access(p_attr);
        }  else {
            smpu_att0_to_tfm_attr(att0, p_attr);
        }
        return FULLY_COVERED;
    }

    /* Check whether the region to be checked is partially covered by this SMPU.
     * We already know that it isn't fully covered
     */
    /* Before we go further, check that "p + s - 1" won't over- or under-flow,
     * so we don't return the wrong attributes (even though the core TFM code
     * will still deny access in those cases)
     */
    if ((s == 0U) || ((uintptr_t)p > UINTPTR_MAX - s)) {
        /* Just deny access altogether */
        set_to_deny_all_access(p_attr);
        return FULLY_COVERED;
    }
    /* Partly covered if p <= limit and address <= p + s - 1 */
    if (((uintptr_t)p <= limit) && (address <= (uintptr_t)p + s - 1)) {
        if (pc_mismatch) {
            /* Access denied - PC doesn't match */
            set_to_deny_all_access(p_attr);
        }  else {
            smpu_att0_to_tfm_attr(att0, p_attr);
        }
        return PARTLY_COVERED;
    }

    return NOT_COVERED;
}

/* API functions */

void combine_mem_attr(struct mem_attr_info_t *p_attr,
                      const struct mem_attr_info_t *p_attr2)
{
    /* If only one is valid, we should return invalid to prevent access */
    if (p_attr->is_valid && !p_attr2->is_valid) {
        tfm_memcpy(p_attr, p_attr2, sizeof(*p_attr));
        return;
    }
    if (!p_attr->is_valid && p_attr2->is_valid) {
        return;
    }
    if (!p_attr->is_mpu_enabled) { p_attr->is_mpu_enabled = p_attr2->is_mpu_enabled; }
    if (!p_attr->is_xn) { p_attr->is_xn = p_attr2->is_xn; }
    if (p_attr->is_priv_rd_allow) { p_attr->is_priv_rd_allow = p_attr2->is_priv_rd_allow; }
    if (p_attr->is_priv_wr_allow) { p_attr->is_priv_wr_allow = p_attr2->is_priv_wr_allow; }
    if (p_attr->is_unpriv_rd_allow) { p_attr->is_unpriv_rd_allow = p_attr2->is_unpriv_rd_allow; }
    if (p_attr->is_unpriv_wr_allow) { p_attr->is_unpriv_wr_allow = p_attr2->is_unpriv_wr_allow; }
}

void SMPU_Get_Access_Rules(const void *p, size_t s,
                           enum cy_en_prot_pc_t pc,
                           struct mem_attr_info_t *p_attr)
{
    enum smpu_coverage_t cov;
    struct mem_attr_info_t attr2;
    bool partly_covered = false;

    /* This function expects a valid pc */
    if (!CY_PROT_IS_PC_VALID(pc)) {
        tfm_core_panic();
    }

    /* Higher-numbered SMPUs have priority */
    for (int i = CPUSS_PROT_SMPU_STRUCT_NR; i > 0; i--) {
        cov = SMPU_Region_Coverage(p, s, pc, &PROT->SMPU.SMPU_STRUCT[i - 1], p_attr);
        switch (cov) {
        case NOT_COVERED:
            /* Move on to check the next SMPU */
            break;

        case PARTLY_COVERED:
            if (partly_covered) {
                /* Combine the stored attributes with these new ones,
                 * putting the result in attr2, because p_attr may get overwritten.
                 * We will always take the more restrictive attributes.
                 * If there are gaps between SMPUs, those gaps are fully accessible,
                 * so we can safely ignore them
                 */
                combine_mem_attr(&attr2, p_attr);
            } else {
                /* Store the attributes */
                tfm_memcpy(&attr2, p_attr, sizeof(attr2));
                partly_covered = true;
            }
            break;

        case FULLY_COVERED:
            /* It's fully covered by this SMPU */
            return;

        default:
            /* This is unreachable */
            tfm_core_panic();
        }
    }

    if (partly_covered) {
        /* The combined attrs are in attr2, not p_attr */
        tfm_memcpy(p_attr, &attr2, sizeof(*p_attr));
        return;
    }

    /* If we get here, no SMPU covers it, so all access is allowed */
    p_attr->is_mpu_enabled = false;
    p_attr->is_valid = true;
    p_attr->is_xn = true;
    p_attr->is_priv_rd_allow = true;
    p_attr->is_priv_wr_allow = true;
    p_attr->is_unpriv_rd_allow = true;
    p_attr->is_unpriv_wr_allow = true;
}

void SMPU_Print_Config(const SMPU_Resources *smpu_dev)
{
    if (is_runtime(smpu_dev)) {
        LOG_MSG("%s - configured algorithmically.", smpu_name(smpu_dev));

        dump_smpu(smpu_dev->smpu);
    } else {
        print_smpu_config(smpu_name(smpu_dev), &smpu_dev->slave_config);
    }
}

cy_en_prot_status_t SMPU_Configure(const SMPU_Resources *smpu_dev)
{
    cy_en_prot_status_t ret;

    if (is_runtime(smpu_dev)) {
        cy_stc_smpu_cfg_t slave_config;

        /* Start with a verbatim copy of the slave config */
        tfm_memcpy(&slave_config,
                   &smpu_dev->slave_config,
                   sizeof(slave_config));

        ret = populate_region(smpu_dev->smpu, &slave_config);
        if (ret != CY_PROT_SUCCESS) {
            return ret;
        }

        ret = Cy_Prot_ConfigSmpuSlaveStruct(smpu_dev->smpu,
                                            &slave_config);
    } else {
        /* Use the slave config verbatim */
        ret = Cy_Prot_ConfigSmpuSlaveStruct(smpu_dev->smpu,
                                            &smpu_dev->slave_config);
    }

    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }
    ret = Cy_Prot_ConfigSmpuMasterStruct(smpu_dev->smpu,
                                         &smpu_dev->master_config);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }
    ret = Cy_Prot_EnableSmpuSlaveStruct(smpu_dev->smpu);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }
    ret = Cy_Prot_EnableSmpuMasterStruct(smpu_dev->smpu);
    return ret;
}

/* Only allow privileged secure PC=1 bus masters to change unconfigured SMPUs */
cy_en_prot_status_t protect_unconfigured_smpus(void)
{
    const cy_stc_smpu_cfg_t smpu_config = COMMON_SMPU_MASTER_CONFIG;
    cy_en_prot_status_t ret = CY_PROT_SUCCESS;
    unsigned i;
    uint32_t att0, att1;

    for (i = 0; i < CPUSS_PROT_SMPU_STRUCT_NR; i++) {
        att0 = PROT->SMPU.SMPU_STRUCT[i].ATT0;
        att1 = PROT->SMPU.SMPU_STRUCT[i].ATT1;

        if ((_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT0_ENABLED, att0) == 0)
            && (_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT1_ENABLED, att1) == 0)) {

            ret = Cy_Prot_ConfigSmpuMasterStruct(&PROT->SMPU.SMPU_STRUCT[i],
                                                 &smpu_config);
            if (ret != CY_PROT_SUCCESS) {
                break;
            }
            ret = Cy_Prot_EnableSmpuMasterStruct(&PROT->SMPU.SMPU_STRUCT[i]);
            if (ret != CY_PROT_SUCCESS) {
                break;
            }
        }
    }

    return ret;
}

/* Exported per-SMPU macros */
#define DEFINE_SMPU(N) const SMPU_Resources SMPU##N##_Resources = { \
    .smpu = PROT_SMPU_SMPU_STRUCT##N, \
    .slave_config = SMPU##N##_SLAVE_CONFIG, \
    .master_config = SMPU##N##_MASTER_CONFIG, \
}; \

#if (RTE_SMPU0)
DEFINE_SMPU(0)
#endif /* RTE_SMPU0 */

#if (RTE_SMPU1)
DEFINE_SMPU(1)
#endif /* RTE_SMPU1 */

#if (RTE_SMPU2)
DEFINE_SMPU(2)
#endif /* RTE_SMPU2 */

#if (RTE_SMPU3)
DEFINE_SMPU(3)
#endif /* RTE_SMPU3 */

#if (RTE_SMPU4)
DEFINE_SMPU(4)
#endif /* RTE_SMPU4 */

#if (RTE_SMPU5)
DEFINE_SMPU(5)
#endif /* RTE_SMPU5 */

#if (RTE_SMPU6)
DEFINE_SMPU(6)
#endif /* RTE_SMPU6 */

#if (RTE_SMPU7)
DEFINE_SMPU(7)
#endif /* RTE_SMPU7 */

#if (RTE_SMPU8)
DEFINE_SMPU(8)
#endif /* RTE_SMPU8 */

#if (RTE_SMPU9)
DEFINE_SMPU(9)
#endif /* RTE_SMPU9 */

#if (RTE_SMPU10)
DEFINE_SMPU(10)
#endif /* RTE_SMPU10 */

#if (RTE_SMPU11)
DEFINE_SMPU(11)
#endif /* RTE_SMPU11 */

#if (RTE_SMPU12)
DEFINE_SMPU(12)
#endif /* RTE_SMPU12 */

#if (RTE_SMPU13)
DEFINE_SMPU(13)
#endif /* RTE_SMPU13 */

/* Note that SMPUs 14 and 15 are fixed by romboot */
