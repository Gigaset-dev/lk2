/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <kernel/mutex.h>
#include <lib/nftl.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>

struct nftl_part {
    struct nftl_info info;
    struct nftl_info *parent;

    struct list_node node;
    int ref;

    /* start erase block */
    u64 offset;
};

static struct {
    struct list_node list;
    mutex_t lock;
} nftlparts = {
    .list = LIST_INITIAL_VALUE(nftlparts.list),
    .lock = MUTEX_INITIAL_VALUE(nftlparts.lock),
};

static void nftl_part_register(struct nftl_part *part)
{
    DEBUG_ASSERT(part);

    part->ref = 1;
    mutex_acquire(&nftlparts.lock);
    list_add_tail(&nftlparts.list, &part->node);
    mutex_release(&nftlparts.lock);
}

static void nftl_part_unregister(struct nftl_part *part)
{
    DEBUG_ASSERT(part);

    mutex_acquire(&nftlparts.lock);
    list_delete(&part->node);
    mutex_release(&nftlparts.lock);
    part->ref--;
}

struct nftl_info *nftl_search_by_address(u64 address, u64 *start)
{
    struct nftl_part *entry;
    struct nftl_info *info = NULL;

    mutex_acquire(&nftlparts.lock);
    list_for_every_entry(&nftlparts.list, entry, struct nftl_part, node) {
        if (entry->parent != NULL && entry->offset <= address
            && entry->info.total_size > address - entry->offset) {
            info = &entry->info;
            *start = entry->offset;
            break;
        }
    }
    mutex_release(&nftlparts.lock);

    return info;
}

struct nftl_info *nftl_open(const char *name)
{
    struct nftl_part *entry;
    struct nftl_info *info = NULL;

    mutex_acquire(&nftlparts.lock);
    list_for_every_entry(&nftlparts.list, entry, struct nftl_part, node) {
        if (!strcmp(entry->info.name, name) || !strcmp(entry->info.label, name)) {
            info = &entry->info;
            entry->ref++;
            break;
        }
    }
    mutex_release(&nftlparts.lock);

    return info;
}

void nftl_close(struct nftl_info *info)
{
    struct nftl_part *part = (struct nftl_part *)info;

    part->ref--;
}

static int nftl_part_block_isbad(struct nftl_info *info, u32 page)
{
    struct nftl_part *part = (struct nftl_part *)info;
    u32 start = part->offset / info->write_size;

    return part->parent->block_isbad(part->parent, page + start);
}

static ssize_t nftl_part_erase(struct nftl_info *info, off_t offset,
                               ssize_t len)
{
    struct nftl_part *part = (struct nftl_part *)info;
    u64 start = part->offset;

    return part->parent->erase(info, offset + start, len);
}

static ssize_t nftl_part_read(struct nftl_info *info, void *buf, off_t offset,
                              ssize_t len)
{
    struct nftl_part *part = (struct nftl_part *)info;
    u64 start = part->offset;

    return part->parent->read(info, buf, offset + start, len);
}

static ssize_t nftl_part_write(struct nftl_info *info, const void *buf,
                               off_t offset, ssize_t len)
{
    struct nftl_part *part = (struct nftl_part *)info;
    u64 start = part->offset;

    return part->parent->write(info, buf, offset + start, len);
}

int nftl_add_part(const char *main_part, const char *sub_part,
                  const char *sub_label, u64 offset, u64 len)
{
    struct nftl_info *parent;
    struct nftl_part *sub;

    parent = nftl_open(main_part);
    if (!parent)
        return ERR_NOT_FOUND;

    if (len > parent->total_size - offset)
        return ERR_INVALID_ARGS;

    sub = malloc(sizeof(struct nftl_part));
    if (!sub)
        return ERR_NO_MEMORY;

    sub->parent = parent;
    sub->offset = offset;
    sub->info.name = strdup(sub_part);
    if (sub_label)
        sub->info.label = strdup(sub_label);
    sub->info.total_size = len;
    sub->info.erase_size = sub->parent->erase_size;
    sub->info.write_size = sub->parent->write_size;
    sub->info.block_isbad = nftl_part_block_isbad;
    sub->info.erase = nftl_part_erase;
    sub->info.read = nftl_part_read;
    sub->info.write = nftl_part_write;
    nftl_part_register(sub);

    return 0;
}

struct nftl_info *nftl_add_master(const char *name)
{
    struct nftl_part *part;

    part = malloc(sizeof(struct nftl_part));
    if (!part)
        return NULL;

    part->parent = NULL;
    part->offset = 0;
    part->info.name = strdup(name);
    part->info.label = strdup(name);

    nftl_part_register(part);

    return &part->info;
}

int nftl_delete_part(const char *name)
{
    struct nftl_info *info;
    struct nftl_part *part;

    info = nftl_open(name);
    if (info) {
        part = (struct nftl_part *)info;
        nftl_part_unregister(part);
    }

    return 0;
}

void nftl_dump_parts(void)
{
    dprintf(INFO, "nftl parts:\n");
    struct nftl_part *entry;

    mutex_acquire(&nftlparts.lock);
    list_for_every_entry(&nftlparts.list, entry, struct nftl_part, node)
        dprintf(INFO, "\t%s, size %lld, offset %lld, ref %d, label %s\n",
               entry->info.name, entry->info.total_size, entry->offset,
               entry->ref, entry->info.label);
    mutex_release(&nftlparts.lock);
}

int nftl_partition_get_offset(const char *name)
{
    struct nftl_info *info;
    struct nftl_part *part;

    info = nftl_open(name);
    if (info) {
        part = (struct nftl_part *)info;
        return part->offset;
    } else {
        dprintf(INFO, "get partition %s offset error.\n", name);
        return -1;
    }
}

int nftl_partition_get_size(const char *name)
{
    struct nftl_info *info;

    info = nftl_open(name);
    if (info)
        return info->total_size;
    else {
        dprintf(INFO, "get partition %s size error.\n", name);
        return -1;
    }
}
