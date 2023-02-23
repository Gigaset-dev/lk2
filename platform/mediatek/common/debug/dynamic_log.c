/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <dynamic_log.h>
#include <kernel/vm.h>
#include <lib/kcmdline.h>
#include <lib/pl_boottags.h>
#include <lk/init.h>
#include <platform/addressmap.h>
#include <string.h>
#include <stdio.h>
#include <uart_api.h>

#define BOOT_TAG_LOG_COM         0x88610004
struct boot_tag_log_com {
    u32 log_port;
    u32 log_baudrate;
    u32 log_enable;
    u32 log_dynamic_switch;
};

static struct boot_tag_log_com log_com_state = {(u32)UART0_BASE, DEBUG_BAUDRATE, 1, 1};

unsigned int log_dynamic_switch(void)
{
    return log_com_state.log_dynamic_switch;
}

unsigned int log_enable(void)
{
    return log_com_state.log_enable;
}

unsigned int log_port(void)
{
    return uart_base_to_id(log_com_state.log_port);
}

unsigned int log_baudrate(void)
{
    return log_com_state.log_baudrate;
}

static void dynamic_log_set_cmdline(uint level)
{
    char tmpbuf[64];

    if (log_dynamic_switch()) {
        memset(tmpbuf, 0x0, sizeof(tmpbuf));
        snprintf(tmpbuf, sizeof(tmpbuf) - 1, "earlycon=uart8250,mmio32,0x%x", log_com_state.log_port);
        kcmdline_append(tmpbuf);

        memset(tmpbuf, 0x0, sizeof(tmpbuf));
        snprintf(tmpbuf, sizeof(tmpbuf) - 1, "console=ttyS%d,%dn1", log_port(), log_baudrate());
        kcmdline_append(tmpbuf);

        kcmdline_append("mtk_printk_ctrl.disable_uart=0");
    } else
        kcmdline_append("mtk_printk_ctrl.disable_uart=1");
}

LK_INIT_HOOK(dynamic_log, dynamic_log_set_cmdline, LK_INIT_LEVEL_TARGET);

static void log_com_pl_boottag_hook(struct boot_tag *tag)
{
    memcpy(&log_com_state, &tag->data, sizeof(log_com_state));
}
PL_BOOTTAGS_INIT_HOOK(log_com, BOOT_TAG_LOG_COM, log_com_pl_boottag_hook);

