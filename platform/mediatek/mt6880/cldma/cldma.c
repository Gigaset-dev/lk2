/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/arch_ops.h>
#include <arch/ops.h>
#include <cldma.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <platform.h>
#include <platform/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "cldma_reg_ops.h"

#define LOCAL_TRACE 0

#define CLDMA_QUEUE_LK 1
#define TGPD_N 1
#define RGPD_N 1
#define LOCAL_BUFF_SIZE 2048

struct GPD {
    unsigned int flag:8;
    unsigned int reserved:8;
    unsigned int allow_len:16;

    struct GPD *p_next;

    unsigned char *p_buf;

    unsigned int buf_len:16;
    unsigned int dbg_id:16;
};

/***********************************************************************
 * GPD structure and operation
 ***********************************************************************/
#define GPD_FLAGS_HWO   (1<<0)
#define GPD_FLAGS_BDP   (1<<1)
#define GPD_FLAGS_BPS   (1<<2)
#define GPD_FLAGS_IOC   (1<<7)

#define GPD_IS_FLAGS_HWO(_pd)       (((struct GPD *)(_pd))->flag & GPD_FLAGS_HWO)
#define GPD_SET_FLAGS_HWO(_pd)      (((struct GPD *)(_pd))->flag |= GPD_FLAGS_HWO)
#define GPD_CLR_FLAGS_HWO(_pd)      (((struct GPD *)(_pd))->flag &= (~GPD_FLAGS_HWO))

#define GPD_IS_FLAGS_BPS(_pd)       (((struct GPD *)(_pd))->flag & GPD_FLAGS_BPS)
#define GPD_SET_FLAGS_BPS(_pd)      (((struct GPD *)(_pd))->flag |= GPD_FLAGS_BPS)
#define GPD_CLR_FLAGS_BPS(_pd)      (((struct GPD *)(_pd))->flag &= (~GPD_FLAGS_BPS))

#define GPD_IS_FLAGS_IOC(_pd)       (((struct GPD *)(_pd))->flag & GPD_FLAGS_IOC)
#define GPD_SET_FLAGS_IOC(_pd)      (((struct GPD *)(_pd))->flag |= GPD_FLAGS_IOC)
#define GPD_CLR_FLAGS_IOC(_pd)      (((struct GPD *)(_pd))->flag &= (~GPD_FLAGS_IOC))

#define GPD_SET_NEXT(_pd, _next)    (((struct GPD *)(_pd))->p_next = (struct GPD *)_next)
#define GPD_GET_NEXT(_pd)           ((struct GPD *)((struct GPD *)(_pd))->p_next)

#define GPD_SET_DATA(_pd, _data)    (((struct GPD *)(_pd))->p_buf = (unsigned char *)_data)
#define GPD_GET_DATA(_pd)           ((unsigned char *)((struct GPD *)(_pd))->p_buf)

#define GPD_SET_BUF_LEN(_pd, _len)  (((struct GPD *)(_pd))->buf_len = _len)
#define GPD_GET_BUF_LEN(_pd)        (((struct GPD *)(_pd))->buf_len)

#define GPD_SET_ALLEN(_pd, _len)    (((struct GPD *)(_pd))->allow_len = _len)
#define GPD_GET_ALLEN(_pd)          (((struct GPD *)(_pd))->allow_len)


/***********************************************************************
 * GPD memory and RX local buffer
 ***********************************************************************/
static struct GPD *s_TGPD, *s_RGPD;
static paddr_t s_TGPD_phys, s_RGPD_phys;
static unsigned char *s_local_rx_buf;
static unsigned int s_local_buf_avai;
static unsigned int s_local_buf_curr;
static unsigned int s_local_buf_size;
static unsigned int s_cldma_init_done;

bool check_timeout(lk_time_t start_tick, lk_time_t timeout_tick)
{
    /* @ return True when timeout */
    return (timeout_tick == 0 || current_time() - start_tick >= timeout_tick);
}

#if 0
static void show_gpd(GPD *p_gpd)
{
    unsigned int *ptr = (unsigned int *)p_gpd;

    LTRACEF("GPD: 0x%x 0x%x 0x%x 0x%x\r\n", ptr[0], ptr[1], ptr[2], ptr[3]);
}
#endif

