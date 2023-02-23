/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <compiler.h>
#include <err.h>
#include <lk/init.h>
#include <platform/pll.h>
#include <platform/reg.h>
#include <pmif_common.h>
#include <pmif.h>
#include <spmi_common.h>

#define SPMI_GROUP_ID   0xB
#define PMIF_CMD_PER_3      (0x1 << PMIF_CMD_EXT_REG_LONG)
#define PMIF_CMD_PER_1_3    ((0x1 << PMIF_CMD_REG) | (0x1 << PMIF_CMD_EXT_REG_LONG))

#define MT6315_DEFAULT_VALUE_READ 0x15
/* MT6315 registers */
enum {
    MT6315_BASE        = 0x0,
    MT6315_READ_TEST   = MT6315_BASE + 0x9,
    MT6315_READ_TEST_1 = MT6315_BASE + 0xb,
};

static struct pmif mt8195_pmif_spmi_arb[] = {
    {
        .swinf_ch_start = PMIF_SWINF_0_CHAN_NO,
        .swinf_no = PMIF_AP_SWINF_NO,
        .mstid = SPMI_MASTER_0,
        .pmifid = PMIF_SPMI,
        .read_cmd = pmif_spmi_read_cmd,
        .write_cmd = pmif_spmi_write_cmd,
    },
};

static int mt6315_read_check(const struct spmi_device *dev);

static struct spmi_device mt8195_spmi_dev[] = {
    {
        .slvid = SPMI_SLAVE_6,
        .grpiden = 0x1 << SPMI_GROUP_ID,
        .mstid = SPMI_MASTER_0,
        .hwcid_addr = 0x0009,
        .hwcid_val = 0x15,
        .swcid_addr = 0x000B,
        .swcid_val = 0x15,
        .wpk_key_addr = 0x3A8,
        .wpk_key_val = 0x15,
        .wpk_key_h_val = 0x63,
        .tma_key_addr = 0x39F,
        .tma_key_val = 0xEA,
        .tma_key_h_val = 0x9C,
        .pmif_arb = &mt8195_pmif_spmi_arb,
        .read_check = mt6315_read_check,
    }, {
        .slvid = SPMI_SLAVE_7,
        .grpiden = 0x1 << SPMI_GROUP_ID,
        .mstid = SPMI_MASTER_0,
        .hwcid_addr = 0x0009,
        .hwcid_val = 0x15,
        .swcid_addr = 0x000B,
        .swcid_val = 0x15,
        .wpk_key_addr = 0x3A8,
        .wpk_key_val = 0x15,
        .wpk_key_h_val = 0x63,
        .tma_key_addr = 0x39F,
        .tma_key_val = 0xEA,
        .tma_key_h_val = 0x9C,
        .pmif_arb = &mt8195_pmif_spmi_arb,
        .read_check = mt6315_read_check,
    }
};

static int mt6315_read_check(const struct spmi_device *dev)
{
    u32 rdata = 0;

    spmi_ext_register_readl(dev, MT6315_READ_TEST, &rdata, 1);
    if (rdata != MT6315_DEFAULT_VALUE_READ) {
        dprintf(CRITICAL, "%s next, slvid:%d rdata = 0x%x.\n",
                __func__, dev->slvid, rdata);
        return ERR_NOT_VALID;
    }

    spmi_ext_register_readl(dev, MT6315_READ_TEST_1, &rdata, 1);
    if (rdata != MT6315_DEFAULT_VALUE_READ) {
        dprintf(CRITICAL, "%s next, slvid:%d rdata = 0x%x.\n",
                __func__, dev->slvid, rdata);
        return ERR_NOT_VALID;
    }

    return 0;
}

static int spmi_config_master(void)
{
    /* Software reset */
    write32(&mtk_rgu->wdt_swsysrst2, (0x88 << 24) | (0x1 << 23));

    write32(&mtk_topckgen->clk_cfg_17_clr, (0x3ff << 8) | (0x1 << 12) | (0x1 << 15));
    write32(&mtk_topckgen->clk_cfg_17_set, (0x3 << 0x8));
    write32(&mtk_topckgen->clk_cfg_update2, (0x1 << 5));

    /* Software reset */
    write32(&mtk_rgu->wdt_swsysrst2, (0x88 << 24));

    /* Enable SPMI */
    write32(&mtk_spmi_mst->mst_req_en, 1);
    write32(&mtk_spmi_mst->rcs_ctrl, 0x15);

    return 0;
}

static int spmi_cali_rd_clock_polarity(const struct spmi_device *dev)
{
    int i;
    bool success = false;
    const struct cali cali_data[] = {
        {SPMI_CK_DLY_1T, SPMI_CK_POL_POS},
        {SPMI_CK_NO_DLY, SPMI_CK_POL_POS},
        {SPMI_CK_NO_DLY, SPMI_CK_POL_NEG},
        {SPMI_CK_DLY_1T, SPMI_CK_POL_NEG}
    };

    /* Indicate sampling clock polarity, 1: Positive 0: Negative */
    for (i = 0; i < countof(cali_data); i++) {
        write32(&mtk_spmi_mst->mst_sampl,
                (cali_data[i].dly << 0x1) | cali_data[i].pol);
        if (dev->read_check(dev) == 0) {
            success = true;
            break;
        }
    }

    if (!success)
        dprintf(CRITICAL, "ERROR - calibration fail for spmi clk");

    return 0;
}

