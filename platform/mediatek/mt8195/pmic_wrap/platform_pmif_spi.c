/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <err.h>
#include <compiler.h>
#include <lk/init.h>
#include <pmif_common.h>
#include <spmi_common.h>
#include <platform/reg.h>
#include <pmif.h>
#include <pmic_wrap_common.h>
#include <platform/pll.h>
#include <platform/wait.h>

static const struct pmif pmif_spi_arb[] = {
    {
        .pmifid = PMIF_SPI,
        .write_field = pwrap_write_field,
        .read_field = pwrap_read_field,
    },
};

static void pmif_spi_config(void)
{
    /* Set srclk_en always valid regardless of ulposc_sel_for_scp */
    write32(&mtk_pwrap->spi_mode_ctrl,
            read32(&mtk_pwrap->spi_mode_ctrl) & ~(0x1 << 7));

    /* Set SPI mode controlled by srclk_en and srvol_en instead of pmif_rdy */
    write32(&mtk_pwrap->spi_mode_ctrl,
            read32(&mtk_pwrap->spi_mode_ctrl) | 0x3 << 12);
    write32(&mtk_pwrap->spi_mode_ctrl,
            read32(&mtk_pwrap->spi_mode_ctrl) & ~(0x1 << 11));

    /* Set spm_sleep_req input from the outside */
    write32(&mtk_pwrap->sleep_protection_ctrl,
            read32(&mtk_pwrap->sleep_protection_ctrl) & ~0x3);

    /* Set scp_sleep_req input from the outside */
    write32(&mtk_pwrap->sleep_protection_ctrl,
            read32(&mtk_pwrap->sleep_protection_ctrl) & ~(0x3 << 9));

    /* Enable SWINF for AP */
    write32(&mtk_pwrap->inf_en, PMIF_SPI_AP);

    /* Enable arbitration for SWINF for AP */
    write32(&mtk_pwrap->arb_en, PMIF_SPI_AP);

    /* Enable PMIF_SPI Command Issue */
    write32(&mtk_pwrap->cmdissue_en, 1);
}

static int reset_spislv(void)
{
    u32 pmicspi_mst_dio_en_backup;

    write32(&mtk_pmicspi_mst->wrap_en, 0);
    write32(&mtk_pmicspi_mst->mux_sel, 1);
    write32(&mtk_pmicspi_mst->man_en, 1);
    pmicspi_mst_dio_en_backup = read32(&mtk_pmicspi_mst->dio_en);
    write32(&mtk_pmicspi_mst->dio_en, 0);

    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_CSL  << 8));
    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_OUTS << 8));
    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_CSH  << 8));
    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_OUTS << 8));
    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_OUTS << 8));
    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_OUTS << 8));
    write32(&mtk_pmicspi_mst->man_acc, (0x1 << 13) | (OP_OUTS << 8));


    /* Wait for PMIC SPI Master to be idle */
    if (check_idle(&mtk_pmicspi_mst->other_busy_sta_0, SPIMST_STA)) {
        dprintf(CRITICAL, "[%s] spi master busy, timeout\n", __func__);
        return ERR_TIMED_OUT;
    }

    write32(&mtk_pmicspi_mst->man_en, 0);
    write32(&mtk_pmicspi_mst->mux_sel, 0);
    write32(&mtk_pmicspi_mst->wrap_en, 1);
    write32(&mtk_pmicspi_mst->dio_en, pmicspi_mst_dio_en_backup);

    return 0;
}

