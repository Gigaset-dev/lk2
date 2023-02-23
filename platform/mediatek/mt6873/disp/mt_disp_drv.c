/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <err.h>
#include <debug.h>
#include <libfdt.h>
#include "disp_drv_platform.h"
#include <lib/pl_boottags.h>
#include "primary_display.h"
#include "disp_drv_log.h"
#include "mt_disp_drv.h"
#include <mblock.h>
#include <kernel/vm.h>
#include <set_fdt.h>
#include <platform/memory_layout.h>

//#define DFO_DISP
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

#define DISP_REG_SET(reg, value) \
        mt65xx_reg_sync_writel(value, reg)

#define DISP_REG_GET(reg) (readl(reg))

unsigned long long  fb_addr_pa_k;
static unsigned int fb_addr_pa;
static void  *fb_addr;
static u32 fb_size;
static u32 fb_offset_logo; // counter of fb_size
static u32 fb_isdirty;
static u32 redoffset_32bit = 1; // ABGR
unsigned long long g_fb_base;
unsigned int g_fb_size;

LCM_PARAMS *lcm_params;

/* tag and struct need same as preloader's platform.h*/
#define BOOT_TAG_DISPLAY_INFO (0x88610029)
struct boot_tag_display_info {
    int islcmfound;
};

struct boot_tag_display_info display_info;
/* bl2_ext update display_info to lk */
PL_BOOTTAGS_TO_BE_UPDATED(display, BOOT_TAG_DISPLAY_INFO, &display_info);

/* lk copy display_info from bl2_ext */
static void display_pl_boottag_hook(struct boot_tag *tag)
{
    memcpy(&display_info, &tag->data, sizeof(struct boot_tag_display_info));
}
PL_BOOTTAGS_INIT_HOOK(display_pl_boottag_hook, BOOT_TAG_DISPLAY_INFO, display_pl_boottag_hook);

u32 mt_disp_get_vram_size(void)
{
    return DISP_GetVRamSize();
}


#ifdef DFO_DISP
static disp_dfo_item_t disp_dfo_setting[] = {
    {"LCM_FAKE_WIDTH",  0},
    {"LCM_FAKE_HEIGHT", 0},
    {"DISP_DEBUG_SWITCH",   0}
};

#define MT_DISP_DFO_DEBUG
#ifdef MT_DISP_DFO_DEBUG
#define disp_dfo_printf(string, args...) dprintf(INFO, "[DISP_DFO]"string, ##args)
#else
#define disp_dfo_printf(string, args...) ()
#endif

unsigned int mt_disp_parse_dfo_setting(void)
{
    unsigned int i, j = 0;
    char tmp[11];
    char *buffer = NULL;
    char *ptr = NULL;

    buffer = (char *)get_env("DFO");
    disp_dfo_printf("env buffer = %s\n", buffer);

    if (buffer != NULL) {
        for (i = 0; i < (sizeof(disp_dfo_setting)/sizeof(disp_dfo_item_t)); i++) {
            j = 0;

            memset((void *)tmp, 0, ARRAY_SIZE(tmp));

            ptr = strstr(buffer, disp_dfo_setting[i].name);

            if (ptr == NULL)
                continue;

            disp_dfo_printf("disp_dfo_setting[%d].name = [%s]\n", i, ptr);

            do {} while ((*ptr++) != ',');

            do {
                tmp[j++] = *ptr++;
            } while (*ptr != ',' && j < ARRAY_SIZE(tmp));

            disp_dfo_setting[i].value = atoi((const char *)tmp);

            disp_dfo_printf("disp_dfo_setting[%d].name = [%s|%d]\n",
                            i, tmp, disp_dfo_setting[i].value);
        }
    } else {
        disp_dfo_printf("env buffer = NULL\n");
    }

    return 0;
}


int mt_disp_get_dfo_setting(const char *string, unsigned int *value)
{
    char *disp_name;
    int  disp_value;
    unsigned int i = 0;

    if (string == NULL)
        return -1;

    for (i = 0; i < (sizeof(disp_dfo_setting)/sizeof(disp_dfo_item_t)); i++) {
        disp_name = disp_dfo_setting[i].name;
        disp_value = disp_dfo_setting[i].value;
        if (!strcmp(disp_name, string)) {
            *value = disp_value;
            disp_dfo_printf("%s = [DEC]%d [HEX]0x%08x\n", disp_name, disp_value, disp_value);
            return 0;
        }
    }

    return 0;
}
#endif

