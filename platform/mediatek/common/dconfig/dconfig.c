/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <dconfig.h>
#include <dtb.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <sboot.h>
#include <stdlib.h>
#include <trace.h>
#include <verified_boot.h>

#include "dconfig_private.h"

#define LOCAL_TRACE 2

#define DCONFIG_VALID_PATTERN 0x533c9696

#define DCONFIG_IMG_NAME "dconfig"
#define DCONFIG_IMG_SIZE 1536
#define DCONFIG_DT_NAME "dconfig-dt"

#define CFG_ENV_SIZE     0x400 //(1KB)
#define CFG_ENV_OFFSET   0x200 // (start of partition)

#define CFG_ENV_DATA_SIZE (CFG_ENV_SIZE - sizeof(g_env.checksum) -      \
                           sizeof(g_env.sig) - sizeof(g_env.sig_1))
#define CFG_ENV_DATA_OFFSET (sizeof(g_env.sig))
#define CFG_ENV_SIG_1_OFFSET (CFG_ENV_SIZE - sizeof(g_env.checksum) - sizeof(g_env.sig_1))
#define CFG_ENV_CHECKSUM_OFFSET (CFG_ENV_SIZE - sizeof(g_env.checksum))
#define ENV_SIG "ENV_v1"

struct dconfig_env_t {
    char sig[8]; // "ENV_v1"
    char *env_data;
    char sig_1[8];  //"ENV_v1"
    int checksum; // checksum for env_data
};

struct boot_tag_soc_id {
    u8 id[32];
};

static char *g_dconfig_img;
static struct dconfig_env_t g_env;
static char *g_dconfig_dtb;
static int g_dconfig_dtb_size;

static unsigned int g_env_valid;
static unsigned int g_env_init_done;

static uint8_t g_device_socid[SOCID_LEN];
static uint8_t g_cert_socid[SOCID_LEN];

#define BOOT_TAG_SOCID        0x88610021

static int socid_is_null(const uint8_t src[SOCID_LEN])
{
    int i;

    for (i = 0; i < SOCID_LEN; i++) {
        if (src[i] != 0)
            return 0;
    }
    return 1;
}

static char *socid_to_hex(char dst[SOCID_LEN * 2 + 1],
                          const uint8_t src[SOCID_LEN])
{
    memset(dst, 0, SOCID_LEN * 2 + 1);

    const uint8_t *pin = &src[0];
    const char *hex = "0123456789ABCDEF";
    char *pout = dst;

    for (; pin < src + SOCID_LEN; pout += 2, pin++) {
        pout[0] = hex[(*pin >> 4) & 0xF];
        pout[1] = hex[*pin & 0xF];
    }
    return dst;
}

static char env_get_char(int index)
{
    return *(g_env.env_data + index);
}

static const char *env_get_addr(int index)
{
    return (g_env.env_data + index);

}
static int envmatch(const char *s1, int i2)
{
    if (s1 == NULL)
        return -1;
    while (*s1 == env_get_char(i2++))
        if (*s1++ == '=')
            return i2;
    if ((*s1 == '\0') && (env_get_char(i2 - 1) == '='))
        return i2;
    return -1;
}

static void dconfig_printenv(void)
{
    int i, nxt, exit = 0;
    const char *env = g_env.env_data;

    LTRACEF_LEVEL(2, "print env data\n");
    for (i = 0; env_get_char(i) != '\0'; i = nxt + 1) {
        for (nxt = i; env_get_char(nxt) != '\0'; ++nxt) {
            if (nxt >= (int)(CFG_ENV_DATA_SIZE)) {
                exit = 1;
                break;
            }
        }
        if (exit == 1)
            break;
        LTRACEF_LEVEL(2, "=> %s\n", env + i);
    }
}

