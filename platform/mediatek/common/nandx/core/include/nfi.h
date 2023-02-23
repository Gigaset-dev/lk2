/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

struct nfi_format {
    int page_size;
    int spare_size;
    int ecc_req;
};

struct nfi {
    int sector_size;
    int sector_spare_size;
    int fdm_size; /*for sector*/
    int fdm_ecc_size;
    int ecc_strength;
    int ecc_parity_size; /*for sector*/

    int (*select_chip)(struct nfi *nfi, int cs);
    int (*set_format)(struct nfi *nfi, struct nfi_format *format);
    int (*set_timing)(struct nfi *nfi, void *timing, int type);
    int (*nfi_ctrl)(struct nfi *nfi, int cmd, void *args);

    int (*reset)(struct nfi *nfi);
    int (*send_cmd)(struct nfi *nfi, short cmd);
    int (*send_addr)(struct nfi *nfi, int col, int row,
             int col_cycle, int row_cycle);
    int (*trigger)(struct nfi *nfi);

    int (*write_page)(struct nfi *nfi, u8 *data, u8 *fdm);
    int (*write_bytes)(struct nfi *nfi, u8 *data, int count);
    int (*read_sectors)(struct nfi *nfi, u8 *data, u8 *fdm,
                u32 sectors);
    int (*read_bytes)(struct nfi *nfi, u8 *data, int count);

    int (*wait_ready)(struct nfi *nfi, int type, u32 timeout);

    int (*enable_randomizer)(struct nfi *nfi, u32 row, bool encode);
    int (*disable_randomizer)(struct nfi *nfi);
};

struct nfi *nfi_init(struct nfi_resource *res);
void nfi_exit(struct nfi *nfi);