static void _mtkfb_draw_block(unsigned int addr, unsigned int x, unsigned int y, unsigned int w,
                                unsigned int h, unsigned int color)
{
    unsigned int i = 0;
    unsigned int j = 0;
    void *start_addr = (void *)(addr+ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4*y+x*4);
    unsigned int pitch = ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4;
    unsigned int *line_addr = start_addr;

    for (j = 0; j < h; j++) {
        line_addr = start_addr;
        for (i = 0; i < w; i++)
            line_addr[i] = color;

        start_addr += pitch;
    }
}

static int _mtkfb_internal_test(unsigned int va, unsigned int w, unsigned int h)
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
        color = i%2 ? 0 : 0xff;
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

    static void *top_addr_va;
    static void *bottom_addr_va;

    if (rc_params->full_content) {
        if (h == 0 || w == 0 || h_bot == 0 || w_bot == 0
            || left_top == NULL || left_bottom == NULL) {
            dprintf(CRITICAL, "the round corner params is invalid, please check the lcm config\n");
            return -1;
        }

        buf_size = h * pitch * 2;
        buf_size_bot = h_bot * pitch * 2;

        *bottom_pa = (unsigned int)(fb_addr_pa + DISP_GetVRamSize() - buf_size);
        bottom_addr_va = fb_addr + DISP_GetVRamSize() - buf_size;
        dprintf(INFO, "bottom_addr_va: 0x%08x, bottom_pa: 0x%08x\n",
                (unsigned int)bottom_addr_va, *bottom_pa);

        *top_pa = (unsigned int)(fb_addr_pa + DISP_GetVRamSize() - buf_size_bot - buf_size);
        top_addr_va = fb_addr + DISP_GetVRamSize() - buf_size_bot - buf_size;
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
        *top_pa = (unsigned int)(fb_addr_pa + DISP_GetVRamSize() - 2 * buf_size);
        top_addr_va = fb_addr + DISP_GetVRamSize() - 2 * buf_size;
        dprintf(INFO, "top_addr_va: 0x%08x, top_pa: 0x%08x\n", (unsigned int)top_addr_va, *top_pa);

        *bottom_pa = (unsigned int)(fb_addr_pa + DISP_GetVRamSize() - buf_size);
        bottom_addr_va = fb_addr + DISP_GetVRamSize() - buf_size;
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
#ifdef DFO_DISP
    unsigned int lcm_fake_width = 0;
    unsigned int lcm_fake_height = 0;
#endif
    u32 boot_mode_addr = 0;

#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
    int ret = -1;
    unsigned int top_addr_pa = 0;
    unsigned int bottom_addr_pa = 0;
    LCM_ROUND_CORNER *round_corner = primary_display_get_corner_params();

    if (round_corner == NULL)
        return;
#endif
#ifdef MTK_HIGH_FRAME_RATE_FOR_DLPT
    DISP_REG_SET(CLK_CFG_1_CLR, 0x000000FF);
    DISP_REG_SET(CLK_CFG_1_SET, 0x8);
    DISP_REG_SET(CLK_CFG_UPDATE, 0x10);
#endif
    fb_addr_pa_k = (unsigned int)lcd_pa_base;

    fb_addr_pa = (unsigned int)(fb_addr_pa_k & 0xffffffffull);
    fb_addr      = lcd_va_base;

    dprintf(0, "fb_va: 0x%08x, fb_pa: 0x%08x, fb_pa_k: 0x%llx\n",
            (unsigned int)fb_addr, fb_addr_pa, (unsigned long long int)fb_addr_pa_k);

    fb_size = ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT) *
                ALIGN_TO(CFG_DISPLAY_HEIGHT, MTK_FB_ALIGNMENT) * CFG_DISPLAY_BPP / 8;

    boot_mode_addr = (fb_addr_pa + fb_size);

    fb_offset_logo = 0;

    primary_display_init(NULL, fdt);
    display_info.islcmfound = DISP_IsLcmFound();
    memset((void *)lcd_va_base, 0x0, DISP_GetVRamSize());

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
    input.layer_en  = 1;
    input.fmt       = redoffset_32bit ? eBGRA8888 : eRGBA8888;
    input.addr      = fb_addr_pa;
    input.src_x     = 0;
    input.src_y     = 0;
    input.src_w     = CFG_DISPLAY_WIDTH;
    input.src_h     = CFG_DISPLAY_HEIGHT;
    input.src_pitch = ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4;
    input.dst_x     = 0;
    input.dst_y     = 0;
    input.dst_w     = CFG_DISPLAY_WIDTH;
    input.dst_h     = CFG_DISPLAY_HEIGHT;

    input.aen       = 1;
    input.alpha     = 0xff;
    primary_display_config_input(&input);

