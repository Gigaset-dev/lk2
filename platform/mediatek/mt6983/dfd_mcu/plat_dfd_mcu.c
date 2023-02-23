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
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

struct decoded_inner_status {
    unsigned int axi_entry;
    unsigned int axi_valid;
    unsigned int rid_state[5];
    unsigned int wvalid[5];
    unsigned int bid[5];
    unsigned int waddr[5];
};

static struct decoded_inner_status inner_status[4];

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
        if (count_header_row) {
            raw_offset = collector->bit_pairs[i].raw_offset + drv.nr_header_row;
            if (drv.sw_version == DFD_SW_V1)
                if (raw_offset >= 8)
                    raw_offset += 2; /* skip two row */
        } else {
            raw_offset = collector->bit_pairs[i].raw_offset;
        }
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

        lastpc[i].pc = dfd_decode(dfd_raw_data, &(little_core[i].pc), true);
        lastpc[i].mode = dfd_decode(dfd_raw_data, &(little_core[i].mode), true);
        elX = lastpc[i].mode >> 2;
        lastpc[i].sp_32 = dfd_decode(dfd_raw_data, &(little_core[i].sp32), true);

        switch (elX) {
        case 0:
            lastpc[i].sp_64 = dfd_decode(dfd_raw_data, &(little_core[i].sp_EL0), true);
            break;
        case 1:
            lastpc[i].sp_64 = dfd_decode(dfd_raw_data, &(little_core[i].sp_EL1), true);
            break;
        case 2:
            lastpc[i].sp_64 = dfd_decode(dfd_raw_data, &(little_core[i].sp_EL2), true);
            break;
        case 3:
            lastpc[i].sp_64 = dfd_decode(dfd_raw_data, &(little_core[i].sp_EL3), true);
            break;
        default:
            dprintf(CRITICAL, "%s, unsupport exception level (%d)\n", __func__, elX);
        }

        lastpc[i].fp_32 = dfd_decode(dfd_raw_data, &(little_core[i].fp32), true);
        lastpc[i].fp_64 = dfd_decode(dfd_raw_data, &(little_core[i].fp64), true);
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

        lastpc[cpu_id].pc = dfd_decode(dfd_raw_data, &(big_core[i].i0_pc), true);

        /* extend the high bits for PC */
        if (lastpc[cpu_id].pc & (0x1ULL << 48))
            lastpc[cpu_id].pc = (u64) 0xffff000000000000 | lastpc[cpu_id].pc;
    }
}

