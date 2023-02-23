/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_gateic.h"

/* i2c control start */
#define MTK_PANEL_I2C_ID_NAME "MTK_I2C_LCD_BIAS"

int mtk_panel_i2c_write_bytes(struct mt_i2c *client,
        unsigned char addr, unsigned char value)
{
    int ret = 0;
    u8 write_data[2] = { 0 };
#if MTK_LCM_DEBUG_DUMP
    u8 read_data[2] = { 0 };
#endif

    if (client == NULL) {
        LCM_MSG("%s: ERROR!! invalid parameters\n", __func__);
        return -EINVAL;
    }

    write_data[0] = addr;
    write_data[1] = value;
    ret = mtk_i2c_write(client->id, client->addr, write_data, 2, NULL);
    if (ret < 0)
        LCM_MSG("%s: [ERROR] id:0x%x, addr:0x%x, write:0x%x with 0x%x fail:%d\n",
             __func__, client->id, client->addr, addr, value, ret);

#if MTK_LCM_DEBUG_DUMP
    read_data[0] = write_data[0];
    read_data[1] = write_data[1];
    ret = mtk_i2c_write_read(client->id, client->addr, &read_data[0], 1, 1, NULL);
    if (ret < 0)
        LCM_MSG("%s: [ERROR] read failed:%d\n", __func__, ret);
    LCM_MSG("%s: ch:%d,addr:0x%x,mod:%d,speed:%d,reg:0x%x,val:0x%x, read:(0x%x,0x%x)\n",
        __func__, client->id, client->addr,
        client->mode, client->speed, addr, value,
        read_data[0], read_data[1]);
#endif

    return ret;
}

int mtk_panel_i2c_write_multiple_bytes(struct mt_i2c *client,
        unsigned char addr, unsigned char *value, unsigned int size)
{
    int ret = 0;
    u8 *write_data = NULL;

    if (client == NULL || value == NULL || size == 0) {
        LCM_MSG("%s, ERROR!! invalid parameters, size:%u\n", __func__, size);
        return -EINVAL;
    }
    write_data = malloc(ROUNDUP(size, 4));
    write_data[0] = addr;
    memcpy(&write_data[1], value, size);


    ret = mtk_i2c_write(client->id, client->addr, write_data, size, NULL);
    if (ret < 0)
        LCM_MSG("%s: [ERROR] write 0x%x fail:%d, size:%u\n",
             __func__, addr, ret, size);

    free(write_data);
    write_data = NULL;
    LCM_DBG("%s: ch:%d,addr:0x%x,mod:%d,speed:%d,reg:0x%x\n",
        __func__, client->id, client->addr,
        client->mode, client->speed, addr);
    return ret;
}
