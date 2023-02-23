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
#pragma once

#include <lib/pl_boottags.h>
#include <stdbool.h>
#include <sys/types.h>

enum BOOT_STAGE {
    BOOT_STAGE_BL2 = 0,
    BOOT_STAGE_BL2_EXT,
    BOOT_STAGE_BL33,
    BOOT_STAGE_AEE,
    BOOT_STAGE_DA,
    BOOT_STAGE_NONE = 0xff
};

enum BOOT_STAGE get_boot_stage(void);
lk_time_t get_pl_boot_time(void);
lk_time_t get_lk_start_time(void);
lk_time_t get_logo_time(void);
lk_time_t get_bl2_ext_boot_time(void);
lk_time_t get_tfa_boot_time(void);
lk_time_t get_sec_os_boot_time(void);
lk_time_t get_gz_boot_time(void);

/* scp loader function implemented by platform */
status_t platform_load_scp(void *fdt);

/* ccu loader function implemented by platform */
status_t platform_load_ccu(void *fdt);

/* spm loader function implemented by platform */
status_t platform_load_spm(void *fdt);

/* apu loader function implemented by platform */
status_t platform_load_apu(void *fdt);

/* apusys_rv loader function implemented by platform */
status_t platform_load_apusys_rv(void *fdt);

/* dpm loader function implemented by platform */
status_t platform_load_dpm(void);

/* sspm loader function implemented by platform */
status_t platform_load_sspm(void);

/* mcupm loader function implemented by platform */
status_t platform_load_mcupm(void);

/* tee loader function implemented by platform */
status_t platform_load_tee(void);

/* picachu loader function implemented by platform */
status_t platform_load_picachu(void);

/* adsp loader function implemented by platform */
status_t platform_load_adsp(void *fdt);

/* vcp loader function implemented by platform */
status_t platform_load_vcp(void *fdt);

/* power-related function implemented by platform */
int platform_cold_reset(void);
int platform_power_hold(bool hold);

/* called at EL3 to make sure drivers and platform is put into a stopped state */
void platform_quiesce_el3(void);

/* default pl_boottags hook function for boot reason implemented by platform */
void platform_pl_boottags_boot_reason_hook(struct boot_tag *tag);

/* default pl_boottags hook function for boot mode implemented by platform */
void platform_pl_boottags_boot_mode_hook(struct boot_tag *tag);

/* platform boot reason from pl_boottags */
uint32_t platform_get_boot_reason(void);

/* platform boot mode from pl_boottags */
uint32_t platform_get_boot_mode(void);

/* modem loader function implemented by platform */
status_t platform_load_modem(void *fdt, u8 b_mode);

/* modem loader function implemented by platform */
int ccci_get_md_version(char buf[], int size);

/* medmcu loader function implemented by platform */
status_t platform_load_medmcu(void *fdt);

/* gpueb loader function implemented by platform */
status_t platform_load_gpueb(void *fdt);

/* default device tree load routine implemented by platform */
void *platform_load_device_tree(void);

/* check rootfs sig and extract data */
int platform_load_rootfs_sig(void);

/* get fastboot command idle timeout function implemented by platform */
int get_fastboot_idle_timeout(void);

/* set tz_tags information */
status_t platform_set_tz_tags(void);

/* get the suffix of active AB bootctrl slot */
const char *get_suffix(void);

/* secure function post init */
void platform_sec_post_init(void);

/* uninit secure module */
status_t platform_sec_uninit(void);

uint32_t need_relocate(void);
