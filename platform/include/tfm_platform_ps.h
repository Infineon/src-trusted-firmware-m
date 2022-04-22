/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLATFORM_PS_H__
#define __TFM_PLATFORM_PS_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */

#include <stdint.h>

#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Creates a new or modifies an existing asset.
 *
 * \param[in] client_id     Identifier of the asset's owner (client)
 * \param[in] uid           Unique identifier for the data
 * \param[in] data_length   The size in bytes of the data in `p_data`
 * \param[in] create_flags  The flags indicating the properties of the data
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_platform_ps_set(
    int32_t client_id,
    psa_storage_uid_t uid,
    uint32_t data_length,
    psa_storage_create_flags_t create_flags);

/**
 * \brief Gets the asset data for the provided uid.
 *
 * \param[in]  client_id    Identifier of the asset's owner (client)
 * \param[in]  uid          Unique identifier for the data
 * \param[in]  data_offset  The offset within the data associated with the `uid`
 *                          to start retrieving data
 * \param[in]  data_size    The amount of data to read (and the minimum
 *                          allocated size of the `p_data` buffer)
 * \param[out] p_data_length The actual size of data retrieved
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_platform_ps_get(
    int32_t client_id,
    psa_storage_uid_t uid,
    uint32_t data_offset,
    uint32_t data_size,
    size_t *p_data_length);

/**
 * \brief Gets the metadata for the provided uid.
 *
 * \param[in]  client_id  Identifier of the asset's owner (client)
 * \param[in]  uid        Unique identifier for the data
 * \param[out] p_info     A pointer to the `psa_storage_info_t` struct that will be
 *                        populated with the metadata
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_platform_ps_get_info(
    int32_t client_id,
    psa_storage_uid_t uid,
    struct psa_storage_info_t *p_info);

/**
 * \brief Removes the provided uid and its associated data from storage.
 *
 * \param[in] client_id  Identifier of the asset's owner (client)
 * \param[in] uid        Unique identifier for the data to be removed
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_platform_ps_remove(
    int32_t client_id,
    psa_storage_uid_t uid);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_PS_H__ */