#ifdef PROJECT_K6873V1_64_BL2_EXT
    mt_disp_update(0, 0, 0, 0);
    dprintf(CRITICAL, "bl2_ext write parameter to lk\n");
    dprintf(CRITICAL, "islcmfound      = %d\n", display_info.islcmfound);
#endif

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

    dprintf(CRITICAL, "display internal test\n");
    _mtkfb_internal_test(fb_addr, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
    spin(100000);
    primary_display_diagnose();

#endif

#ifdef DFO_DISP
    mt_disp_parse_dfo_setting();

    if ((mt_disp_get_dfo_setting("LCM_FAKE_WIDTH", &lcm_fake_width) == 0) &&
        (mt_disp_get_dfo_setting("LCM_FAKE_HEIGHT", &lcm_fake_height) == 0)) {
        if (DISP_Change_LCM_Resolution(lcm_fake_width, lcm_fake_height) != DISP_STATUS_OK)
            dprintf(INFO, "[DISP_DFO]WARNING!!! Change LCM Resolution FAILED!!!\n");
    }
#endif
    DISPMSG("%s done\n", __func__);

}

void disp_init(void *fdt)
{
    status_t err;
    void *fb_base_va;

    /* initialize the frame buffet information */
    g_fb_size = mt_disp_get_vram_size();
    g_fb_base = mblock_alloc(ROUNDUP(g_fb_size, PAGE_SIZE), PAGE_SIZE,
                            MBLOCK_NO_LIMIT, 0x80000000, 1, "framebuffer");
    if (!g_fb_base) {
        dprintf(CRITICAL, "mblock allocation failed\n");
        ASSERT(0);
    }

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_framebuffer", ROUNDUP(g_fb_size, PAGE_SIZE),
            &fb_base_va, PAGE_SIZE_SHIFT,
            (paddr_t)g_fb_base, 0,
            ARCH_MMU_FLAG_CACHED);

    if (err != NO_ERROR) {
        dprintf(CRITICAL, "fastlogo vmm_alloc_physical failed\n");
        ASSERT(0);
    }

    mt_disp_init((void *)g_fb_base, (void *)(fb_base_va), fdt);
}

/**
 * if project show logo in the bl2_ext
 * we need init display path info for any user who want
 * trigger display (ex: mtk_cfb)
 * hw no need init again
 */
static void mt_disp_sw_init(void *lcd_pa_base, void *lcd_va_base)
{
    fb_addr_pa_k = (unsigned int)lcd_pa_base;

    fb_addr_pa = (unsigned int)(fb_addr_pa_k & 0xffffffffull);
    fb_addr      = lcd_va_base;

    dprintf(0, "fb_va: 0x%08x, fb_pa: 0x%08x, fb_pa_k: 0x%llx\n",
            (unsigned int)fb_addr, fb_addr_pa, (unsigned long long int)fb_addr_pa_k);

    fb_size = ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT) *
                ALIGN_TO(CFG_DISPLAY_HEIGHT, MTK_FB_ALIGNMENT) * CFG_DISPLAY_BPP / 8;

    fb_offset_logo = 0;

    primary_display_sw_init(display_info.islcmfound);

    DISPMSG("%s done\n", __func__);
}