#define GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(_N) \
            return_stack_little[i].entry[_N] =  \
                dfd_decode(dfd_raw_data, &(little_core[i].return_Stack##_N), true); \
            if (return_stack_little[i].entry[_N] & (0x1ULL << 48)) { \
                return_stack_little[i].entry[_N] |= (u64)0xffff000000000000; \
            }

#define GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(_N) \
            return_stack_big[i].entry[_N] =  \
                dfd_decode(dfd_raw_data, &(big_core[i].return_Stack##_N), true); \
            if (return_stack_big[i].entry[_N] & (0x1ULL << 48)) { \
                return_stack_big[i].entry[_N] |= (u64)0xffff000000000000; \
            }

static void plat_dfd_return_stack_decode(const u64 *dfd_raw_data)
{
    unsigned int i;
    unsigned int nr_little_core, nr_big_core;
    unsigned int nr_little_entry, nr_big_entry;

    nr_little_core = drv.nr_max_core - drv.nr_big_core;
    nr_big_core = drv.nr_big_core;
    nr_little_entry = drv.nr_rs_entry_little;
    nr_big_entry = drv.nr_rs_entry_big;

    for (i = 0; i < nr_little_core; i++) {
        return_stack_little[i].ptr =
            dfd_decode(dfd_raw_data, &(little_core[i].return_Stack_pointer), true);

        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(0)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(1)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(2)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(3)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(4)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(5)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(6)
        GET_RETURN_STACK_ENTRY_FOR_LITTLE_CORE(7)
    }

    for (i = 0; i < nr_big_core; i++) {
        return_stack_big[i].ptr =
            dfd_decode(dfd_raw_data, &(big_core[i].return_Stack_pointer), true);

        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(0)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(1)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(2)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(3)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(4)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(5)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(6)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(7)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(8)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(9)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(10)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(11)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(12)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(13)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(14)
        GET_RETURN_STACK_ENTRY_FOR_BIG_CORE(15)
    }
}

static void plat_dfd_misc_decode(const u64 *dfd_raw_data)
{
    unsigned int i;

    for (i = 0; i < 4; i++) {
        inner_status[i].axi_entry =
            dfd_decode(dfd_raw_data, &(little_core[i].axi_entry), true);
        inner_status[i].axi_valid =
            dfd_decode(dfd_raw_data, &(little_core[i].axi_valid), true);

        LTRACEF("%s: [CPU%d] axi_entry = 0x%x, axi_valid = 0x%x\n",
            __func__, i, inner_status[i].axi_entry, inner_status[i].axi_valid);

        inner_status[i].rid_state[0] =
            dfd_decode(dfd_raw_data, &(little_core[i].rid_0_state), true);
        inner_status[i].rid_state[1] =
            dfd_decode(dfd_raw_data, &(little_core[i].rid_1_state), true);
        inner_status[i].rid_state[2] =
            dfd_decode(dfd_raw_data, &(little_core[i].rid_2_state), true);
        inner_status[i].rid_state[3] =
            dfd_decode(dfd_raw_data, &(little_core[i].rid_3_state), true);
        inner_status[i].rid_state[4] =
            dfd_decode(dfd_raw_data, &(little_core[i].rid_4_state), true);

        LTRACEF("%s: [CPU%d] rid_state[0] = 0x%x, rid_state[1] = 0x%x, ",
           __func__, i, inner_status[i].rid_state[0], inner_status[i].rid_state[1]);
        LTRACEF("rid_state[2] = 0x%x, rid_state[3] = 0x%x, rid_state[4] = 0x%x\n",
           inner_status[i].rid_state[2], inner_status[i].rid_state[3],
           inner_status[i].rid_state[4]);

        inner_status[i].wvalid[0] =
            dfd_decode(dfd_raw_data, &(little_core[i].wvalid_0), true);
        inner_status[i].wvalid[1] =
            dfd_decode(dfd_raw_data, &(little_core[i].wvalid_1), true);
        inner_status[i].wvalid[2] =
            dfd_decode(dfd_raw_data, &(little_core[i].wvalid_2), true);
        inner_status[i].wvalid[3] =
            dfd_decode(dfd_raw_data, &(little_core[i].wvalid_3), true);
        inner_status[i].wvalid[4] =
            dfd_decode(dfd_raw_data, &(little_core[i].wvalid_4), true);

        LTRACEF("%s: [CPU%d] wvalid[0] = 0x%x, wvalid[1] = 0x%x, ",
            __func__, i, inner_status[i].wvalid[0], inner_status[i].wvalid[1]);
        LTRACEF("wvalid[2] = 0x%x, wvalid[3] = 0x%x, wvalid[4] = 0x%x\n",
            inner_status[i].wvalid[2], inner_status[i].wvalid[3], inner_status[i].wvalid[4]);

        inner_status[i].bid[0] =
            dfd_decode(dfd_raw_data, &(little_core[i].bid_0), true);
        inner_status[i].bid[1] =
            dfd_decode(dfd_raw_data, &(little_core[i].bid_1), true);
        inner_status[i].bid[2] =
            dfd_decode(dfd_raw_data, &(little_core[i].bid_2), true);
        inner_status[i].bid[3] =
            dfd_decode(dfd_raw_data, &(little_core[i].bid_3), true);
        inner_status[i].bid[4] =
            dfd_decode(dfd_raw_data, &(little_core[i].bid_4), true);

        LTRACEF("%s: [CPU%d] bid[0] = 0x%x, bid[1] = 0x%x, ",
            __func__, i, inner_status[i].bid[0], inner_status[i].bid[1]);
        LTRACEF("bid[2] = 0x%x, bid[3] = 0x%x, bid[4] = 0x%x\n",
            inner_status[i].bid[2], inner_status[i].bid[3], inner_status[i].bid[4]);

        inner_status[i].waddr[0] =
            dfd_decode(dfd_raw_data, &(little_core[i].waddr_0), true);
        inner_status[i].waddr[1] =
            dfd_decode(dfd_raw_data, &(little_core[i].waddr_1), true);
        inner_status[i].waddr[2] =
            dfd_decode(dfd_raw_data, &(little_core[i].waddr_2), true);
        inner_status[i].waddr[3] =
            dfd_decode(dfd_raw_data, &(little_core[i].waddr_3), true);
        inner_status[i].waddr[4] =
            dfd_decode(dfd_raw_data, &(little_core[i].waddr_4), true);

        LTRACEF("%s: [CPU%d] waddr[0] = 0x%x, waddr[1] = 0x%x, ",
            __func__, i, inner_status[i].waddr[0], inner_status[i].waddr[1]);
        LTRACEF("waddr[2] = 0x%x, waddr[3] = 0x%x, waddr[4] = 0x%x\n",
            inner_status[i].waddr[2], inner_status[i].waddr[3], inner_status[i].waddr[4]);
    }
}

static unsigned int check_errata_2217821(void)
{
    struct decoded_inner_status *data;
    unsigned int result;
    unsigned int error_flag;
    unsigned int state;
    int i, j;

    result = 0;
    for (i = 0; i < 4; i++) {

        dprintf(CRITICAL, "%s: [CPU%d] check start!\n", __func__, i);

        data = &(inner_status[i]);
        /* Incoming flitq full */
        if ((data->axi_entry == 0x7) && (data->axi_valid != 0))
            error_flag = 1;
        else
            error_flag = 0;

        if (error_flag == 0)
            continue;
        else
            dprintf(CRITICAL, "%s: [CPU%d] 1st rule matched!\n", __func__, i);

        /* All snp reqbufs full */
        for (j = 0; j < 5; j++)
            if (data->rid_state[j] == 0) {
                error_flag = 0;
                break;
            }

        if (error_flag == 0)
            continue;
        else
            dprintf(CRITICAL, "%s: [CPU%d] 2nd rule matched!\n", __func__, i);

        /* At leaast one snp reqbuf in state 0b001 */
        state = 0;
        for (j = 0; j < 5; j++)
            if (data->rid_state[j] == 0x1) {
                state = 1;
                break;
            }

        error_flag &= state;

        if (error_flag == 0)
            continue;
        else
            dprintf(CRITICAL, "%s: [CPU%d] 3rd rule matched!\n", __func__, i);

        /* At least one snp reqbuf is an unserialised SnpDVMSync */
        state = 0;
        for (j = 0; j < 5; j++)
            if ((data->wvalid[j] == 0) && (data->bid[j] == 0xd) && (data->waddr[j] == 0x4)) {
                state = 1;
                break;
            }

        error_flag &= state;

        if (error_flag == 1)
            dprintf(CRITICAL, "%s: [CPU%d] 4th rule matched!\n", __func__, i);

        result |= error_flag;

        dprintf(CRITICAL, "%s: [CPU%d] check end! (reslut = %d)\n", __func__, i, result);
    }

    return result;
}

static unsigned int plat_dfd_misc_dump(char *buf, unsigned int max_buf_size)
{
    unsigned int len = 0;

    if (check_errata_2217821() == 1)
        len += snprintf(buf, max_buf_size, "\nFound the problem of ARM CPU erratum #2217821\n");

    return len;
}

void plat_dfd_mcu_init(void)
{
    drv.plt_dfd_op.plt_dfd_misc_decode = plat_dfd_misc_decode;
    drv.plt_dfd_op.plt_dfd_misc_dump = plat_dfd_misc_dump;
    drv.plt_dfd_op.plt_dfd_lastpc_decode = plat_dfd_lastpc_decode;
    //drv.plt_dfd_op.plt_dfd_return_stack_decode = plat_dfd_return_stack_decode;

    /* Change version number for DFD parsing tool
     * It is not required by default.
     * We only do it when DFD parser needs different version number for parsing.
     */
    memcpy(drv.chip_id + 4, "_v1", 3);
}

static int plat_dfd_mcu_dump(char *buf, int *wp)
{
    if (buf == NULL || wp == NULL)
        return -1;

    *wp += plat_dfd_misc_dump(buf + *wp, PLAT_DFD_MCU_BUF_LENGTH - *wp);

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


