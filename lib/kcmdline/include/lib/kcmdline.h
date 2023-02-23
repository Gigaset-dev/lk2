/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stddef.h>

/*
 * kcmdline_finalized() - update buffered boot args change to device tree blob
 *
 * This function update all buffered kernel boot args changes, including
 * appended boot args and substitued args to kernel device tree blob.
 *
 * @fdt:    pointer to device tree blob
 *
 * return:
 *     NO_ERROR, on success
 *     otherwise, on failure
 */
int kcmdline_finalized(void *fdt);

/*
 * kcmdline_print() - print buffered boot args changes
 */
void kcmdline_print(void);

/*
 * kcmdline_append() - append a boot args string to kcmdline buffer
 *
 * This function append a arg string to kcmdline buffer, which will be updated
 * to kernel cmdline (bootargs) of device tree blob later.
 *
 * @append_arg: pointer to the arg string to be appended to kernel cmdline
 *
 * return:
 *     NO_ERROR, on success
 *     otherwise, on failure
 */
int kcmdline_append(const char *append_arg);

/*
 * kcmdline_subst() - add a boot arg substitution request to kcmdline buffer
 *
 * This function records a boot arg substitution request, and keep it in a list.
 * The args in substitution list will be updated to kernel cmdline (bootargs)
 * of device tree blob later.
 *
 * @old_arg: old arg to be searched and substituted
 * @new_arg: new arg for substitution when old_arg exists
 *
 * return:
 *     NO_ERROR, on success
 *     otherwise, on failure
 */
int kcmdline_subst(const char *old_arg, const char *new_arg);
