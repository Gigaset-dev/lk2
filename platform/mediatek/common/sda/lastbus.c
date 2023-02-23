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
#include <malloc.h>
#include <platform/lastbus.h>
#include <reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

static struct cfg_lastbus my_cfg_lastbus;

const char *lastbus_compatible[] = {
    "/lastbus",
    "/lastbus/monitors",
};

static int get_lastbus_setting_from_fdt(void *fdt)
{
    int len, offset, value, node;
    int num = 0, i;
    int num_idle_mask;
    u32 *data;

    if (!fdt)
        return -ENODATA;

    offset = fdt_path_offset(fdt, lastbus_compatible[0]);
    if (offset < 0) {
        dprintf(CRITICAL, "%s: couldn't find the lastbus node\n", __func__);
        return -ENODATA;
    }

    /* get enabled */
    data = (u32 *)fdt_getprop(fdt, offset, "enabled", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find property enabled\n", __func__);
        my_cfg_lastbus.enabled = 0;
        return -ENODATA;
    } else {
        value = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: enabled = %d\n", __func__, value);
        my_cfg_lastbus.enabled = value;
    }

    /* get sw_version */
    data = (u32 *)fdt_getprop(fdt, offset, "sw_version", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find property sw_version\n", __func__);
        my_cfg_lastbus.sw_version = LASTBUS_SW_V1;
    } else {
        value = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: sw_version = %d\n", __func__, value);
        my_cfg_lastbus.sw_version = value;
    }

    /* get timeout_ms */
    data = (u32 *)fdt_getprop(fdt, offset, "timeout_ms", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find property timeout_ms\n", __func__);
        my_cfg_lastbus.timeout_ms = 0xFFFFFFFF;
    } else {
        value = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: timeout_ms = %d\n", __func__, value);
        my_cfg_lastbus.timeout_ms = value;
    }

    /* get timeout_type */
    data = (u32 *)fdt_getprop(fdt, offset, "timeout_type", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find property timeout_type\n", __func__);
        my_cfg_lastbus.timeout_type = LASTBUS_TIMEOUT_FIRST;
    } else {
        value = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: timeout_type = %d (0:first/1:last)\n", __func__, value);
        if ((value != LASTBUS_TIMEOUT_FIRST) && (value != LASTBUS_TIMEOUT_LAST)) {
            dprintf(CRITICAL, "%s: timeout_type(%d) is invalid!\n", __func__, value);
            return -EINVAL;
        }
        my_cfg_lastbus.timeout_type = value;
    }

    offset = fdt_subnode_offset(fdt, offset, "monitors");
    if (offset < 0) {
        dprintf(CRITICAL, "%s: couldn't find the monitors subnode\n", __func__);
        return -ENODATA;
    }

    /* get monitors */
    fdt_for_each_subnode(node, fdt, offset) {
        if (node >= 0) {
            /* get monitor name */
            data = (u32 *)fdt_getprop(fdt, node, "monitor_name", &len);
            if (!data || !len) {
                dprintf(CRITICAL, "%s: couldn't find property monitor name\n", __func__);
                return -ENODATA;
            }

            if (len <= MAX_MONITOR_NAME_LEN) {
                strncpy(my_cfg_lastbus.monitors[num].name, (char const *)data, len);
            } else {
                strncpy(my_cfg_lastbus.monitors[num].name, (char const *)data,
                        MAX_MONITOR_NAME_LEN);
                my_cfg_lastbus.monitors[num].name[MAX_MONITOR_NAME_LEN-1] = '\0';
            }

            dprintf(INFO, "%s: name = %s\n", __func__, my_cfg_lastbus.monitors[num].name);

            /* get monitor base */
            data = (u32 *)fdt_getprop(fdt, node, "base", &len);
            if (!data || !len) {
                dprintf(CRITICAL, "%s: couldn't find property monitor base\n", __func__);
                return -ENODATA;
            } else {
                value = fdt32_to_cpu(data[0]);
                if (!value) {
                    dprintf(CRITICAL, "%s: fail to get base\n", __func__);
                    return -ENODATA;
                } else {
                    dprintf(INFO, "%s: base = 0x%x\n", __func__, value);
                    my_cfg_lastbus.monitors[num].base = (value + KERNEL_ASPACE_BASE);
                }
            }

            /* get monitor num_ports */
            data = (u32 *)fdt_getprop(fdt, node, "num_ports", &len);
            if (!data || !len) {
                dprintf(CRITICAL, "%s: couldn't find property num_ports\n", __func__);
                return -ENODATA;
            } else {
                value = fdt32_to_cpu(data[0]);
                dprintf(INFO, "%s: num_ports = %d\n", __func__, value);
                my_cfg_lastbus.monitors[num].num_ports = value;
            }

            /* get monitor idle_mask */
            data = (u32 *)fdt_getprop(fdt, node, "idle_masks", &len);
            if (!data || !len) {
                my_cfg_lastbus.monitors[num].idle_mask_en = 0;
            } else {
                my_cfg_lastbus.monitors[num].idle_mask_en = 1;

                num_idle_mask = len / (sizeof(unsigned int) * 2);
                if (num_idle_mask > NR_MAX_LASTBUS_IDLE_MASK) {
                    dprintf(CRITICAL, "%s: Error: number of idle_masks(%d) is great than %d!\n",
                        __func__, num_idle_mask, NR_MAX_LASTBUS_IDLE_MASK);
                    return -EINVAL;
                } else {
                    dprintf(INFO, "%s: num_idle_mask = %d\n", __func__, num_idle_mask);
                    my_cfg_lastbus.monitors[num].num_idle_mask = num_idle_mask;
                }

                for (i = 0; i < num_idle_mask; i++) {
                    value = fdt32_to_cpu(*data++);
                    if (value == 0x0) {
                        dprintf(CRITICAL, "%s: invalid idle_mask offset (0x%x)\n", __func__, value);
                        return -EINVAL;
                    }
                    my_cfg_lastbus.monitors[num].idle_masks[i].reg_offset = value;
                    dprintf(INFO, "%s: idle_masks[%d].reg_offset = 0x%x\n", __func__, i, value);
                    value = fdt32_to_cpu(*data++);
                    my_cfg_lastbus.monitors[num].idle_masks[i].reg_value = value;
                    dprintf(INFO, "%s: idle_masks[%d].reg_value  = 0x%x\n", __func__, i, value);
                }
            }

            /* get monitor bus_freq_mhz */
            data = (u32 *)fdt_getprop(fdt, node, "bus_freq_mhz", &len);
            if (!data || !len) {
                dprintf(CRITICAL, "%s: couldn't find property bus_freq_mhz\n", __func__);
                return -ENODATA;
            } else {
                value = fdt32_to_cpu(data[0]);
                dprintf(INFO, "%s: bus_freq_mhz = %d\n", __func__, value);
                dprintf(INFO, "================================================\n");
                my_cfg_lastbus.monitors[num].bus_freq_mhz = value;
            }

            num++;
        }
    }

    if (num == 0) {
        dprintf(CRITICAL, "%s: there is no lastbus monitor node\n", __func__);
        return -ENODATA;
    } else {
        my_cfg_lastbus.num_used_monitors = num;
        if (num > NR_MAX_LASTBUS_MONITOR) {
            dprintf(CRITICAL, "%s: Error: number of monitors(%d) is great than %d!\n",
                    __func__, num, NR_MAX_LASTBUS_MONITOR);
            return -EINVAL;
        } else {
            dprintf(INFO, "%s: num_used_monitors = %d\n", __func__, num);
        }
    }
    return 0;
}

