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

#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <load_image.h>
#include <lib/show_animation_common.h>
#include <mblock.h>
#include <mt_disp_drv.h>
#include <mt_logo.h>
#include <trace.h>

#define LOCAL_TRACE 0

/* Logo Max  size */
#define LK_LOGO_MAX_SIZE        (0x1000000) //16M

/********** show_animation_ver:  charging animation version  ************/
/*                                                                       */
/* version 0: show 4 recatangle growing animation without battery number */
/* version 1: show wave animation with  battery number                   */
/*                                                                       */
/***                                                                   ***/

int show_animation_ver = VERSION_OLD_ANIMATION;

static void mt_lk_logo_memory_deinit(struct LOGO_MEMORY_T *logo_memory)
{
    mblock_free(logo_memory->logo_base_addr_pa);
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)logo_memory->logo_base_addr_va);

    mblock_free(logo_memory->dec_logo_base_addr_pa);
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)logo_memory->dec_logo_base_addr_va);
}


static int mt_lk_logo_memory_init(struct LOGO_MEMORY_T *logo_memory)
{
    int ret, err = NO_ERROR;
    unsigned int size_of_fb_buffer = 0;

    logo_memory->logo_base_addr_pa = mblock_alloc(LK_LOGO_MAX_SIZE, PAGE_SIZE,
                                         MBLOCK_NO_LIMIT, 0, 0, "system_lk2_logo");

    if (!logo_memory->logo_base_addr_pa) {
        dprintf(CRITICAL, "logo_addr memory failed to allocate");
        return ERR_NO_MEMORY;
    }

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                          "vm-logo-reserved",
                          LK_LOGO_MAX_SIZE,
                          &logo_memory->logo_base_addr_va,
                          PAGE_SIZE_SHIFT,
                          (paddr_t)logo_memory->logo_base_addr_pa,
                          0,
                          ARCH_MMU_FLAG_CACHED);

    if (err != NO_ERROR)
        panic("err=%d, vm_logo (0x%p)\n", err, logo_memory->logo_base_addr_va);
    else
        LTRACEF("vmm logo addr=0x%llx", logo_memory->logo_base_addr_pa);

    ret = load_partition("logo", "logo", logo_memory->logo_base_addr_va, LK_LOGO_MAX_SIZE);

    if (ret < 0) {
        dprintf(CRITICAL, "logo.bin not loaded");
        mblock_free(logo_memory->logo_base_addr_pa);
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)logo_memory->logo_base_addr_va);
        return ret;
    }

    size_of_fb_buffer = mt_get_fb_size();
    LTRACEF("Size of framebuffer = %d\n", size_of_fb_buffer);

    logo_memory->dec_logo_base_addr_pa = mblock_alloc(size_of_fb_buffer, PAGE_SIZE,
                                             MBLOCK_NO_LIMIT, 0, 0, "system_lk2_dec-logo");

    if (!logo_memory->dec_logo_base_addr_pa) {
        dprintf(CRITICAL, "dec_logo_addr memory failed to allocate");
        mblock_free(logo_memory->logo_base_addr_pa);
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)logo_memory->logo_base_addr_va);
        return ERR_NO_MEMORY;
    }

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                          "vm-dec-logo-reserved",
                          size_of_fb_buffer,
                          &logo_memory->dec_logo_base_addr_va,
                          PAGE_SIZE_SHIFT,
                          (paddr_t)logo_memory->dec_logo_base_addr_pa,
                          0,
                          ARCH_MMU_FLAG_CACHED);

    if (err != NO_ERROR)
        panic("err=%d, vm_dec_logo (0x%p)\n", err, logo_memory->dec_logo_base_addr_va);
    else
        LTRACEF("vmm dec_logo addr=0x%llx size=0x%x\n", logo_memory->dec_logo_base_addr_pa,
                size_of_fb_buffer);


    return NO_ERROR;
}



static void sync_anim_version(void *logo_addr)
{
#ifdef ANIMATION_NEW
    show_animation_ver = VERSION_NEW_ANIMATION;
#else
    show_animation_ver = VERSION_OLD_ANIMATION;
    LTRACEF("not define ANIMATION_NEW:show old animation\n");
#endif

    if (logo_addr == NULL)
        return;

    if (show_animation_ver == VERSION_NEW_ANIMATION) {
        unsigned int logonum = *(unsigned int *)logo_addr;

        LTRACEF("pinfo[0]=0x%08x, pinfo[1]=0x%08x, pinfo[2]=%d\n",
            *(unsigned int *)logo_addr,
            *((unsigned int *)logo_addr + 1),
            *((unsigned int *)logo_addr + 2));

        LTRACEF("define ANIMATION_NEW: show new animation with capacity num\n");
        LTRACEF("CAPACITY_LEFT =%d, CAPACITY_TOP =%d\n", (CAPACITY_LEFT), (CAPACITY_TOP));
        LTRACEF("LCM_HEIGHT=%d, LCM_WIDTH=%d\n", (CAPACITY_RIGHT), (CAPACITY_BOTTOM));
        if (logonum < 6)
            show_animation_ver = VERSION_OLD_ANIMATION;
        else
            show_animation_ver = VERSION_NEW_ANIMATION;
    }
}

