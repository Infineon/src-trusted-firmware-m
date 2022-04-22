#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


# This function applies patches
function(patch_src WORKING_DIRECTORY PATCH_FILES)
    # generate list of patches to validate
    string(REPLACE ";" "\" \"" PARSED_PATCH_FILES "${PATCH_FILES}")
    set(PARSED_PATCH_FILES "\"${PARSED_PATCH_FILES}\"")
    
    # Validate if patch is already applied
    set(EXECUTE_COMMAND "${GIT_EXECUTABLE}" apply --check -R ${PATCH_FILES})
    execute_process(COMMAND ${EXECUTE_COMMAND}
        WORKING_DIRECTORY ${WORKING_DIRECTORY}
        RESULT_VARIABLE IS_PATCH_APPLIED
        ERROR_QUIET
    )
    if (NOT ${IS_PATCH_APPLIED} EQUAL "0")
        set(EXECUTE_COMMAND "${GIT_EXECUTABLE}" apply --verbose ${PATCH_FILES})
        execute_process(COMMAND ${EXECUTE_COMMAND}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
            RESULT_VARIABLE IS_PATCH_APPLIED
            COMMAND_ECHO STDOUT
        )
        if (NOT ${IS_PATCH_APPLIED} EQUAL "0")
            message( FATAL_ERROR "Failed to apply patches to ${MBEDCRYPTO_PATH}" )
        endif()
    endif()
endfunction()
