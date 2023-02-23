/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <atags.h>
#include <debug.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <sboot.h>
#include <set_fdt.h>
#include <string.h>

#define BOOT_TAG_MASP_DATA          0x88610030
#define ATAG_MASP_DATA              0x41000866
#define NUM_SBC_PUBK_HASH           8
#define NUM_CRYPTO_SEED             16
#define NUM_RID                     4
#define TMPBUF_SIZE                 200

struct boot_tag_masp_data {
    unsigned int rom_info_sbc_attr;
    unsigned int rom_info_sdl_attr;
    unsigned int hw_sbcen;
    unsigned int lock_state;
    unsigned int rid[NUM_RID];
    unsigned char crypto_seed[NUM_CRYPTO_SEED];
    unsigned int sbc_pubk_hash[NUM_SBC_PUBK_HASH];
};

struct boot_tag_masp_data atags_info;

static void atags_pl_boottag_hook(struct boot_tag *tag)
{
    memcpy(&atags_info, &tag->data, sizeof(atags_info));
}
PL_BOOTTAGS_INIT_HOOK(atags, BOOT_TAG_MASP_DATA, atags_pl_boottag_hook);

unsigned int *prep_atag_masp_data(unsigned int *ptr)
{
    *ptr++ = tag_size(boot_tag_masp_data);
    *ptr++ = ATAG_MASP_DATA;
    ptr = sec_fill_atag_masp_data_adapter(ptr);

    return ptr;
}

void set_fdt_atag_masp(void *fdt)
{

#if (MTK_SECURITY_SW_SUPPORT)
    int offset, ret;
    char temp_buf[TMPBUF_SIZE];
    char *ptr,
    *buf = temp_buf;

    ptr = (char *)prep_atag_masp_data((unsigned int *)buf);
    offset = fdt_path_offset(fdt, "/chosen");
    ASSERT(offset >= 0);
    ret = fdt_setprop(fdt, offset, "atag,masp", buf, ptr - buf);
    if (ret)
        ASSERT(0);
#endif

}
SET_FDT_INIT_HOOK(set_fdt_atag_masp, set_fdt_atag_masp);