static void cfg_gpd(unsigned int rx, unsigned int i, unsigned char *ptr, unsigned int size)
{
    struct GPD *p_gpd;
    paddr_t buf_phys;

    if (rx)
        p_gpd = &s_RGPD[i];
    else
        p_gpd = &s_TGPD[i];

    /* translate to physical address */
    buf_phys = vaddr_to_paddr((void *)ptr);
    LTRACEF("iobuffer %p (phys 0x%lx)\n", ptr, buf_phys);

    if (buf_phys && size) {
        GPD_CLR_FLAGS_IOC(p_gpd);
        GPD_CLR_FLAGS_BPS(p_gpd);
        GPD_SET_DATA(p_gpd, buf_phys);
        if (rx) {
            GPD_SET_ALLEN(p_gpd, size);
            GPD_SET_BUF_LEN(p_gpd, 0);
        } else
            GPD_SET_BUF_LEN(p_gpd, size);
        GPD_SET_FLAGS_HWO(p_gpd);
    } else
        GPD_CLR_FLAGS_HWO(p_gpd);

    arch_clean_cache_range((unsigned int)p_gpd, sizeof(struct GPD));
    //show_gpd(p_gpd);
}

static int cldma_gpd_init(void)
{
    int i;
    void *vptr;

    /*
     * Request at least 10 GPD for TX/RX and cache align,
     * the memory is only allocated once at init and will keep in HEAP afterwards,
     * thereofre, no free memory at the current design.
     * The memalign API will do 64 byte alignment as well, CACHE_LINE = 64.
     */
    vptr = memalign(CACHE_LINE, sizeof(struct GPD) * (TGPD_N + 4 + RGPD_N + 4));
    if (!vptr)
        return ERR_NO_MEMORY;

    /* TPGD_N+3 means 16*4=64 for TPGD while RGPD will at different cache line. */
    s_TGPD = (struct GPD *) vptr;
    s_RGPD = &s_TGPD[TGPD_N + 3];
    LTRACEF("%s mem addr:0x%x s_TGPD:0x%x s_RGPD:0x%x\r\n", __func__, (unsigned int)vptr,
        (unsigned int)s_TGPD, (unsigned int)s_RGPD);

    /* translate to physical address */
    s_TGPD_phys = vaddr_to_paddr((void *)s_TGPD);
    s_RGPD_phys = vaddr_to_paddr((void *)s_RGPD);
    LTRACEF("iobuffer %p (TPGD_phys 0x%lx)\n", s_TGPD, s_TGPD_phys);
    LTRACEF("iobuffer %p (RPGD_phys 0x%lx)\n", s_RGPD, s_RGPD_phys);


    memset(s_TGPD, 0, sizeof(struct GPD) * (TGPD_N + 1));
    memset(s_RGPD, 0, sizeof(struct GPD) * (RGPD_N + 1));

    for (i = 0; i < TGPD_N; i++)
        GPD_SET_NEXT(&s_TGPD[i], &s_TGPD[i + 1]);
    for (i = 0; i < RGPD_N; i++)
        GPD_SET_NEXT(&s_RGPD[i], &s_RGPD[i + 1]);

    return 0;
}

static int cldma_buffer_init(void)
{
    void *vptr;

    s_local_buf_size = LOCAL_BUFF_SIZE;

    /*
     * Request at least 10 GPD for TX/RX and cache align,
     * the memory is only allocated once at init and will keep in HEAP afterwards,
     * thereofre, no free memory at the current design.
     * The memalign API will do 64 byte alignment as well, CACHE_LINE = 64.
     */
    vptr = memalign(CACHE_LINE, s_local_buf_size);
    if (!vptr)
        return ERR_NO_MEMORY;

    s_local_rx_buf = (unsigned char *)vptr;
    LTRACEF("%s mem addr:0x%x\r\n", __func__, (unsigned int)s_local_rx_buf);

    return 0;
}

static int cldma_tx_wait_idle(lk_time_t start_tick, lk_time_t timeout_tick)
{
    if (cldma_reg_tx_active(CLDMA_QUEUE_LK)) {
        cldma_reg_stop_tx_que(CLDMA_QUEUE_LK);
        while (cldma_reg_tx_active(CLDMA_QUEUE_LK)) {
            if (check_timeout(start_tick, timeout_tick))
                return -1;
        }
    }
    cldma_reg_clr_tx_int();
    return 0;
}

static int cldma_rx_wait_idle(unsigned int start_tick, unsigned int timeout_tick)
{
    if (cldma_reg_rx_active(CLDMA_QUEUE_LK)) {
        cldma_reg_stop_rx_que(CLDMA_QUEUE_LK);
        while (cldma_reg_rx_active(CLDMA_QUEUE_LK)) {
            if (check_timeout(start_tick, timeout_tick))
                return -1;
        }
    }
    cldma_reg_clr_rx_int();
    return 0;
}

static int cldma_tx_wait_fsm_err_to_rdy(unsigned int start_tick, unsigned int timeout_tick)
{
    if (!cldma_reg_get_tx_ldmu_err())
        return 0;

    cldma_reg_clr_tx_ldmu_err();
    while (cldma_reg_get_tx_ldmu_err()) {
        if (check_timeout(start_tick, timeout_tick))
            return -1;
    }
    return 0;
}

