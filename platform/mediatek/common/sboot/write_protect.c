/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bootctrl.h>
#include <debug.h>
#include <lib/bio.h>
#include <partition_wp.h>
#include <platform/boot_mode.h>
#include <platform/wdt.h>
#include <platform/reg.h>
#include <sboot.h>
#include <stdlib.h>
#include <string.h>
#include <sysenv.h>
#include <ufs_interface.h>
#include <ufs_platform.h>
#include <verified_boot_state.h>
#include <write_protect.h>

#define WRITE_PROTECT_PARTITION_NAME_SZ (32)

static int set_write_protect(void)
{
    int err = 0;
    int wp_type = WP_POWER_ON;
    const char *wp_start;
    const char *wp_end;
    const char *tmp_name;
    const char *ab_suffix = (char *)get_suffix();

    /* eMMC keep WP_POWER_ON, UFS change to WP_PERMANENT_AWP */
    if ((read32(GPIO_BANK_BASE) & TRAP_UFS_FIRST) != 0)
        wp_type = WP_PERMANENT_AWP;

    mtk_wdt_restart_timer();
    err = sec_sync_sml_data_adapter();
    if (err != 0) {
        dprintf(CRITICAL, "[%s] SIMLOCK Data Not sync(0x%x)\n", __func__, err);
        return err;
    }

    /* group 1, OTP must in 1st entry, cannot move */
    if (ufs_otp_lock_req("otp") == 1) {
        err = partition_write_prot_set("otp", "otp", WP_PERMANENT);
        dprintf(CRITICAL, "[%s] Lock otp partition ...\n", __func__);
    }

    if (err != 0) {
        dprintf(CRITICAL, "[%s]: Lock otp failed:%d\n", __func__, err);
        return err;
    }

    /* group 2 */
    err = partition_write_prot_set("preloader", "preloader", wp_type);

    if (err != 0) {
        dprintf(CRITICAL, "[%s] Lock preloader partition failed: %d\n", __func__, err);
        return err;
    }

    /* group 3 */
    if (is_ab_enable()) {
        if (ab_suffix[1] == 'a') {
            wp_start = "md1img_a";
            wp_end = "logo";
        } else {
            wp_start = "sec1";
            tmp_name = "system_b";
            if (bio_open_by_label(tmp_name))
                wp_end = "system_b";
            else
                wp_end = "tee_b";
        }

        dprintf(CRITICAL, "[%s]: Lock %s->%s\n", __func__, wp_start, wp_end);
        err = partition_write_prot_set(wp_start, wp_end, wp_type);

        if (err != 0) {
            dprintf(CRITICAL, "[%s]: Lock %s->%s failed:%d\n",
                __func__, wp_start, wp_end, err);
            return err;
        }

    } else {
        wp_start = "sec1";

        if (bio_open_by_label("system"))
            wp_end = "system";
        else
            wp_end = "super";

        dprintf(CRITICAL, "[%s]: Lock %s->%s\n", __func__, wp_start, wp_end);
        err = partition_write_prot_set(wp_start, wp_end, wp_type);

        if (err != 0) {
            dprintf(CRITICAL, "[%s]: Lock %s->%s failed:%d\n",
                __func__, wp_start, wp_end, err);
            return err;
        }

      }

    /* group 4 */
    if (sec_is_protect2_ready_for_wp_adapter() == 0)
        wp_start = "protect2";
    else
        wp_start = "seccfg";

    wp_end = "seccfg";
    dprintf(CRITICAL, "[%s] Lock %s->%s\n", __func__, wp_start, wp_end);
    err = partition_write_prot_set(wp_start, wp_end, wp_type);

    if (err != 0) {
        dprintf(CRITICAL, "[%s]: Lock %s->%s failed:%d\n",
            __func__, wp_start, wp_end, err);
        return err;
        }

    return err;
}

int write_protect_flow(u8 boot_mode, uint32_t boot_state)
{
    int ret = 0;

#if (MTK_SECURITY_SW_SUPPORT)
    if (boot_mode == META_BOOT || boot_mode == FACTORY_BOOT || boot_state == BOOT_STATE_ORANGE) {
        dprintf(CRITICAL, "[SEC]Bypass Write Protect!\n");
        return ret;
    }

    ret = set_write_protect();
#endif

    return ret;
}

