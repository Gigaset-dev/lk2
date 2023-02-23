/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <libfdt.h>
#include <mblock.h>
#include <platform/addressmap.h>
#include <platform_mtk.h>
#include <reg.h>
#include <set_fdt.h>

#define CONSYS_EMI_MAX_ADDR (0x29FFFFFFFULL)
#define CONSYS_EMI_MIN_ADDR (0x250000000ULL)

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#define REGION_ID_CONNINFRA 46
#define REGION_ID_SCP 45
#define REGION_ID_PDMA 44
#endif

#define CONNSYS_WIFI_MBLOCK_NAME "shared-dma-pool_wifi-reserve-memory_dma"

struct connsys_dt_dram_info {
    unsigned int start_hi;
    unsigned int start_lo;
    unsigned int size_hi;
    unsigned int size_lo;
};

struct connsys_dt_size_info {
    unsigned int size_hi;
    unsigned int size_lo;
};

static unsigned int consys_get_fdt_prop(void *fdt, int offset, char *name)
{
    const struct fdt_property *prop;
    unsigned int *data;
    unsigned int value;
    int len;

    prop = fdt_get_property(fdt, offset, name, &len);
    if (!prop) {
        dprintf(CRITICAL, "[%s][%s] is not found\n", __func__, name);
        return 0;
    }
    data = (uint32_t *)prop->data;
    value = fdt32_to_cpu(*data);

    return value;
}

static void consys_pmic_setting(void)
{
    unsigned int val;

    /* write 0x1C804150[17] 0x1 ARB_EN */
    val = readl(PMIF_SPMI_BASE + 0x150);
    val = (val & 0xfffdffff) | 0x20000;
    writel(val, PMIF_SPMI_BASE + 0x150);

    /* write 0x1C80405C 0x149b149c */
    /* [31:16]DCXO_CMD_1_ADDR_0 = BUCK_VS2_VOTER_CON0_SET */
    /* [15:0] DCXO_CMD_0_ADDR_0 = BUCK_VS2_VOTER_CON0_CLR */
    writel(0x149b149c, PMIF_SPMI_BASE + 0x5c);

    /* write 0x1C804060 0x00100010 */
    /* [31:16]DCXO_CMD_1_WDATA_0 */
    /* [15:0] DCXO_CMD_0_WDATA_0 */
    writel(0x00100010, PMIF_SPMI_BASE + 0x60);

    /* write 0x1C804788[11:8] 0x4 DCXO_0_REQCTRL_SLVID = 4 */
    val = readl(PMIF_SPMI_BASE + 0x788);
    val = (val & 0xfffff0ff) | 0x400;
    writel(val, PMIF_SPMI_BASE + 0x788);

    /* write 0x1C804788[0] 0x0 DCXO_0_REQCTRL_BYTECNT = 0 (1 Byte) */
    val = readl(PMIF_SPMI_BASE + 0x788);
    val = val & 0xfffffffe;
    writel(val, PMIF_SPMI_BASE + 0x788);

    /* write 0x1C804028[0] 0x1 DCXOINF0_EN */
    val = readl(PMIF_SPMI_BASE + 0x28);
    val = (val & 0xfffffffe) | 0x1;
    writel(val, PMIF_SPMI_BASE + 0x28);

    dprintf(INFO, "%s", __func__);
}

