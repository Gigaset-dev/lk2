/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <errno.h>
#include <libfdt.h>
#include <platform_halt.h>
#include <platform/dbgtop.h>
#include <platform/mboot_params.h>
#include <platform/wdt.h>
#include <reg.h>
#include <set_fdt.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * The dbgtop-drm hardware module handshakes with other hardware modules
 * (including DRAMC for DDR_RSV_MODE, DfD_mcu, ...etc) on exception reboot.
 * The RGU hardware module cannot reset the system until these handshakings
 * are done.
 * Set RGU's timeout time to 120 seconds (unit 0.5ms) such
 * that dbgtop-drm has enough time to complete handshakings.
 * For legacy platform:
 * Set RGU's timeout time to 60 seconds (which is a magic number).
 */
#define RGU_TIMEOUT 0x3a980

#define DBG_ADDR 1

#define MTK_DBGTOP_MODE                 (DBGTOP_BASE+0x0000)
#define MTK_DBGTOP_DEBUG_CTL            (DBGTOP_BASE+0x0030)
#define MTK_DBGTOP_DEBUG_CTL2           (DBGTOP_BASE+0x0034)
#define MTK_DBGTOP_LATCH_CTL            (DBGTOP_BASE+0x0040)
#define MTK_DBGTOP_LATCH_CTL2           (DBGTOP_BASE+0x0044)
#define MTK_DBGTOP_MFG_REG              (DBGTOP_BASE+0x0060)

/* DBGTOP_MODE */
#define MTK_DBGTOP_MODE_KEY             (0x22000000)
#define MTK_DBGTOP_MODE_DDR_RESERVE     (0x00000001)

/* DBGTOP_DEBUG_CTL */
#define MTK_DBGTOP_DEBUG_CTL_KEY        (0x59000000)
#define MTK_DBGTOP_DVFSRC_PAUSE_PULSE   (0x00080000)
#define MTK_DBGTOP_DVFSRC_SUCECESS_ACK  (0x00800000)

/* DBGTOP_DEBUG_CTL2 */
#define MTK_DBGTOP_DEBUG_CTL2_KEY       (0x55000000)
#define MTK_DBGTOP_DVFSRC_EN            (0x00000200)

/* DBGTOP_LATCH_CTL */
#define MTK_DBGTOP_LATCH_CTL_KEY        (0x95000000)
#define MTK_DBGTOP_LATCH_EN             (0x00000001)
#define MTK_DBGTOP_DVFSRC_LATCH_EN      (0x00002000)

/* DBGTOP_LATCH_CTL2 */
#define MTK_DBGTOP_LATCH_CTL2_KEY       (0x95000000)

#define MTK_DBGTOP_DFD_EN               (0x00020000)
#define MTK_DBGTOP_DFD_THERM1_DIS       (0x00040000)
#define MTK_DBGTOP_DFD_THERM2_DIS       (0x00080000)

#define MTK_DBGTOP_DFD_TIMEOUT_SHIFT    (0)
#define MTK_DBGTOP_DFD_TIMEOUT_MASK     (0x1FFFF << MTK_DBGTOP_DFD_TIMEOUT_SHIFT)

#define MTK_DBGTOP_DFD_EN_V2            (0x00080000)
#define MTK_DBGTOP_DFD_THERM1_DIS_V2    (0x00100000)
#define MTK_DBGTOP_DFD_THERM2_DIS_V2    (0x00200000)

#define MTK_DBGTOP_DFD_TIMEOUT_SHIFT_V2 (0)
#define MTK_DBGTOP_DFD_TIMEOUT_MASK_V2  (0x7FFFF << MTK_DBGTOP_DFD_TIMEOUT_SHIFT_V2)

/* DBGTOP_MFG_REG */
#define MTK_DBGTOP_MFG_REG_KEY          (0x77000000)
#define MTK_DBGTOP_MFG_PWR_ON           (0x00000001)
#define MTK_DBGTOP_MFG_PWR_EN           (0x00000002)

static vaddr_t DBGTOP_BASE;
static u32 dfd_timeout;
static u32 rgu_timeout = RGU_TIMEOUT;
static u32 version = 2;
static int dis_ddr_rsv_mode;

const char *dbgtop_compatible[] = {
    "/soc/drm",
    "/soc/toprgu",
    "/toprgu",
    NULL
};

