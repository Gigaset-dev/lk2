/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <err.h>
#include <errno.h>
#include <lib/console.h>
#include <partition_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int get_name_list(void)
{
    int part_cnt = 0, i;
    char *name_list[NUM_PARTITIONS] = {NULL};

    partition_get_name_list((char **)&name_list, &part_cnt);
    printf("[PART_UTILS] cmd: partitions has (%d) entries\n", part_cnt);
    for (i = 0; i < part_cnt; i++)
        printf("[PART_UTILS] (%d) %s\n", i, name_list[i]);

    return NO_ERROR;
}

static int cmd_partition_utils(int argc, const cmd_args *argv)
{
    int rc = 0;

    if (argc < 2) {
        printf("not enough arguments:\n");
        printf("%s print - print all partition name\n", argv[0].str);
        return -1;
    }

    if (!strcmp(argv[1].str, "print")) {
        rc = get_name_list();
    } else {
        printf("unrecognized subcommand\n");
        return -1;
    }

    return rc;
}
STATIC_COMMAND_START
STATIC_COMMAND("part_utils", "partition_utls debug commands", &cmd_partition_utils)
STATIC_COMMAND_END(part_utils);
