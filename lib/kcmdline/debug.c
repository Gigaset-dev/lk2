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
#include <lib/console.h>
#include <lib/kcmdline.h>
#include <libfdt.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if WITH_LIB_CKSUM
#include <lib/cksum.h>
#endif

#if defined(WITH_LIB_CONSOLE)

#if LK_DEBUGLEVEL > 0
static int cmd_kcmdline(int argc, const cmd_args *argv);

STATIC_COMMAND_START
STATIC_COMMAND("kcmdline", "kcmdline debug commands", &cmd_kcmdline)
STATIC_COMMAND_END(kcmdline);

static int cmd_kcmdline(int argc, const cmd_args *argv)
{
    int rc = 0;

    if (argc < 2) {
notenoughargs:
        printf("not enough arguments:\n");
usage:
        printf("%s print - print kcmdline buffer\n", argv[0].str);
        printf("%s append <append_arg>\n", argv[0].str);
        printf("%s subst <old_arg> <new_arg>\n", argv[0].str);
        printf("%s finalized <part_name> - write buffer to <part_name> dtb\n",
               argv[0].str);
        return -1;
    }

    if (!strcmp(argv[1].str, "print")) {
        kcmdline_print();
    } else if (!strcmp(argv[1].str, "append")) {
        rc = kcmdline_append(argv[2].str);
        if (rc != NO_ERROR) {
            printf("kcmdline append %s failed, rc=%d\n", argv[2].str, rc);
            return -1;
        }
    } else if (!strcmp(argv[1].str, "subst")) {
        rc = kcmdline_subst(argv[2].str, argv[3].str);
        if (rc != NO_ERROR) {
            printf("kcmdline subst failed, old: %s, new: %s, rc=%d\n",
                   argv[2].str, argv[3].str, rc);
            return -1;
        }
    } else if (!strcmp(argv[1].str, "finalized")) {
        void *fdt = NULL; //to be give a valid *fdt

        rc = kcmdline_finalized(fdt);
        if (rc != NO_ERROR) {
            printf("kcmdline finalized failed, rc=%d\n", rc);
            return -1;
        }
    } else {
        printf("unrecognized subcommand\n");
        goto usage;
    }

    return rc;
}
#endif
#endif
