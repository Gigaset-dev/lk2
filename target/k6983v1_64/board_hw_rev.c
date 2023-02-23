/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <board_hw_rev.h>
#include <debug.h>
#include <gpio_api.h>
#include <sys/types.h>

#define GPIO_HW_ID0 (GPIO203)
#define GPIO_HW_ID1 (GPIO206)

static void configure_board_id_pin(enum GPIO_PIN pin)
{
    mt_set_gpio_mode(pin, GPIO_MODE_00);
    mt_set_gpio_dir(pin, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE);
    mt_set_gpio_pull_select(pin, GPIO_PULL_DOWN);
}

/*
 * NOTE: This function is for the internal project of MT6983.
 * --------------------------------------
 * | GPIO203      GPIO206    PCB_ID     |
 * |   Low          Low      Phone1     |
 * |   Low          High     EVB1       |
 * |   High         Low      Phone2     |
 * |   High         High     EVB2       |
 * --------------------------------------
 * Customers should implement their own custom_get_mdtbo_index()
 */
int custom_get_mdtbo_index(void)
{
    int idx = 0;
    s32 hw_id0;
    s32 hw_id1;

    configure_board_id_pin(GPIO_HW_ID0);
    configure_board_id_pin(GPIO_HW_ID1);
    hw_id0 = mt_get_gpio_in(GPIO_HW_ID0);
    hw_id1 = mt_get_gpio_in(GPIO_HW_ID1);

    dprintf(INFO, "hw_id0 = %d, hw_id1 = %d\n", hw_id0, hw_id1);

    if (hw_id0 == GPIO_IN_ONE) {
        if (hw_id1 == GPIO_IN_ONE) {
            idx = 3;
            dprintf(INFO, "EVB2, idx = %d\n", idx);
        } else {
            idx = 2;
            dprintf(INFO, "Phone2, idx = %d\n", idx);
        }
    } else {
        if (hw_id1 == GPIO_IN_ONE) {
            idx = 1;
            dprintf(INFO, "EVB1, idx = %d\n", idx);
        } else {
            idx = 0;
            dprintf(INFO, "Phone1, idx = %d\n", idx);
        }
    }

    return idx;
}