static void init_reg_clock(struct pmif *arb)
{
    /* Set SoC SPI IO driving strength to 4 mA */
    write32(&mtk_iocfg->drv_cfg1_clr, 0x3F << 24);
    write32(&mtk_iocfg->drv_cfg2_clr, 0x3F);
    write32(&mtk_iocfg->drv_cfg1_set, 0x9  << 24);
    write32(&mtk_iocfg->drv_cfg2_set, 0x9);

    /* Configure SPI protocol */
    write32(&mtk_pmicspi_mst->ext_ck_write, 1);
    write32(&mtk_pmicspi_mst->ext_ck_read, 0);
    write32(&mtk_pmicspi_mst->cshext_write, 0);
    write32(&mtk_pmicspi_mst->cshext_read, 0);
    write32(&mtk_pmicspi_mst->cslext_write, 0);
    write32(&mtk_pmicspi_mst->cslext_read, 0x100);

    /* Set Read Dummy Cycle Number (Slave Clock is 18MHz) */
    arb->write_field(PMIC_DEW_RDDMY_NO, DUMMY_READ_CYCLES, 0xffff, 0x0);
    write32(&mtk_pmicspi_mst->rddmy, DUMMY_READ_CYCLES);

    /* Enable DIO mode */
    arb->write_field(PMIC_DEW_DIO_EN, 0x1, 0xffff, 0x0);

    /* Wait for completion of sending the commands */
    if (check_idle(&mtk_pwrap->inf_busy_sta, PMIF_SPI_AP)) {
        dprintf(CRITICAL, "[%s] pmif channel busy, timeout\n", __func__);
        return;
    }

    if (check_idle(&mtk_pwrap->other_busy_sta_0, PMIF_CMD_STA)) {
        dprintf(CRITICAL, "[%s] pmif cmd busy, timeout\n", __func__);
        return;
    }

    if (check_idle(&mtk_pmicspi_mst->other_busy_sta_0, SPIMST_STA)) {
        dprintf(CRITICAL, "[%s] spi master busy, timeout\n", __func__);
        return;
    }

    write32(&mtk_pmicspi_mst->dio_en, 1);
}

static void init_spislv(struct pmif *arb)
{
    /* Turn on SPI IO filter function */
    arb->write_field(PMIC_FILTER_CON0, SPI_FILTER, 0xffff, 0x0);
    /* Turn on SPI IO SMT function to improve noise immunity */
    arb->write_field(PMIC_SMT_CON1, SPI_SMT, 0xffff, 0x0);
    /* Turn off SPI IO pull function for power saving */
    arb->write_field(PMIC_GPIO_PULLEN0_CLR, SPI_PULL_DISABLE, 0xffff, 0x0);
    /* Enable SPI access in SODI-3.0 and Suspend modes */
    arb->write_field(PMIC_RG_SPI_CON0, 0x2, 0xffff, 0x0);
    /* Set SPI IO driving strength to 4 mA */
    arb->write_field(PMIC_DRV_CON1, SPI_DRIVING, 0xffff, 0x0);
}

static int init_sistrobe(struct pmif *arb)
{
    u32 rdata = 0;
    int si_sample_ctrl;
    /* Random data for testing */
    const u32 test_data[30] = {
        0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996,
        0x9669, 0x6996, 0x9669, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A,
        0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x1B27,
        0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27,
        0x1B27, 0x1B27
    };

    for (si_sample_ctrl = 0; si_sample_ctrl < 16; si_sample_ctrl++) {
        write32(&mtk_pmicspi_mst->si_sampling_ctrl, si_sample_ctrl << 5);

        rdata = arb->read_field(PMIC_DEW_READ_TEST, 0xffff, 0x0);
        if (rdata == DEFAULT_VALUE_READ_TEST)
            break;
    }

    if (si_sample_ctrl >= 15) {
        dprintf(CRITICAL, "[%s] Failed to sampling = %#x, data = %#x.\n",
                __func__, si_sample_ctrl, rdata);
        return ERR_NOT_VALID;
    }

    /*
     * Add the delay time of SPI data from PMIC to align the start boundary
     * to current sampling clock edge.
     */
    for (int si_dly = 0; si_dly < 10; si_dly++) {
        arb->write_field(PMIC_RG_SPI_CON2, si_dly, 0xffff, 0x0);

        int start_boundary_found = 0;

        for (int i = 0; i < countof(test_data); i++) {
            arb->write_field(PMIC_DEW_WRITE_TEST, test_data[i], 0xffff, 0x0);
            rdata = arb->read_field(PMIC_DEW_WRITE_TEST, 0xffff, 0x0);
            if ((rdata & 0x7fff) != (test_data[i] & 0x7fff)) {
                start_boundary_found = 1;
                break;
            }
        }
        if (start_boundary_found == 1)
            break;
    }

    /*
     * Change the sampling clock edge to the next one which is the middle
     * of SPI data window.
     */
    write32(&mtk_pmicspi_mst->si_sampling_ctrl, ++si_sample_ctrl << 5);

    /* Read Test */
    rdata = arb->read_field(PMIC_DEW_READ_TEST, 0xffff, 0x0);
    if (rdata != DEFAULT_VALUE_READ_TEST) {
        dprintf(CRITICAL, "[%s] Failed for read test, data = %#x.\n",
                __func__, rdata);
        return ERR_NOT_VALID;
    }

    return 0;
}

