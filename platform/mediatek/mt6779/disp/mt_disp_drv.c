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

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <err.h>
#include <debug.h>
/* #include <video_fb.h> */
#include <libfdt.h>
#include "disp_drv_platform.h"
#include "mt_disp_drv.h"
/* #include <env.h> */
#include "lcm_drv.h"
/* #include <platform/mt_gpt.h> */
#include "primary_display.h"
#include <arch/arm/mmu.h>
#include "disp_drv_log.h"
#include <mblock.h>
#include <kernel/vm.h>
#include <set_fdt.h>
#include <platform/memory_layout.h>
/* #include <platform/boot_mode.h> */
/* #include <pal_log.h> */

//#define DFO_DISP
#define FB_LAYER            0
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
# define BOOT_MENU_LAYER     1
# define TOP_LAYER  2
# define BOTTOM_LAYER   3
#else
# define BOOT_MENU_LAYER     3
#endif

unsigned long long  fb_addr_pa_k;
static void  *fb_addr_pa;
static void  *fb_addr;
static void  *logo_db_addr;
static void  *logo_db_addr_pa;
static u32 fb_size;
static u32 fb_offset_logo; // counter of fb_size
static u32 fb_isdirty;
static u32 redoffset_32bit = 1; // ABGR
unsigned int g_fb_base;
unsigned int g_fb_size;

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
unsigned long long  ext_fb_addr_pa_k;
static void  *ext_fb_addr_pa;
static void  *ext_fb_addr;
static void  *ext_logo_db_addr;
static void  *ext_logo_db_addr_pa;
static u32 ext_fb_size;
static u32 ext_fb_offset_logo; // counter of fb_size
static u32 ext_fb_isdirty;
#endif

extern LCM_PARAMS *lcm_params;

extern void disp_log_enable(int enable);
extern void dbi_log_enable(int enable);
extern void *memset(void *, int, unsigned int);
extern void arch_clean_cache_range(addr_t start, size_t len);
extern u32 memory_size(void);

u32 mt_disp_get_vram_size(void)
{
    return DISP_GetVRamSize();
}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
u32 mt_disp_get_ext_vram_size(void)
{
    return EXT_DISP_GetVRamSize();
}
#endif

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

            memset((void *)tmp, 0, sizeof(tmp)/sizeof(tmp[0]));

            ptr = strstr(buffer, disp_dfo_setting[i].name);

            if (ptr == NULL)
                continue;

            disp_dfo_printf("disp_dfo_setting[%d].name = [%s]\n", i, ptr);

            do {
                ;
            } while ((*ptr++) != ',');

            do {
                tmp[j++] = *ptr++;
            } while (*ptr != ',' && j < sizeof(tmp)/sizeof(tmp[0]));

            disp_dfo_setting[i].value = atoi((const char *)tmp);

            disp_dfo_printf("disp_dfo_setting[%d].name = [%s|%d]\n", i, tmp, disp_dfo_setting[i].value);
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

