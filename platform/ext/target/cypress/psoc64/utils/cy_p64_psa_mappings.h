/*
 * Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _CY_P64_PSA_MAPPINGS_H_
#define _CY_P64_PSA_MAPPINGS_H_

#include "psa/crypto.h"


psa_key_usage_t cy_p64_psa_map_key_usage_cy2psa(psa_key_usage_t cy_usages);

psa_algorithm_t cy_p64_psa_map_alg_cy2psa(psa_algorithm_t cy_alg);
psa_algorithm_t cy_p64_psa_map_alg_psa2cy(psa_algorithm_t psa_alg);


#endif  /* _CY_P64_PSA_MAPPINGS_H_ */