static int get_dbgtop_base(void *fdt)
{
    int len, offset, i, array_len;
    u32 *data;

    if (!fdt)
        return -ENODATA;

    array_len = countof(dbgtop_compatible);

    for (i = 0; i < array_len; i++) {
        offset = fdt_path_offset(fdt, dbgtop_compatible[i]);
        if (offset >= 0)
            break;
    }

    if (i >= array_len) {
        dprintf(CRITICAL, "%s: couldn't find the drm node\n", __func__);
        return -ENODATA;
    }

    data = (u32 *)fdt_getprop(fdt, offset, "reg", &len);
    if (!data) {
        dprintf(CRITICAL, "%s: couldn't find property reg\n", __func__);
        return -ENODATA;
    }

    DBGTOP_BASE = fdt32_to_cpu(data[DBG_ADDR]);
    dprintf(INFO, "%s: DBGTOP_BASE phys = 0x%lx\n", __func__, DBGTOP_BASE);
    if (!DBGTOP_BASE) {
        dprintf(CRITICAL, "%s: Failed to get dbgtop base\n", __func__);
        return -ENODATA;
    }

    /*
     * NoteXXX: The physical address gotten from the device tree cannot
     *          be dereferenced directly since LK doesn't create a linear
     *          mapping for the hardware registers by default.
     *          Need to manually map the physical address to the virtual
     *          address.
     */
    DBGTOP_BASE += KERNEL_ASPACE_BASE;

    /*
     * Parsing rgu_timeout/dfd_timeout_len from dts to replace the
     * default value for legacy platform ex. mt6893
     */

    data = (u32 *)fdt_getprop(fdt, offset, "rgu_timeout", &len);
    if (data)
        rgu_timeout = fdt32_to_cpu(data[0]);
    else
        dprintf(INFO, "%s: couldn't find property rgu_timeout, use 120s\n", __func__);

    data = (u32 *)fdt_getprop(fdt, offset, "ver", &len);
    if (data)
        version = fdt32_to_cpu(data[0]);
    else
        dprintf(INFO, "%s: couldn't find property dfd_timeout_len, use ver2\n", __func__);

    data = (u32 *)fdt_getprop(fdt, offset, "dis_ddr_rsv_mode", &len);
    if (data) {
        dis_ddr_rsv_mode = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: foundd property dis_ddr_rsv_mode = %d\n", __func__, dis_ddr_rsv_mode);
    }

    return 0;
}

static status_t mtk_dbgtop_shutdown_callback(void *data,
    platform_halt_action suggested_action, platform_halt_reason reason)
{
    int ret;

    if (suggested_action == HALT_ACTION_REBOOT && reason == HALT_REASON_SW_RESET) {
            ret = mtk_dbgtop_dram_reserved(0);
            dprintf(CRITICAL,
                "\n%s to disable DDR_RSV_MODE when lk2 normal reboot\n",
                (ret) ? "Failed" : "Succeeded");
    }
    return 0;
}

/*
 * dbgtop_drm_init: Initialization function
 * @fdt: pointer to flatten device tree of Linux Kernel
 *
 * Return 0 on success, and negative values for failures.
 */
int dbgtop_drm_init(void *fdt)
{
    int ret;

    ret = get_dbgtop_base(fdt);
    if (ret < 0)
        return ret;

    mtk_wdt_dfd_timeout_en(REQ_EN);
    mtk_wdt_dfd_timeout(rgu_timeout);

    if (DBGTOP_BASE) {
        if (version == 1)
            dfd_timeout = readl(MTK_DBGTOP_LATCH_CTL2) &
                    MTK_DBGTOP_DFD_TIMEOUT_MASK;
        else
            dfd_timeout = readl(MTK_DBGTOP_LATCH_CTL2) &
                    MTK_DBGTOP_DFD_TIMEOUT_MASK_V2;
    }

    if (dis_ddr_rsv_mode) {
        /*
         * To de-couple the dbgtop-drm driver and the DRAMC driver (which
         * implement the DDR_RSV_MODE function), set a flag dis_ddr_rsv_mode
         * in the dts file and use the flag for determining whether the
         * dbgtop-drm driver can enable DDR_RSV_MODE.
         */
        ret = mtk_dbgtop_dram_reserved(0);
        dprintf(CRITICAL,
            "\n%s to disable DDR_RSV_MODE through dts\n",
            (ret) ? "Failed" : "Succeeded");
    } else if (!mboot_params_current_abnormal_boot()) {
        ret = mtk_dbgtop_dram_reserved(1);
        dprintf(CRITICAL,
            "\n%s to eable DDR_RSV_MODE in normal-boot mode\n",
            (ret) ? "Failed" : "Succeeded");
    } else
        dprintf(CRITICAL, "Not enable DDR_RSV_MODE in abnormal-boot mode\n");


    /*
     * Need to disable ddr rsv mode when lk2 normal reboot.
     */
    ret = register_platform_halt_callback("mtk_dbgtop_shutdown_callback",
                                        &mtk_dbgtop_shutdown_callback, NULL);
    if (ret) {
        dprintf(CRITICAL, "%s register shutdown callback failed, ret=%d\n", __func__, ret);
        return ret;
    }

#ifdef TEST_DBGTOP
    mtk_dbgtop_dram_reserved(1);
    mtk_dbgtop_is_reserve_ddr_enabled();
    mtk_dbgtop_dfd_count_en(1);
    mtk_dbgtop_dfd_therm1_dis(1);
    mtk_dbgtop_dfd_therm2_dis(0);
    mtk_dbgtop_dfd_timeout(0, 0);
    mtk_dbgtop_dfd_timeout_reset();
#endif

    return 0;
}

