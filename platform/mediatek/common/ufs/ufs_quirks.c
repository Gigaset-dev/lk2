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

#include <ufs_cfg.h>
#include "ufs_hcd.h"
#include "ufs_quirks.h"
#include "ufs_types.h"
#include "ufs_utils.h"

#include <string.h> //For memcpy, memset

static struct ufs_dev_fix ufs_fixups[] = {
    /* UFS cards deviations table */
    UFS_FIX(UFS_VENDOR_SKHYNIX, UFS_ANY_MODEL, UFS_DEVICE_QUIRK_LIMITED_RPMB_MAX_RW_SIZE),
    UFS_FIX(UFS_VENDOR_TOSHIBA, "THGLF2G9C8KBADG", UFS_DEVICE_QUIRK_PA_TACTIVATE),
    UFS_FIX(UFS_VENDOR_TOSHIBA, "THGLF2G9D8KBADG", UFS_DEVICE_QUIRK_PA_TACTIVATE),
    UFS_FIX(UFS_VENDOR_SAMSUNG, UFS_ANY_MODEL, UFS_DEVICE_QUIRK_HOST_PA_TACTIVATE),

    END_FIX
};

void ufs_advertise_fixup_device(struct ufs_hba *hba)
{
    int err;
    struct ufs_dev_fix *f;

    for (f = ufs_fixups; f->quirk; f++) {
        if (((f->wmanufacturerid == hba->dev_info.wmanufacturerid) ||
                (f->wmanufacturerid == UFS_ANY_VENDOR)) &&
                (STR_PRFX_EQUAL(f->product_id, hba->dev_info.product_id) ||
                 !strcmp(f->product_id, UFS_ANY_MODEL)))
            hba->dev_quirks |= f->quirk;
    }
}

