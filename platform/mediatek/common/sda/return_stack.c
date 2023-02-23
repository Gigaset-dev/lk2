/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <malloc.h>
#include <platform/dfd_mcu.h>
#include "return_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int return_stack_dump(char *buf, int *wp)
{
    if (buf == NULL || wp == NULL)
        return -1;

    *wp += snprintf(buf + *wp, RETURN_STACK_BUF_LENGTH - *wp,
               "\n*************************** retrun stack ***************************\n");

    if (!dfd_internal_dump_before_reboot())
        *wp += snprintf(buf + *wp, RETURN_STACK_BUF_LENGTH - *wp, "NO DFD trigger\n");
    else
        *wp += dfd_get_decoded_return_stack(buf + *wp, RETURN_STACK_BUF_LENGTH - *wp);

    return 1;
}

int return_stack_get(void **data, int *len)
{
    int ret;
    *len = 0;
    *data = malloc(RETURN_STACK_BUF_LENGTH);
    if (*data == NULL)
        return 0;

    ret = return_stack_dump(*data, len);
    if (ret < 0 || *len > RETURN_STACK_BUF_LENGTH) {
        *len = (*len > RETURN_STACK_BUF_LENGTH) ? RETURN_STACK_BUF_LENGTH : *len;
        return ret;
    }

    return 1;
}

void return_stack_put(void **data)
{
    free(*data);
}
