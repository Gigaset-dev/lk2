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

#include <smc_id_table.h>
#include <smc.h>
/* platform dependent spm.h*/
#include <platform/spm.h>

enum mt_lpm_smc_user_id {
    mt_lpm_smc_user_cpu_pm = 0,
    mt_lpm_smc_user_spm_dbg,
    mt_lpm_smc_user_spm,
    mt_lpm_smc_user_cpu_pm_lp,
    mt_lpm_smc_user_secure_cpu_pm,
    mt_lpm_smc_user_secure_spm_dbg,
    mt_lpm_smc_user_secure_spm,
    mt_lpm_smc_user_max,
};

#define MT_LPM_SMC_MAGIC         0xDA000000
#define MT_LPM_SMC_USER_MASK     0xff
#define MT_LPM_SMC_USER_SHIFT    16
#define MT_LPM_SMC_USER_ID_MASK  0x0000ffff

/* LPM behavior */
#define MT_LPM_SMC_ACT_SET              (1UL<<0)
#define MT_LPM_SMC_ACT_CLR              (1UL<<1)
#define MT_LPM_SMC_ACT_GET              (1UL<<2)
#define MT_LPM_SMC_ACT_PUSH             (1UL<<3)
#define MT_LPM_SMC_ACT_POP              (1UL<<4)
#define MT_LPM_SMC_ACT_SUBMIT           (1UL<<5)
#define MT_LPM_SMC_ACT_COMPAT           (1UL<<31)

/* sink user id to smc's user id */
#define MT_LPM_SMC_USER_SINK(user, uid) \
               (((uid & MT_LPM_SMC_USER_ID_MASK)\
                  | ((user & MT_LPM_SMC_USER_MASK)\
                    << MT_LPM_SMC_USER_SHIFT))\
                | MT_LPM_SMC_MAGIC)


/* sink cpu pm's smc id */
#define MT_LPM_SMC_USER_ID_CPU_PM(uid) \
    MT_LPM_SMC_USER_SINK(mt_lpm_smc_user_secure_cpu_pm, uid)
/* sink spm debug's smc id */
#define MT_LPM_SMC_USER_ID_SPM_DBG(uid) \
    MT_LPM_SMC_USER_SINK(mt_lpm_smc_user_secure_spm_dbg, uid)
/* sink spm's smc id */
#define MT_LPM_SMC_USER_ID_SPM(uid) \
    MT_LPM_SMC_USER_SINK(mt_lpm_smc_user_secure_spm, uid)


/* sink cpu pm's user id */
#define MT_LPM_SMC_USER_CPU_PM(uid)\
    MT_LPM_SMC_USER_ID_CPU_PM(uid)
/* sink spm debug's user id */
#define MT_LPM_SMC_USER_SPM_DBG(uid)\
    MT_LPM_SMC_USER_ID_SPM_DBG(uid)
/* sink spm's user id */
#define MT_LPM_SMC_USER_SPM(uid)\
    MT_LPM_SMC_USER_ID_SPM(uid)


#define MT_LPM_SMC_IMPL(_lp_id, _act, _val1, _val2, res)\
    __smc_conduit(MTK_SIP_BL_LPM_CONTROL,\
                  _lp_id, _act, _val1, _val2,\
                  0, 0, 0, res)


#define MT_LPM_SMC_CPU_PM(_lp_id, _act, _val1, _val2, res)\
    MT_LPM_SMC_IMPL(MT_LPM_SMC_USER_CPU_PM(_lp_id),\
                    _act, _val1, _val2, res)


#define MT_LPM_SMC_SPM(_lp_id, _act, _val1, _val2, res)\
    MT_LPM_SMC_IMPL(MT_LPM_SMC_USER_SPM(_lp_id),\
                    _act, _val1, _val2, res)

/* wrap function for smc call to load spmfw */
#ifdef SPM_FW_LOAD_LEGACY
#define LOAD_SPMFW_SMC(addr, size, res)\
    __smc_conduit(MTK_SIP_KERNEL_SPM_ARGS, SPM_ARGS_SPMFW_INIT, addr, size,\
    0, 0, 0, 0, res)
#else
#define LOAD_SPMFW_SMC(addr, size, res)\
    MT_LPM_SMC_SPM(MT_SPM_SMC_UID_FW_INIT, MT_LPM_SMC_ACT_SET, addr, size,\
    res)
#endif
