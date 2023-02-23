/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#pragma once

enum MDUMP_t {
    MDUMP_DUMMY,
    MDUMP_L2TCM,
    MDUMP_L1C,
    MDUMP_REGDUMP,
    MDUMP_TBUF,
    MDUMP_DRAM,
    MDUMP_TOTAL
};

int scp_crash_dump(void *crash_buffer);
void scp_memorydump_size_probe(void *fdt);
