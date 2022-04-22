/*
 * Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto.h"
#include "cy_p64_psacrypto.h"
#include "cy_p64_psa_mappings.h"


/*
 *****************************************************************************
 */
typedef struct usage_mapping_table_s {
    psa_key_usage_t     cy_usage;
    psa_key_usage_t     psa_usage;
} usage_mapping_table_t;

static usage_mapping_table_t usage_mapping_table[] = {
    {.cy_usage = CY_P64_PSA_KEY_USAGE_EXPORT,       .psa_usage = PSA_KEY_USAGE_EXPORT       },
    {.cy_usage = CY_P64_PSA_KEY_USAGE_COPY,         .psa_usage = PSA_KEY_USAGE_COPY         },
    {.cy_usage = CY_P64_PSA_KEY_USAGE_ENCRYPT,      .psa_usage = PSA_KEY_USAGE_ENCRYPT      },
    {.cy_usage = CY_P64_PSA_KEY_USAGE_DECRYPT,      .psa_usage = PSA_KEY_USAGE_DECRYPT      },
    {.cy_usage = CY_P64_PSA_KEY_USAGE_SIGN_HASH,    .psa_usage = PSA_KEY_USAGE_SIGN_HASH    },
    {.cy_usage = CY_P64_PSA_KEY_USAGE_VERIFY_HASH,  .psa_usage = PSA_KEY_USAGE_VERIFY_HASH  },
    {.cy_usage = CY_P64_PSA_KEY_USAGE_DERIVE,       .psa_usage = PSA_KEY_USAGE_DERIVE       },
};

psa_key_usage_t cy_p64_psa_map_key_usage_cy2psa(psa_key_usage_t cy_usages)
{
    uint32_t    i;
    psa_key_usage_t psa_usages = 0;

    for (i = 0;
         i < sizeof(usage_mapping_table)/sizeof(usage_mapping_table_t);
         i++) {
        if (cy_usages & usage_mapping_table[i].cy_usage) {
            psa_usages |= usage_mapping_table[i].psa_usage;
        }
    }

    return psa_usages;
}

/*
 *****************************************************************************
 */
typedef struct alg_base_mapping_table_s {
    psa_algorithm_t     cy_alg_base;
    psa_algorithm_t     psa_alg_base;
} alg_base_mapping_table_t;

/* It's overkilled to calculate the shifting value.
 * The xxx_MASK_SHIFT here is for solid programming and code completeness
 * when xxx_HASH_MASK are not at the same bits.
 * Make sure the `shift` values match its `mask` values.
 * The value of zero should be optimized out in the pre-compiler stage.
 */
#define CY_P64_PSA_ALG_HASH_MASK_SHIFT          (0)
#define PSA_ALG_HASH_MASK_SHIFT                 (0)

static alg_base_mapping_table_t alg_base_mapping_table[] = {
    {.cy_alg_base = CY_P64_PSA_ALG_CIPHER_MAC_BASE,         .psa_alg_base = PSA_ALG_CIPHER_MAC_BASE         },
    {.cy_alg_base = CY_P64_PSA_ALG_HMAC_BASE,               .psa_alg_base = PSA_ALG_HMAC_BASE               },
    {.cy_alg_base = CY_P64_PSA_ALG_RSA_PKCS1V15_SIGN_BASE,  .psa_alg_base = PSA_ALG_RSA_PKCS1V15_SIGN_BASE  },
    {.cy_alg_base = CY_P64_PSA_ALG_RSA_PSS_BASE,            .psa_alg_base = PSA_ALG_RSA_PSS_BASE            },
    {.cy_alg_base = CY_P64_PSA_ALG_ECDSA_BASE,              .psa_alg_base = PSA_ALG_ECDSA_BASE              },
    {.cy_alg_base = CY_P64_PSA_ALG_DETERMINISTIC_ECDSA_BASE,.psa_alg_base = PSA_ALG_DETERMINISTIC_ECDSA_BASE},
    {.cy_alg_base = CY_P64_PSA_ALG_RSA_OAEP_BASE,           .psa_alg_base = PSA_ALG_RSA_OAEP_BASE           },
    {.cy_alg_base = CY_P64_PSA_ALG_HKDF_BASE,               .psa_alg_base = PSA_ALG_HKDF_BASE               },
    {.cy_alg_base = CY_P64_PSA_ALG_TLS12_PRF_BASE,          .psa_alg_base = PSA_ALG_TLS12_PRF_BASE          },
    {.cy_alg_base = CY_P64_PSA_ALG_TLS12_PSK_TO_MS_BASE,    .psa_alg_base = PSA_ALG_TLS12_PSK_TO_MS_BASE    },
};

psa_algorithm_t cy_p64_psa_map_alg_cy2psa(psa_algorithm_t cy_alg)
{
    uint32_t    i;
    psa_algorithm_t cy_alg_base, cy_alg_hash;
    psa_algorithm_t psa_alg_base = 0, psa_alg_hash;
    psa_algorithm_t psa_alg;

    cy_alg_hash = (cy_alg >> CY_P64_PSA_ALG_HASH_MASK_SHIFT) & CY_P64_PSA_ALG_HASH_MASK;
    cy_alg_base = cy_alg ^ cy_alg_hash;
    for (i = 0;
         i < sizeof(alg_base_mapping_table) / sizeof(alg_base_mapping_table_t);
         i++) {
        if (cy_alg_base == alg_base_mapping_table[i].cy_alg_base) {
            psa_alg_base = alg_base_mapping_table[i].psa_alg_base;
            break;
        }
    }
    psa_alg_hash = (cy_alg_hash & PSA_ALG_HASH_MASK) << PSA_ALG_HASH_MASK_SHIFT;
    psa_alg = psa_alg_base | psa_alg_hash;

    return psa_alg;
}

psa_algorithm_t cy_p64_psa_map_alg_psa2cy(psa_algorithm_t psa_alg)
{
    uint32_t    i;
    psa_algorithm_t cy_alg_base = 0, cy_alg_hash;
    psa_algorithm_t psa_alg_base, psa_alg_hash;
    psa_algorithm_t cy_alg;

    psa_alg_hash = (psa_alg >> PSA_ALG_HASH_MASK_SHIFT) & PSA_ALG_HASH_MASK;
    psa_alg_base = psa_alg ^ psa_alg_hash;
    for (i = 0;
         i < sizeof(alg_base_mapping_table) / sizeof(alg_base_mapping_table_t);
         i++) {
        if (psa_alg_base == alg_base_mapping_table[i].psa_alg_base) {
            cy_alg_base = alg_base_mapping_table[i].cy_alg_base;
            break;
        }
    }
    cy_alg_hash = (psa_alg_hash & CY_P64_PSA_ALG_HASH_MASK) << CY_P64_PSA_ALG_HASH_MASK_SHIFT;
    cy_alg = cy_alg_base | cy_alg_hash;

    return cy_alg;
}