/*
 * Initliaze charging animation parameters
 */
static void init_fb_screen(struct LCM_SCREEN_T *physical_screen)
{
    unsigned int fb_size = mt_get_fb_size();

    physical_screen->width = CFG_DISPLAY_WIDTH;
    physical_screen->height = CFG_DISPLAY_HEIGHT;
    physical_screen->fb_size = fb_size;
    physical_screen->fill_dst_bits = CFG_DISPLAY_BPP;
    physical_screen->bits_per_pixel = CFG_DISPLAY_BPP;

    // in JB2.MP need to align width and height to 32 ,but jb5.mp needn't
    physical_screen->needAllign = 1;
    physical_screen->allignWidth = CFG_DISPLAY_ALIGN_WIDTH;

    /* In GB, no need to adjust 180 showing logo ,for fb driver dealing the change */
    /* but in JB, need adjust it for screen 180 roration           */
    physical_screen->need180Adjust = 0;   // need sync with chip driver

    LTRACEF("MTK_LCM_PHYSICAL_ROTATION = %s\n", MTK_LCM_PHYSICAL_ROTATION);

    if (strncmp(MTK_LCM_PHYSICAL_ROTATION, "270", 3) == 0)
        physical_screen->rotation = 270;
    else if (strncmp(MTK_LCM_PHYSICAL_ROTATION, "90", 2) == 0)
        physical_screen->rotation = 90;
    else if ((strncmp(MTK_LCM_PHYSICAL_ROTATION, "180", 3) == 0)
        && (physical_screen->need180Adjust == 1))
        physical_screen->rotation = 180;
    else
        physical_screen->rotation = 0;
}

/*
 * Show first boot logo when phone boot up
 */
void mt_disp_show_boot_logo(void)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_animation_logo(BOOT_LOGO_INDEX, mt_get_fb_addr(),
        logo_memory.dec_logo_base_addr_va, logo_memory.logo_base_addr_va, physical_screen);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

/*
 * Custom interface : charging state
 */
void mt_disp_enter_charging_state(void)
{
    LTRACEF("do nothing\n");
}

/*
 * Show full battery when poweroff charging
 */
void mt_disp_show_battery_full(void)
{
    LTRACE_ENTRY;
    mt_disp_show_battery_capacity(100);
}

/*
 * Show animation when poweroff charging
 */
void mt_disp_show_battery_capacity(unsigned int capacity)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_animation_battery_by_ver(capacity, mt_get_fb_addr(),
                                  logo_memory.dec_logo_base_addr_va,
                                  logo_memory.logo_base_addr_va,
                                  physical_screen, show_animation_ver);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

/*
 * Show charging over logo
 */
void mt_disp_show_charger_ov_logo(void)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_animation_logo(CHARGER_OV_INDEX, mt_get_fb_addr(),
        logo_memory.dec_logo_base_addr_va, logo_memory.logo_base_addr_va, physical_screen);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

/*
 * Show low battery logo
 */
void mt_disp_show_low_battery(void)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_animation_logo(LOW_BATTERY_INDEX, mt_get_fb_addr(),
        logo_memory.dec_logo_base_addr_va, logo_memory.logo_base_addr_va, physical_screen);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

void mt_disp_show_charging(int index)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_animation_logo(index + LOW_BATTERY01_INDEX, mt_get_fb_addr(),
        logo_memory.dec_logo_base_addr_va, logo_memory.logo_base_addr_va, physical_screen);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

void mt_disp_show_plug_charger(void)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_animation_logo(LOW_BATTERY_REMIND_INDEX, mt_get_fb_addr(),
        logo_memory.dec_logo_base_addr_va, logo_memory.logo_base_addr_va, physical_screen);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

/*
 * Fill rectangle region for with black  or other color
 */
static void mt_disp_fill_rect(unsigned int left, unsigned int top,
                       unsigned int right, unsigned int bottom,
                       unsigned int color)
{
    int err = 0;
    struct LCM_SCREEN_T physical_screen = {0};
    struct LOGO_MEMORY_T logo_memory = {0};

    struct RECT_REGION_T rect = {left, top, right, bottom};

    LTRACE_ENTRY;
    err = mt_lk_logo_memory_init(&logo_memory);
    if (err != NO_ERROR)
        return;

    init_fb_screen(&physical_screen);
    sync_anim_version(logo_memory.logo_base_addr_va);
    fill_rect_with_color(mt_get_fb_addr(), rect, color, physical_screen);
    mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    mt_lk_logo_memory_deinit(&logo_memory);
}

/*
 * Clear boot/battery/charging logo by filling the black or other color screen in display layer
 */
void mt_disp_clear_screen(unsigned int color)
{
    mt_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, color);
}
