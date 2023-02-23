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
#include <assert.h>
#include <compiler.h>
#include <dconfig.h>
#include <debug.h>
#include <dtb.h>
#include <err.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <load_image.h>
#include <platform_mtk.h>
#include <sys/types.h>

/*
 * default implementations of these routines, if the platform code
 * chooses not to implement.
 */

/*
 * Since pl_boottags hook functions for boot mode and boot reason share the same
 * variables (boot_mode and boot_reason) with the corresponding getter functions,
 * a platform who implements its own hook functions also needs to implement WEAK
 * getter functions in platform*.c. Otherwise, getter function would return incorrect
 * value due to different variable instances.
 * (hook function uses one in platform*.c, but getter function uses another in init.c.)
 *
 * NOTE: Revising static variables as __WEAK variables DOES NOT work.
 */
#define BOOT_TAG_BOOT_REASON    0x88610001
static uint32_t    boot_reason;

#define BOOT_TAG_BOOT_MODE      0x88610002
static uint32_t    boot_mode;

__WEAK void platform_pl_boottags_boot_reason_hook(struct boot_tag *tag)
{
    boot_reason = (uint32_t)tag->data;
}
PL_BOOTTAGS_INIT_HOOK(platform_pl_boottags_boot_reason_hook,
                      BOOT_TAG_BOOT_REASON,
                      platform_pl_boottags_boot_reason_hook);

__WEAK void platform_pl_boottags_boot_mode_hook(struct boot_tag *tag)
{
    boot_mode = (uint32_t)tag->data;
}
PL_BOOTTAGS_INIT_HOOK(platform_pl_boottags_boot_mode_hook,
                      BOOT_TAG_BOOT_MODE,
                      platform_pl_boottags_boot_mode_hook);

__WEAK uint32_t platform_get_boot_reason(void)
{
    return boot_reason;
}

__WEAK uint32_t platform_get_boot_mode(void)
{
    return boot_mode;
}

__WEAK void *platform_load_device_tree(void)
{
    int32_t ret = NO_ERROR;
    void *dtb_addr;
    void *dtbo_ptr = NULL;
    uint32_t dtbo_size = 0;
    void *overlayed_dtb = NULL;
    void *dconfig_overlayed_dtb = NULL;

    dtb_addr = malloc(DTB_MAX_SIZE);
    ASSERT(dtb_addr);

    ret = load_partition("lk", "lk_main_dtb", dtb_addr, DTB_MAX_SIZE);
    if (ret <= 0) {
        dprintf(CRITICAL, "Error load main dtb (%d)\n", ret);
        free(dtb_addr);
        return NULL;
    }

    /*
     * set overlayed_dtb init value as dtb base in case later dtbo
     * load or overlay might fail, then still can play with dconfig
     */
    overlayed_dtb = (void *)dtb_addr;

    ret = load_dtbo("dtbo", "dtbo", NULL, &dtbo_ptr, &dtbo_size);
    dprintf(ALWAYS, "dtbo_size=%u\n", dtbo_size);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "load_dtbo fail, fdt size(%d)\n", fdt_totalsize(overlayed_dtb));
        goto setup_dconfig_dt;
    }

    ret = dtb_overlay((void *)dtb_addr, fdt_totalsize(dtb_addr),
                    dtbo_ptr, dtbo_size, &overlayed_dtb);
    if (ret != NO_ERROR)
        dprintf(CRITICAL, "DT overlay fail (%d)\n", ret);

setup_dconfig_dt:
    /*
     * To reduce boot time (load and verify dconfig_dtb again at mt_boot),
     * for normal LK, dconfig_dtb will be freed until mt_boot.
     */
    dconfig_overlayed_dtb = dconfig_dtb_overlay(overlayed_dtb, fdt_totalsize(overlayed_dtb), 0);
    if (dconfig_overlayed_dtb) {
        /* free previous allocated from dtb_overlay and adopt new one */
        if (overlayed_dtb != dtb_addr)
            free(overlayed_dtb);
        overlayed_dtb = dconfig_overlayed_dtb;
    }

    /*
     * compact the merged dt and reveal it's real size, the
     * intention behind is for ro operation in platform level
     * so any write operation later will fail and hint user
     */
    ret = fdt_pack(overlayed_dtb);
    ASSERT(ret >= 0);

    /*
     * dtb_addr != overlayed_dtb: one of overlay operations succeed.
     * It is safe to free dtb_addr (memory space of lk_main_dtb).
     * dtb_addr = overlayed_dtb: none of overlay operations succeed.
     * Need to return dtb_addr for fallback.
     */
    if (dtb_addr != overlayed_dtb)
        free(dtb_addr);
    if (dtbo_ptr)
        free(dtbo_ptr);

    return overlayed_dtb;
}

__WEAK enum BOOT_STAGE get_boot_stage(void)
{
#if LK_AS_BL2
    return BOOT_STAGE_BL2;
#elif LK_AS_BL2_EXT
    return BOOT_STAGE_BL2_EXT;
#elif LK_AS_BL33
    return BOOT_STAGE_BL33;
#elif LK_AS_AEE
    return BOOT_STAGE_AEE;
#elif LK_AS_DA
    return BOOT_STAGE_DA;
#else
    return BOOT_STAGE_NONE;
#endif
}

__WEAK lk_time_t get_pl_boot_time(void)
{
    return 0;
}

__WEAK lk_time_t get_lk_start_time(void)
{
    return 0;
}

__WEAK lk_time_t get_logo_time(void)
{
    return 0;
}

__WEAK lk_time_t get_bl2_ext_boot_time(void)
{
    return 0;
}

__WEAK lk_time_t get_tfa_boot_time(void)
{
    return 0;
}

__WEAK lk_time_t get_sec_os_boot_time(void)
{
    return 0;
}

__WEAK lk_time_t get_gz_boot_time(void)
{
    return 0;
}

__WEAK status_t platform_load_scp(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_ccu(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_spm(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_apu(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_apusys_rv(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_dpm(void)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_sspm(void)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_mcupm(void)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_tee(void)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_picachu(void)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_adsp(void *fdt)
{
    return NO_ERROR;
}

__WEAK void platform_quiesce_el3(void)
{
}

__WEAK status_t platform_load_modem(void *fdt, u8 b_mode)
{
    return NO_ERROR;
}

__WEAK int ccci_get_md_version(char buf[], int size)
{
    return 0;
}

__WEAK status_t platform_load_medmcu(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_gpueb(void *fdt)
{
    return NO_ERROR;
}

__WEAK status_t platform_load_vcp(void *fdt)
{
    return NO_ERROR;
}

__WEAK int platform_load_rootfs_sig(void)
{
    return 0;
}

__WEAK int get_fastboot_idle_timeout(void)
{
    return 0;
}

__WEAK status_t platform_set_tz_tags(void)
{
    return NO_ERROR;
}

__WEAK const char *get_suffix(void)
{
    return "";
}

__WEAK void platform_sec_post_init(void)
{

}

__WEAK status_t platform_sec_uninit(void)
{
    return NO_ERROR;
}

__WEAK uint32_t need_relocate(void)
{
    return 0;
}