/*
 * mtk_dbgtop_dram_reserved: Enable/disable DDR_RSV_MODE
 * @enable: zero to disable DDR_RSV_MODE, non-zero to enable it
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_dram_reserved(int enable)
{
    u32 tmp;

    if (!DBGTOP_BASE)
        return -1;

    tmp = readl(MTK_DBGTOP_MODE);
    tmp = (enable) ? (tmp | MTK_DBGTOP_MODE_DDR_RESERVE) : (tmp & ~MTK_DBGTOP_MODE_DDR_RESERVE);
    tmp |= MTK_DBGTOP_MODE_KEY;
    writel(tmp, MTK_DBGTOP_MODE);

    dprintf(INFO, "%s: MTK_DBGTOP_MODE(0x%x)\n",
        __func__, readl(MTK_DBGTOP_MODE));

    return 0;
}

/*
 * mtk_dbgtop_is_reserve_ddr_enabled: Query if DDR_RSV_MODE is enabled
 *
 * Return 1 if DDR_RSV_MODE is enabled, 0 if it is disabled, negative
 * values if there is any failure.
 */
int mtk_dbgtop_is_reserve_ddr_enabled(void)
{
    unsigned int drm_mode;

    if (!DBGTOP_BASE)
        return -1;

    drm_mode = readl(MTK_DBGTOP_MODE);

    if (drm_mode & 0x1)
        return 1;
    else
        return 0;
}

