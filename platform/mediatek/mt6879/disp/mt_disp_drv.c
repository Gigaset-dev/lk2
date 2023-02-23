/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <mt_disp_drv.h>
#include <mblock.h>
#include <platform/mboot_params.h>
#include <platform/memory_layout.h>
#include <platform/mrdump_params.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <set_fdt.h>

#include "ddp_manager.h"
#include "ddp_reg.h"
#include "disp_drv_log.h"
#include "disp_drv_platform.h"
#include "primary_display.h"

#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
#include <lcm_common_drv.h>
#else
#define MTK_LCM_NAME_LENGTH (128)
#endif

#define FB_LAYER            0
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
#define BOOT_MENU_LAYER     1
#define TOP_LAYER    2
#define BOTTOM_LAYER    3
#else
#define BOOT_MENU_LAYER     3
#endif

#ifndef CKSYS_BASE
#define CKSYS_BASE (0x10000000)
#endif

#ifndef CLK_CFG_1
#define CLK_CFG_1   ((UINT32P)(CKSYS_BASE+0x020))
#endif

#ifndef CLK_CFG_UPDATE
#define CLK_CFG_UPDATE   ((UINT32P)(CKSYS_BASE+0x004))
#endif

#ifndef CLK_CFG_1_SET
#define CLK_CFG_1_SET   ((UINT32P)(CKSYS_BASE+0x024))
#endif

#ifndef CLK_CFG_1_CLR
#define CLK_CFG_1_CLR   ((u32P)(CKSYS_BASE+0x028))
#endif

unsigned long long  fb_addr_pa_k;
static unsigned int fb_addr_pa;
static void  *fb_addr;
static u32 fb_size;
static u32 fb_offset_logo; // counter of fb_size
static u32 fb_isdirty;
static u32 redoffset_32bit = 1; // ABGR
static bool g_fb_init;
unsigned long long g_fb_base;
unsigned int g_fb_size;

LCM_PARAMS *lcm_params;

/* tag and struct need same as preloader's platform.h*/
#define BOOT_TAG_DISPLAY_INFO (0x88610029)
struct boot_tag_display_info {
    int islcmfound;
    char lcm_name[MTK_LCM_NAME_LENGTH];
};

struct boot_tag_display_info display_info;
/* bl2_ext update display_info to lk */
PL_BOOTTAGS_TO_BE_UPDATED(display, BOOT_TAG_DISPLAY_INFO, &display_info);

/* lk copy display_info from bl2_ext */
static void display_pl_boottag_hook(struct boot_tag *tag)
{
    struct boot_tag_display_info *boot_tag_temp = (struct boot_tag_display_info *) &tag->data;

    memcpy(&display_info, boot_tag_temp, sizeof(struct boot_tag_display_info));
}
PL_BOOTTAGS_INIT_HOOK(display_pl_boottag_hook, BOOT_TAG_DISPLAY_INFO, display_pl_boottag_hook);

u32 mt_disp_get_vram_size(void)
{
    return DISP_GetVRamSize();
}

static void _mtkfb_draw_block(u64 addr, unsigned int x, unsigned int y, unsigned int w,
                                unsigned int h, unsigned int color)
{
    unsigned int i = 0;
    unsigned int j = 0;
    void *start_addr = (void *)(addr+CFG_DISPLAY_ALIGN_WIDTH*4*y+x*4);
    unsigned int pitch = CFG_DISPLAY_ALIGN_WIDTH*4;
    unsigned int *line_addr = start_addr;

    for (j = 0; j < h; j++) {
        line_addr = start_addr;
        for (i = 0; i < w; i++)
            line_addr[i] = color;

        start_addr += pitch;
    }
}

static int _mtkfb_internal_test(u64 va, unsigned int w, unsigned int h)
{
    /* this is for debug, used in bring up day */
    unsigned int i = 0;
    unsigned int color = 0;
    int _internal_test_block_size = 120;

    for (i = 0; i < w * h / _internal_test_block_size / _internal_test_block_size; i++) {
        color = (i & 0x1) * 0xff;
        color += 0xff000000U;
        _mtkfb_draw_block(va,
                  i % (w / _internal_test_block_size) * _internal_test_block_size,
                  i / (w / _internal_test_block_size) * _internal_test_block_size,
                  _internal_test_block_size, _internal_test_block_size, color);
    }

    /* primary_display_trigger(1); */
    mt_disp_update(0, 0, 0, 0);

    return 0;
}