static unsigned int calculate_timeout_setting(unsigned int bus_freq_mhz, unsigned int timeout_ms)
{
    unsigned int value;

    value = ((timeout_ms * 1000 * bus_freq_mhz) >> 10) - 1;
    //dprintf(INFO, "%s: bus_freq_mhz = %d, timeout_ms = %d, timeout_threshold = 0x%x\n",
    //        __func__, bus_freq_mhz, timeout_ms, value);
    return value;
}

static void lastbus_init_monitor_v1(struct lastbus_monitor *m, unsigned int timeout_ms,
                                    unsigned int timeout_type)
{
    unsigned int bus_freq_mhz, timeout_setting;
    unsigned int i, reg_offset, reg_value;

    if (m->idle_mask_en == 1) {
        for (i = 0; i < m->num_idle_mask; i++) {
            reg_offset = m->idle_masks[i].reg_offset;
            reg_value = m->idle_masks[i].reg_value;
            writel(reg_value, m->base + reg_offset);
            dprintf(INFO, "%s: set idle_mask 0x%lx = 0x%x\n",
                    __func__, m->base + reg_offset, reg_value);
        }
    }

    /* clear timeout status with DBG_CKEN */
    writel((LASTBUS_TIMEOUT_CLR | LASTBUS_DEBUG_CKEN), m->base);

    /* de-assert clear bit with DBG_CKEN */
    writel(LASTBUS_DEBUG_CKEN, m->base);

    if (timeout_ms == 0xFFFFFFFF) {
        /* set maximum timeout for 1st edition*/
        writel(((0xFFFF << TIMEOUT_THRES_SHIFT) | LASTBUS_DEBUG_CKEN |
               (timeout_type << TIMEOUT_TYPE_SHIFT)), m->base);
        writel(((0xFFFF << TIMEOUT_THRES_SHIFT) | LASTBUS_DEBUG_CKEN | LASTBUS_DEBUG_EN |
               (timeout_type << TIMEOUT_TYPE_SHIFT)), m->base);
    } else {
        bus_freq_mhz = m->bus_freq_mhz;
        timeout_setting = calculate_timeout_setting(bus_freq_mhz, timeout_ms);
        if (timeout_setting > 0xFFFF)
            timeout_setting = 0xFFFF;
        writel(((timeout_setting << TIMEOUT_THRES_SHIFT) | LASTBUS_DEBUG_CKEN |
               (timeout_type << TIMEOUT_TYPE_SHIFT)), m->base);
        writel(((timeout_setting << TIMEOUT_THRES_SHIFT) | LASTBUS_DEBUG_CKEN |
               LASTBUS_DEBUG_EN | (timeout_type << TIMEOUT_TYPE_SHIFT)), m->base);
    }
    dprintf(INFO, "%s: base setting = 0x%x\n", __func__, readl(m->base));
}

