/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <compiler.h>
#include <debug.h>
#include <errno.h>
#include <lib/bio.h>
#include <platform.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sysenv.h>
#include <trace.h>

struct env_t {
    char sig[8]; // "ENV_v1"
    char *env_data;
    char sig_1[8];  //"ENV_v1"
    int checksum; // checksum for env_data
};

enum {
    SYSENV_RW_AREA = 0,
    SYSENV_RO_AREA,
    SYSENV_AREA_MAX,
};

#define CFG_ENV_SIZE 0x4000 //(16KB)
#define CFG_ENV_RW_OFFSET   0x20000
#define CFG_ENV_RO_OFFSET   0x24000

#define CFG_ENV_DATA_SIZE (CFG_ENV_SIZE - sizeof(g_env[area].checksum) \
        - sizeof(g_env[area].sig) - sizeof(g_env[area].sig_1))
#define CFG_ENV_DATA_OFFSET (sizeof(g_env[area].sig))
#define CFG_ENV_SIG_1_OFFSET (CFG_ENV_SIZE - sizeof(g_env[area].checksum) \
        - sizeof(g_env[area].sig_1))
#define CFG_ENV_CHECKSUM_OFFSET (CFG_ENV_SIZE - sizeof(g_env[area].checksum))

#define ENV_PART "para"
#define ENV_SIG "ENV_v1"

#define LOCAL_TRACE 0

static struct env_t g_env[SYSENV_AREA_MAX];
static int env_valid[SYSENV_AREA_MAX];
static char env_buffer[SYSENV_AREA_MAX][CFG_ENV_SIZE];

static char env_get_char(int index, unsigned int area)
{
    return *(g_env[area].env_data + index);
}

static char *env_get_addr(int index, unsigned int area)
{
    return (g_env[area].env_data + index);

}
static int envmatch(const char *s1, int i2, unsigned int area)
{
    if (s1 == NULL)
        return -EINVAL;
    while (*s1 == env_get_char(i2++, area))
        if (*s1++ == '=')
            return i2;
    if (*s1 == '\0' && env_get_char(i2-1, area) == '=')
        return i2;
    return -EINVAL;
}

static int write_env_area(char *env_buf, unsigned int area)
{
    size_t count;
    ssize_t len;
    int i, checksum = 0;
    off_t offset;
    size_t size = CFG_ENV_SIZE;
    bdev_t *bdev = NULL;

    if (area >= SYSENV_AREA_MAX) {
        LTRACEF("Invalid area %d\n", area);
        return -EINVAL;
    }

    bdev = bio_open_by_label(ENV_PART);
    if (bdev == NULL) {
        LTRACEF("%s partition does not exist\n", ENV_PART);
        return -ENODEV;
    }

    count = sizeof(ENV_SIG);
    memcpy(env_buf, ENV_SIG, count);
    memcpy(env_buf + CFG_ENV_SIG_1_OFFSET, ENV_SIG, count);

    for (i = 0; i < (int)CFG_ENV_DATA_SIZE; i++)
        checksum += *(env_buf + CFG_ENV_DATA_OFFSET + i);
    *((int *)env_buf + CFG_ENV_CHECKSUM_OFFSET/4) = checksum;

    offset = (area == SYSENV_RW_AREA) ? CFG_ENV_RW_OFFSET : CFG_ENV_RO_OFFSET;
    len = bio_write(bdev, env_buf, offset, size);
    bio_close(bdev);
    if (len < 0 || (size_t)len != size)
        return -EIO;
    return 0;
}

static int read_env_area(char *env_buf, unsigned int area)
{
    ssize_t len;
    off_t offset;
    size_t size = CFG_ENV_SIZE;
    bdev_t *bdev = NULL;

    if (area >= SYSENV_AREA_MAX) {
        LTRACEF("Invalid area %d\n", area);
        return -EINVAL;
    }

    bdev = bio_open_by_label(ENV_PART);
    if (bdev == NULL) {
        LTRACEF("%s does not exist\n", ENV_PART);
        return -ENODEV;
    }

    offset = (area == SYSENV_RW_AREA) ? CFG_ENV_RW_OFFSET : CFG_ENV_RO_OFFSET;
    len = bio_read(bdev, env_buf, offset, size);
    bio_close(bdev);
    if (len < 0 || (size_t)len != size)
        return -EIO;
    return 0;
}