static void _mtkfb_draw_block(unsigned int addr, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color)
{
    unsigned int i = 0;
    unsigned int j = 0;
    void *start_addr = (void *)(addr + ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4*y + x*4);
    unsigned int pitch = ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4;
    unsigned int *line_addr = start_addr;

    for (j = 0; j < h; j++) {
        line_addr = start_addr;
        for (i = 0; i < w; i++) {
            line_addr[i] = color;
        }
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

    primary_display_trigger(1);

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

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
static void _ext_mtkfb_draw_block(unsigned int addr, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color)
{
    int i = 0;
    int j = 0;
    void *start_addr = addr + ALIGN_TO(CFG_EXT_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4*y + x*4;
    unsigned int pitch = ALIGN_TO(CFG_EXT_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4;
    unsigned int *line_addr = start_addr;

    for (j = 0; j < h; j++) {
        line_addr = start_addr;
        for (i = 0; i < w; i++) {
            line_addr[i] = color;
        }
        start_addr += pitch;
    }
}

static int _ext_mtkfb_internal_test(unsigned int va, unsigned int w, unsigned int h)
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

    arch_clean_cache_range((unsigned int)ext_fb_addr, EXT_DISP_GetFBRamSize());
    external_display_trigger(1);

    return 0;
}

static int _ext_mtkfb_internal_test2(void)
{
    /* this is for debug, used in bring up day */
    unsigned int i = 0;
    unsigned int color;
    int bar_num = 16;
    unsigned int bar_size;

    bar_size = ext_fb_size / bar_num;

    for (i = 0; i < bar_num; i++) {
        color = i%2 ? 0 : 0xff;
        memset(ext_fb_addr + i * bar_size, color, bar_size);
    }
    arch_clean_cache_range((unsigned int)ext_fb_addr, EXT_DISP_GetFBRamSize());
    external_display_trigger(1);

    return 0;
}
#endif
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
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
    unsigned int pitch = DISP_GetScreenWidth();
    unsigned char *left_top = rc_params->lt_addr;
    unsigned char *left_bottom = rc_params->lb_addr;
    unsigned char *right_top = rc_params->rt_addr;
    unsigned char *right_bottom = rc_params->rb_addr;
    unsigned int buf_size = 0;

    static void *top_addr_va;
    static void *bottom_addr_va;

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
    dprintf(INFO, "bottom_addr_va: 0x%08x, bottom_pa: 0x%08x\n", (unsigned int)bottom_addr_va, *bottom_pa);

    assemble_image((void *)top_addr_va, (void *)left_top, (void *)right_top, h, pitch, w);
    assemble_image((void *)bottom_addr_va, (void *)left_bottom, (void *)right_bottom, h, pitch, w);

    return 0;
}
#endif

static void mt_disp_init(void *lcd_pa_base, void *lcd_va_base, void *fdt)
{
    u32 boot_mode_addr = 0;

#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
    int ret = -1;
    unsigned int top_addr_pa = 0;
    unsigned int bottom_addr_pa = 0;
    LCM_ROUND_CORNER *round_corner = primary_display_get_corner_params();
#endif
    /// fb base pa and va
    /* fb_addr_pa_k = arm_mmu_va2pa((unsigned int)lcd_pa_base); */
    fb_addr_pa_k = (unsigned int)lcd_pa_base;

    fb_addr_pa   = (void *)(unsigned int)(fb_addr_pa_k & 0xffffffffull);
    fb_addr      = lcd_va_base;

    dprintf(0, "fb_va: 0x%08x, fb_pa: 0x%08x, fb_pa_k: 0x%llx\n", (unsigned int)fb_addr, (unsigned int)fb_addr_pa, fb_addr_pa_k);

    fb_size = ALIGN_TO(CFG_DISPLAY_WIDTH, MTK_FB_ALIGNMENT) * ALIGN_TO(CFG_DISPLAY_HEIGHT, MTK_FB_ALIGNMENT) * CFG_DISPLAY_BPP / 8;
    // pa;
    boot_mode_addr = ((u32)fb_addr_pa + fb_size);

#if defined(MBLOCK_SUPPORT)
    logo_db_addr_pa = (void *)(u32)mblock_reserve_ext(
                      LK_LOGO_MAX_SIZE, PAGE_SIZE, 0x80000000, 0, "logo_db_addr_pa");

    if (!logo_db_addr_pa) {
        dprintf(CRITICAL, "Warning! logo_db_addr_pa is not taken from mb\n");
        assert(0);
    }

    // va;
    logo_db_addr = logo_db_addr_pa;
#endif

    fb_offset_logo = 0;

    primary_display_init(NULL, fdt);
    memset((void *)lcd_va_base, 0x0, DISP_GetVRamSize());

    disp_input_config input;

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

            primary_display_config_input(&input);
        }
    }
#endif
    memset(&input, 0, sizeof(disp_input_config));
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

    memset(&input, 0, sizeof(disp_input_config));
    input.layer     = FB_LAYER;
    input.layer_en  = 1;
    input.fmt       = redoffset_32bit ? eBGRA8888 : eRGBA8888;
    input.addr      = (unsigned int)fb_addr_pa;
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

#if 0
    /* debug for bringup */
    dprintf(CRITICAL, "display show background\n");
    primary_display_trigger(true);
    spin(100000);
    primary_display_diagnose();
    /*
        while(1) {
            primary_display_trigger(true);
            if (!primary_display_is_video_mode()) {
                dprintf(CRITICAL, "cmd mode trigger wait\n");
                spin(100000);
            }
            primary_display_diagnose();
        }
    */
    dprintf(CRITICAL, "display internal test\n");
    _mtkfb_internal_test2();
    spin(100000);
    primary_display_diagnose();
#endif

#ifdef DFO_DISP
    unsigned int lcm_fake_width = 0;
    unsigned int lcm_fake_height = 0;

    mt_disp_parse_dfo_setting();

    if ((0 == mt_disp_get_dfo_setting("LCM_FAKE_WIDTH", &lcm_fake_width)) && (0 == mt_disp_get_dfo_setting("LCM_FAKE_HEIGHT", &lcm_fake_height))) {
        if (DISP_STATUS_OK != DISP_Change_LCM_Resolution(lcm_fake_width, lcm_fake_height)) {
            dprintf(INFO, "[DISP_DFO]WARNING!!! Change LCM Resolution FAILED!!!\n");
        }
    }
#endif

    DISPMSG("mt_disp_init() done\n");

}

