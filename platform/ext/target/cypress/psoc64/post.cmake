#-------------------------------------------------------------------------------
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


####################### Generate platform S/NS defines ########################

# Function to generate file with compile and interface definitions
function(cy_generate_defines file_name target_name prefix_content suffix_content)
    set(_DEFINITIONS_ "")
    set(_DEFINITION_LIST_ "")

    # capture definitions
    foreach(_TARGET_ ${target_name})

        get_property(_IS_COMPILE_DEFINITIONS_ TARGET ${_TARGET_} PROPERTY COMPILE_DEFINITIONS DEFINED)
        if (${_IS_COMPILE_DEFINITIONS_})
            get_target_property(_ITEMS_ ${_TARGET_} COMPILE_DEFINITIONS)
            if (NOT _ITEMS_ STREQUAL "_ITEMS_-NOTFOUND")
                list(APPEND _DEFINITION_LIST_ ${_ITEMS_})
            endif()
        endif()

        get_target_property(_ITEMS_ ${_TARGET_} INTERFACE_COMPILE_DEFINITIONS)
        if (NOT _ITEMS_ STREQUAL "_ITEMS_-NOTFOUND")
            list(APPEND _DEFINITION_LIST_ ${_ITEMS_})
        endif()
    endforeach()

    # remove duplicates
    list(REMOVE_DUPLICATES _DEFINITION_LIST_)

    # put each definition in double quotes
    foreach(_DEF_ ${_DEFINITION_LIST_})
        set(_DEFINITIONS_ "${_DEFINITIONS_}\"${_DEF_}\" ")
    endforeach()

    # generate file
    file(GENERATE OUTPUT ${file_name} CONTENT "${prefix_content}${_DEFINITIONS_}${suffix_content}")
endfunction()


# generate compiler definitions for non-security target
cy_generate_defines("defines_ns.mk" "platform_ns;platform_region_defs;tfm_app_ns_interface;tfm_partition_defs" "TFM_PLATFORM_NS_DEFS=" "")
