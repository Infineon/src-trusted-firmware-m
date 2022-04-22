/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_nv_counters.h"

#include <limits.h>
#include <stddef.h>

#include "nv_counters.h"
#include "static_checks.h"

/* Compilation time checks to be sure the defines are well defined */
#ifndef TFM_NV_COUNTERS_SECTOR_SIZE
#error "TFM_NV_COUNTERS_SECTOR_SIZE must be defined in flash_layout.h"
#endif

#ifndef TFM_NV_COUNTERS_FLASH_DEV
#error "TFM_NV_COUNTERS_FLASH_DEV must be defined in flash_layout.h"
#endif
/* End of compilation time checks to be sure the defines are well defined */

#define SECTOR_OFFSET    0
#define NV_COUNTER_SIZE  sizeof(uint32_t)
#define INIT_VALUE_SIZE  sizeof(uint32_t)
#define CHECKSUM_SIZE    sizeof(uint32_t)
#define NUM_NV_COUNTERS  ((TFM_NV_COUNTERS_SECTOR_SIZE - INIT_VALUE_SIZE \
                            - CHECKSUM_SIZE) / NV_COUNTER_SIZE)

#define NV_COUNTERS_INITIALIZED 0xC0DE0042

static uint32_t nv_counters_offset;
#define BACKUP_ADDRESS (nv_counters_offset + TFM_NV_COUNTERS_SECTOR_SIZE)

/**
 * \brief Struct representing the NV counter data in flash.
 */
struct nv_counters_t {
    union {
        struct {
            uint32_t checksum;
            uint32_t init_value; /**< Watermark to indicate if the NV counters have been
                          *   initialised
                          */
            uint32_t counters[NUM_NV_COUNTERS]; /**< Array of NV counters */
        };
        uint32_t data[2U + NUM_NV_COUNTERS];
    }; 
};

static uint32_t calc_checksum(const uint32_t *data, size_t len)
{
    uint32_t sum = 0U;

    for (uint32_t i = 0U; i < len/sizeof(uint32_t); i++) {
        sum ^= data[i];
    }
    return sum;
}

static bool is_valid(const struct nv_counters_t *nv_counters)
{
    return ((nv_counters->init_value == NV_COUNTERS_INITIALIZED) &&
            (0U == calc_checksum(nv_counters->data, sizeof(nv_counters->data))));
}

static void set_checksum(struct nv_counters_t *nv_counters)
{
    uint32_t sum = calc_checksum(nv_counters->data + 1U,
                                 sizeof(nv_counters->data)
                                  - sizeof(nv_counters->checksum));

    nv_counters->checksum = sum;
}

#ifndef CY_FLASH_LAYOUT_FROM_POLICY
void nvc_flash_block(uint32_t *base, size_t *size)
{
    *base = FLASH_NV_COUNTERS_AREA_OFFSET;
    *size = FLASH_NV_COUNTERS_AREA_SIZE;
}
#endif

enum tfm_plat_err_t tfm_plat_init_nv_counter(void)
{
    int32_t  err;
    uint32_t i;
    struct nv_counters_t nv_counters;
    size_t size;

    /* Get the base address and size */
    nvc_flash_block(&nv_counters_offset, &size);