static int cldma_rx_wait_fsm_err_to_rdy(unsigned int start_tick, unsigned int timeout_tick)
{
    if (!cldma_reg_get_rx_ldmu_err())
        return 0;

    cldma_reg_clr_rx_ldmu_err();
    while (cldma_reg_get_rx_ldmu_err()) {
        if (check_timeout(start_tick, timeout_tick))
            return -1;
    }
    return 0;
}


#define CLDMA_WAIT_DONE             0
#define CLDMA_WAIT_TIMEOUT          1
#define CLDMA_WAIT_HW_ERR_ASSERT    2

static int wait_tx_gpd_done(unsigned int num, unsigned int start_tick, unsigned int timeout_tick)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        while (1) {
            arch_invalidate_cache_range((unsigned int)&s_TGPD[i], sizeof(struct GPD));
            if (!(GPD_IS_FLAGS_HWO(&s_TGPD[i])))
                break;
            if (cldma_reg_tx_error(CLDMA_QUEUE_LK))
                return CLDMA_WAIT_HW_ERR_ASSERT;
            if (check_timeout(start_tick, timeout_tick))
                return CLDMA_WAIT_TIMEOUT;
        }
    }
    return CLDMA_WAIT_DONE;
}

static int wait_rx_gpd_done(unsigned int num, unsigned int start_tick, unsigned int timeout_tick)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        while (1) {
            arch_invalidate_cache_range((unsigned int)&s_RGPD[i], sizeof(struct GPD));
            if (!(GPD_IS_FLAGS_HWO(&s_RGPD[i])))
                break;
            if (cldma_reg_rx_error(CLDMA_QUEUE_LK))
                return CLDMA_WAIT_HW_ERR_ASSERT;
            if (check_timeout(start_tick, timeout_tick))
                return CLDMA_WAIT_TIMEOUT;
        }
    }
    return CLDMA_WAIT_DONE;
}

static void cldma_tx_start(void)
{
    cldma_reg_set_tx_start_addr(CLDMA_QUEUE_LK, (unsigned char *)s_TGPD_phys);
    cldma_reg_start_tx_que(CLDMA_QUEUE_LK);
}

static void cldma_rx_start(void)
{
    cldma_reg_set_rx_start_addr(CLDMA_QUEUE_LK, (unsigned char *)s_RGPD_phys);
    cldma_reg_start_rx_que(CLDMA_QUEUE_LK);
}

int cldma_put_data(unsigned char *buf, unsigned int len, lk_time_t timeout)
{
    unsigned int sent_num = 0, send_num_curr;
    lk_time_t timeout_tick;
    lk_time_t start_tick;
    unsigned int ret = 0;
    unsigned int wait_err_code = 0;

    if (!s_cldma_init_done) {
        ret = -1;
        goto err;
    }
    if (buf == NULL) {
        ret = -2;
        goto err;
    }

    if (len == 0) {
        ret = -3;
        goto err;
    }

    if (timeout < 100)
        timeout = 100;

    timeout_tick = timeout;
    start_tick = current_time();

    if (cldma_tx_wait_idle(start_tick, timeout_tick) != 0) {
        ret = -4;
        goto err;
    }
    if (cldma_tx_wait_fsm_err_to_rdy(start_tick, timeout_tick) != 0) {
        ret = -5;
        goto err;
    }

    /* This is used for cache flush, and it will flush previous address as well. */
    arch_clean_cache_range((unsigned int)buf, len);

    while (len) {
        if (len > s_local_buf_size) {
            cfg_gpd(0, 0, &buf[sent_num], s_local_buf_size);
            send_num_curr = s_local_buf_size;
        } else {
            cfg_gpd(0, 0, &buf[sent_num], len);
            send_num_curr = len;
        }

        mb(); /* ensure the data is up-to-date */
        cldma_tx_start();
        wait_err_code = wait_tx_gpd_done(1, start_tick, timeout_tick);
        if (wait_err_code == CLDMA_WAIT_HW_ERR_ASSERT) {
            ret = -6;
            goto err;
        } else if (wait_err_code == CLDMA_WAIT_TIMEOUT) {
            ret = -7;
            goto err;
        }
        sent_num += send_num_curr;
        len -= send_num_curr;
    }

err:
    cldma_reg_stop_tx_que(CLDMA_QUEUE_LK);
    if (sent_num > 0)
        ret = (int)sent_num;
    else
        cldma_reg_dump();

    return ret;
}


static const char *hex_str = "0123456789ABCDEF";

static void show_char(char ch)
{
    LTRACEF("%c%c ", hex_str[(ch >> 4) & 0xF], hex_str[ch & 0xF]);
}

