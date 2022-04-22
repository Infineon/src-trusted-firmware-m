/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PS_DEFS_H__
#define __TFM_PS_DEFS_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Invalid UID */
#define TFM_PS_INVALID_UID 0

/* OEM UIDs: 0x100 ~ 0x1FF */
#define TFM_PS_OEM_UID_MIN      0x100
#define TFM_PS_OEM_UID_NUM      0x100
#define TFM_PS_OEM_UID_MAX      (TFM_PS_OEM_UID_MIN + TFM_PS_OEM_UID_NUM - 1)
#if defined(TFM_ENABLE_PS_OEM_UID)
static inline bool is_tfm_ps_oem_uid(psa_storage_uid_t uid) {
    return ((uid >= TFM_PS_OEM_UID_MIN) && (uid <= TFM_PS_OEM_UID_MAX));
}
#else
static inline bool is_tfm_ps_oem_uid(psa_storage_uid_t uid)
{
    (void)uid; /* unused parameter */
    return false;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PS_DEFS_H__ */