    /* Verify that the size is exactly two sectors */
    if (size != 2 * TFM_NV_COUNTERS_SECTOR_SIZE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = TFM_NV_COUNTERS_FLASH_DEV.Initialize(NULL);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Read the whole sector so we can write it back to flash later */
    err = TFM_NV_COUNTERS_FLASH_DEV.ReadData(nv_counters_offset,
                                             &nv_counters,
                                             TFM_NV_COUNTERS_SECTOR_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (is_valid(&nv_counters)) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    /* Check the backup watermark */
    TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "The addition operation is performed on static values")
    err = TFM_NV_COUNTERS_FLASH_DEV.ReadData(BACKUP_ADDRESS,
                                             &nv_counters,
                                             TFM_NV_COUNTERS_SECTOR_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Erase sector before writing to it */
    err = TFM_NV_COUNTERS_FLASH_DEV.EraseSector(nv_counters_offset);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (is_valid(&nv_counters)) {
        /* Copy from the backup to the main */
        err = TFM_NV_COUNTERS_FLASH_DEV.ProgramData(nv_counters_offset,
                                                    &nv_counters,
                                                    TFM_NV_COUNTERS_SECTOR_SIZE);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        return TFM_PLAT_ERR_SUCCESS;
    }

    /* Add watermark to indicate that NV counters have been initialized */
    nv_counters.init_value = NV_COUNTERS_INITIALIZED;

    /* Initialize all counters to 0 */
    for (i = 0; i < NUM_NV_COUNTERS; i++) {
        nv_counters.counters[i] = 0;
    }

    set_checksum(&nv_counters);

    /* Write the in-memory block content after modification to flash */
    err = TFM_NV_COUNTERS_FLASH_DEV.ProgramData(nv_counters_offset,
                                                &nv_counters,
                                                TFM_NV_COUNTERS_SECTOR_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val)
{
    int32_t  err;
    uint32_t flash_addr;
    uint32_t nv_counter_id = (uint32_t)counter_id;

    if (nv_counter_id >= NUM_NV_COUNTERS) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "The addition operation is performed on static values")
    flash_addr = nv_counters_offset
                            + offsetof(struct nv_counters_t, counters)
                            + (nv_counter_id * NV_COUNTER_SIZE);

    if (size != NV_COUNTER_SIZE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = TFM_NV_COUNTERS_FLASH_DEV.ReadData(flash_addr, val, NV_COUNTER_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_set_nv_counter(enum tfm_nv_counter_t counter_id,
                                            uint32_t value)
{
    int32_t  err;
    struct nv_counters_t nv_counters;
    uint32_t nv_counter_id = (uint32_t)counter_id;

    if (nv_counter_id >= NUM_NV_COUNTERS) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    /* Read the whole sector so we can write it back to flash later */
    err = TFM_NV_COUNTERS_FLASH_DEV.ReadData(nv_counters_offset,
                                             &nv_counters,
                                             TFM_NV_COUNTERS_SECTOR_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (value != nv_counters.counters[nv_counter_id]) {

        if (value < nv_counters.counters[nv_counter_id]) {
            return TFM_PLAT_ERR_INVALID_INPUT;
        }

        /* Erase backup sector */
        TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "The addition operation is performed on static values")
        err = TFM_NV_COUNTERS_FLASH_DEV.EraseSector(BACKUP_ADDRESS);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        nv_counters.counters[nv_counter_id] = value;

        set_checksum(&nv_counters);

        /* write sector data to backup sector */
        TFM_COVERITY_DEVIATE_LINE(cert_int30_c, "The addition operation is performed on static values")
        err = TFM_NV_COUNTERS_FLASH_DEV.ProgramData(BACKUP_ADDRESS,
                                                    &nv_counters,
                                                    TFM_NV_COUNTERS_SECTOR_SIZE);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        /* Erase sector before writing to it */
        err = TFM_NV_COUNTERS_FLASH_DEV.EraseSector(nv_counters_offset);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        /* Write the in-memory block content after modification to flash */
        err = TFM_NV_COUNTERS_FLASH_DEV.ProgramData(nv_counters_offset,
                                                    &nv_counters,
                                                    TFM_NV_COUNTERS_SECTOR_SIZE);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_increment_nv_counter(
                                           enum tfm_nv_counter_t counter_id)
{
    uint32_t security_cnt;
    enum tfm_plat_err_t err;

    err = tfm_plat_read_nv_counter(counter_id,
                                   sizeof(security_cnt),
                                   (uint8_t *)&security_cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (security_cnt == UINT32_MAX) {
        return TFM_PLAT_ERR_MAX_VALUE;
    }

    return tfm_plat_set_nv_counter(counter_id, security_cnt + 1U);
}
