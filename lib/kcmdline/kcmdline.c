/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <err.h>
#include <kernel/mutex.h>
#include <kernel/thread.h>
#include <lib/kcmdline.h>
#include <libfdt.h>
#include <list.h>
#include <lk/init.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#if LK_DEBUGLEVEL > 0
#define LOCAL_TRACE 1
#else
#define LOCAL_TRACE 0
#endif

#define CMDLINE_OVERFLOW_STR    "[ERROR] CMDLINE overflow"
#define CMDLINE_LEN   2048

struct subst_entry {
    struct list_node node;
    char *old_arg;
    char *new_arg;
};

/* variable for keeping substitution arg */
struct list_node subst_list = LIST_INITIAL_VALUE(subst_list);

/* variable for keeping append arg */
static char *cmdline_buf;
static char *cmdline_tail;
static char *cmdline_end;

static mutex_t lock = MUTEX_INITIAL_VALUE(lock);

static inline void validate_cmdline_boundary(const char *tail, const char *end)
{
    if (tail >= end)
        panic(CMDLINE_OVERFLOW_STR);
}

static void dump_cmdline(void *fdt)
{
    int len;
    int chosen_node_offset;
    const char *cmdline;

    chosen_node_offset = fdt_path_offset(fdt, "/chosen");
    if (chosen_node_offset < 0) {
        LTRACEF("can't find chosen node.\n");
        return;
    }

    cmdline = fdt_getprop(fdt, chosen_node_offset, "bootargs", &len);
    if (!cmdline) {
        LTRACEF("fdt_getprop bootargs failed.\n");
        return;
    }

    LTRACEF("cmdline len=%zd, str=\"%s\"\n", strlen(cmdline), cmdline);
}

static void kcmdline_init(uint level)
{
    cmdline_buf = (char *)malloc(CMDLINE_LEN);
    ASSERT(cmdline_buf);

    memset(cmdline_buf, 0, CMDLINE_LEN);
    cmdline_tail = cmdline_buf;
    cmdline_end = cmdline_buf + CMDLINE_LEN;
}
LK_INIT_HOOK(kcmdline_init, &kcmdline_init, LK_INIT_LEVEL_HEAP);

int kcmdline_finalized(void *fdt)
{
    int ret = NO_ERROR;
    int len, cx;
    int offset;
    const void *fdt_bootargs = NULL;
    const void *fdt_bootargs_ext = NULL;
    char *temp_ptr = NULL;
    struct subst_entry *entry;
    struct subst_entry *temp;

    if (!cmdline_buf)
        return ERR_NOT_READY;

    if (!fdt) {
        LTRACEF("Invalid args: fdt null\n");
        return ERR_INVALID_ARGS;
    }

    if (fdt_check_header(fdt) != 0) {
        LTRACEF("fdt_check_header failed\n");
        return ERR_INVALID_ARGS;
    }

    mutex_acquire(&lock);

    /* cmdline_buf is filled with bootargs before kcmdline_finalize is called */
    if (*cmdline_buf != 0x0) {
        temp_ptr = (char *)malloc(strlen(cmdline_buf) + 1);
        if (!temp_ptr) {
            ret = ERR_NO_MEMORY;
            goto bootargs;
        }

        cx = snprintf(temp_ptr, (strlen(cmdline_buf) + 1), "%s", cmdline_buf);
        ASSERT(cx == (int)strlen(cmdline_buf));
    }

bootargs:
    /* Reset cmdline_tail */
    cmdline_tail = cmdline_buf;
    offset = fdt_path_offset(fdt, "/chosen");
    ASSERT(offset >= 0);

    fdt_bootargs = fdt_getprop(fdt, offset, "bootargs", &len);
    if (!fdt_bootargs) {
        ret = ERR_NOT_FOUND;
        goto bootargs_ext;
    }

    /* add appended string to final string */
    cx = snprintf(cmdline_tail, CMDLINE_LEN, "%s", (char *)fdt_bootargs);
    ASSERT(cx == (int)strlen(fdt_bootargs));

    cmdline_tail += cx;

bootargs_ext:
    /*
     * According to DeviceTreeOverlayGuide.pdf, bootloader should
     * concatenate bootargs in main dtb with bootargs_ext in dtbo
     */
    fdt_bootargs_ext = fdt_getprop(fdt, offset, "bootargs_ext", &len);
    if (!fdt_bootargs_ext)
        goto runtime;

    cx = snprintf(cmdline_tail, cmdline_end - cmdline_tail, " %s", (char *)fdt_bootargs_ext);
    ASSERT(cx == (int)strlen(fdt_bootargs_ext) + 1);

    cmdline_tail += cx;

runtime:
    if (temp_ptr) {
        cx = snprintf(cmdline_tail, cmdline_end - cmdline_tail, "%s", temp_ptr);
        ASSERT(cx == (int)strlen(temp_ptr));
        cmdline_tail += cx;
    }

subst:
    /* subst string in final string */
    list_for_every_entry_safe(&subst_list, entry, temp, struct subst_entry, node) {
        char *pos = strstr(cmdline_buf, entry->old_arg);
        size_t old_len = strlen(entry->old_arg);

        if (pos && ((*(pos + old_len) == ' ') || (*(pos + old_len) == '\0'))) {
            size_t new_len = strlen(entry->new_arg);
            char *p;

            /* erase old arg with space */
            memset(pos, ' ', old_len);
            if (old_len >= new_len) {
                p = pos; /* replace old arg with new arg */
            } else {
                /* append new arg in the end of cmdline */
                validate_cmdline_boundary(cmdline_tail + new_len + 2,
                                          cmdline_end);
                p = cmdline_tail;
                cmdline_tail += (new_len + 1);
                *p++ = ' ';
            }
            memcpy(p, entry->new_arg, new_len);
        }

        /* free memory and delete node */
        free(entry->old_arg);
        free(entry->new_arg);
        list_delete(&entry->node);
        free(entry);
        entry = NULL;
    }

final:
    ret = fdt_setprop(fdt, offset, "bootargs", cmdline_buf, strlen(cmdline_buf) + 1);
    if (ret != 0) {
        dprintf(CRITICAL, "fdt_setprop error.\n");
        ret = ERR_GENERIC;
    }

    if (temp_ptr)
        free(temp_ptr);

    free(cmdline_buf);
    cmdline_buf = NULL;

    dump_cmdline(fdt);

    mutex_release(&lock);

    return ret;
}