void dconfig_initenv(void)
{
    int i, checksum, img_size;
    char *env_buffer;
    uint8_t dconfig_socid[SOCID_LEN] = {0}, dconfig_dt_socid[SOCID_LEN] = {0};

    ASSERT(g_env_init_done != DCONFIG_VALID_PATTERN);
    checksum = 0;
    g_env_init_done = DCONFIG_VALID_PATTERN;
    g_env_valid = 0;

    g_dconfig_img = malloc(DCONFIG_IMG_SIZE);
    if (g_dconfig_img == NULL) {
        LTRACEF("malloc failed\n");
        goto failed;
    }
    memset(g_dconfig_img, 0, DCONFIG_IMG_SIZE);
    img_size = load_dconfig_partition(DCONFIG_IMG_NAME, g_dconfig_img, DCONFIG_IMG_SIZE,
                                      dconfig_socid);
    if (img_size != DCONFIG_IMG_SIZE) {
        LTRACEF("load_dconfig partition fail, size = %d\n", img_size);
        goto failed;
    }
    env_buffer = g_dconfig_img + CFG_ENV_OFFSET;
    g_env.env_data = env_buffer + CFG_ENV_DATA_OFFSET;
    memcpy(g_env.sig, env_buffer, sizeof(g_env.sig));
    memcpy(g_env.sig_1, env_buffer + CFG_ENV_SIG_1_OFFSET, sizeof(g_env.sig_1));

    if (!strcmp(g_env.sig, ENV_SIG) && !strcmp(g_env.sig_1, ENV_SIG)) {
        g_env.checksum = *((int *)env_buffer + CFG_ENV_CHECKSUM_OFFSET / 4);
        for (i = 0; i < (int)(CFG_ENV_DATA_SIZE); i++)
            checksum += g_env.env_data[i];
        if (checksum != g_env.checksum) {
            LTRACEF("checksum mismatch s %d d %d!\n", g_env.checksum, checksum);
            goto failed;
        }
    } else {
        LTRACEF("ENV SIG is wrong\n");
        goto failed;
    }
    dconfig_printenv();

    /* Load device tree */
    g_dconfig_dtb = malloc(DCONFIG_DTB_SIZE);
    if (g_dconfig_dtb != NULL) {
        int dimg_size;

        dimg_size = load_dconfig_partition(DCONFIG_DT_NAME, g_dconfig_dtb, DCONFIG_DTB_SIZE,
                                           dconfig_dt_socid);
        if (dimg_size > 0) {
            g_dconfig_dtb_size = (uint32_t) dimg_size;
            LTRACEF("dconfig dtb image found %d\n", g_dconfig_dtb_size);
            g_dconfig_dtb = realloc(g_dconfig_dtb, g_dconfig_dtb_size);
            if (g_dconfig_dtb == NULL) {
                LTRACEF("dconfig dtb buffer resize failed\n");
                goto failed;
            }
        } else {
            LTRACEF("No dconfig dt image\n");
            free(g_dconfig_dtb);
            g_dconfig_dtb = NULL;
            g_dconfig_dtb_size = 0;
        }
    } else {
        LTRACEF("malloc failed\n");
        goto failed;
    }

    if (memcmp(dconfig_socid, dconfig_dt_socid, SOCID_LEN) != 0) {
        LTRACEF("cert socid mismatch\n");
        goto failed;
    }
    memcpy(g_cert_socid, dconfig_socid, SOCID_LEN);

    g_env_valid = DCONFIG_VALID_PATTERN;
    LTRACEF("dconfig initialize success\n");
    return;

failed:
    free(g_dconfig_img);
    g_dconfig_dtb_size = 0;
    free(g_dconfig_dtb);
    g_dconfig_img = NULL;
}

const char *dconfig_getenv(const char *name)
{
    int i, nxt;

    ASSERT(g_env_init_done == DCONFIG_VALID_PATTERN);
    if (g_env_valid != DCONFIG_VALID_PATTERN)
        return NULL;

    LTRACEF("get_env %s\n", name);
    for (i = 0; env_get_char(i) != '\0'; i = nxt + 1) {
        int val;

        for (nxt = i; env_get_char(nxt) != '\0'; ++nxt) {
            if (nxt >= CFG_ENV_SIZE)
                return NULL;
        }
        val = envmatch(name, i);
        if (val < 0)
            continue;
        return env_get_addr(val);
    }

    return NULL;
}

int dconfig_socid_match(void)
{
    char cert_s[SOCID_LEN * 2 + 1], device_s[SOCID_LEN * 2 + 1];
    bool img_auth_required;

    ASSERT(g_env_init_done == DCONFIG_VALID_PATTERN);
    if (g_env_valid != DCONFIG_VALID_PATTERN)
        return 0;

    img_auth_required = sec_get_vfy_policy_adapter(DCONFIG_PART);
    if (img_auth_required) {
        LTRACEF_LEVEL(2, "socid cert %s device %s\n",
                      socid_to_hex(cert_s, g_cert_socid),
                      socid_to_hex(device_s, g_device_socid));

        if (socid_is_null(g_cert_socid)) {
            TRACEF("Null cert socid\n");
            return 0;
        }
        if (memcmp(g_cert_socid, g_device_socid, SOCID_LEN) != 0) {
            TRACEF("dconfig cert/device socid mismatch\n");
            return 0;
        }
    }
    return 1;
}

void *dconfig_dtb_overlay(void *fdtbuf, int fdtbuf_size, int free_dt)
{
    void *merged_dtb = NULL;
    int32_t err;

    ASSERT(g_env_init_done == DCONFIG_VALID_PATTERN);
    if (g_env_valid != DCONFIG_VALID_PATTERN)
        return NULL;

    if (g_dconfig_dtb == NULL)
        return NULL;
    ASSERT(g_dconfig_dtb_size != 0);

    err = dtb_overlay(fdtbuf, fdtbuf_size, g_dconfig_dtb, g_dconfig_dtb_size, &merged_dtb);
    if (!err)
        dprintf(ALWAYS, "dconfig overlay ok, merged fdt size(%d)\n", fdt_totalsize(merged_dtb));

    if (free_dt) {
        free(g_dconfig_dtb);
        g_dconfig_dtb = NULL;
        g_dconfig_dtb_size = 0;
    }

    return merged_dtb;
}


static void pl_boottags_socid_hook(struct boot_tag *tag)
{
    struct boot_tag_soc_id *soc_id = (struct boot_tag_mem *)(&tag->data);

    memcpy(&g_device_socid, &soc_id->id, sizeof(g_device_socid));
}

PL_BOOTTAGS_INIT_HOOK(socid, BOOT_TAG_SOCID, pl_boottags_socid_hook);