static int _mtkfb_internal_test2(void)
{
    /* this is for debug, used in bring up day */
    unsigned int i = 0;
    unsigned int color;
    unsigned int bar_num = 16;
    unsigned int bar_size;

    bar_size = fb_size / bar_num;

    for (i = 0; i < bar_num; i++) {
        color = i%2 ? 0x11 : 0x77;
        memset(fb_addr + i * bar_size, color, bar_size);
    }

    primary_display_trigger(1);

    return 0;
}

#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
void load_image(void *dst, void *src, int h, int picture_w, int w)
{
    int i = 0;

    for (i = 0; i < h; i++)
        memcpy(dst+picture_w*i*2, src+2*w*i, w*2);
}

void assemble_image(void *dst, void *left, void *right, int h, int picture_w, int w)
{
    int i = 0;

    for (i = 0; i < h; i++) {
        memcpy(dst+picture_w*i*2, left+2*w*i, w*2);
        memcpy(dst+(picture_w*(i+1)-w)*2, right+2*w*i, w*2);
    }
}

int round_corner_init(unsigned int *top_pa, unsigned int *bottom_pa, LCM_ROUND_CORNER *rc_params)
{
    unsigned int h = rc_params->h;
    unsigned int w = rc_params->w;
    unsigned int h_bot = rc_params->h_bot;
    unsigned int w_bot = rc_params->w_bot;
    unsigned int pitch = DISP_GetScreenWidth();
    unsigned char *left_top = rc_params->lt_addr;
    unsigned char *left_bottom = rc_params->lb_addr;
    unsigned char *right_top = rc_params->rt_addr;
    unsigned char *right_bottom = rc_params->rb_addr;
    unsigned int buf_size = 0;
    unsigned int buf_size_bot = 0;
    unsigned int vram_size = 0;

    static void *top_addr_va;
    static void *bottom_addr_va;

    vram_size = DISP_GetVRamSize();
    if (rc_params->full_content) {
        if (h == 0 || w == 0 || h_bot == 0 || w_bot == 0
            || left_top == NULL || left_bottom == NULL) {
            dprintf(CRITICAL, "the round corner params is invalid, please check the lcm config\n");
            return -1;
        }

        buf_size = h * pitch * 2;
        buf_size_bot = h_bot * pitch * 2;

        *bottom_pa = (unsigned int)(fb_addr_pa + vram_size - buf_size);
        bottom_addr_va = fb_addr + vram_size - buf_size;
        dprintf(INFO, "bottom_addr_va: 0x%08x, bottom_pa: 0x%08x\n",
                (unsigned int)bottom_addr_va, *bottom_pa);

        *top_pa = (unsigned int)(fb_addr_pa + vram_size - buf_size_bot - buf_size);
        top_addr_va = fb_addr + vram_size - buf_size_bot - buf_size;
        dprintf(INFO, "top_addr_va: 0x%08x, top_pa: 0x%08x\n", (unsigned int)top_addr_va, *top_pa);

        load_image((void *)top_addr_va, (void *)left_top, h, pitch, w);
        load_image((void *)bottom_addr_va, (void *)left_bottom, h_bot, pitch, w_bot);
    } else {
        if (h == 0 || w == 0 || left_top == NULL || left_bottom == NULL
            || right_top == NULL || right_bottom == NULL) {
            dprintf(CRITICAL, "the round corner params is invalid, please check the lcm config\n");
            return -1;
        }

        buf_size = h * pitch * 2;
        *top_pa = (unsigned int)(fb_addr_pa + vram_size - 2 * buf_size);
        top_addr_va = fb_addr + vram_size - 2 * buf_size;
        dprintf(INFO, "top_addr_va: 0x%08x, top_pa: 0x%08x\n", (unsigned int)top_addr_va, *top_pa);

        *bottom_pa = (unsigned int)(fb_addr_pa + vram_size - buf_size);
        bottom_addr_va = fb_addr + vram_size - buf_size;
        dprintf(INFO, "bottom_addr_va: 0x%08x, bottom_pa: 0x%08x\n",
                (unsigned int)bottom_addr_va, *bottom_pa);

        assemble_image((void *)top_addr_va, (void *)left_top, (void *)right_top, h, pitch, w);
        assemble_image((void *)bottom_addr_va, (void *)left_bottom, (void *)right_bottom, h,
                        pitch, w);
    }

    return 0;
}
#endif

