/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include "dfd_decode.h"
#include <malloc.h>
#include <platform/dfd_mcu.h>
#include <platform/plat_dfd_mcu.h>
#include <reg.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

void save_dfd_status(void)
{
    dfd_status = readl(DFD_STATUS_RETURN);
    /* DO NOT print any message here, because this function will be called in EL3 */
}

static u64 dfd_decode(const u64 *raw, const struct reg_collector *collector,
                      const bool count_header_row)
{
    u64 reg = 0;
    unsigned int i = 0;
    unsigned int raw_offset = 0;
    unsigned int bit_offset = 0;
    unsigned int inv = 0;

    if (raw == NULL || collector == NULL)
        return 0;

    for (i = 0; i < DFD_REG_LENGTH; ++i) {
        if (count_header_row)
            raw_offset = collector->bit_pairs[i].raw_offset + drv.nr_header_row;
        else
            raw_offset = collector->bit_pairs[i].raw_offset;

        bit_offset = collector->bit_pairs[i].bit_offset;
        inv = collector->bit_pairs[i].inv & 0x3;

        if (inv == 0)
            reg |= ((raw[raw_offset] & (((u64)1)<<bit_offset))>>bit_offset) << i;
        else if (inv == 1)
            reg |= ((~raw[raw_offset] & (((u64)1)<<bit_offset))>>bit_offset) << i;
        else if (inv == 2)
            reg |= ((u64)0) << i;
        else if (inv == 3)
            reg |= ((u64)1) << i;
    }
    return reg;
}

static void plat_dfd_lastpc_decode(const u64 *dfd_raw_data)
{
    unsigned int i, cpu_id;
    unsigned int nr_max_core, nr_little_core, nr_big_core;
    unsigned int elX;

    nr_max_core = drv.nr_max_core;
    nr_big_core = drv.nr_big_core;
    nr_little_core = nr_max_core - nr_big_core;

    for (i = 0; i < nr_little_core; i++) {
        /* power status from DFD dump */
        lastpc[i].power_state =
            dfd_decode(dfd_raw_data, &(spmc_power_state[i].power_state), true);

        if (lastpc[i].power_state != DFD_CORE_PWR_ON &&
            lastpc[i].power_state != DFD_CORE_PWR_RETENTION)
                continue;

        lastpc[i].pc = dfd_decode(dfd_raw_data, &(l[i].pc), true);
        lastpc[i].sp_32 = dfd_decode(dfd_raw_data, &(l[i].sp32), true);
        lastpc[i].fp_32 = dfd_decode(dfd_raw_data, &(l[i].fp32), true);
        lastpc[i].fp_64 = dfd_decode(dfd_raw_data, &(l[i].fp64), true);
    }

    if (nr_big_core == 0)
        return;

    for (i = 0; i < nr_big_core; i++) {
        cpu_id = i + nr_little_core;

        /* power status from DFD dump */
        lastpc[cpu_id].power_state =
            dfd_decode(dfd_raw_data, &(spmc_power_state[cpu_id].power_state), true);

        if (lastpc[cpu_id].power_state != DFD_CORE_PWR_ON
            && lastpc[cpu_id].power_state != DFD_CORE_PWR_RETENTION)
                continue;

        lastpc[cpu_id].pc = dfd_decode(dfd_raw_data, &(big[i].i0_pc), true);

        /* extend the high bits for PC */
        if (lastpc[cpu_id].pc & (0x1ULL << 48))
            lastpc[cpu_id].pc = (u64) 0xffff000000000000 | lastpc[cpu_id].pc;
    }
}

#define GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(_N) \
            return_stack_little[i].entry[_N] =  \
                dfd_decode(dfd_raw_data, &(l[i].return_Stack##_N), true); \
            if (return_stack_little[i].entry[_N] & (0x1ULL << 48)) { \
                return_stack_little[i].entry[_N] |= (u64)0xffff000000000000; \
            }

static void plat_dfd_return_stack_decode(const u64 *dfd_raw_data)
{
    unsigned int i;
    unsigned int nr_little_core, nr_big_core;

    nr_little_core = drv.nr_max_core - drv.nr_big_core;
    nr_big_core = drv.nr_big_core;

    for (i = 0; i < nr_little_core; i++) {
        return_stack_little[i].ptr =
            dfd_decode(dfd_raw_data, &(l[i].return_Stack_pointer), true);

        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(0)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(1)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(2)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(3)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(4)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(5)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(6)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(7)
    }
}

void plat_dfd_mcu_init(void)
{
      drv.plt_dfd_op.plt_dfd_lastpc_decode = plat_dfd_lastpc_decode;
      drv.plt_dfd_op.plt_dfd_return_stack_decode = plat_dfd_return_stack_decode;
}

static int plat_dfd_mcu_dump(char *buf, int *wp)
{
    if (buf == NULL || wp == NULL)
        return -1;

    return 1;
}

int plat_dfd_mcu_get(void **data, int *len)
{
    int ret;
    *len = 0;
    *data = malloc(PLAT_DFD_MCU_BUF_LENGTH);
    if (*data == NULL)
        return 0;

    ret = plat_dfd_mcu_dump(*data, len);

    if (ret < 0 || *len > PLAT_DFD_MCU_BUF_LENGTH) {
        *len = (*len > PLAT_DFD_MCU_BUF_LENGTH) ? PLAT_DFD_MCU_BUF_LENGTH : *len;
        return ret;
    }

    return 1;
}

void plat_dfd_mcu_put(void **data)
{
    free(*data);
}

