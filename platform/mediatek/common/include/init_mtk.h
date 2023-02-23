/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <lk/init.h>

/*
 * Mediatek use LK for different boot stages/application. Define wrapper macros
 * to allow us to specify the LK_INIT_HOOK()'s scope.
 */

/*
 * internal help wrppaer, not to be used. Use MT_LK_INIT_HOOK() macro to
 * register init hook.
 */
#define MT_LK_INIT_HOOK_BL2(_name, _hook, _level)
#define MT_LK_INIT_HOOK_BL2_EXT(_name, _hook, _level)
#define MT_LK_INIT_HOOK_BL33(_name, _hook, _level)
#define MT_LK_INIT_HOOK_AEE(_name, _hook, _level)

#if LK_AS_BL2
#  undef MT_LK_INIT_HOOK_BL2
#  define MT_LK_INIT_HOOK_BL2(_name, _hook, _level) \
     LK_INIT_HOOK(_name, _hook, _level)
#elif LK_AS_BL2_EXT
#  undef MT_LK_INIT_HOOK_BL2_EXT
#  define MT_LK_INIT_HOOK_BL2_EXT(_name, _hook, _level) \
     LK_INIT_HOOK(_name, _hook, _level)
#elif LK_AS_BL33
#  undef MT_LK_INIT_HOOK_BL33
#  define MT_LK_INIT_HOOK_BL33(_name, _hook, _level) \
     LK_INIT_HOOK(_name, _hook, _level)
#elif LK_AS_AEE
#  undef MT_LK_INIT_HOOK_AEE
#  define MT_LK_INIT_HOOK_AEE(_name, _hook, _level) \
     LK_INIT_HOOK(_name, _hook, _level)
#else
#  error "Unknown LK_AS variant."
#endif

/*
 * Mediatek LK_INIT_HOOK wrapper with stage argument: BL2, BL2_EXT, BL33, AEE
 *
 * Hook functions registered by this macro will only be generated when
 * lk was built as specified stage, while the hook functions registered
 * by original LK_INIT_HOOK() will be generated for all LK_AS stage.
 */
#define MT_LK_INIT_HOOK(_stage, _name, _hook, _level) \
    MT_LK_INIT_HOOK_##_stage(_name, _hook, _level)