void kcmdline_print(void)
{
    struct subst_entry *entry;

    if (!cmdline_buf)
        return;

    mutex_acquire(&lock);
    LTRACEF("append cmdline: %s\n", cmdline_buf);
    LTRACEF("append cmdline size: %zd\n", strlen(cmdline_buf));
    LTRACEF("subst list:\n");

    /* traverse list to show subst_list  */
    list_for_every_entry(&subst_list, entry, struct subst_entry, node) {
        LTRACEF("old_arg: %s, new_arg: %s\n", entry->old_arg, entry->new_arg);
    }
    mutex_release(&lock);
}

int kcmdline_append(const char *append_arg)
{
    size_t append_arg_len;

    if (!cmdline_buf)
        return ERR_NOT_READY;

    if (!append_arg)
        return ERR_INVALID_ARGS;

    mutex_acquire(&lock);
    append_arg_len = strlen(append_arg);
    validate_cmdline_boundary(cmdline_tail + append_arg_len + 1, cmdline_end);
    cmdline_tail += snprintf(cmdline_tail, cmdline_end - cmdline_tail, " %s", append_arg);
    mutex_release(&lock);

    return NO_ERROR;
}

int kcmdline_subst(const char *old_arg, const char *new_arg)
{
    struct subst_entry *entry;

    if (!cmdline_buf)
        return ERR_NOT_READY;

    if (!old_arg || !new_arg) {
        LTRACEF("Invalid args: old_arg(%p), new_arg(%p)\n", old_arg, new_arg);
        return ERR_INVALID_ARGS;
    }

    entry = (struct subst_entry *)malloc(sizeof(struct subst_entry));
    if (!entry)
        return ERR_NO_MEMORY;

    memset(entry, 0, sizeof(struct subst_entry));
    entry->old_arg = strdup(old_arg);
    entry->new_arg = strdup(new_arg);

    if (!entry->old_arg || !entry->new_arg) {
        free(entry->old_arg);
        free(entry->new_arg);
        free(entry);

        return ERR_NO_MEMORY;
    }

    mutex_acquire(&lock);
    list_add_tail(&subst_list, &entry->node);
    mutex_release(&lock);

    return NO_ERROR;
}
