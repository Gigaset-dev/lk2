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

#include <debug.h>
#include <gpio_api.h>
#include <libfdt.h>
#include <lk/init.h>
#include <mtk_key.h>
#include <platform/addressmap.h>
#include <pmic_keys.h>
#include <pmic_psc.h>
#include <printf.h>
#include <reg.h>
#include <trace.h>

#define LOCAL_TRACE 1

#define KP_MEM1    (KPD_BASE + 0x0004)

#define KPD_NUM_KEYS    72

struct keypad_dts_data {
     u32 kpd_sw_pwrkey;
     u32 kpd_hw_pwrkey;
     u32 kpd_sw_rstkey;
     u32 kpd_hw_rstkey;
     u32 kpd_use_extend_type;
     u32 kpd_hw_map_num;
     u32 kpd_hw_init_map[KPD_NUM_KEYS];
     u32 kpd_hw_recovery_key;
     u32 kpd_hw_factory_key;
     u32 kpd_gpio_index;
};

static struct keypad_dts_data kpd_dts_data;

static int key_mapping[MAX_KEY_NUM] = {17, 0, 8, 17, 42, 9, 17, 17, 0};

static bool pmic_just_rst;

static unsigned int kpd_fdt_getprop_u32(const void *fdt, int nodeoffset, const char *name,
                                                   unsigned int default_val)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        return fdt32_to_cpu(*(unsigned int *)data);
    else
        return default_val;
}

static void kpd_fdt_getprop_u32_array(const void *fdt, int nodeoffset, const char *name,
                                      char *out_value, int maxlen)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        memcpy(out_value, data, len > maxlen ? maxlen : len);
    else
        memset(out_value, 0, sizeof(u32) * maxlen);
}

static void init_key_mapping(void)
{
    key_mapping[RECOVERY_KEY] = kpd_dts_data.kpd_hw_recovery_key;
    key_mapping[BOOT_MENU_KEY] = kpd_dts_data.kpd_hw_recovery_key;
    key_mapping[MENU_SELECT_KEY] = kpd_dts_data.kpd_hw_recovery_key;
    key_mapping[FACTORY_KEY] = kpd_dts_data.kpd_hw_factory_key;
    key_mapping[MENU_OK_KEY] = kpd_dts_data.kpd_hw_factory_key;
    key_mapping[PMIC_PWR_KEY] = kpd_dts_data.kpd_hw_pwrkey;
    key_mapping[PMIC_RST_KEY] = kpd_dts_data.kpd_hw_rstkey;
    key_mapping[META_KEY] = 42;
    key_mapping[CAMERA_KEY] = 9;
}

