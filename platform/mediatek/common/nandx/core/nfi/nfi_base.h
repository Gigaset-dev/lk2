/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <nfi.h>
#include "nfiecc.h"

#define NFI_TIMEOUT             1000000

enum randomizer_op {
    RAND_ENCODE,
    RAND_DECODE
};

struct bad_mark_ctrl {
    void (*bad_mark_swap)(struct nfi *nfi, u8 *buf, u8 *fdm);
    u8 *(*fdm_shift)(struct nfi *nfi, u8 *fdm, u32 sector);
    u32 sector;
    u32 position;
};

struct nfi_caps {
    u8 max_fdm_size;
    u8 fdm_ecc_size;
    u8 ecc_parity_bits;
    const int *spare_size;
    u32 spare_size_num;
};

struct nfi_base {
    struct nfi nfi;
    struct nfi_resource res;
    struct nfiecc *ecc;
    struct nfi_format format;
    struct nfi_caps *caps;
    struct bad_mark_ctrl bad_mark_ctrl;

    /* page_size + spare_size */
    u8 *buf;

    /* used for spi nand */
    u8 cmd_mode;
    u32 op_mode;

    u32 page_sectors;

    event_t done;

    /* for read/write */
    int col;
    int row;
    int access_len;
    int rw_sectors;
    void *dma_addr;
    int read_status;

    bool dma_en;
    bool nfi_irq_en;
    bool auto_format;
    bool ecc_en;
    bool ecc_clk_en;
    bool randomize_en;
    bool custom_sector_en;
    bool bad_mark_swap_en;

    enum nfiecc_deccon ecc_deccon;
    enum nfiecc_mode ecc_mode;

    void (*set_op_mode)(void *regs, u32 mode);
    bool (*is_page_empty)(struct nfi_base *nb, u8 *data, u8 *fdm,
                  int sectors);

    int (*rw_prepare)(struct nfi_base *nb, int sectors, u8 *data, u8 *fdm,
              bool read);
    void (*rw_trigger)(struct nfi_base *nb, bool read);
    int (*rw_wait_done)(struct nfi_base *nb, int sectors, bool read);
    int (*rw_data)(struct nfi_base *nb, u8 *data, u8 *fdm, u32 sectors,
               bool read);
    void (*rw_complete)(struct nfi_base *nb, u8 *data, u8 *fdm, bool read);
};

static inline struct nfi_base *nfi_to_base(struct nfi *nfi)
{
    return container_of(nfi, struct nfi_base, nfi);
}

struct nfi *nfi_extend_init(struct nfi_base *nb);
void nfi_extend_exit(struct nfi_base *nb);
struct nfi *nfi_extend_spi_init(struct nfi_base *nb);
void nfi_extend_spi_exit(struct nfi_base *nb);