// Attention: this api indicates whether the lcm is connected
int DISP_IsLcmFound(void)
{
    return primary_display_is_lcm_connected();
}

static void mt_disp_init(void *lcd_pa_base, void *lcd_va_base, void *fdt)
{
    u32 boot_mode_addr = 0;
    int ret = -1;

#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
    unsigned int top_addr_pa = 0;
    unsigned int bottom_addr_pa = 0;
    LCM_ROUND_CORNER *round_corner = primary_display_get_corner_params();

    if (round_corner == NULL)
        return;
#endif

#ifdef MTK_HIGH_FRAME_RATE_FOR_DLPT
    DISP_REG_SET(NULL, CLK_CFG_1_CLR, 0x000000FF);
    DISP_REG_SET(NULL, CLK_CFG_1_SET, 0x8);
    DISP_REG_SET(NULL, CLK_CFG_UPDATE, 0x10);
#endif
    fb_addr_pa_k = (unsigned long long)lcd_pa_base;

    fb_addr_pa = (unsigned int)(fb_addr_pa_k & 0xffffffffull);
    fb_addr      = lcd_va_base;
    if (mboot_params_current_abnormal_boot() > 0)
        fb_addr = lcd_va_base + DISP_GetVRamSize();

    dprintf(0, "fb_va: 0x%llx, fb_pa: 0x%08x, fb_pa_k: 0x%llx\n",
            (u64)fb_addr, fb_addr_pa, (unsigned long long)fb_addr_pa_k);
    fb_size = CFG_DISPLAY_ALIGN_WIDTH * CFG_DISPLAY_ALIGN_HEIGHT *
                CFG_DISPLAY_BPP / 8;

    boot_mode_addr = fb_addr_pa + fb_size;
    if (mboot_params_current_abnormal_boot() > 0)
        boot_mode_addr = fb_addr_pa + DISP_GetVRamSize();

    fb_offset_logo = 0;

    primary_display_init(fdt);

    display_info.islcmfound = DISP_IsLcmFound();
    ret = snprintf(display_info.lcm_name, MTK_LCM_NAME_LENGTH - 1,
        "%s", mt_disp_get_lcm_id());
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        dprintf(0, "%s, snprintf failed, %d\n", __func__, ret);
    display_info.lcm_name[MTK_LCM_NAME_LENGTH - 1] = '\0';
    dprintf(0, "%s %d, va=0x%lx, found:%d, lcm:\"%s\"\n",
            __func__, __LINE__, (unsigned long)lcd_va_base,
            display_info.islcmfound, display_info.lcm_name);

    if (mboot_params_current_abnormal_boot() > 0)
        memset(fb_addr, 0x0, DISP_GetMBRamSize());
    else
        memset(fb_addr, 0x0, DISP_GetVRamSize());

    struct disp_input_config input;

#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
    if (round_corner->round_corner_en) {
        if (round_corner != NULL)
            ret = round_corner_init(&top_addr_pa, &bottom_addr_pa, round_corner);

        if (ret == 0) {
            memset(&input, 0, sizeof(input));
            input.layer     = TOP_LAYER;
            input.layer_en  = 1;
            input.fmt       = eRGBA4444;
            input.addr      = top_addr_pa;
            input.src_x     = 0;
            input.src_y     = 0;
            input.src_w     = CFG_DISPLAY_WIDTH;
            input.src_h     = round_corner->h;
            input.src_pitch = CFG_DISPLAY_WIDTH*2;
            input.dst_x     = 0;
            input.dst_y     = 0;
            input.dst_w     = CFG_DISPLAY_WIDTH;
            input.dst_h     = round_corner->h;
            input.aen       = 1;
            input.alpha     = 0xff;

            primary_display_config_input(&input);

            memset(&input, 0, sizeof(input));
            input.layer     = BOTTOM_LAYER;
            input.layer_en  = 1;
            input.fmt       = eRGBA4444;
            input.addr      = bottom_addr_pa;
            input.src_x     = 0;
            input.src_y     = 0;
            input.src_w     = CFG_DISPLAY_WIDTH;
            input.src_h     = round_corner->h;
            input.src_pitch = CFG_DISPLAY_WIDTH*2;
            input.dst_x     = 0;
            input.dst_y     = CFG_DISPLAY_HEIGHT-round_corner->h;
            input.dst_w     = CFG_DISPLAY_WIDTH;
            input.dst_h     = round_corner->h;
            input.aen       = 1;
            input.alpha     = 0xff;

            if (round_corner->full_content) {
                input.src_h     = round_corner->h_bot;
                input.dst_y     = CFG_DISPLAY_HEIGHT-round_corner->h_bot;
                input.dst_h     = round_corner->h_bot;
            }
            primary_display_config_input(&input);
        }
    }
#endif
    memset(&input, 0, sizeof(struct disp_input_config));
    input.layer     = BOOT_MENU_LAYER;
    input.layer_en  = 1;
    input.fmt       = redoffset_32bit ? eBGRA8888 : eRGBA8888;
    input.addr      = boot_mode_addr;
    input.src_x     = 0;
    input.src_y     = 0;
    input.src_w     = CFG_DISPLAY_WIDTH;
    input.src_h     = CFG_DISPLAY_HEIGHT;
    input.src_pitch = CFG_DISPLAY_WIDTH*4;
    input.dst_x     = 0;
    input.dst_y     = 0;
    input.dst_w     = CFG_DISPLAY_WIDTH;
    input.dst_h     = CFG_DISPLAY_HEIGHT;
    input.aen       = 1;
    input.alpha     = 0xff;

    primary_display_config_input(&input);

    memset(&input, 0, sizeof(struct disp_input_config));
    input.layer     = FB_LAYER;
    if (mboot_params_current_abnormal_boot() > 0)
        input.layer_en  = 0;
    else
        input.layer_en  = 1;
    input.fmt       = redoffset_32bit ? eBGRA8888 : eRGBA8888;
    input.addr      = fb_addr_pa;
    input.src_x     = 0;
    input.src_y     = 0;
    input.src_w     = CFG_DISPLAY_WIDTH;
    input.src_h     = CFG_DISPLAY_HEIGHT;
    input.src_pitch = CFG_DISPLAY_ALIGN_WIDTH*4;
    input.dst_x     = 0;
    input.dst_y     = 0;
    input.dst_w     = CFG_DISPLAY_WIDTH;
    input.dst_h     = CFG_DISPLAY_HEIGHT;

    input.aen       = 1;
    input.alpha     = 0xff;
    primary_display_config_input(&input);

    dprintf(CRITICAL, "bl2_ext write parameter to lk\n");
    dprintf(CRITICAL, "islcmfound      = %d\n", display_info.islcmfound);
    dprintf(CRITICAL, "lcm_name        = %s\n", display_info.lcm_name);

#if 0
    /* debug for bringup */
    dprintf(CRITICAL, "display show background\n");
    mt_disp_update(0, 0, 0, 0);
    spin(100000);
    primary_display_diagnose();

    while (0) {
        primary_display_trigger(true);
        if (!primary_display_is_video_mode()) {
            /*video mode no need to wait*/
            dprintf(CRITICAL, "cmd mode trigger wait\n");
            spin(100000);
        }
        primary_display_diagnose();
    }

    dprintf(CRITICAL, "display internal test2\n");
    _mtkfb_internal_test((u64)fb_addr, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    spin(100000);
    primary_display_diagnose();
#endif

    DISPMSG("%s done\n", __func__);

}

void disp_init(void *fdt)
{
    status_t err;
    void *fb_base_va;
    const struct reserved_t *fb_mblock;
    int ret;
    u64 fb_addr = 0;
    u64 fd_size = 0;


#ifndef PROJECT_TYPE_FPGA
    dprintf(0, "%s: 0x%p\n", __func__, fdt);
#else
    dprintf(0, "[FPGA]%s: 0x%p\n", __func__, fdt);
#endif

    ret = primary_display_lcm_probe(NULL, fdt);
    if (ret) {
        dprintf(CRITICAL, "lcm not found\n");
        ASSERT(0);
    }

    /* initialize the frame buffet information */
    g_fb_size = DISP_GetVRamSize() + DISP_GetMBRamSize();
    if (mboot_params_current_abnormal_boot() > 0) {
        fb_mblock = mrdump_get_reserved_info("framebuffer", &fb_addr, &fd_size);
        if (!fb_addr) {
            dprintf(CRITICAL, "fb_mblock query failed\n");
            return;
        } else
            g_fb_base = fb_addr;
    } else {
        fb_addr = mblock_alloc(ROUNDUP(g_fb_size, PAGE_SIZE), PAGE_SIZE,
                            0xFFFFFFFF, 0x0, 1, "framebuffer");
        if (!fb_addr) {
            dprintf(CRITICAL, "mblock allocation failed\n");
            ASSERT(0);
        }
    }
    g_fb_base = fb_addr;
    g_fb_init = true;

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_framebuffer", ROUNDUP(g_fb_size, PAGE_SIZE),
            &fb_base_va, PAGE_SIZE_SHIFT,
            (paddr_t)g_fb_base, 0,
            ARCH_MMU_FLAG_CACHED);

    if (err != NO_ERROR) {
        dprintf(CRITICAL, "fastlogo vmm_alloc_physical failed\n");
        ASSERT(0);
    }

    dprintf(0, "%s, %d, fb_base:0x%llx, fb_va:0x%llx, size:%u\n",
        __func__, __LINE__, g_fb_base, (unsigned long long)fb_base_va, g_fb_size);
    mt_disp_init((void *)g_fb_base, (void *)(fb_base_va), fdt);
}