static void consys_init_conninfra(void *fdt)
{
    status_t ret = NO_ERROR;
    paddr_t mblock_pa = 0, scp_shm_pa = 0;
    off_t mblock_sz = 0;
    int offset;
    unsigned int msize, malign, mmax;
    fdt64_t mblock;
    int len;
    const struct fdt_property *prop;
    unsigned int *data;
    fdt64_t scp_shm_addr;
    unsigned int scp_shm_size;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
#endif

    if (!fdt) {
        dprintf(CRITICAL, "%s fdt is NULL\n", __func__);
        return;
    }

    offset = fdt_path_offset(fdt, "/soc/consys");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/consys");
        if (offset < 0) {
            dprintf(CRITICAL, "%s consys offset is not found\n", __func__);
            return;
        }
    }

    msize = consys_get_fdt_prop(fdt, offset, "emi-size");
    malign = consys_get_fdt_prop(fdt, offset, "emi-alignment");
    mmax = consys_get_fdt_prop(fdt, offset, "emi-max-addr");

    if (msize == 0 || malign == 0 || mmax == 0) {
        dprintf(CRITICAL, "fail to get value from dts\n", __func__);
        return;
    }

    scp_shm_addr = consys_get_fdt_prop(fdt, offset, "scp-shm-addr");
    scp_shm_size = consys_get_fdt_prop(fdt, offset, "scp-shm-size");

    msize += scp_shm_size;
    dprintf(CRITICAL, "[%s] scp_shm [%llx][%x] size=[%x]\n", __func__,
                    scp_shm_addr, scp_shm_size, msize);

    if (need_relocate()) {
        /* to support DRAM 24GB */
        mblock_pa = mblock_alloc_range(msize, malign, CONSYS_EMI_MIN_ADDR, CONSYS_EMI_MAX_ADDR, 0,
            0, "consys_emi_reserved");
    } else {
        mblock_pa = mblock_alloc(msize, malign, mmax, 0, 0, "consys_emi_reserved");
    }

    if (!mblock_pa) {
        dprintf(CRITICAL, "%s mblock allocation failed", __func__);
        return;
    }

    mblock = cpu_to_fdt64(mblock_pa);
    ret = fdt_setprop(fdt, offset, "emi-addr", &mblock, sizeof(mblock));
    if (ret < 0)
        dprintf(CRITICAL, "%s setprop emi-addr fail!! ret = %d\n", __func__, ret);

    if (scp_shm_size > 0) {
        scp_shm_pa = mblock_pa + msize - scp_shm_size;
        scp_shm_addr = cpu_to_fdt64(scp_shm_pa);
        ret = fdt_setprop(fdt, offset, "scp-shm-addr", &scp_shm_addr, sizeof(scp_shm_addr));
        if (ret < 0)
            dprintf(CRITICAL, "%s setprop scp-shm-addr fail!! ret = %d\n", __func__, ret);
    }

    dprintf(CRITICAL, "[%s] pa[%llx][%llx]\n", __func__, mblock, scp_shm_addr);

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = mblock_pa;
    region_info.end = (mblock_pa + msize - scp_shm_size);
    region_info.region = REGION_ID_CONNINFRA;
    emi_mpu_set_protection(&region_info);

    /* consys-scp shm */
    if (scp_shm_pa > 0 && scp_shm_size > 0) {
        region_info.start = scp_shm_pa;
        region_info.end = (scp_shm_pa + scp_shm_size);
        region_info.region = REGION_ID_SCP;
        emi_mpu_set_protection(&region_info);
    }
#endif
}

static void consys_init_wifi(void *fdt)
{
    status_t ret = NO_ERROR;
    paddr_t mblock_pa = 0;
    int offset;
    unsigned int mblock, msize, malign, mmax;
    struct connsys_dt_dram_info rsv_mem_reg_property = {0};
    struct connsys_dt_size_info rsv_mem_size_property = {0};
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
#endif

    if (!fdt) {
        dprintf(CRITICAL, "%s fdt is NULL\n", __func__);
        return;
    }

    offset = fdt_path_offset(fdt, "/soc/wifi");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/wifi");
        if (offset < 0) {
            dprintf(CRITICAL, "%s consys offset is not found\n", __func__);
            return;
        }
    }

    msize = consys_get_fdt_prop(fdt, offset, "emi-size");
    malign = consys_get_fdt_prop(fdt, offset, "emi-alignment");
    mmax = consys_get_fdt_prop(fdt, offset, "emi-max-addr");

    if (msize == 0 || malign == 0 || mmax == 0) {
        dprintf(CRITICAL, "fail to get value from dts\n", __func__);
        return;
    }


    /*
     *   Create reserve-memory use mblock API
     *
     *   wifi_mem: wifi-reserve-memory {
     *      compatible = "shared-dma-pool";
     *      no-map;
     *      size = <0 0xf20000>;
     *      alignment = <0 0x1000000>;
     *      alloc-ranges = <0 0x40000000 0 0x80000000>;
     *   };
     *
     */

    /* Generate reserve-memory with compatible = "shared-dma-pool" */
    /* and add subnode to fdt */
    if (need_relocate()) {
        /* to support DRAM 24GB */
        mblock_pa = mblock_alloc_range(msize, malign,
            CONSYS_EMI_MIN_ADDR, CONSYS_EMI_MAX_ADDR, 0,
            0, CONNSYS_WIFI_MBLOCK_NAME);
    } else {
        mblock_pa = mblock_alloc(msize, malign, mmax, 0, 0, CONNSYS_WIFI_MBLOCK_NAME);
    }

    if (!mblock_pa) {
        dprintf(CRITICAL, "%s mblock allocation failed\n", __func__);
        return;
    }

    if (fdt_path_offset(fdt, "/soc/wifi") >= 0)
        ret = mblock_pre_alloc_phandle("/soc/wifi", CONNSYS_WIFI_MBLOCK_NAME);
    else
        ret = mblock_pre_alloc_phandle("/wifi", CONNSYS_WIFI_MBLOCK_NAME);
    if (ret)
        dprintf(CRITICAL, "%s pre alloc phandle failed!! ret = %d\n", __func__, ret);

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = mblock_pa;
    region_info.end = (mblock_pa + msize);
    region_info.region = REGION_ID_PDMA;
    emi_mpu_set_protection(&region_info);
#endif
}

static void consys_init(void *fdt)
{
    consys_pmic_setting();
    consys_init_conninfra(fdt);
    consys_init_wifi(fdt);
}


SET_FDT_INIT_HOOK(consys_init, consys_init);
