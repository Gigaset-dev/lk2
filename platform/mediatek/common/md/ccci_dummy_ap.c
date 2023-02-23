/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <libfdt.h>
#include <load_image.h>
#include <malloc.h>
#include <mt_gic.h>
#include <platform.h>
#include <platform/interrupts.h>
#include <platform/wdt.h>
#include <smc.h>
#include <smc_id_table.h>
#include <string.h>
#include <sys/types.h>

#include "ccci_dummy_ap_plat.h"
#include "ccci_ld_md_core.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_sec.h"
#include "ccci_lk_plat.h"
#include "ccci_lk_plat_md_pwr.h"

#define LOCAL_TRACE         0

/* ========================================================================
 * AP WDT relate functions
 * ========================================================================
 */
static void disable_ap_wdt(void)
{
    ALWAYS_LOG("Disable AP WDT\n");
    mtk_wdt_config(false);
}

static void reset_ap(void)
{
    platform_halt(HALT_ACTION_REBOOT, HALT_REASON_SW_RESET);
}

/* ========================================================================
 * DummyAP arguments relate
 * ========================================================================
 */
static char s_cmd_buffer[128];
static unsigned int s_by_pass_brom;
static unsigned int s_by_pass_let_md_go;
static unsigned int s_md_wdt_work_mode;

enum {
    IGNORE_MD_WDT = 0,
    RESET_AP,
    RESET_MD_BY_SPM,
    RESET_MD_BY_RGU
};

static void setting_dump(void)
{
    const char *cfg;

    if (s_by_pass_brom)
        cfg = "[Yes]";
    else
        cfg = "[No]";
    ALWAYS_LOG("Bypass BROM:      %s\n", cfg);

    if (s_by_pass_let_md_go)
        cfg = "[Yes]";
    else
        cfg = "[No]";
    ALWAYS_LOG("Bypass MD boot:   %s\n", cfg);

    switch (s_md_wdt_work_mode) {
    case IGNORE_MD_WDT:
        cfg = "[Ignore MD WDT]";
        break;
    case RESET_AP:
        cfg = "[Reset AP]";
        break;
    case RESET_MD_BY_SPM:
        cfg = "[Reset MD only by SPM]";
        break;
    case RESET_MD_BY_RGU:
        cfg = "[Reset MD only by RGU]";
        break;
    default:
        cfg = "[Reset AP-def]";
        s_md_wdt_work_mode = RESET_AP;
        break;
    }
    ALWAYS_LOG("MD WDT work mode: %s\n", cfg);
}

static void apply_setting(char cmd[], char val[])
{
    int value;

    ALWAYS_LOG("Get: [%s]:[%s]\n", cmd, val);
    if (strcmp("by_pass_brom", (const char *)cmd) == 0) {
        value = atoi((const char *)val);
        s_by_pass_brom = value;
        ALWAYS_LOG("Set: [%s] to %d\n", cmd, value);
        return;
    }
    if (strcmp("by_pass_trigger_md_boot", (const char *)cmd) == 0) {
        value = atoi((const char *)val);
        s_by_pass_let_md_go = value;
        ALWAYS_LOG("Set: [%s] to %d\n", cmd, value);
        return;
    }
    if (strcmp("md_wdt_work_mode", (const char *)cmd) == 0) {
        value = atoi((const char *)val);
        s_md_wdt_work_mode = value;
        ALWAYS_LOG("Set: [%s] to %d\n", cmd, value);
        return;
    }
}