/**
 * if project show logo in the bl2_ext
 * we need init display path info for any user who want
 * trigger display (ex: mtk_cfb)
 * hw no need init again
 */
static void mt_disp_sw_init(void *lcd_pa_base, void *lcd_va_base, void *fdt)
{
    fb_addr_pa_k = (unsigned long long)lcd_pa_base;

    fb_addr_pa = (unsigned int)(fb_addr_pa_k & 0xffffffffull);
    fb_addr      = lcd_va_base;

    dprintf(0, "fb_va: 0x%llx, fb_pa: 0x%08x, fb_pa_k: 0x%llx\n",
            (u64)fb_addr, fb_addr_pa, (unsigned long long)fb_addr_pa_k);

    fb_size = CFG_DISPLAY_ALIGN_WIDTH * CFG_DISPLAY_ALIGN_HEIGHT *
                CFG_DISPLAY_BPP / 8;

    fb_offset_logo = 0;

    primary_display_sw_init(display_info.islcmfound, fdt, display_info.lcm_name);

    DISPMSG("%s done\n", __func__);
}

void disp_sw_init(void *fdt)
{
    status_t err;
    void *fb_base_va = NULL;
    const struct reserved_t *fb_mblock;
    int ret;

    /* lcm already compare from bl2_ext, just get driver by idx */
    if (!display_info.islcmfound)
        dprintf(CRITICAL, "no lcm info from bl2_ext\n");

    ret = primary_display_lcm_probe(display_info.lcm_name, fdt);
    if (ret) {
        dprintf(CRITICAL, "no lcm found by idx\n");
        ASSERT(0);
    }
    /* initialize the frame buffet information */
    g_fb_size = DISP_GetVRamSize();
    fb_mblock = mblock_query_reserved_by_name("framebuffer", 0);
    if (!fb_mblock) {
        dprintf(CRITICAL, "fb_mblock query failed\n");
        ASSERT(0);
    }
    g_fb_base = fb_mblock->start;
    g_fb_init = true;

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_framebuffer", ROUNDUP(g_fb_size, PAGE_SIZE),
            &fb_base_va, PAGE_SIZE_SHIFT,
            (paddr_t)g_fb_base, 0,
            ARCH_MMU_FLAG_CACHED);

    if (err != NO_ERROR) {
        dprintf(CRITICAL, "fastlogo vmm_alloc_physical failed\n");
        ASSERT(0);
    }
    mt_disp_sw_init((void *)g_fb_base, (void *)(fb_base_va), fdt);
}