void ccci_show_raw(char buf[], int len)
{
    int i;

    if (len >= 16)
        len = 16;

    LTRACEF("raw [%d]:", len);
    for (i = 0; i < len; i++)
        show_char(buf[i]);
    LTRACEF("\n");
}

int cldma_get_data(unsigned char *buf, unsigned int len, lk_time_t timeout)
{
    unsigned int ret = 0, recv_num = 0;
    unsigned int wait_read_len = 0;
    lk_time_t timeout_tick;
    lk_time_t start_tick;
    unsigned char *tmp_ptr;
    unsigned int need_cpy = 0, get_len, cpy_size;

    unsigned int wait_err_code = 0;

    if (buf == NULL) {
        ret = -1;
        goto err;
    }

    if (len == 0) {
        ret = -2;
        goto err;
    }

    if (s_local_buf_avai) {
        if (s_local_buf_avai >= len) {
            memcpy(buf, &s_local_rx_buf[s_local_buf_curr], len);
            s_local_buf_curr += len;
            s_local_buf_avai -= len;
            return len;
        }
        memcpy(buf, &s_local_rx_buf[s_local_buf_curr], s_local_buf_avai);
        buf += s_local_buf_avai;
        len -= s_local_buf_avai;
        s_local_buf_curr = 0;
        s_local_buf_avai = 0;
    }

    wait_read_len = len;
    tmp_ptr = buf;
    arch_clean_cache_range((unsigned int)buf, len);

    if (timeout < 100)
        timeout = 100;

    timeout_tick = timeout;
    start_tick = current_time();

    while (wait_read_len) {
        if (cldma_rx_wait_idle(start_tick, timeout_tick) != 0) {
            ret = -3;
            goto err;
        }

        if (cldma_rx_wait_fsm_err_to_rdy(start_tick, timeout_tick) != 0) {
            ret = -4;
            goto err;
        }

        if (wait_read_len >= s_local_buf_size) {
            /* clean cache of tmp_ptr is done before while it's been asigned.*/
            cfg_gpd(1, 0, tmp_ptr, s_local_buf_size);
            need_cpy = 0;
        } else {
            arch_clean_cache_range((unsigned int)s_local_rx_buf, s_local_buf_size);
            cfg_gpd(1, 0, s_local_rx_buf, s_local_buf_size);
            need_cpy = 1;
        }

        mb(); /* ensure the data is up-to-date */
        cldma_rx_start();

        wait_err_code = wait_rx_gpd_done(1, start_tick, timeout_tick);
        if (wait_err_code == CLDMA_WAIT_HW_ERR_ASSERT) {
            ret = -5;
            goto err;
        } else if (wait_err_code == CLDMA_WAIT_TIMEOUT) {
            ret = -6;
            goto err;
        }

        get_len = GPD_GET_BUF_LEN(&s_RGPD[0]);
        if (get_len) {
            LTRACEF("get len:%d\n", get_len);

            /*
             * s_RGPD will be transfer into PA in the cfg gpd, however, the invalidate API
             * only accept VA, so use s_local_rx_buf to flash the cache.
             */

            arch_invalidate_cache_range((unsigned int)s_local_rx_buf, get_len);
        }
        if (need_cpy) {
            if (wait_read_len >= get_len)
                cpy_size = get_len;
            else {
                cpy_size = wait_read_len;
                s_local_buf_curr = cpy_size;
                s_local_buf_avai = get_len - cpy_size;
                get_len = wait_read_len;
            }
            memcpy(tmp_ptr, s_local_rx_buf, cpy_size);
            ccci_show_raw((char *)s_local_rx_buf, cpy_size);
        }

        wait_read_len -= get_len;
        tmp_ptr += get_len;
        recv_num += get_len;

        cldma_reg_stop_rx_que(CLDMA_QUEUE_LK);

        /*
         * Return the received length, once there's no more RX data from hardware.
         * Therefore, the return length may be less then user required.
         */
        if (wait_err_code == CLDMA_WAIT_DONE) {
                LTRACEF("cldma get rx data done len=%d \r\n", get_len);
                goto err;
        }
    }

err:
    if (recv_num > 0)
        ret = (int)recv_num;
    else
        cldma_reg_dump();
    return ret;
}

void cldma_init(void)
{
    /* Set HW left /right hand domain ID */
    cldma_reg_set_domain_id();

    if (cldma_gpd_init() < 0) {
        dprintf(CRITICAL, "GPD memory alloc fail\r\n");
        return;
    }
    if (cldma_buffer_init() < 0) {
        dprintf(CRITICAL, "Rx local buffer alloc fail\r\n");
        return;
    }
    s_cldma_init_done = 1;
    dprintf(ALWAYS, "CLDMA init done!!\r\n");
}