static void get_command_and_update(char cmd[], int size)
{
    int i, j = 0;
    int step = 0;
    char curr_cmd[64];
    char val[64];

    for (i = 0; i < size; i++) {
        if (!(((cmd[i] >= '0') && (cmd[i] <= '9')) ||
              ((cmd[i] >= 'A') && (cmd[i] <= 'Z')) ||
              ((cmd[i] >= 'a') && (cmd[i] <= 'z')) ||
              (cmd[i] == '\n') ||
              (cmd[i] == '_')  ||
              (cmd[i] == '=')))
            continue;

        if (step == 0) { // Find start
            if (cmd[i] != '\n') {
                curr_cmd[j++] = cmd[i];
                step = 1;
            }
        } else if (step == 1) { // Find command end
            if (cmd[i] == '=') {
                curr_cmd[j] = 0;
                step = 2;
                j = 0;
            } else
                curr_cmd[j++] = cmd[i];
        } else if (step == 2) { // Find value
            if (cmd[i] != '\n') {
                val[j++] = cmd[i];
                continue;
            }
            val[j] = 0;
            j = 0;
            apply_setting(curr_cmd, val);
            step = 0;
        }
    }
    if (j) {
        val[j] = 0;
        apply_setting(curr_cmd, val);
    }
}

static void arguments_parsing(int size)
{
    /* Make default value */
    s_by_pass_brom = 1;
    s_by_pass_let_md_go = 0;
    s_md_wdt_work_mode = RESET_AP;

    /* change setting by command */
    if (size > 0)
        get_command_and_update(s_cmd_buffer, size);
    else
        ALWAYS_LOG("Using default setting\n");

    /* Dump final setting */
    setting_dump();
}

static int load_settings(void)
{
    return load_partition(CFG_PARTITION, "dummy_ap_cfg", s_cmd_buffer, sizeof(s_cmd_buffer));
}

/* ========================================================================
 * Let modem go
 * ========================================================================
 */

static void let_md_go(void)
{
    struct __smccc_res res;

    if (s_by_pass_brom) {
        __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_POWER_CONFIG, MD_LK_BYPASS_BROM,
                      1, 0, 0, 0, 0, &res);
        ALWAYS_LOG("bypass BROM result:0x%lx\n", res.a0);
    } else
        ALWAYS_LOG("BROM will be run\n");

    if (!s_by_pass_let_md_go) {
        __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_POWER_CONFIG, MD_LK_BOOT_UP_FOR_UNGATE,
                      1, 0, 0, 0, 0, &res);
        ALWAYS_LOG("bypass brom result:0x%lx\n", res.a0);
    } else
        ALWAYS_LOG("Skip trigger boot\n");
}

/* ========================================================================
 * MD WDT relate functions
 * ========================================================================
 */
#define IRQ_POLARITY_ACTIVE_HIGH    0
static unsigned int s_mem_cnt;
static enum handler_return md_wdt_irq_handler(void *arg)
{
    unsigned int cnt;

    ALWAYS_LOG("Get MD WDT IRQ\n");
    cnt = ccci_get_ap_rgu_wdt_non_rst_reg_val();

    // update counter
    cnt++;
    s_mem_cnt++;
    ccci_set_ap_rgu_wdt_non_rst_reg_val(cnt);
    ALWAYS_LOG("Current wdt cnt :%d@RGU, %u@Memory\n", cnt, s_mem_cnt);

    switch (s_md_wdt_work_mode) {
    case IGNORE_MD_WDT:
        ALWAYS_LOG("Ignore it\n");
        break;

    case RESET_AP:
        ALWAYS_LOG("Reset AP\n");
        reset_ap();
        break;

    case RESET_MD_BY_SPM:
        ALWAYS_LOG("Reset MD by SPM\n");
        ccci_md_power_off();
        ALWAYS_LOG("Power off MD......\n");
        spin(5000);
        ALWAYS_LOG("Power on MD......\n");
        ccci_test_md_power_on();
        let_md_go();
        break;

    case RESET_MD_BY_RGU:
        ALWAYS_LOG("Reset MD by RGU -- Phase out\n");

        break;

    default:
        ALWAYS_LOG("Reset AP--default\n");
        reset_ap();
        break;
    }

    return INT_RESCHEDULE;
}

