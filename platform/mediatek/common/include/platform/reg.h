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

#include <reg.h>

#define writew(v, a)       (*REG16(a) = (v))
#define readw(a)           (*REG16(a))
#define read32(addr)       (*REG32(addr))
#define write32(addr, val) (*REG32(addr) = (val))

#define __clrsetbits(bits, addr, clr, set) \
    write##bits(addr, (read##bits(addr) & ~((uint##bits##_t)(clr))) | (set))

#define clrbits32(addr, clr)         __clrsetbits(32, addr, clr, 0)
#define setbits32(addr, set)         __clrsetbits(32, addr, 0, set)
#define clrsetbits32(addr, clr, set) __clrsetbits(32, addr, clr, set)