void disp_sw_init(void)
{
    status_t err;
    void *fb_base_va;
    struct reserved_t *fb_mblock;

    /* initialize the frame buffet information */
    g_fb_size = mt_disp_get_vram_size();
    fb_mblock = mblock_query_reserved_by_name("framebuffer", 0);
    if (!fb_mblock) {
        dprintf(CRITICAL, "fb_mblock query failed\n");
        ASSERT(0);
    } else
        g_fb_base = fb_mblock->start;

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_framebuffer", ROUNDUP(g_fb_size, PAGE_SIZE),
            &fb_base_va, PAGE_SIZE_SHIFT,
            (paddr_t)g_fb_base, 0,
            ARCH_MMU_FLAG_CACHED);

    if (err != NO_ERROR) {
        dprintf(CRITICAL, "fastlogo vmm_alloc_physical failed\n");
        ASSERT(0);
    }
    mt_disp_sw_init((void *)g_fb_base, (void *)(fb_base_va));
}

void *mt_get_fb_addr(void)
{
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
    return fb_size;
}


void __mt_disp_update(u32 x, u32 y, u32 width, u32 height, bool logo)
{
#ifndef MACH_FPGA_NO_DISPLAY
    addr_t va = (addr_t)fb_addr;

    dprintf(CRITICAL, "logo:%d fb dump: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n", logo,
            *(unsigned int *)va, *(unsigned int *)(va+4), *(unsigned int *)(va+8),
            *(unsigned int *)(va+0xC));

    arch_clean_cache_range((addr_t)fb_addr, DISP_GetFBRamSize());
    __primary_display_trigger(true, logo);
#endif
}

void mt_disp_update(u32 x, u32 y, u32 width, u32 height)
{
    __mt_disp_update(x, y, width, height, true);
}

void mt_disp_update_no_logo(u32 x, u32 y, u32 width, u32 height)
{
    __mt_disp_update(x, y, width, height, false);
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

    fb_base_h = cpu_to_fdt32(fb_base_h);
    fb_base_l = cpu_to_fdt32(fb_base_l);
    g_fb_size = cpu_to_fdt32(g_fb_size);
    /* placed in the DT chosen node */
    offset = fdt_path_offset(fdt, "/chosen");
    fdt_setprop(fdt, offset, "atag,videolfb-fb_base_h", &fb_base_h, sizeof(fb_base_h));
    fdt_setprop(fdt, offset, "atag,videolfb-fb_base_l", &fb_base_l, sizeof(fb_base_l));
    fdt_setprop(fdt, offset, "atag,videolfb-vramSize", &g_fb_size, sizeof(g_fb_size));
}
SET_FDT_INIT_HOOK(mt_disp_config_frame_buffer, mt_disp_config_frame_buffer);

const char *mt_disp_get_lcm_id(void)
{
    return primary_display_get_lcm_name();
}

void disp_get_fb_address(u32 *fbVirAddr, u32 *fbPhysAddr)
{
    *fbVirAddr = fb_addr;
    *fbPhysAddr = fb_addr;
}

u32 mt_disp_get_redoffset_32bit(void)
{
    return redoffset_32bit;
}

void target_atag_videolfb(void *fdt)
{
    char ptr[512];
    int offset;
    const char   *lcmname = mt_disp_get_lcm_id();
    unsigned int *p       = NULL;
    unsigned long long *phy_p   = (unsigned long long *)ptr;

    *phy_p = fb_addr_pa_k;
    p = (unsigned int *)(phy_p + 1);
    *p++ = display_info.islcmfound;
    *p++ = mt_disp_get_lcd_time();
    *p++ = DISP_GetVRamSize();
    strncpy((char *)p, lcmname, strlen(lcmname) + 1);
    p += (strlen(lcmname) + 1 + 4) >> 2;

    dprintf(CRITICAL, "videolfb - fb_base    = 0x%llx\n", fb_addr_pa_k);
    dprintf(CRITICAL, "videolfb - islcmfound = %d\n", display_info.islcmfound);
    dprintf(CRITICAL, "videolfb - fps        = %d\n", mt_disp_get_lcd_time());
    dprintf(CRITICAL, "videolfb - vram       = %d\n", DISP_GetVRamSize());
    dprintf(CRITICAL, "videolfb - lcmname    = %s\n", lcmname);

    offset = fdt_path_offset(fdt, "/chosen");
    fdt_setprop(fdt, offset, "atag,videolfb", ptr, (void *)p - (void *)ptr);
}
SET_FDT_INIT_HOOK(target_atag_videolfb, target_atag_videolfb);