static void md_wdt_irq_init(void *fdt)
{
    int node_offset = 0, prop_offset = 0;
    int len;
    const char *name = NULL;
    const void *val;
    fdt32_t *ptr = NULL;
    unsigned int irq_id = 0, attr = 0;

    ALWAYS_LOG("Init MD WDT start ...\n");

    node_offset = fdt_path_offset(fdt, "/mddriver");
    if (node_offset < 0) {
        ALWAYS_LOG("[ccci]/mddriver not found, try /soc/mddriver\n");
        node_offset = fdt_path_offset(fdt, "/soc/mddriver");
        if (node_offset < 0) {
            ALWAYS_LOG("[ccci] /soc/mddriver not found!\n");
            return;
        }
    }

    for (prop_offset = fdt_first_property_offset(fdt, node_offset);
        prop_offset >= 0; prop_offset = fdt_next_property_offset(fdt, prop_offset)) {

        len = 0;
        val = fdt_getprop_by_offset(fdt, prop_offset, &name, &len);

        if (val) {
            if (strncmp(name, "interrupts", strlen("interrupts")) == 0) {
                ptr = (fdt32_t *)val;
                ptr++; // Skip GIC_SPI
                irq_id = fdt32_to_cpu(*ptr);
                ptr++;
                attr = fdt32_to_cpu(*ptr);
                ALWAYS_LOG("[ccci] irq_id: %d attr:%d\n", irq_id, attr);
            }
        } else
            ALWAYS_LOG("[ccci] %s line:%d: return NULL\n", __func__, __LINE__);
    }

    if (!irq_id) {
        ALWAYS_LOG("[ccci] Get irq_id from device tree fail\n");
        return;
    }

    irq_id += GIC_OFFSET_NUM;
    ALWAYS_LOG("[ccci] irq_id(+offset:%d): %d\n", GIC_OFFSET_NUM, irq_id);

    if (attr)
        mt_irq_set_sens(irq_id, GIC_EDGE_SENSITIVE);
    else
        mt_irq_set_sens(irq_id, GIC_LEVEL_SENSITIVE);
    mt_irq_set_polarity(irq_id, IRQ_POLARITY_ACTIVE_HIGH);
    register_int_handler(irq_id, md_wdt_irq_handler, NULL);
    unmask_interrupt(irq_id);
    ALWAYS_LOG("Init MD WDT done!!\n");
}

/* ========================================================================
 * Dummy AP main function
 * ========================================================================
 */

static const struct download_info s_md_download_list_dummy_ap_def[] = {
    /* img type | partition | image name | ref_flag */
    {main_img,    "md1img",   "md1rom",     DUMMY_AP_FLAG_MAIN},
    {dsp_img,     "super",    "md1dsp",     DUMMY_AP_FLAG},
    {ramdisk_img, "userdata", "md1ramdisk", DUMMY_AP_FLAG},
    {0,       NULL,       NULL,     0},
};

const struct download_info *ccci_get_tbl_for_test(void)
{
    return s_md_download_list_dummy_ap_def;
}

static void dummy_ap_task(void *fdt)
{
    int size;
    int i = 5;
    struct __smccc_res res;

    ALWAYS_LOG("Welcome to use dummy AP! v0.2\n");

    /* Disable AP WDT */
    disable_ap_wdt();

    if (!fdt) {
        ALWAYS_LOG("Dummy AP meet some error, enter while(1), -^-!!!\n");
        while (1)
            ;
    }
    /* Get MD WDT info and init */
    md_wdt_irq_init(fdt);

    /* Load dummy ap configure from storage*/
    size = load_settings();

    /* Parsing settings */
    arguments_parsing(size);

    /* Special init for dummy AP */
    ccci_md_special_init();

    /* Power on MD */
    ccci_test_md_power_on();

    /* Trigger modem run by setting */
    let_md_go();

    while (i--) {
        spin(1000000);
        __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_POWER_CONFIG, MD_LK_GET_BOOT_UP_STATUS,
                      0, 0, 0, 0, 0, &res);
        ALWAYS_LOG("boot status[0x%lX], boot check[0x%lX : 0x%lX]\n",
                   res.a1, res.a2, res.a3);
    }

    /* Enter while 1 */
    ALWAYS_LOG("enter while(1), ^O^!!!!!!!!!\n");
    while (1)
        ;
}

/* Export to external ==================================== */
void ccci_post_process(void *fdt)
{
    dummy_ap_task(fdt);
}