void disp_init(void *fdt)
{
    status_t err;
    void *g_fb_base_va;

    /* initialize the frame buffet information */
    g_fb_size = mt_disp_get_vram_size();
    g_fb_base = mblock_alloc(ROUNDUP(g_fb_size, PAGE_SIZE), PAGE_SIZE, MBLOCK_NO_LIMIT, 0x80000000, 0, "framebuffer");
    if (!g_fb_base)
        dprintf(CRITICAL, "mblock allocation failed\n");

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_framebuffer", ROUNDUP(g_fb_size, PAGE_SIZE),
            &g_fb_base_va, PAGE_SIZE_SHIFT,
            (paddr_t)g_fb_base, 0,
            ARCH_MMU_FLAG_CACHED);
    if (err != NO_ERROR) {
        dprintf(CRITICAL, "fastlogo vmm_alloc_physical failed\n");
    }

    mt_disp_init((void *)g_fb_base, (void *)(g_fb_base_va), fdt);
}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
void mt_ext_disp_init(void *lcd_pa_base, void *lcd_va_base)
{
    u32 ext_boot_mode_addr = 0;
    /// fb base pa and va
    ext_fb_addr_pa_k = arm_mmu_va2pa(lcd_pa_base);

    ext_fb_addr_pa   = ext_fb_addr_pa_k & 0xffffffffull;
    ext_fb_addr      = lcd_va_base;

    dprintf(0, "ext_fb_va: 0x%08x, ext_fb_pa: 0x%08x, ext_fb_pa_k: 0x%llx\n", ext_fb_addr, ext_fb_addr_pa, ext_fb_addr_pa_k);

    ext_fb_size = ALIGN_TO(CFG_EXT_DISPLAY_WIDTH, MTK_FB_ALIGNMENT) * ALIGN_TO(CFG_EXT_DISPLAY_HEIGHT, MTK_FB_ALIGNMENT) * CFG_EXT_DISPLAY_BPP / 8;
    // pa;
    ext_boot_mode_addr = ((u32)ext_fb_addr_pa + ext_fb_size);
    //ext_logo_db_addr_pa = (void *)((u32) SCRATCH_ADDR + SCRATCH_SIZE);

    // va;
    //ext_logo_db_addr = (void *)((u32) SCRATCH_ADDR + SCRATCH_SIZE);

    ext_fb_offset_logo = 0;

    external_display_init(NULL);

    memset((void *)lcd_va_base, 0x0, EXT_DISP_GetVRamSize());

    disp_input_config ext_input;

    memset(&ext_input, 0, sizeof(disp_input_config));
    ext_input.layer     = BOOT_MENU_LAYER;
    ext_input.layer_en  = 1;
    ext_input.fmt       = redoffset_32bit ? eBGRA8888 : eRGBA8888;
    ext_input.addr      = ext_boot_mode_addr;
    ext_input.src_x     = 0;
    ext_input.src_y     = 0;
    ext_input.src_w     = CFG_EXT_DISPLAY_WIDTH;
    ext_input.src_h     = CFG_EXT_DISPLAY_HEIGHT;
    ext_input.src_pitch = CFG_EXT_DISPLAY_WIDTH*4;
    ext_input.dst_x     = 0;
    ext_input.aen       = 1;
    ext_input.alpha     = 0xff;
    external_display_config_input(&ext_input);

    memset(&ext_input, 0, sizeof(disp_input_config));
    ext_input.layer         = FB_LAYER;
    ext_input.layer_en      = 1;
    ext_input.fmt           = redoffset_32bit ? eBGRA8888 : eRGBA8888;
    ext_input.addr          = ext_fb_addr_pa;
    ext_input.src_x         = 0;
    ext_input.src_y         = 0;
    ext_input.src_w         = CFG_EXT_DISPLAY_WIDTH;
    ext_input.src_h         = CFG_EXT_DISPLAY_HEIGHT;
    ext_input.src_pitch = ALIGN_TO(CFG_EXT_DISPLAY_WIDTH, MTK_FB_ALIGNMENT)*4;
    ext_input.dst_x         = 0;
    ext_input.dst_y         = 0;
    ext_input.dst_w         = CFG_EXT_DISPLAY_WIDTH;
    ext_input.dst_h         = CFG_EXT_DISPLAY_HEIGHT;

    ext_input.aen           = 1;
    ext_input.alpha         = 0xff;
    external_display_config_input(&ext_input);

    /* external internal test */
#if 1
    dprintf(CRITICAL, "external display internal test\n");
    /* _ext_mtkfb_internal_test(ext_fb_addr, CFG_EXT_DISPLAY_WIDTH, CFG_EXT_DISPLAY_HEIGHT); */
    _ext_mtkfb_internal_test2();
#endif
    DISPMSG("mt_ext_disp_init() done\n");
}