void disp_el3_setup(void)
{
    int i;

    for (i = 0 ; i < 13 ; i++) {
        writel(0, SMI_LARB0_SEC_CON + i * 0x4);
        writel(0, SMI_LARB1_SEC_CON + i * 0x4);
    }

    /* Disable sec display control */
    writel(0xfffffffe, DISP_REG_MMSYS_SECURITY_DISABLE2);
}

void *mt_get_fb_addr(void)
{
    //fb_addr already judmp to mrdump addr in aee-lk
    fb_isdirty = 1;
    return (void *)(fb_addr + fb_offset_logo * fb_size);
}

void *mt_get_tempfb_addr(void)
{
    //use offset = 2 as tempfb for decompress logo
    dprintf(0, "%s: 0x%08x ,fb_addr 0x%08x\n",
            __func__, (unsigned int)(fb_addr + 2*fb_size), (unsigned int)fb_addr);
    return (void *)(fb_addr + 2*fb_size);
}

u32 mt_get_fb_size(void)
{
    //fb_addr already judmp to mrdump addr in aee-lk, no need add size
    if (mboot_params_current_abnormal_boot() > 0)
        return 0;

    return fb_size;
}

void mt_disp_update(u32 x, u32 y, u32 width, u32 height)
{
#ifndef MACH_FPGA_NO_DISPLAY
    addr_t va = (addr_t)fb_addr;

    if (!g_fb_init) {
        dprintf(CRITICAL, "disp has no framebuffer, skip update\n");
        return;
    }

    dprintf(CRITICAL, "fb dump: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
            *(unsigned int *)va, *(unsigned int *)(va+4), *(unsigned int *)(va+8),
            *(unsigned int *)(va+0xC));

    if (mboot_params_current_abnormal_boot() > 0)
        arch_clean_cache_range((addr_t)fb_addr, DISP_GetMBRamSize());
    else
        arch_clean_cache_range((addr_t)fb_addr, DISP_GetFBRamSize());

    primary_display_trigger(true);
#endif
}