static void get_kpd_dts_info(void *fdt)
{
    int offset;

    memset(&kpd_dts_data, 0, sizeof(kpd_dts_data));

    if (fdt) {
        if (fdt_check_header(fdt) != 0)
            panic("Bad DTB header.\n");
    } else {
        dprintf(CRITICAL, "[kpd] fdt null, fall back to default\n");
        goto apply_default;
    }

    offset = fdt_node_offset_by_compatible(fdt, 0, "mediatek,kp");
    if (offset >= 0) {
        kpd_dts_data.kpd_sw_pwrkey = kpd_fdt_getprop_u32(fdt, offset, "mediatek,sw-pwrkey", 0);
        kpd_dts_data.kpd_hw_pwrkey = kpd_fdt_getprop_u32(fdt, offset, "mediatek,hw-pwrkey", 0);
        kpd_dts_data.kpd_sw_rstkey = kpd_fdt_getprop_u32(fdt, offset, "mediatek,sw-rstkey", 0);
        kpd_dts_data.kpd_hw_rstkey = kpd_fdt_getprop_u32(fdt, offset, "mediatek,hw-rstkey", 0);
        kpd_dts_data.kpd_use_extend_type = kpd_fdt_getprop_u32(fdt, offset,
                                                               "mediatek,use-extend-type", 0);
        kpd_dts_data.kpd_hw_recovery_key = kpd_fdt_getprop_u32(fdt, offset,
                                                               "mediatek,hw-recovery-key", 0);
        kpd_dts_data.kpd_hw_factory_key = kpd_fdt_getprop_u32(fdt, offset,
                                                              "mediatek,hw-factory-key", 0);
        kpd_dts_data.kpd_gpio_index = kpd_fdt_getprop_u32(fdt, offset,
                                                          "mediatek,gpio_key_index", UINT_MAX);
        LTRACEF("kpd_dts_data.kpd_gpio_index = %d\n", kpd_dts_data.kpd_gpio_index);
        LTRACEF("kpd_sw_pwrkey = %d, kpd_hw_pwrkey = %d\n",
                        kpd_dts_data.kpd_sw_pwrkey, kpd_dts_data.kpd_hw_pwrkey);
        LTRACEF("kpd_sw_rstkey = %d, kpd_hw_rstkey = %d\n",
                        kpd_dts_data.kpd_sw_rstkey, kpd_dts_data.kpd_hw_rstkey);
        LTRACEF("kpd_hw_recovery_key = %d, kpd_hw_factory_key = %d\n",
                        kpd_dts_data.kpd_hw_recovery_key, kpd_dts_data.kpd_hw_factory_key);
        kpd_dts_data.kpd_hw_map_num = kpd_fdt_getprop_u32(fdt, offset,
                                                          "mediatek,kpd-hw-map-num", 0);
        kpd_fdt_getprop_u32_array(fdt, offset, "mediatek,kpd-hw-init-map",
                        (char *)kpd_dts_data.kpd_hw_init_map, KPD_NUM_KEYS);
        return;
    }

apply_default:
    /* default setting when dts not found: recovery key = pmic home, factory key = vol down*/
    kpd_dts_data.kpd_hw_pwrkey = 8;
    kpd_dts_data.kpd_hw_rstkey = 17;
    kpd_dts_data.kpd_hw_recovery_key = kpd_dts_data.kpd_hw_rstkey;
    kpd_dts_data.kpd_hw_factory_key = 0;
    kpd_dts_data.kpd_gpio_index = UINT_MAX; // default no gpio key support
}

static void mtk_kpd_gpio_set(void)
{
    if (kpd_dts_data.kpd_gpio_index != UINT_MAX) {
        mt_set_gpio_mode(kpd_dts_data.kpd_gpio_index, 0); // GPIO mode 0
        mt_set_gpio_pull_enable(kpd_dts_data.kpd_gpio_index, 1); // pull en
        mt_set_gpio_pull_select(kpd_dts_data.kpd_gpio_index, 1); // pull up
        mt_set_gpio_out(kpd_dts_data.kpd_gpio_index, 0);         // input
    }
}

void keypad_init(void *fdt)
{
    get_kpd_dts_info(fdt);
    init_key_mapping();
    mtk_kpd_gpio_set();
}

bool detect_key(enum key_index index)
{
    int idx, bit, din;
    int key = key_mapping[index];

    if (key >= KPD_NUM_KEYS)
        return false;

    if (index == PMIC_PWR_KEY) {
        if (is_pmic_key_pressed(POWER_KEY) == 1) {
            LTRACEF("power key is pressed\n");
            return true;
        }

        return false;
    }

    if (index == PMIC_RST_KEY || index == BOOT_MENU_KEY) {
        if (is_pmic_key_pressed(HOME_KEY) == 1) {
            LTRACEF("home key is pressed\n");
            return true;
        }

        return false;
    }

    if (index == MENU_OK_KEY && kpd_dts_data.kpd_gpio_index != UINT_MAX) {
        if (!mt_get_gpio_in(kpd_dts_data.kpd_gpio_index)) {
            LTRACEF("MENU_OK_KEY is pressed\n");
            return true;
        }
        return false;
    }

    idx = key / 16;
    bit = key % 16;

    din = ((unsigned short)*REG16(KP_MEM1 + (idx << 2))) & (1U << bit);
    if (!din) {
        LTRACEF("key %d is pressed\n", key);
        return true;
    }

    return false;
}

bool is_pmic_just_rst(void)
{
    return pmic_just_rst;
}

static void detect_pmic_just_rst(uint level)
{
    if (is_pmic_long_press_reset()) {
        LTRACEF("Detected just recover from a reset\n");
        clr_pmic_long_press_reset();
        pmic_just_rst = true;
    } else
        pmic_just_rst = false;
}

LK_INIT_HOOK(detect_pmic_just_rst, detect_pmic_just_rst, LK_INIT_LEVEL_PLATFORM);