void mt_ext_disp_deinit(void)
{
    external_display_suspend();
}
#endif

void mt_disp_power(bool on)
{
    dprintf(0, "mt_disp_power %d\n", on);
    return;
}

void mt_free_logo_from_mblock(void)
{
#if defined(MBLOCK_SUPPORT)
    if (logo_db_addr_pa) {
        mblock_create(
                      (u64)(unsigned long)logo_db_addr_pa, (u64)(LK_LOGO_MAX_SIZE));

        logo_db_addr_pa = NULL;
        logo_db_addr = NULL;
    }
#endif
}

void *mt_get_logo_db_addr(void)
{
    dprintf(0, "mt_get_logo_db_addr: 0x%08x\n", (unsigned int)logo_db_addr);
    return logo_db_addr;
}

void *mt_get_logo_db_addr_pa(void)
{
    dprintf(0, "mt_get_logo_db_addr_pa: 0x%08x\n", (unsigned int)logo_db_addr_pa);
    return logo_db_addr_pa;
}

void *mt_get_fb_addr(void)
{
    fb_isdirty = 1;
    return (void *)((u32)fb_addr + fb_offset_logo * fb_size);
}

void *mt_get_tempfb_addr(void)
{
    //use offset = 2 as tempfb for decompress logo
    dprintf(0, "mt_get_tempfb_addr: 0x%08x , fb_addr 0x%08x\n", (unsigned int)((u32)fb_addr + 2*fb_size), (unsigned int)fb_addr);
    return (void *)((u32)fb_addr + 2*fb_size);
}

