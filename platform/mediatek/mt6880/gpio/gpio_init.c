/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <gpio_api.h>
#include <libfdt.h>
#include <platform/gpio.h>
#include <trace.h>

#include "gpio_const.h"
#include "gpio_init.h"

#define LOCAL_TRACE 0

static int mt_gpio_get_default_chip_from_dtb(const void *fdt, struct gpio_pin *gpio_data)
{
    unsigned int *data;
    int off, len, j, pin;

    off = fdt_path_offset(fdt, GPIO_DT_NODE_NAME_SOC);
    if (off < 0) {
        LTRACEF("[GPIO] Failed to find " GPIO_DT_NODE_NAME_SOC " in dtb\n");
        return -1;
    }

    data = (unsigned int *)fdt_getprop(fdt, off, GPIO_DT_NODE_PROP_NAME, &len);
    if (len <= 0 || !data) {
        LTRACEF("[GPIO] Fail to found property " GPIO_DT_NODE_PROP_NAME "\n");
        return -1;
    }

    /*
     * Init pin mode as GPIO_INIT_NO_COVER
     * If pin mode is not override by dtb
     * means the pin should skip the setting.
     */
    for (pin = 0; pin < MT_GPIO_BASE_MAX; pin++)
        gpio_data[pin].mode = GPIO_MODE_INIT_NO_COVER;

    if ((len % ELEMENTS_PER_GPIO) == 0) {
        /* Per element use 4 bytes */
        len /= (ELEMENTS_PER_GPIO * 4);
        for (j = 0; j < len; j++) {
            pin = fdt32_to_cpu(*data);

            if (pin >= MT_GPIO_BASE_MAX) {
                data += 7;
                continue;
            }

            data++;
            gpio_data[pin].mode = (unsigned char) fdt32_to_cpu(*data);

            data++;
            gpio_data[pin].dir = (unsigned char) fdt32_to_cpu(*data);

            data++;
            gpio_data[pin].dataout = (unsigned char) fdt32_to_cpu(*data);

            data++;
            gpio_data[pin].pullen = (unsigned char) fdt32_to_cpu(*data);

            data++;
            gpio_data[pin].pull = (unsigned char) fdt32_to_cpu(*data);

            data++;
            gpio_data[pin].smt = (unsigned char) fdt32_to_cpu(*data);

            data++;
        }
    } else {
        LTRACEF("[GPIO] len %d not multiple of %d\n", len, ELEMENTS_PER_GPIO);
        return -1;
    }

    return 0;
}

static void mt_gpio_set_default_chip(const void *fdt)
{
    enum GPIO_PIN pin = GPIO0;
    struct gpio_pin *gpio_data = NULL;
    bool skip_free = false;

    if (fdt) {
        if (fdt_check_header(fdt) != 0)
            panic("Bad DTB header.\n");
    } else {
        dprintf(CRITICAL, "[GPIO] fdt null, fall back to default\n");
        goto apply_default;
    }

    /* alloc memory for gpio settings */
    gpio_data = malloc(sizeof(struct gpio_pin) * MT_GPIO_BASE_MAX);
    if (!gpio_data) {
        dprintf(CRITICAL, "[GPIO] Fail to alloc mem, fall back to default\n");
        goto apply_default;
    }

    memset(gpio_data, 0, sizeof(struct gpio_pin) * MT_GPIO_BASE_MAX);

    /* check if valid gpio setting is in the dtb first */
    if (mt_gpio_get_default_chip_from_dtb(fdt, gpio_data) < 0) {
        dprintf(CRITICAL, "[GPIO] fall back to default\n");
        free(gpio_data);
    } else
        goto apply_setting;

apply_default:
    gpio_data = gpio_array;
    skip_free = true;

apply_setting:
    for (pin = GPIO0; pin < MT_GPIO_BASE_MAX; pin++) {
        /* Skip pins which is set as GPIO_INIT_NO_COVER */
        if (gpio_data[pin].mode == GPIO_MODE_INIT_NO_COVER)
            continue;

        /* set GPIOx_MODE*/
        mt_set_gpio_mode(pin, gpio_data[pin].mode);

        /* set GPIOx_DIR*/
        mt_set_gpio_dir(pin, gpio_data[pin].dir);

        /* set GPIOx_PULL*/
        mt_set_gpio_pull_select(pin, gpio_data[pin].pull);

        /* set GPIOx_PULLEN*/
        mt_set_gpio_pull_enable(pin, gpio_data[pin].pullen);

        /* set GPIOx_DATAOUT*/
        mt_set_gpio_out(pin, gpio_data[pin].dataout);

        /* set GPIO0_SMT */
        mt_set_gpio_smt(pin, gpio_data[pin].smt);

        LTRACEF(
            "[init] GPIO%d mode(%d) dir(%d) pull(%d) pullen(%d) dataout(%d) smt(%d)\r\n",
            pin,
            gpio_data[pin].mode,
            gpio_data[pin].dir,
            gpio_data[pin].pull,
            gpio_data[pin].pullen,
            gpio_data[pin].dataout,
            gpio_data[pin].smt
        );
    }

    if (!skip_free)
        free(gpio_data);
}

void mt_gpio_set_default(const void *fdt)
{
    /* apply setting only in realchip */
#ifndef PROJECT_TYPE_FPGA
    mt_gpio_set_default_chip(fdt);
#endif
}
