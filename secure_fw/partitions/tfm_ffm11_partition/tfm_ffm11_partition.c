/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "psa/service.h"
#include "psa_manifest/tfm_ffm11_partition.h"
#include "tfm/tfm_spm_services.h"
#include "tfm_sp_log.h"
#include "static_checks.h"

/**
 * \brief An example service implementation that prints out a message.
 */
TFM_COVERITY_DEVIATE_BLOCK(MISRA_C_2012_Rule_9_1, "psa_get() handles all parameters by CPU registers")
static void tfm_ffm11_service1(void)
{
    psa_status_t status;
    uint32_t arg;
    psa_msg_t msg;
    size_t num;

    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(TFM_FFM11_SERVICE1_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    TFM_COVERITY_FP_LINE(UNINIT, "psa_get() sets msg")
    switch (msg.type) {
    case PSA_IPC_CALL:
        TFM_COVERITY_FP_LINE(UNINIT, "psa_get() sets msg")
        if (msg.in_size[0] != sizeof(arg)) {
            status = PSA_ERROR_PROGRAMMER_ERROR;
            break;
        }
        /* Print arg from client */
        num = psa_read(msg.handle, 0, &arg, sizeof(arg));
        if (num != msg.in_size[0]) {
            status = PSA_ERROR_PROGRAMMER_ERROR;
            break;
        }
        LOG_INFFMT("[Example FFM11 partition] Service called! arg=%x\r\n", arg);
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}
TFM_COVERITY_BLOCK_END(MISRA_C_2012_Rule_9_1)

/**
 * \brief The example FFM-1.1 partition's entry function.
 */
void tfm_ffm11_partition_main(void)
{
    psa_signal_t signals;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_FFM11_SERVICE1_SIGNAL) {
            tfm_ffm11_service1();
        }
        /*
         * The other services are created in yaml for testing manifest tool,
         * but not handled here. They are reserved for future use.
         */
    }
}
