/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Retrieve the location and size of the NSPE image.
 *
 * \param [out] base  Location of the block of flash to use for the NSPE image
 * \param [out] size  Number of bytes of flash to use for the NSPE image
 *
 * \return void
 */
void tfm_nspe_flash_block(uint32_t *base, size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* __IMAGES_H__ */