static void mt_disp_adjusting_hardware_addr(void)
{
    dprintf(CRITICAL, "%s fb_offset_logo = %d fb_size=%d\n",
            __func__, fb_offset_logo, fb_size);
    if (fb_offset_logo == 0) {
        mt_get_fb_addr();
        memcpy(fb_addr, (void *)(fb_addr + 3 * fb_size), fb_size);
        mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    }
}

u32 mt_disp_get_lcd_time(void)
{
    static unsigned int fps;

    if (!fps) {
        fps = primary_display_get_vsync_interval();

        dprintf(CRITICAL, "%s, fps=%d\n", __func__, fps);

        if (!fps)
            fps = 6000;
    }

    return fps;
}

void mt_disp_config_frame_buffer(void *fdt)
{
    unsigned int fb_base_h = 0;
    unsigned int fb_base_l = g_fb_base & 0xffffffff;
    int offset;
    int err = 0;

    fb_base_h = cpu_to_fdt32(fb_base_h);
    fb_base_l = cpu_to_fdt32(fb_base_l);
    g_fb_size = cpu_to_fdt32(g_fb_size);
    /* placed in the DT chosen node */
    offset = fdt_path_offset(fdt, "/chosen");
    if (offset < 0) {
        DISPERR("[%s] fdt_path_offset /chosen failed\n", __func__);
    } else {
        err = fdt_setprop(fdt, offset, "atag,videolfb-fb_base_h", &fb_base_h, sizeof(fb_base_h));
        if (err != 0)
            DISPERR("[%s] fdt_setprop atag,videolfb-fb_base_h failed\n", __func__);
        err = fdt_setprop(fdt, offset, "atag,videolfb-fb_base_l", &fb_base_l, sizeof(fb_base_l));
        if (err != 0)
            DISPERR("[%s] fdt_setprop atag,videolfb-fb_base_l failed\n", __func__);
        err = fdt_setprop(fdt, offset, "atag,videolfb-vramSize", &g_fb_size, sizeof(g_fb_size));
        if (err != 0)
            DISPERR("[%s] fdt_setprop atag,videolfb-vramSize failed\n", __func__);
    }
}
SET_FDT_INIT_HOOK(mt_disp_config_frame_buffer, mt_disp_config_frame_buffer);

