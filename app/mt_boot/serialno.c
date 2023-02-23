/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app/mt_boot.h>
#include <compiler.h>
#include <debug.h>
#if defined(MTK_USB_UNIQUE_SERIAL)
#include <platform/sec_devinfo.h>
#endif
#include <string.h>
#include <sysenv.h>
/*PRIZE-uique serial-tangan-20170920-start*/
#include <lib/bio.h>
#include <ctype.h>
/*PRIZE-uique serial-tangan-20170920-start*/

#define SN_BUF_LEN  19  /* fastboot use 13 bytes as default, max is 19 */
#define DEFAULT_SERIAL_NUM "0123456789ABCDEF"


#ifdef MTK_USB_UNIQUE_SERIAL
static char udc_chr[32] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ456789"};

int get_serial(u64 hwkey, u32 chipid, char ser[SN_BUF_LEN])
{
    u16 hashkey[4];
    u32 idx, ser_idx;
    u32 digit;

    memset(ser, 0x00, SN_BUF_LEN);

    /* split to 4 key with 16-bit width each */
    for (idx = 0; idx < countof(hashkey); idx++) {
        hashkey[idx] = (u16)(hwkey & 0xffff);
        hwkey >>= 16;
    }

    /* hash the key with chip id */
    for (idx = 0; idx < countof(hashkey); idx++) {
        digit = (chipid % 10);
        hashkey[idx] = (hashkey[idx] >> digit) | (hashkey[idx] << (16 - digit));
        chipid = (chipid / 10);
    }

    /* generate serial using hashkey */
    ser_idx = 0;
    for (idx = 0; idx < countof(hashkey); idx++) {
        ser[ser_idx++] = (hashkey[idx] & 0x001f);
        ser[ser_idx++] = (hashkey[idx] & 0x00f8) >> 3;
        ser[ser_idx++] = (hashkey[idx] & 0x1f00) >> 8;
        ser[ser_idx++] = (hashkey[idx] & 0xf800) >> 11;
    }
    for (idx = 0; idx < ser_idx; idx++)
        ser[idx] = udc_chr[(int)ser[idx]];
    ser[ser_idx] = 0x00;
    return 0;
}

/*PRIZE-uique serial-tangan-20170920-start*/
static inline int read_product_info_prize(char *buf)
{
    int offset = 116;//64+40+12
    char tmp_prize[SN_BUF_LEN] = {0};
	int tmp = 0;
    int len = 0;
    
	if (!buf) return 0;
    bdev_t *dev;
    dev = bio_open_by_label("proinfo");
    if(!dev){
        dprintf(CRITICAL, "fail to open proinfo");
        return 0;
    }
    len = bio_read(dev, tmp_prize, offset, SN_BUF_LEN);

	for ( ; tmp < len; tmp++) {
        buf[tmp] = tmp_prize[tmp];
		if ( (buf[tmp] == 0 || buf[tmp] == 0x20) && tmp > 0) {
			break;
		}else if ( !isalpha(buf[tmp]) && !isdigit(buf[tmp])){
            return 0;
        }
			
	}
	return tmp;
}
/*PRIZE-uique serial-tangan-20170920-end*/

static inline int read_product_usbid(char *serialno)
{
    u64 key;
    u32 hrid_size, ser_len;
    u32 i, chip_code = 0, errcode = 0;
    const char *p = SUB_PLATFORM; /* read platform from lk2/target/xxxx/rule.mk */
    /*PRIZE-uique serial-tangan-20170920-start*/
	char tmp_prize[SN_BUF_LEN] = {0};
	unsigned ser_len_prize = 0;
	ser_len_prize = read_product_info_prize(tmp_prize);
	if(ser_len_prize == 0){
	/*PRIZE-uique serial-tangan-20170920-end*/

    /* calculate by platform string
     * eg. mt6895 = 109*2^5 + 116*2^4 + 54*2^3 + 56*2^2 + 57*2 + 53
     */
    while (*p) {
        /* prevent u32 from overflowing */
        if (chip_code > (chip_code << 1) ||
            UINT32_MAX - (chip_code << 1) < (u32)*p) {
            chip_code = UINT32_MAX;
            break;
        }
        chip_code = ((chip_code << 1) + (u32)*p);
        p = p + 1;
    }

    /* read hrid*/
    hrid_size = (u32)HRID_SIZE;

    /* check ser_buf len. if need 128bit id, should defined into cust_usb.h */
    if (hrid_size * 8 > SN_BUF_LEN) {
        hrid_size = 2;
        errcode = 1;
    }

    for (i = 0; i < hrid_size / 2; i++) {
        key = get_devinfo_with_index(13 + i * 2); /* 13, 15 */
        key = (key << 32) | (unsigned int)get_devinfo_with_index(
                12 + i * 2); /* 12, 14 */
        if (key != 0) {
            get_serial(key, chip_code, serialno);
        } else {
            memcpy(serialno, DEFAULT_SERIAL_NUM, strlen(DEFAULT_SERIAL_NUM));
            errcode = 2;
        }
    }
    /*PRIZE-uique serial-tangan-20170920-start*/
	}else{
		memset( serialno, 0, sizeof(serialno));
        int temp_i = 0;
        for(;temp_i<ser_len_prize;temp_i++){
            serialno[temp_i] = tmp_prize[temp_i];
        }
		serialno[temp_i] = '\0'; 
	}
	/*PRIZE-uique serial-tangan-20170920-end*/
    return errcode;
}
#endif
char *get_serialno(void)
{
    static char sn_buf[SN_BUF_LEN + 1];
    int errcode;
    unsigned int len;
    char *id_tmp = get_env("MTK_DEVICE_ID");

    if (strlen(sn_buf) == 0) {
        if (!id_tmp) {
            dprintf(CRITICAL, "Set serial # to default value.\n");
            len = strlen(DEFAULT_SERIAL_NUM);
            strncpy(sn_buf, DEFAULT_SERIAL_NUM, len);
            sn_buf[len] = '\0';
        } else {
            dprintf(CRITICAL, "Set serial # from para.\n");
            len = strlen(id_tmp);
            len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
            strncpy(sn_buf, id_tmp, len);
            sn_buf[len] = '\0';
        }

#ifdef MTK_USB_UNIQUE_SERIAL
        errcode = read_product_usbid(sn_buf);
        if (errcode)
            dprintf(CRITICAL, "Set serial # from efuse. error: %d\n", errcode);
        len = strlen(sn_buf);
        len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
        sn_buf[len] = '\0';
#endif
    }
    return sn_buf;
}


