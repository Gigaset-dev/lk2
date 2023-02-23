/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <verified_boot.h>

/* dummy functions for secure boot flow */
int sec_cert_parser_and_vfy(const char *part_name,
                            const char *img_name,
                            uint8_t *p_hdr,
                            uint32_t hdr_sz,
                            off_t start_offset,
                            uint32_t oem_opt)
{
    return 0;
}
int sec_hdr_auth(uint8_t *p_hdr, uint32_t hdr_size)
{
    return 0;
}
int sec_img_auth(uint8_t *buf, uint32_t buf_sz)
{
    return 0;
}
