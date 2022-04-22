/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NV_COUNTERS_H__
#define __NV_COUNTERS_H__

#include <stddef.h>
#include <stdint.h>

#include "Driver_Flash.h"
#include "flash_layout.h"

/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH TFM_NV_COUNTERS_FLASH_DEV;

/**
 * \brief Retrieve the base address and size of the non-volatile counters
 *        flash region.
 *
 * Note that this function should ensure that the values returned do
 * not result in a security compromise.
 *
 * \param[out] base            Base address of NVC flash region
 * \param[out] size            Size of NVC flash region
 *
 * \return void
 */
void nvc_flash_block(uint32_t *base, size_t *size);

#endif /* __NV_COUNTERS_H__ */