void env_init(void)
{
    int ret, i, area, checksum;

    for (area = 0; area < SYSENV_AREA_MAX; area++) {
        checksum = 0;
        env_valid[area] = 0;
        memset(env_buffer[area], 0x00, CFG_ENV_SIZE);
        g_env[area].env_data = env_buffer[area] + CFG_ENV_DATA_OFFSET;
        ret = read_env_area(env_buffer[area], area);
        if (ret < 0) {
            LTRACEF("read_env_area %d fail, ret = %d\n", area, ret);
            continue;
        }

        memcpy(g_env[area].sig, env_buffer[area], sizeof(g_env[area].sig));
        memcpy(g_env[area].sig_1, env_buffer[area] + CFG_ENV_SIG_1_OFFSET,
                sizeof(g_env[area].sig_1));
        g_env[area].sig[7] = '\0';
        g_env[area].sig_1[7] = '\0';
        if (!strcmp(g_env[area].sig, ENV_SIG) && !strcmp(g_env[area].sig_1, ENV_SIG)) {
            g_env[area].checksum = *((int *)env_buffer[area] + CFG_ENV_CHECKSUM_OFFSET/4);
            for (i = 0; i < (int)(CFG_ENV_DATA_SIZE); i++)
                checksum += g_env[area].env_data[i];
            if (checksum != g_env[area].checksum) {
                dprintf(CRITICAL, "checksum of area %d mismatch s %d d %d!\n",
                    area, g_env[area].checksum, checksum);
            } else {
                dprintf(CRITICAL, "ENV of area %d initialize success\n", area);
                env_valid[area] = 1;
            }
        } else {
            dprintf(CRITICAL, "ENV SIG of area %d not found, it is empty now\n", area);
        }
        if (!env_valid[area])
            memset(env_buffer[area], 0x00, CFG_ENV_SIZE);
    }
}

static char *get_env_with_area(const char *name, unsigned int area)
{
    int i, nxt, val;

    if (area >= SYSENV_AREA_MAX) {
        LTRACEF("Invalid area %d\n", area);
        return NULL;
    }

    if (name == NULL) {
        LTRACEF("Invalid parameter in %s\n", __func__);
        return NULL;
    }

    if (!env_valid[area])
        return NULL;

    for (i = 0; env_get_char(i, area) != '\0'; i = nxt + 1) {
        for (nxt = i; env_get_char(nxt, area) != '\0'; ++nxt) {
            if (nxt >= CFG_ENV_SIZE)
                return NULL;
        }
        val = envmatch(name, i, area);
        if (val < 0)
            continue;
        return env_get_addr(val, area);
    }

    return NULL;
}

char *get_env(const char *name)
{
    return get_env_with_area(name, SYSENV_RW_AREA);
}

char *get_ro_env(const char *name)
{
    return get_env_with_area(name, SYSENV_RO_AREA);
}

static int set_env_with_area(const char *name, const char *value, int area)
{
    int len, oldval, ret;
    char *env, *nxt = NULL;

    if (area != SYSENV_RW_AREA) {
        LTRACEF("Can not write read-only sysenv area\n");
        return -1;
    }
    if (name == NULL || value == NULL) {
        LTRACEF("Invalid parameter in %s\n", __func__);
        return -1;
    }

    char *env_data = g_env[area].env_data;

    LTRACEF_LEVEL(2, "set_env %s %s\n", name, value);

    oldval = -1;

    if (!env_valid[area]) {
        env = env_data;
        goto add;
    }

    for (env = env_data; *env; env = nxt + 1) {
        for (nxt = env; *nxt; ++nxt)
            ;
        oldval = envmatch(name, env-env_data, area);
        if (oldval >= 0)
            break;
    }

    if (oldval > 0) {
        if (*++nxt == '\0') {
            if (env > env_data)
                env--;
            else
                *env = '\0';
        } else {
            for (;;) {
                *env = *nxt++;
                if ((*env == '\0') && (*nxt == '\0'))
                    break;
                ++env;
            }
        }
        *++env = '\0';
    }

    for (env = env_data; *env || *(env+1); ++env)
        ;
    if (env > env_data)
        ++env;

add:
    /*
     * Overflow when:
     * "name" + "=" + "val" +"\0\0"  > ENV_SIZE - (env-env_data)
     */
    len = strlen(name) + 2;
    /* add '=' for first arg, ' ' for all others */
    len += strlen(value) + 1;

    if (len > (&env_data[CFG_ENV_DATA_SIZE]-env)) {
        LTRACEF("Error: sysenv overflow, \"%s\" deleted\n", name);
        return -1;
    }
    while ((*env = *name++) != '\0')
        env++;

    *env = '=';

    while ((*++env = *value++) != '\0')
        ;


    /* end is marked with double '\0' */
    *++env = '\0';
    memset(env, 0x00, CFG_ENV_DATA_SIZE-(env-env_data));
    ret = write_env_area(env_buffer[area], area);
    if (ret < 0) {
        LTRACEF("write env fail\n");
        memset(env_buffer[area], 0x00, CFG_ENV_SIZE);
        return -EIO;
    }
    env_valid[area] = 1;
    return 0;

}

int set_env(const char *name, const char *value)
{
    return set_env_with_area(name, value, SYSENV_RW_AREA);
}

void print_env(void)
{
    int i, nxt, area, exit;

    for (area = 0; area < SYSENV_AREA_MAX; area++) {
        LTRACEF("env area: %d\n", area);
        char *env = g_env[area].env_data;

        if (!env_valid[area]) {
            LTRACEF("no valid env\n");
            continue;
        }
        LTRACEF("env:\n");
        exit = 0;
        for (i = 0; env_get_char(i, area) != '\0'; i = nxt + 1) {
            for (nxt = i; env_get_char(nxt, area) != '\0'; ++nxt) {
                if (nxt >= (int)(CFG_ENV_DATA_SIZE)) {
                    exit = 1;
                    break;
                }
            }
            if (exit == 1)
                break;
            LTRACEF("%s\n", env + i);
        }
    }
}