u32 mt_get_fb_size(void)
{
    return fb_size;
}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
void *mt_get_ext_logo_db_addr(void)
{
    dprintf(0, "mt_get_ext_logo_db_addr: 0x%08x\n", ext_logo_db_addr);
    return ext_logo_db_addr;
}

void *mt_get_ext_logo_db_addr_pa(void)
{
    dprintf(0, "mt_get_ext_logo_db_addr_pa: 0x%08x\n", ext_logo_db_addr_pa);
    return ext_logo_db_addr_pa;
}

void *mt_get_ext_fb_addr(void)
{
    ext_fb_isdirty = 1;
    return (void *)((u32)ext_fb_addr + ext_fb_offset_logo * ext_fb_size);
}

void *mt_get_ext_tempfb_addr(void)
{
    //use offset = 2 as tempfb for decompress logo
    dprintf(0, "mt_get_ext_tempfb_addr: 0x%08x , ext_fb_addr 0x%08x\n", (void *)((u32)ext_fb_addr + 2*ext_fb_size), (void *)ext_fb_addr);
    return (void *)((u32)ext_fb_addr + 2 * ext_fb_size);
}

INT32 mt_get_ext_fb_size(void)
{
    return ext_fb_size;
}
#endif

void mt_disp_update(u32 x, u32 y, u32 width, u32 height)
{
    arch_clean_cache_range((unsigned int)fb_addr, DISP_GetFBRamSize());
    primary_display_trigger(true);
}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
void mt_ext_disp_update(u32 x, u32 y, u32 width, u32 height)
{
    unsigned int va = ext_fb_addr;

    arch_clean_cache_range((unsigned int)ext_fb_addr, EXT_DISP_GetFBRamSize());
    external_display_trigger(true);

    if (!external_display_is_video_mode()) {
        /*video mode no need to wait*/
        dprintf(CRITICAL, "cmd mode trigger wait\n");
        spin(30000);
    }
}
#endif

static void mt_disp_adjusting_hardware_addr(void)
{
    dprintf(CRITICAL, "mt_disp_adjusting_hardware_addr fb_offset_logo = %d fb_size=%d\n", fb_offset_logo, fb_size);
    if (fb_offset_logo == 0) {
        mt_get_fb_addr();
        memcpy(fb_addr, (void *)((u32)fb_addr + 3 * fb_size), fb_size);
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

// Attention: this api indicates whether the lcm is connected
int DISP_IsLcmFound(void)
{
    return primary_display_is_lcm_connected();
}

const char *mt_disp_get_lcm_id(void)
{
    return primary_display_get_lcm_name();
}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
const char *mt_ext_disp_get_lcm_id(void)
{
    return external_display_get_lcm_name();
}

int EXT_DISP_IsLcmFound(void)
{
    return external_display_is_lcm_connected();
}
#endif

void disp_get_fb_address(u32 *fbVirAddr, u32 *fbPhysAddr)
{
    *fbVirAddr = (u32)fb_addr;
    *fbPhysAddr = (u32)fb_addr;
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
    *p++ = DISP_IsLcmFound();
    *p++ = mt_disp_get_lcd_time();
    *p++ = DISP_GetVRamSize();
    strncpy((char *)p, lcmname, strlen(lcmname) + 1);
    p += (strlen(lcmname) + 1 + 4) >> 2;

    dprintf(CRITICAL, "videolfb - fb_base    = 0x%llx\n", fb_addr_pa_k);
    dprintf(CRITICAL, "videolfb - islcmfound = %d\n", DISP_IsLcmFound());
    dprintf(CRITICAL, "videolfb - fps        = %d\n", mt_disp_get_lcd_time());
    dprintf(CRITICAL, "videolfb - vram       = %d\n", DISP_GetVRamSize());
    dprintf(CRITICAL, "videolfb - lcmname    = %s\n", lcmname);

    offset = fdt_path_offset(fdt, "/chosen");
    fdt_setprop(fdt, offset, "atag,videolfb", ptr, (void *)p - (void *)ptr);
}
SET_FDT_INIT_HOOK(target_atag_videolfb, target_atag_videolfb);
