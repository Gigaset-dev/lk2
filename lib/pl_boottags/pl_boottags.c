/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <lib/pl_boottags.h>
#include <lk/init.h>
#include <platform/memory_layout.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define boot_tag_next(t)    ((void *)(t) + (t)->hdr.size)

extern const struct pl_boottags_init __pl_boottags[];
extern const struct pl_boottags_init __pl_boottags_end[];

/* Get parameters from preloader as early as possible. */
static void pl_boottags_init_hook(uint level)
{
    struct boot_tag *tag;
    struct boot_tag *start_addr = (struct boot_tag *)PL_BOOTTAGS_BASE;

    for (const struct pl_boottags_init *ptr = __pl_boottags; ptr != __pl_boottags_end; ptr++) {
        LTRACEF("%s looks for 0x%x\n", ptr->name, ptr->id);
        for (tag = start_addr; tag->hdr.size; tag = boot_tag_next(tag)) {
            LTRACEF("tag->hdr.tag = 0x%x\n", tag->hdr.tag);
            if (ptr->id == tag->hdr.tag) {
                ptr->hook(tag);
                break;
            }
        }
    }
}
LK_INIT_HOOK(pl_boottags, pl_boottags_init_hook, LK_INIT_LEVEL_PLATFORM_EARLY - 1);


extern const struct pl_boottags_to_be_updated __pl_boottags_to_be_updated[];
extern const struct pl_boottags_to_be_updated __pl_boottags_to_be_updated_end[];

void update_pl_boottags(struct boot_tag *start_addr)
{
    struct boot_tag *tag;

    for (const struct pl_boottags_to_be_updated *ptr = __pl_boottags_to_be_updated;
            ptr != __pl_boottags_to_be_updated_end; ptr++) {
        LTRACEF("%s looks for 0x%x to update\n", ptr->name, ptr->id);
        for (tag = start_addr; tag->hdr.size; tag = boot_tag_next(tag)) {
            LTRACEF("tag->hdr.tag = 0x%x\n", tag->hdr.tag);
            if (ptr->id == tag->hdr.tag) {
                memcpy(&tag->data, ptr->data, tag->hdr.size - sizeof(struct boot_tag_header));
                break;
            }
        }
    }
}