/*
 * mtk_dbgtop_drm_latch_en: Enable/disable DRM latch
 * @value: zero to disable DRM latch, non-zero to enable it
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_drm_latch_en(int value)
{
    u32 tmp;

    if (!DBGTOP_BASE)
        return -1;

    tmp = readl(MTK_DBGTOP_LATCH_CTL);

    tmp = (value) ? (tmp | MTK_DBGTOP_LATCH_EN) : (tmp & ~MTK_DBGTOP_LATCH_EN);

    tmp |= MTK_DBGTOP_LATCH_CTL_KEY;
    writel(tmp, MTK_DBGTOP_LATCH_CTL);

    dprintf(INFO, "%s: MTK_DBGTOP_LATCH_CTL(0x%x)\n", __func__,
        readl(MTK_DBGTOP_LATCH_CTL));

    return 0;
}

/*
 * mtk_dbgtop_dfd_count_en: Enable/disable DfD handshaking
 * @value: zero to disable DfD handshaking, non-zero to enable it
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_dfd_count_en(int value)
{
    u32 tmp;

    if (!DBGTOP_BASE)
        return -1;

    tmp = readl(MTK_DBGTOP_LATCH_CTL2);

    if (version == 1)
        tmp = (value) ? (tmp | MTK_DBGTOP_DFD_EN) : (tmp & ~MTK_DBGTOP_DFD_EN);
    else
        tmp = (value) ? (tmp | MTK_DBGTOP_DFD_EN_V2) : (tmp & ~MTK_DBGTOP_DFD_EN_V2);

    tmp |= MTK_DBGTOP_LATCH_CTL2_KEY;
    writel(tmp, MTK_DBGTOP_LATCH_CTL2);

    dprintf(INFO, "%s: MTK_DBGTOP_LATCH_CTL2(0x%x)\n", __func__,
        readl(MTK_DBGTOP_LATCH_CTL2));

    return 0;
}

/*
 * mtk_dbgtop_dfd_therm1_dis: Disable DFD_mcu on thermal reboot to avoid damage
 *                            IC due to over-temperature
 * @value: zero to disable the protection, non-zero to enable it
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_dfd_therm1_dis(int value)
{
    u32 tmp;

    if (!DBGTOP_BASE)
        return -1;

    tmp = readl(MTK_DBGTOP_LATCH_CTL2);

    if (version == 1)
        tmp = (value) ? (tmp | MTK_DBGTOP_DFD_THERM1_DIS) : (tmp & ~MTK_DBGTOP_DFD_THERM1_DIS);
    else
        tmp = (value) ?
            (tmp | MTK_DBGTOP_DFD_THERM1_DIS_V2) : (tmp & ~MTK_DBGTOP_DFD_THERM1_DIS_V2);

    tmp |= MTK_DBGTOP_LATCH_CTL2_KEY;
    writel(tmp, MTK_DBGTOP_LATCH_CTL2);

    dprintf(INFO, "%s: MTK_DBGTOP_LATCH_CTL2(0x%x)\n", __func__,
        readl(MTK_DBGTOP_LATCH_CTL2));

    return 0;
}

/*
 * mtk_dbgtop_dfd_therm2_dis: Disable DFD_mcu on thermal reboot to avoid damage
 *                            IC due to over-temperature
 * @value: zero to disable the protection, non-zero to enable it
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_dfd_therm2_dis(int value)
{
    u32 tmp;

    if (!DBGTOP_BASE)
        return -1;

    tmp = readl(MTK_DBGTOP_LATCH_CTL2);

    if (version == 1)
        tmp = (value) ? (tmp | MTK_DBGTOP_DFD_THERM2_DIS) : (tmp & ~MTK_DBGTOP_DFD_THERM2_DIS);
    else
        tmp = (value) ?
            (tmp | MTK_DBGTOP_DFD_THERM2_DIS_V2) : (tmp & ~MTK_DBGTOP_DFD_THERM2_DIS_V2);

    tmp |= MTK_DBGTOP_LATCH_CTL2_KEY;
    writel(tmp, MTK_DBGTOP_LATCH_CTL2);

    dprintf(INFO, "%s: MTK_DBGTOP_LATCH_CTL2(0x%x)\n", __func__,
        readl(MTK_DBGTOP_LATCH_CTL2));

    return 0;
}

/*
 * mtk_dbgtop_dfd_timeout: Set the required timeout value of each caller before
 *                         RGU reset, and take the maximum as timeout value
 * @value_abnormal: timeout value for the abnormal reset (aka. exception reboot)
 * @value_normal: timeout value for normal reset such as adb reboot
 *
 * Note: caller needs to set normal timeout value to 0 by default.
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_dfd_timeout(u32 value_abnormal, u32 value_normal)
{
    u32 tmp;

    if (!DBGTOP_BASE)
        return -1;

    if (version == 1) {
        value_normal <<= MTK_DBGTOP_DFD_TIMEOUT_SHIFT;
        value_normal &= MTK_DBGTOP_DFD_TIMEOUT_MASK;
    } else {
        value_normal <<= MTK_DBGTOP_DFD_TIMEOUT_SHIFT_V2;
        value_normal &= MTK_DBGTOP_DFD_TIMEOUT_MASK_V2;
    }

    if (dfd_timeout < value_normal)
        dfd_timeout = value_normal;

    if (version == 1) {
        value_abnormal <<= MTK_DBGTOP_DFD_TIMEOUT_SHIFT;
        value_abnormal &= MTK_DBGTOP_DFD_TIMEOUT_MASK;
    } else {
        value_abnormal <<= MTK_DBGTOP_DFD_TIMEOUT_SHIFT_V2;
        value_abnormal &= MTK_DBGTOP_DFD_TIMEOUT_MASK_V2;
    }

    /* break if dfd timeout >= target value_abnormal */
    tmp = readl(MTK_DBGTOP_LATCH_CTL2);

    if (version == 1) {
        if ((tmp & MTK_DBGTOP_DFD_TIMEOUT_MASK) >=
            value_abnormal)
        return 0;
    } else {
        if ((tmp & MTK_DBGTOP_DFD_TIMEOUT_MASK_V2) >=
            value_abnormal)
        return 0;
    }

    /* set dfd timeout */
    if (version == 1)
        tmp &= ~MTK_DBGTOP_DFD_TIMEOUT_MASK;
    else
        tmp &= ~MTK_DBGTOP_DFD_TIMEOUT_MASK_V2;

    tmp |= value_abnormal | MTK_DBGTOP_LATCH_CTL2_KEY;
    writel(tmp, MTK_DBGTOP_LATCH_CTL2);

    dprintf(INFO, "%s: MTK_DBGTOP_LATCH_CTL2(0x%x)\n", __func__,
        readl(MTK_DBGTOP_LATCH_CTL2));

    return 0;
}

/*
 * mtk_dbgtop_dfd_timeout_reset: Reset the timeout value for DfD
 *
 * Return 0 on success, and negative values for failures.
 */
int mtk_dbgtop_dfd_timeout_reset(void)
{
    u32 tmp;

    if (!dfd_timeout)
        return -1;

    tmp = readl(MTK_DBGTOP_LATCH_CTL2);

    if (version == 1)
        tmp &= ~MTK_DBGTOP_DFD_TIMEOUT_MASK;
    else
        tmp &= ~MTK_DBGTOP_DFD_TIMEOUT_MASK_V2;

    tmp |= dfd_timeout | MTK_DBGTOP_LATCH_CTL2_KEY;
    writel(tmp, MTK_DBGTOP_LATCH_CTL2);

    dprintf(INFO, "%s: MTK_DBGTOP_LATCH_CTL2(0x%x)\n", __func__,
        readl(MTK_DBGTOP_LATCH_CTL2));

    return 0;
}