static int spmi_mst_init(void)
{
    int i;

    /* config IOCFG */
    write32(&mtk_iocfg->eh_cfg_clr, 0x3ff << 6);
    write32(&mtk_iocfg->ies_cfg1_clr, 0x1 << 20);
    write32(&mtk_iocfg->ies_cfg1_set, 0x1 << 21);
    write32(&mtk_iocfg->pu_cfg1_clr, 0x3 << 21);
    write32(&mtk_iocfg->pd_cfg1_clr, 0x3 << 21);
    write32(&mtk_iocfg->smt_cfg0_set, 0x3 << 25);
    write32(&mtk_iocfg->tdsel_cfg1_clr, 0xf << 8);
    write32(&mtk_iocfg->rdsel_cfg0_clr, 0x3 << 22);
    write32(&mtk_iocfg->drv_cfg3_clr, 0x2d);
    write32(&mtk_iocfg->drv_cfg3_set, 0x12);

    spmi_config_master();

    for (i = 0; i < countof(mt8195_spmi_dev); i++)
        spmi_cali_rd_clock_polarity(&mt8195_spmi_dev[i]);

    return 0;
}

static void pmif_spmi_force_normal_mode(void)
{
    unsigned int value = 0;

    value = read32(&mtk_pmif->spi_mode_ctrl);
    /* listen srclken_0 only for entering normal or sleep mode */
    value &= (~(0x1 << 7));
    write32(&mtk_pmif->spi_mode_ctrl, value);

    /* enable srclken_en or srvol_en(VREQ) control */
    value |= (0x3 << 12);
    write32(&mtk_pmif->spi_mode_ctrl, value);
    /* disable srclken_rc control */
    value &= (~(0x1 << 11));
    write32(&mtk_pmif->spi_mode_ctrl, value);

    /* enable spm/scp sleep request */
    value = read32(&mtk_pmif->sleep_protection_ctrl);
    value &= (~0x3);
    write32(&mtk_pmif->sleep_protection_ctrl, value);
    value &= (~(0x3 << 9));
    write32(&mtk_pmif->sleep_protection_ctrl, value);

}

static void pmif_spmi_enable(void)
{
    u32 cmd_per;

    /* clear all cmd permission for per channel */
    write32(&mtk_pmif->inf_cmd_per_0, 0);
    write32(&mtk_pmif->inf_cmd_per_1, 0);
    write32(&mtk_pmif->inf_cmd_per_2, 0);
    write32(&mtk_pmif->inf_cmd_per_3, 0);

    /* enable if we need cmd 0~3 permission for per channel */
    cmd_per = PMIF_CMD_PER_3 << 28 | PMIF_CMD_PER_3 << 24 |
              PMIF_CMD_PER_3 << 20 | PMIF_CMD_PER_3 << 16 |
              PMIF_CMD_PER_3 << 8 | PMIF_CMD_PER_3 << 4 |
              PMIF_CMD_PER_1_3 << 0;
    write32(&mtk_pmif->inf_cmd_per_0, cmd_per);

    cmd_per = PMIF_CMD_PER_3 << 4;
    write32(&mtk_pmif->inf_cmd_per_1, cmd_per);

    /*
     * set bytecnt max limitation.
     * hw bytecnt indicate when we set 0, it can send 1 byte;
     * set 1, it can send 2 byte.
     */
    write32(&mtk_pmif->inf_max_bytecnt_per_0, 0);
    write32(&mtk_pmif->inf_max_bytecnt_per_1, 0);
    write32(&mtk_pmif->inf_max_bytecnt_per_2, 0);
    write32(&mtk_pmif->inf_max_bytecnt_per_3, 0);

    /* Add latency limitation */
    write32(&mtk_pmif->lat_cnter_en, 0x255);
    write32(&mtk_pmif->lat_limit_0, 0);
    write32(&mtk_pmif->lat_limit_1, 0x4);
    write32(&mtk_pmif->lat_limit_2, 0x8);
    write32(&mtk_pmif->lat_limit_4, 0x8);
    write32(&mtk_pmif->lat_limit_6, 0x3FF);
    write32(&mtk_pmif->lat_limit_9, 0x4);
    write32(&mtk_pmif->lat_limit_loading, 0x255);

    write32(&mtk_pmif->timer_ctrl, 0x3);
    write32(&mtk_pmif->init_done, 1);
}

struct pmif *get_pmif_controller(int inf, int mstid)
{
    if (inf == PMIF_SPMI && mstid < countof(mt8195_pmif_spmi_arb))
        return (struct pmif *)&mt8195_pmif_spmi_arb[mstid];

    dprintf(CRITICAL, "Failed to get controller: inf = %d, mstid = %d\n",
            inf, mstid);
    return NULL;
}

static int pmif_spmi_init(void)
{
    if (is_pmif_init_done() != 0) {
        pmif_spmi_force_normal_mode();
        pmif_spmi_enable_swinf(get_pmif_controller(PMIF_SPMI, SPMI_MASTER_0));
        pmif_spmi_enable_cmdIssue();
        pmif_spmi_enable();
        if (is_pmif_init_done())
            return ERR_NOT_READY;
    }

    if (spmi_mst_init()) {
        dprintf(CRITICAL, "[%s] failed to init spmi master\n", __func__);
        return ERR_NOT_FOUND;
    }

    return 0;
}

void platform_pmif_spmi_init(void)
{
    pmif_clk_init();
    pmif_spmi_init();

    spmi_device_register(mt8195_spmi_dev, countof(mt8195_spmi_dev));
}
