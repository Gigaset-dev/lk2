/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <lib/bio.h>
#include <sboot.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

/* ------------------------------------------- */
/* Add to disable EMMC power on protect for ATM*/
/* ------------------------------------------- */
#define OFFSET     (350)
#define KEY1       (12345678)
#define KEY2       (23131123)
#define VALID_FLAG (1)

static int scramble(int plainText, int key)
{
    return plainText ^ key;
}

static int descramble(int cipherText, int key)
{
    return cipherText ^ key;
}

int get_atm_enable_status(void)
{
    int atm_enable_flag = 0;

    ssize_t len;
    int flag = 0, tmp = 0;
    char part_name[] = "proinfo";
    unsigned char *b_buf = NULL, *s_buf = NULL;
    const int flag_len = 8;
    bdev_t *dev;

    /* Disable ATM Mode Once eFuse SBC_EN Is Programmed */
    if (sec_efuse_sbc_enabled_adapter()) {
        dprintf(ALWAYS, "[ATM] is disabled\n");
        return atm_enable_flag;
    }

    dev = bio_open_by_label(part_name);
    if (!dev) {
        LTRACEF("partition:%s open fails.\n", part_name);
        return 0;
    }

    s_buf = (unsigned char *)malloc((OFFSET + flag_len) * sizeof(unsigned char));
    if (s_buf)
        memset(s_buf, 0, OFFSET + flag_len);
    else
        goto exit;

    b_buf = (unsigned char *)malloc((flag_len + 1) * sizeof(unsigned char));
    if (b_buf)
        memset(b_buf, 0, flag_len + 1);
    else
        goto exit;

    len = bio_read(dev, s_buf, 0, (size_t)(OFFSET + flag_len));

    if (len < 0) {
        LTRACEF("bio_read error:%d\n", (int)len);
        goto exit;
    }

    for (; tmp < flag_len; tmp++)
        b_buf[tmp] = s_buf[OFFSET + tmp];

    b_buf[flag_len] = '\0';

    dprintf(INFO, "get atoi(b_buf) = %d\n", atoi((const char *)b_buf));
    flag = descramble(atoi((const char *)b_buf), KEY2) - KEY1;
    dprintf(INFO, "get flag = %d\n", flag);

    if (flag == VALID_FLAG) {
        atm_enable_flag = 1;
        dprintf(ALWAYS, "[ATM] enable flag = %d!\n", atm_enable_flag);
    }

exit:
    if (s_buf) {
        free(s_buf);
        s_buf = NULL;
    }
    if (b_buf) {
        free(b_buf);
        b_buf = NULL;
    }

    bio_close(dev);
    return atm_enable_flag;
}
