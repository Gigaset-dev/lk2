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

#include <lock_state.h>
#include "lock_state_internal.h"
#include <stddef.h>
/* ==================================
 * description of internal lock state
 * ==================================
 * Device is in LKS_DEFAULT at launch.
 * If user does lock operation when device is in LKS_DEFAULT state,
 * device will enter LKS_MP_DEFAULT.
 * With LKS_MP_DEFAULT, device manufacturer could apply
 * further restriction on device access permission such as meta mode.
 * This could be used to distinguish device in factory and device
 * for shipping.
 * In order to use this capability, device manufacturer will need
 * to do additional lock operation before shipping.
 * After user does unlock operation, device will leave LKS_DEFAULT
 * and LKS_MP_DEFAULT permanently. Even if user locks the device
 * again, device will only enter LKS_LOCK. This way, we can
 * tell whether unlock operation has been performed in device
 * life cycle and use it as warranty bit. seccfg is protected
 * by power on write protect no matter which boot mode we boot into,
 * so there should be no way for user to modify seccfg after we
 * leave lk. Device PCB layout should be handled with care when device
 * manufacturer wants to use power on write protect feature and
 * warranty bit.
 */

int get_lock_state(uint32_t *lock_state)
{
    int ret = STATUS_GET_LOCK_STATE_OK;
    uint32_t lock_state_internal = LKS_DEFAULT;

    if (lock_state == NULL)
        return STATUS_GET_LOCK_STATE_FAIL_INVALID_BUF;

    ret = seccfg_get_lock_state(&lock_state_internal);
    if (ret) {
        *lock_state = DEVICE_STATE_LOCKED;
        return STATUS_GET_LOCK_STATE_FAIL_GENERAL;
    }

    if (lock_state_internal == LKS_UNLOCK)
        *lock_state = DEVICE_STATE_UNLOCKED;
    else
        *lock_state = DEVICE_STATE_LOCKED;

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
    uint32_t lock_state_internal = LKS_DEFAULT;

    if (warranty == NULL)
        return STATUS_GET_WARRANTY_FAIL_INVALID_BUF;

    /* seccfg is emtpy at launch and failing to get its
     * content does not necessarily mean there's error.
     * seccfg is protected by power on write protect so
     * we don't expect anyone could access it after we
     * leave lk.
     */
    ret = seccfg_get_lock_state(&lock_state_internal);
    if (ret) {
        *warranty = 1;
        return STATUS_GET_LOCK_STATE_OK;
    }

    if (lock_state_internal == LKS_DEFAULT ||
        lock_state_internal == LKS_MP_DEFAULT) {
        *warranty = 1;
    } else if (lock_state_internal == LKS_LOCK ||
        lock_state_internal == LKS_UNLOCK) {
        *warranty = 0;
    }

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
    uint32_t lock_state_internal = LKS_DEFAULT;

    ret = seccfg_get_lock_state(&lock_state_internal);
    if (ret)
        lock_state_internal = LKS_DEFAULT;

    if (lock_state == DEVICE_STATE_LOCKED) {
        if (lock_state_internal == LKS_DEFAULT)
            lock_state_internal = LKS_MP_DEFAULT;
        else if (lock_state_internal == LKS_MP_DEFAULT)
            lock_state_internal = LKS_MP_DEFAULT;
        else
            lock_state_internal = LKS_LOCK;
    } else if (lock_state == DEVICE_STATE_UNLOCKED)
        lock_state_internal = LKS_UNLOCK;

    ret = seccfg_set_lock_state(lock_state_internal);
    if (ret)
        return STATUS_SET_LOCK_STATE_FAIL_GENERAL;

    return ret;
}
