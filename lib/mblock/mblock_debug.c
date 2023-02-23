/*
 * Copyright (c) 2008 Travis Geiselbrecht
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
#if defined(WITH_LIB_CONSOLE)
#include <compiler.h>
#include <lib/console.h>
#include <debug.h>
#include <mblock.h>
#include <platform.h>
#include <reg.h>

static int mblock_cmd_test(int argc, const cmd_args *argv)
{
    u64 start, large, free;
    int ret;
    const struct reserved_t *ptr_in, *ptr_out;

    printf("basic mblock_alloc\n");
    start = mblock_alloc(0x400000, 0x1000, MBLOCK_NO_LIMIT, 0, 1, "basic_alloc_mapping");
    printf("basic mblock_alloc ret = %llx\n", start);
    free = start;
    printf("nomaping mblock_alloc\n");
    start = mblock_alloc(0x400000, 0x1000, MBLOCK_NO_LIMIT, 0, 0, "no_maping_basic_alloc");
    printf("nomapping mblock_alloc ret = %llx\n", start);
    printf("large mblock_alloc\n");
    start = mblock_alloc(0x5000000, 0x1000, MBLOCK_BELOW_3G, 0, 0, "large_alloc_nomapping");
    printf("large mblock_alloc ret = %llx\n", start);
    large = start;
    mblock_show();
    printf("static mblock_alloc 0x80000000\n");
    start = mblock_alloc(0x5000000, 0x1000, MBLOCK_BELOW_3G,
        0x80000000, 0, "static1_alloc_nomapping");
    printf("static mblock_alloc ret = %llx\n", start);
    printf("static mblock_alloc exceed limit on 0x110000000\n");
    start = mblock_alloc(0x5000000, 0x1000, MBLOCK_BELOW_3G, 0x110000000, 0, "static2_nomapping");
    printf("static mblock_alloc exceed limit ret = %llx\n", start);
    printf("mblock_resize on large\n");
    ret = mblock_resize(large, 0x5000000, 0x4000000);
    printf("mblock_resize on large ret = %d\n", ret);
    printf("free address 0x%llx\n", free);
    ret = mblock_free(free);
    printf("mblock_free ret = %d\n", ret);
    printf("free_partial 0x%llx size=0x%llx\n", start+0x100000, 0x100000);
    ret = mblock_free_partial(large+0x100000, 0x100000);
    printf("mblock_free_partial ret=%d\n", ret);
    printf("mblock_query_reserved_by_name index 0\n");
    ptr_out = mblock_query_reserved_by_name("large_alloc_nomapping", 0);
    ptr_in = ptr_out;
    printf("mblock_query_reserved_by_name ret=%x\n", (unsigned int *)ptr_out);
    printf("mblock_query_reserved_by_name with index\n");
    ptr_out = mblock_query_reserved_by_name("large_alloc_nomapping", ptr_in);
    printf("mblock_query_reserved_by_name with index ret=%x\n", (unsigned int *)ptr_out);
    printf("mblock_query_reserved_by_name with next index\n");
    ptr_in = ptr_out;
    ptr_out = mblock_query_reserved_by_name("large_alloc_nomapping", ptr_in);
    printf("mblock_query_reserved_by_name with next index ret=%x\n", (unsigned int *)ptr_out);
    printf("mblock_query_reserved_count\n");
    ret = mblock_query_reserved_count("large_alloc_nomapping");
    printf("mblock_query_reserved_count ret=%x\n", ret);
    mblock_show();


    return 0;
}

static int mblock_cmd_show(int argc, const cmd_args *argv)
{
    mblock_show();
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("mblock_test", "test mblock", &mblock_cmd_test)
STATIC_COMMAND("mblock_show", "show current mblock info", &mblock_cmd_show)
STATIC_COMMAND_END(mblock);

#endif
