/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <pmif.h>
#include <sys/types.h>

enum {
    PMIF_CMD_REG_0,
    PMIF_CMD_REG,
    PMIF_CMD_EXT_REG,
    PMIF_CMD_EXT_REG_LONG,
};

struct pmif {
    int swinf_ch_start;
    u32 swinf_no;
    u32 mstid;
    u32 pmifid;
    int (*read_cmd)(u8 opc, u8 sid, u16 addr, u8 *buf, u8 len);
    int (*write_cmd)(u8 opc, u8 sid, u16 addr, const u8 *buf, u8 len);
    u32 (*read_field)(u32 reg, u32 mask, u32 shift);
    void (*write_field)(u32 reg, u32 val, u32 mask, u32 shift);
};

int is_pmif_init_done(void);
void pmif_spmi_enable_cmdIssue(void);
void pmif_spmi_enable_swinf(struct pmif *arb);
int pmif_spmi_read_cmd(u8 opc, u8 sid, u16 addr, u8 *buf, u8 len);
int pmif_spmi_write_cmd(u8 opc, u8 sid, u16 addr, const u8 *buf, u8 len);
void pmif_mpu_init(void *fdt);
void pmif_enable_mpu(void);
