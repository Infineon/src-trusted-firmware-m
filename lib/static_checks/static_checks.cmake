#-------------------------------------------------------------------------------
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

if(${TFM_COVERITY_CHECK})
    add_definitions(-DTFM_COVERITY_CHECK)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})

########################### install files #####################################

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/static_checks.h
        DESTINATION ${INSTALL_INTERFACE_INC_DIR})
