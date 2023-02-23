/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2019, MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#pragma once

#define MAX_RSP_SIZE 64
/*
 * The page size of MISC is 2k bytes.
 * 2 pages is used for recovery mode bootloader message
 * designed by Google, and the extra 1 page is preserved
 * for the future usage.
 */
#define MISC_PAGES   3
#define MISC_PAGE_SZ 2048

#include <sys/types.h>

#define STATE_OFFLINE   0
#define STATE_COMMAND   1
#define STATE_COMPLETE  2
#define STATE_ERROR     3
#define STATE_RETURN    4

struct fastboot_controller_interface {
    const char *name;
    status_t (*init)(void);
    void (*fini)(void);
    status_t (*start)(void);
    status_t (*stop)(void);
    int (*read)(void *buf, unsigned int len);
    int (*write_with_timeout)(void *buf, unsigned int len, lk_time_t timeout);
    status_t (*event_wait)(void);
};

struct fastboot_var {
    struct fastboot_var *next;
    const char *name;
    const char *value;
};

extern void *download_base;
extern unsigned int download_max;
extern unsigned int download_size;

extern struct fastboot_var *varlist;
void fastboot_okay(const char *info);
void fastboot_fail(const char *reason);
void fastboot_register(const char *prefix,
                       void (*handle)(const char *arg, void *data, unsigned int sz),
                       int allowed_when_security_on, int forbidden_when_lock_on);
void fastboot_unregister_all(void);
void fastboot_publish(const char *name, const char *value);
void fastboot_unpublish_all(void);
void fastboot_update_var(const char *name, const char *value);
const char *fastboot_get_var(const char *name);
void fastboot_info(const char *reason);
void fastboot_exit(void);
int fastboot_init(void *base, unsigned int size);
void fastboot_set_state(unsigned int state);
unsigned int fastboot_get_state(void);
status_t register_fastboot_controller_interface(struct fastboot_controller_interface *interface);
const char *get_fastboot_controller_interface_name(void);