const char *mt_disp_get_lcm_id(void)
{
    return primary_display_get_lcm_name();
}

u32 mt_disp_get_width(void)
{
    return primary_display_get_width();
}

u32 mt_disp_get_align_width(void)
{
    return primary_display_get_align_width();
}

u32 mt_disp_get_height(void)
{
    if (mboot_params_current_abnormal_boot() > 0)
        return primary_display_get_height() / 8;
    return primary_display_get_height();
}

u32 mt_disp_get_align_height(void)
{
    return primary_display_get_align_height();
}

int mt_disp_get_bpp(void)
{
    return primary_display_get_bpp();
}

void disp_get_fb_address(u32 *fbVirAddr, u32 *fbPhysAddr)
{
    *fbVirAddr = (u32)fb_addr;
    *fbPhysAddr = (u32)fb_addr;
}

u32 mt_disp_get_redoffset_32bit(void)
{
    return redoffset_32bit;
}

bool mt_disp_get_fb_init_status(void)
{
    return g_fb_init;
}

void target_atag_videolfb(void *fdt)
{
    char ptr[512];
    int offset;
    int err;
    const char   *lcmname = mt_disp_get_lcm_id();
    unsigned int *p       = NULL;
    unsigned long long *phy_p   = (unsigned long long *)ptr;

    *phy_p = fb_addr_pa_k;
    p = (unsigned int *)(phy_p + 1);
    *p++ = display_info.islcmfound;
    *p++ = mt_disp_get_lcd_time();
    *p++ = DISP_GetVRamSize();
    if (lcmname != NULL) {
        strncpy((char *)p, lcmname, strlen(lcmname) + 1);
        p += (strlen(lcmname) + 1 + 4) >> 2;
    } else {
        DISPERR("[%s] lcmname is NULL\n", __func__);
    }

    dprintf(CRITICAL, "videolfb - fb_base    = 0x%llx\n", fb_addr_pa_k);
    dprintf(CRITICAL, "videolfb - islcmfound = %d\n", display_info.islcmfound);
    dprintf(CRITICAL, "videolfb - fps        = %d\n", mt_disp_get_lcd_time());
    dprintf(CRITICAL, "videolfb - vram       = %d\n", DISP_GetVRamSize());
    dprintf(CRITICAL, "videolfb - lcmname    = %s\n", lcmname);

    offset = fdt_path_offset(fdt, "/chosen");
    if (offset < 0) {
        DISPERR("[%s] fdt_path_offset /chosen failed\n", __func__);
    } else {
        err = fdt_setprop(fdt, offset, "atag,videolfb", ptr, (void *)p - (void *)ptr);
        if (err != 0)
            DISPERR("[%s] fdt_setprop atag,videolfb failed\n", __func__);
    }
}

SET_FDT_INIT_HOOK(target_atag_videolfb, target_atag_videolfb);

