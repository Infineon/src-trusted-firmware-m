/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 * Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "os_wrapper/common.h"
#include "os_wrapper/mutex.h"
#include "os_wrapper/semaphore.h"
#include "os_wrapper/thread.h"

__attribute__((weak))
void *os_wrapper_thread_new(const char* name, int32_t stack_size,
                            os_wrapper_thread_func func, void *arg,
                            uint32_t priority)
{
    (void) name;
    (void) stack_size;
    (void) func;
    (void) arg;
    (void) priority;

    return NULL;
}

__attribute__((weak))
void *os_wrapper_semaphore_create(uint32_t max_count, uint32_t initial_count,
                                     const char* name)
{
    (void) max_count;
    (void) initial_count;
    (void) name;

    return NULL;
}

__attribute__((weak))
uint32_t os_wrapper_semaphore_acquire(void *handle, uint32_t timeout)
{
    (void) handle;
    (void) timeout;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_semaphore_release(void *handle)
{
    (void) handle;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_semaphore_release_isr(void *handle)
{
    (void) handle;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
void os_wrapper_isr_yield(uint32_t yield)
{
    (void) yield;
}

__attribute__((weak))
uint32_t os_wrapper_semaphore_delete(void *handle)
{
    (void) handle;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
void *os_wrapper_mutex_create(void)
{
    return NULL;
}

__attribute__((weak))
uint32_t os_wrapper_mutex_acquire(void *handle, uint32_t timeout)
{
    (void) handle;
    (void) timeout;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_mutex_release(void *handle)
{
    (void) handle;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_mutex_delete(void *handle)
{
    (void) handle;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
void *os_wrapper_thread_get_handle(void)
{
    return NULL;
}

__attribute__((weak))
uint32_t os_wrapper_thread_get_priority(void *handle, uint32_t *priority)
{
    (void) handle;
    (void) priority;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
void os_wrapper_thread_exit(void)
{
}

__attribute__((weak))
uint32_t os_wrapper_join_thread(void* handle)
{
    (void) handle;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_thread_set_flag(void *handle, uint32_t flags)
{
    (void) handle;
    (void) flags;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_thread_set_flag_isr(void *handle, uint32_t flags)
{
    (void) handle;
    (void) flags;

    return OS_WRAPPER_SUCCESS;
}

__attribute__((weak))
uint32_t os_wrapper_thread_wait_flag(uint32_t flags, uint32_t timeout)
{
    (void) flags;
    (void) timeout;

    return OS_WRAPPER_SUCCESS;
}