static void lastbus_init_v1(void)
{
    struct lastbus_monitor *m;
    unsigned int num_used_monitors, timeout_ms, timeout_type;
    unsigned int i;

    num_used_monitors = my_cfg_lastbus.num_used_monitors;
    timeout_ms = my_cfg_lastbus.timeout_ms;
    timeout_type = my_cfg_lastbus.timeout_type;

    for (i = 0; i < num_used_monitors; i++) {
        m = &my_cfg_lastbus.monitors[i];
        lastbus_init_monitor_v1(m, timeout_ms, timeout_type);
    }
}

void lastbus_init(void *fdt)
{
    if (get_lastbus_setting_from_fdt(fdt) < 0)
        return;

    if (my_cfg_lastbus.enabled == 0)
        return;

    if (my_cfg_lastbus.sw_version == LASTBUS_SW_V1) {
        lastbus_init_v1();
        return;
    }
}

void lastbus_init_aee(void *fdt)
{
    /* get lastbus setting from fdt to initialize my_cfg_lastbus */
    get_lastbus_setting_from_fdt(fdt);
}

static bool lastbus_is_timeout_v1(const struct lastbus_monitor *m)
{
#if 0
    /* XXX
     * workaround for timeout loss.
     * debug_ctrl_ao*: infrabus_clk_detect [7:5] != 0
     */
    if (readl(m->base) & 0xe0)
        return 1;
#endif

    return (readl(m->base) & LASTBUS_TIMEOUT);
}

static unsigned long long gray_code_to_binary_convert(const unsigned long long gray_code)
{
    unsigned int gray_array[64], binary_array[64];
    int i;
    unsigned long long value = 0;
    unsigned long long tmp_value;

    for (i = 0; i < 64; i++)
        gray_array[i] = ((gray_code >> i) & 0x1);

    binary_array[63] = gray_array[63];
    for (i = 62; i >= 0; i--)
        binary_array[i] = gray_array[i] ^ binary_array[i+1];

    for (i = 0; i < 64; i++) {
       tmp_value = binary_array[i];
       value |= (tmp_value << i);
    }

    return value;
}

static void lastbus_dump_monitor_v1(const struct lastbus_monitor *m, char *buf, int *wp)
{
    unsigned int i;
    unsigned long long grad_code, bin_code;

    *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp, "--- ");
    *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp, "%s %p %d",
                    m->name, (void *)(m->base - KERNEL_ASPACE_BASE), m->num_ports);
    *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp, " ---\n");

    for (i = 0; i < m->num_ports; i++) {
        *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp, "%08x\n",
                        readl(m->base + 0x408 + (i * 4)));
    }

    grad_code = readl(m->base + 0x404);
    grad_code = (grad_code << 32) | readl(m->base + 0x400);
    bin_code = gray_code_to_binary_convert(grad_code);
    dprintf(INFO, "%s: gray_code = 0x%llx, binary = 0x%llx\n", __func__, grad_code, bin_code);

    *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp, "timestamp: 0x%llx\n", bin_code);
}

static int lastbus_dump_v1(char *buf, int *wp)
{
    struct lastbus_monitor *m;
    unsigned int num_used_monitors;
    unsigned int i;
    bool is_timeout, timeout_occurred = false;

    *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp,
            "\n*********************** %s lastbus ***********************\n", SUB_PLATFORM);

    num_used_monitors = my_cfg_lastbus.num_used_monitors;

    for (i = 0; i < num_used_monitors; i++) {
        is_timeout = false;

        m = &my_cfg_lastbus.monitors[i];

        is_timeout = lastbus_is_timeout_v1(m);

        if (is_timeout) {
            dprintf(INFO, "%s: lastbus timeout happened (%s)\n", __func__, m->name);
            lastbus_dump_monitor_v1(m, buf, wp);
        }

        timeout_occurred |= is_timeout;
    }

    *wp += snprintf(buf + *wp, LASTBUS_BUF_LENGTH - *wp,
                    "\n#lastbus timeout = %s\n",
                    timeout_occurred ? "True" : "False");

    return 1;
}

int lastbus_get(void **data, int *len)
{
    int ret;
    *len = 0;
    *data = malloc(LASTBUS_BUF_LENGTH);
    if (*data == NULL)
        return 0;

    ret = lastbus_dump_v1(*data, len);

    if (ret < 0 || *len > LASTBUS_BUF_LENGTH) {
        *len = (*len > LASTBUS_BUF_LENGTH) ? LASTBUS_BUF_LENGTH : *len;
        return ret;
    }

    return 1;
}

void lastbus_put(void **data)
{
    free(*data);
}
