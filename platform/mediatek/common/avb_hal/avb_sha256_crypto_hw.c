/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#if (AVB_SHA256_CRYPTO_HW_SUPPORT)

#include <avb_sha.h>
#include <debug.h>
#include <sboot.h>
#include <stdlib.h>
#include <string.h>
#include <verified_boot_error.h>

void avb_sha256_init(AvbSHA256Ctx *ctx)
{
    uint32_t ret = 0;

    ret = sec_sha256_tfa_init_adapter();
    if (ret)
        dprintf(CRITICAL, "sec_sha256_init_adapter() error(0x%x)\n", ret);

    return;
}

void avb_sha256_update(AvbSHA256Ctx *ctx, const uint8_t *data, size_t len)
{
    uint32_t ret = 0;

    if (ctx == NULL)
        return;

    if (data == NULL)
        return;

    ret = sec_sha256_tfa_process_adapter(data, len);
    if (ret)
        dprintf(CRITICAL, "sec_sha256_process_adapter() error(0x%x)\n", ret);

    return;
}

uint8_t *avb_sha256_final(AvbSHA256Ctx *ctx)
{
    uint32_t ret = 0;

    if (ctx == NULL)
        return NULL;

    ret = sec_sha256_tfa_done_adapter(ctx->buf);
    if (ret)
        dprintf(CRITICAL, "sec_sha256_done_adapter() error(0x%x)\n", ret);

    return ctx->buf;
}
#endif //#if (AVB_SHA256_CRYPTO_HW_SUPPORT)
