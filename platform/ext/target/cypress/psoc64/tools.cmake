#-------------------------------------------------------------------------------
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

####################### PSoC64 test suite manifest list #######################

if(PSOC_TEST)
    configure_file(${TFM_TEST_PATH}/suites/psoc/psoc_tests_manifest.yaml
                   ${CMAKE_CURRENT_BINARY_DIR}/psoc_tests_manifest.yaml)

    set(MANIFEST_LISTS  ${MANIFEST_LISTS} ${CMAKE_CURRENT_BINARY_DIR}/psoc_tests_manifest.yaml)
endif()
