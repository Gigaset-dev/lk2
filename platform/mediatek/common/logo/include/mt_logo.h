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

#pragma once

//physical address (pa) & virtual address (va) for mblock allocation
struct LOGO_MEMORY_T {
    uint64_t logo_base_addr_pa;
    uint64_t dec_logo_base_addr_pa;
    void *logo_base_addr_va;
    void *dec_logo_base_addr_va;
};


void mt_disp_show_boot_logo(void);
void mt_disp_enter_charging_state(void);
void mt_disp_show_battery_full(void);
void mt_disp_show_battery_capacity(unsigned int capacity);
void mt_disp_show_charger_ov_logo(void);
void mt_disp_show_low_battery(void);
void mt_disp_show_charging(int index);
void mt_disp_show_plug_charger(void);
void mt_disp_clear_screen(unsigned int color);
