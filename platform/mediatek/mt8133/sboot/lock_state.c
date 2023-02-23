/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <lock_state.h>
#include <stddef.h>

int get_lock_state(uint32_t *lock_state)
{
    int ret = STATUS_GET_LOCK_STATE_OK;

    if (lock_state == NULL)
        return STATUS_GET_LOCK_STATE_FAIL_INVALID_BUF;

    /* dummy funstions, To-do: implement it*/
    *lock_state = DEVICE_STATE_UNLOCKED;

    return ret;
}

const char *get_lock_state_str(void)
{
    uint32_t lock_state = DEVICE_STATE_LOCKED;

    if (get_lock_state(&lock_state))
        return "unknown";

    if (lock_state == DEVICE_STATE_LOCKED)
        return "locked";
    else if (lock_state == DEVICE_STATE_UNLOCKED)
        return "unlocked";

    return "unknown";
}

int get_warranty(uint32_t *warranty)
{
    int ret = STATUS_GET_LOCK_STATE_OK;

    if (warranty == NULL)
        return STATUS_GET_WARRANTY_FAIL_INVALID_BUF;

    /* dummy funstions, To-do: implement it*/
   *warranty = 1;

    return ret;
}

const char *get_warranty_str(void)
{
    unsigned int warranty = 1;

    if (get_warranty(&warranty))
        return "unknown";

    if (warranty == 1)
        return "yes";
    else if (warranty == 0)
        return "voided";

    return "unknown";
}

int set_lock_state(uint32_t lock_state)
{
    int ret = STATUS_SET_LOCK_STATE_OK;

    /* dummy function, To-do: implement it */

    return ret;
}