static void init_staupd(struct pmif *arb)
{
    /* Unlock SPI Slave registers */
    arb->write_field(PMIC_SPISLV_KEY, 0xbade, 0xffff, 0x0);

    /* Enable CRC of PMIC 0 */
    arb->write_field(PMIC_DEW_CRC_EN, 0x1, 0xffff, 0x0);

    /* Wait for completion of sending the commands */
    if (check_idle(&mtk_pwrap->inf_busy_sta, PMIF_SPI_AP)) {
        dprintf(CRITICAL, "[%s] pmif channel busy, timeout\n", __func__);
        return;
    }

    if (check_idle(&mtk_pwrap->other_busy_sta_0, PMIF_CMD_STA)) {
        dprintf(CRITICAL, "[%s] pmif cmd busy, timeout\n", __func__);
        return;
    }

    if (check_idle(&mtk_pmicspi_mst->other_busy_sta_0, SPIMST_STA)) {
        dprintf(CRITICAL, "[%s] spi master busy, timeout\n", __func__);
        return;
    }

    /* Configure CRC of PMIC Interface */
    write32(&mtk_pwrap->crc_ctrl, 0x1);
    write32(&mtk_pwrap->sig_mode, 0x0);

    /* Lock SPI Slave registers */
    arb->write_field(PMIC_SPISLV_KEY, 0x0, 0xffff, 0x0);

    /* Set up PMIC Siganature */
    write32(&mtk_pwrap->pmic_sig_addr, PMIC_DEW_CRC_VAL);

    /* Set up PMIC EINT */
    write32(&mtk_pwrap->pmic_eint_sta_addr, PMIC_INT_STA);

    /* Enable Status update for PMIC and Setup period to 100us */
    write32(&mtk_pwrap->staupd_ctrl, (0x5 << 4) | 0x5);

    /* Enable TSX/DCXO for GPS AUXADC */
    write32(&mtk_pwrap->staupd_ctrl,
            read32(&mtk_pwrap->staupd_ctrl) | (0xc << 8));
}

int pmif_spi_init(struct pmif *arb)
{
    pmif_spi_config();

    /* Reset spislv */
    if (reset_spislv())
        return ERR_NOT_READY;

    /* Enable WRAP */
    write32(&mtk_pmicspi_mst->wrap_en, 0x1);

    /* SPI Waveform Configuration */
    init_reg_clock(arb);

    /* SPI Slave Configuration */
    init_spislv(arb);

    /* Input data calibration flow; */
    if (init_sistrobe(arb)) {
        dprintf(CRITICAL, "[%s] data calibration fail\n", __func__);
        return ERR_NOT_READY;
    }

    /* Lock SPISLV Registers */
    arb->write_field(PMIC_SPISLV_KEY, 0x0, 0xffff, 0x0);

    /*
     * Status update function initialization
     * 1. Check signature using CRC (CRC 0 only)
     * 2. Update EINT
     * 3. Read back AUXADC thermal data for GPS
     */
    init_staupd(arb);

    /* Configure PMIF Timer */
    write32(&mtk_pwrap->timer_ctrl, 0x3);

    /* Enable interfaces and arbitration */
    write32(&mtk_pwrap->inf_en, PMIF_SPI_HW_INF | PMIF_SPI_MD |
            PMIF_SPI_AP_SECURE | PMIF_SPI_AP);

    write32(&mtk_pwrap->arb_en, PMIF_SPI_HW_INF | PMIF_SPI_MD | PMIF_SPI_AP_SECURE |
            PMIF_SPI_AP | PMIF_SPI_STAUPD | PMIF_SPI_TSX_HW | PMIF_SPI_DCXO_HW);

    /* Enable GPS AUXADC HW 0 and 1 */
    write32(&mtk_pwrap->other_inf_en,
            read32(&mtk_pwrap->other_inf_en) | (0x3 << 4));

    /* Set INIT_DONE */
    write32(&mtk_pwrap->init_done, 0x1);

    return 0;
}

void platform_pmif_spi_init(void)
{
    if (pmif_spi_init(&pmif_spi_arb[0]))
        dprintf(CRITICAL, "[%s] init fails\n", __func__);
}
