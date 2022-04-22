/*
 * Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "utilities.h"
#include "cy_p64_jwt_policy.h"
#include "cy_p64_syscalls.h"
#include "tfm_plat_defs.h"
#include "cy_policy.h"

/**
 * \brief Retrieve policy from SysCall, Decode it, and Create JSON structure
 *
 * \param[out]  json_packet  Pointer to JSON node of 'cy_p64_cJSON'
 *
 * \retval tfm_plat_err_t
 *         CY_JWT_SUCCESS         Success
 *         otherwise              error code
 */

enum tfm_plat_err_t get_decoded_policy_jwt_packet(cy_p64_cJSON **json_packet)
{
    cy_p64_error_codes_t status;
    char *jwtPacket;
    /* Instead of deleting the decoded json packet with cy_p64_cJSON_Delete(),
    keep it buffered in a static variable for further access */
    static cy_p64_cJSON *json = NULL;

    if (!json_packet) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }
    if (json) {
        *json_packet = json;
        return TFM_PLAT_ERR_SUCCESS;
    }
    /* Do SFB syscall to get the JWT packet address only once*/

    status = cy_p64_get_provisioning_details(CY_P64_POLICY_JWT, &jwtPacket, NULL);
    if (status == CY_P64_SUCCESS) {
        status = cy_p64_decode_payload_data(jwtPacket, &json);
        if (status != CY_P64_SUCCESS) {
                ERROR_MSG("Failed to decode provisioning jwt packet\r\n");
                tfm_core_panic();
        }
    } else {
        ERROR_MSG("Failed to get provisioning jwt packet\r\n");
        tfm_core_panic();
    }

    *json_packet = json;

    return TFM_PLAT_ERR_SUCCESS;
}

/**
 * \brief Retrieve uint32 type value from the device provision policy
 *
 * \param[in] *path  Path to the json item in the policy
 *
 * \retval Returns uint32_t value if found, triggers tfm_core_panic if not
 */

uint32_t cy_policy_get_uint32(const char *path)
{
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket = NULL;
    const cy_p64_cJSON *json;
    uint32_t value = 0;
    enum tfm_plat_err_t err;

    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        ERROR_MSG("Failed to get policy jwt packet.\r\n");
        tfm_core_panic();
    }

    json = cy_p64_find_json_item(path, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_uint32(json, &value);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        ERROR_MSG("Failed to get uint32 from policy from path: ");
        ERROR_MSG(path);
        ERROR_MSG("\r\n");
        tfm_core_panic();
    }

    return value;
}

/**
 * \brief Retrieve optional boolean value from the device provision policy
 *
 * Matches true, false, 0, or 1
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
                                 bool *dest)
{
    cy_p64_error_codes_t status;
    const cy_p64_cJSON *json = cy_p64_find_json_item(path, jsonPacket);

    /* "Not present" is valid, in which case we do nothing */
    if (json != NULL) {
        status = cy_p64_json_get_boolean(json, dest);
        if (status != CY_P64_SUCCESS) {
            /* Some boolean fields are documented as integers - 1 to enable, 0 to disable */
            uint32_t val;
            if (CY_P64_SUCCESS == cy_p64_json_get_uint32(json, &val)) {
                switch ((int)val) {
                case 0:
                    *dest = false;
                    break;
                case 1:
                    *dest = true;
                    break;
                default:
                    ERROR_MSG("Invalid value in policy at path: ");
                    ERROR_MSG(path);
                    ERROR_MSG("\r\n");
                    tfm_core_panic();
                }
            } else {
                ERROR_MSG("Failed to get bool from policy from path: ");
                ERROR_MSG(path);
                ERROR_MSG("\r\n");
                tfm_core_panic();
            }
        }
    }
}

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
                                   uint32_t *dest)
{
    cy_p64_error_codes_t status;
    const cy_p64_cJSON *json = cy_p64_find_json_item(path, jsonPacket);

    /* "Not present" is valid, in which case we just return false */
    if (json == NULL) {
        return false;
    }

    status = cy_p64_json_get_uint32(json, dest);
    if (status != CY_P64_SUCCESS) {
        ERROR_MSG("Failed to get uint32 from policy from path: ");
        ERROR_MSG(path);
        ERROR_MSG("\r\n");
        tfm_core_panic();
    }

    return true;
}

/**
 * \brief Retrieve hardware related settings from the device provision policy
 *
 * \param[out] *settings  Struct with provisioned hardware settings
 */

void cy_get_policy_hw_settings(struct policy_hw_settings *settings)
{
    cy_p64_error_codes_t status;
    cy_p64_cJSON *jsonPacket = NULL;
    const cy_p64_cJSON *json;
    uint32_t base;
    bool     tmp;
    enum tfm_plat_err_t err;

    err = get_decoded_policy_jwt_packet(&jsonPacket);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        settings->uart_base = 0;
        settings->uart_enabled = false;
        return;
    }

    /* Logging UART settings */
    json = cy_p64_find_json_item(CY_DEBUG_UART_BASE_PATH, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_uint32(json, &base);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        settings->uart_base = 0;
        settings->uart_enabled = false;
        return;
    } else {
        settings->uart_base = base;
    }

    json = cy_p64_find_json_item(CY_DEBUG_UART_FLAG_PATH, jsonPacket);
    if(json != NULL) {
        status = cy_p64_json_get_boolean(json, &tmp);
    }
    if (json == NULL || status != CY_P64_SUCCESS) {
        settings->uart_enabled = false;
    } else {
        settings->uart_enabled = tmp;
    }
}