void disp_lcm_fill_lcm_dts_phandle(void *fdt)
{
    int dts_is_cphy = primary_display_get_is_cphy();
    int err;

#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
    int phy_type = MTK_LCM_MIPI_DPHY;

    if (dts_is_cphy)
        phy_type = MTK_LCM_MIPI_CPHY;

    mtk_lcm_drv_update_panel_for_kernel(MTK_LCM_FUNC_DSI,
                display_info.lcm_name, phy_type, fdt);

    return;
#else
    int nodeoffset, data, phandle, nodeoffset1, need_prefix;
    int idx;
    const void *val, *val1;
    char buf[100];
    const char *phy_type = "mediatek,mt6879-mipi-tx-cphy";

    snprintf(buf, sizeof(buf) - 1, "/soc/dsi");
    nodeoffset = fdt_path_offset(fdt, buf);
    if (nodeoffset < 0)
        need_prefix = 0;
    else
        need_prefix = 1;

    /* get /dsi/panel%d/... node*/
    idx = primary_display_get_panel_index();

    if (need_prefix)
        snprintf(buf, sizeof(buf) - 1, "/soc/dsi/panel%d/port/endpoint", idx);
    else
        snprintf(buf, sizeof(buf) - 1, "/dsi/panel%d/port/endpoint", idx);
    nodeoffset = fdt_path_offset(fdt, buf);
    if (nodeoffset < 0) {
        DISPERR("[%s] get panel node:%s failed\n", __func__, buf);
        goto error;
    }

    phandle = fdt_get_phandle(fdt, nodeoffset);
    DISPMSG("dsi panel target phandle:0x%x of\"%s\"\n", phandle, buf);

    /* get /dsi/ports/... node */
    if (need_prefix)
        nodeoffset = fdt_path_offset(fdt, "/soc/dsi/ports/port/endpoint");
    else
        nodeoffset = fdt_path_offset(fdt, "/dsi/ports/port/endpoint");

    if (nodeoffset < 0) {
        DISPERR("%s get dsi port node failed\n", __func__);
        goto error;
    }

    val = fdt_getprop(fdt, nodeoffset, "remote-endpoint", NULL);
    if (val) {
        memcpy(&data, val, sizeof(data));
        data = fdt32_to_cpu(data);
        DISPMSG("default dsi remote-endport = <0x%x> before lk set\n", data);
    } else {
        DISPERR("default dsi remote-endport in dtb is empty\n");
    }

    phandle = cpu_to_fdt32(phandle);
    err = fdt_setprop(fdt, nodeoffset, "remote-endpoint", &phandle, sizeof(phandle));
    if (err != 0)
        DISPERR("[%s] fdt_setprop remote-endpoint failed\n", __func__);

    val = fdt_getprop(fdt, nodeoffset, "remote-endpoint", NULL);
    if (val) {
        memcpy(&data, val, sizeof(data));
        data = fdt32_to_cpu(data);
        DISPMSG("update dsi remote-endport = <0x%x> after lk set\n", data);
    } else {
        DISPERR("update dsi remote-endport failed when lk set\n");
    }

error:
    dprintf(0, "set dsi panel index to dtb failed.\n");

    if (dts_is_cphy) {
        if (need_prefix)
            nodeoffset1 = fdt_path_offset(fdt, "/soc/mipi_tx_config");
        else
            nodeoffset1 = fdt_path_offset(fdt, "/mipi_tx_config");

        if (nodeoffset1 < 0) {
            DISPERR("[%s] get dsi mipi tx node failed\n", __func__);
            goto error2;
        }
        val1 = fdt_getprop(fdt, nodeoffset1, "compatible", NULL);
        if (val1)
            DISPMSG("set dsi mipi tx data as %s before\n", (char *)val1);
        else
            DISPERR("dsi mipi tx data is empty before set\n");
        err = fdt_setprop_string(fdt, nodeoffset1, "compatible", phy_type);
        if (err != 0)
            DISPERR("[%s] fdt_setprop_string compatible failed\n", __func__);
        val1 = fdt_getprop(fdt, nodeoffset1, "compatible", NULL);
        if (val1)
            DISPMSG("set dsi mipi tx data as %s done\n", (char *)val1);
        else
            DISPERR("dsi mipi tx data is empty after set\n");
    }

    return;

error2:
    dprintf(0, "set mipi tx to dtb failed.\n");
#endif
}
SET_FDT_INIT_HOOK(disp_lcm_fill_lcm_dts_phandle, disp_lcm_fill_lcm_dts_phandle);

