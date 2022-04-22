/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CY_POLICY_H__
#define __CY_POLICY_H__

#include "cy_p64_jwt_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief policy tree & public functions
 *
 * prov_req
 * `-- tfm
 *     |-- attestation_data
 *     |   |-- verification_service_url         string
 *     |   |-- attestation_profile_definition   string
 *     |   |-- hw_version                       string
 *     |   `-- implementation_id (ARRAY)        uint8 array
 *     |-- its
 *     |   |-- offset                           uint_32
 *     |   |-- size                             uint_32
 *     |   |-- num_assets                       uint_32
 *     |   `-- max_asset_size                   uint_32
 *     |-- ps
 *     |   |-- offset                           uint_32
 *     |   |-- size                             uint_32
 *     |   |-- num_assets                       uint_32
 *     |   `-- max_asset_size                   uint_32
 *     |-- nv_counters
 *     |   |-- offset                           uint_32
 *     |   `-- size                             uint_32
 *     `-- debug_port
 *         |-- uart_base                        uint_32
 *         `-- enabled                          boolean
 */

/* Provisioning policy paths */
#define CY_ATT_DATA_URL_PATH "tfm/attestation_data/verification_service_url"
#define CY_ATT_DATA_PROFILE_PATH "tfm/attestation_data/attestation_profile_definition"
#define CY_ATT_IMPL_ID_PATH "tfm/attestation_data/implementation_id"
#define CY_ATT_HW_VERSION_PATH "tfm/attestation_data/hw_version"

#define CY_ITS_ADDR_PATH "tfm/its/offset"
#define CY_ITS_SIZE_PATH "tfm/its/size"
#define CY_ITS_MAX_ASSET_SIZE_PATH "tfm/its/max_asset_size"
#define CY_ITS_NUM_ASSETS_PATH "tfm/its/num_assets"

#define CY_PS_ADDR_PATH "tfm/ps/offset"
#define CY_PS_SIZE_PATH "tfm/ps/size"
#define CY_PS_MAX_ASSET_SIZE_PATH "tfm/ps/max_asset_size"
#define CY_PS_NUM_ASSETS_PATH "tfm/ps/num_assets"

#define CY_NV_COUNTERS_ADDR_PATH "tfm/nv_counters/offset"
#define CY_NV_COUNTERS_SIZE_PATH "tfm/nv_counters/size"

#define CY_DEBUG_UART_BASE_PATH "tfm/debug_port/uart_base"
#define CY_DEBUG_UART_FLAG_PATH "tfm/debug_port/enabled"


/**
 * \brief Stores the hardware-related policy settings
 */
struct policy_hw_settings {
    uint32_t uart_base;
    bool uart_enabled;
};

/**
 * \brief Retrieve policy from SysCall, Decode it, and Create cJSON structure
 *
 * \param[out]  json_packet  Pointer to JSON node of 'cy_p64_cJSON'
 *
 * \retval tfm_plat_err_t
 *         TFM_PLAT_ERR_SUCCESS       Success
 *         TFM_PLAT_ERR_INVALID_INPUT Incorrect input argument
 *         otherwise                  TFM core panic
 */
enum tfm_plat_err_t get_decoded_policy_jwt_packet(cy_p64_cJSON **json_packet);

/**
 * \brief Retrieve optional boolean value from the device provision policy
 *
 * \param[in] *path        Path to the json item in the policy
 * \param[in] *jsonPacket  Pointer to decoded JSON packet
 * \param[out] dest        Resulting boolean. Unchanged if path isn't
 *                         present in jsonPacket
 *
 * \retval None
 */
void cy_policy_get_optional_bool(const char *path,
                                 const cy_p64_cJSON *jsonPacket,
                                 bool *dest);

/**
 * \brief Retrieve optional uint32 type value from the device provision policy
 *
 * \param[in] *path        Path to the json item in the policy
 * \param[in] *jsonPacket  Pointer to decoded JSON packet
 * \param[out] dest        Resulting uint32_t. Unchanged if path isn't
 *                         present in jsonPacket
 *
 * \retval Returns true if found, false if not
 */
bool cy_policy_get_optional_uint32(const char *path,
                                   const cy_p64_cJSON *jsonPacket,
                                   uint32_t *dest);

/**
 * \brief Retrieve uint32 type value from the device provision policy
 *
 * \param[in] *path  Path to the json item in the policy
 *
 * \retval Returns uint32_t value if found, triggers tfm_core_panic if not
 */
uint32_t cy_policy_get_uint32 (const char *path);

/**
 * \brief Retrieve hardware related settings from the device provision policy
 *
 * \param[out] *settings  Struct with provisioned hardware settings
 */
void cy_get_policy_hw_settings(struct policy_hw_settings *settings);

#ifdef __cplusplus
}
#endif

#endif  /* __CY_POLICY_H__ */
