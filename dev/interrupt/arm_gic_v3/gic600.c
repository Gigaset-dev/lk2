/*
 * Copyright (c) 2017-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Driver for GIC-600 specific features. This driver only overrides
 * APIs that are different to those generic ones in GICv3 driver.
 *
 * GIC-600 supports independently power-gating redistributor interface.
 */

#include <assert.h>
#include <dev/interrupt/arm_gic.h>
#include <platform/gic.h>

/* GIC-600 specific register offsets */
#define GICR_PWRR   0x24

/* GICR_PWRR fields */
#define PWRR_RDPD_SHIFT     0
#define PWRR_RDAG_SHIFT     1
#define PWRR_RDGPD_SHIFT    2
#define PWRR_RDGPO_SHIFT    3

#define PWRR_RDPD   (1 << PWRR_RDPD_SHIFT)
#define PWRR_RDAG   (1 << PWRR_RDAG_SHIFT)
#define PWRR_RDGPD  (1 << PWRR_RDGPD_SHIFT)
#define PWRR_RDGPO  (1 << PWRR_RDGPO_SHIFT)

/*
 * Values to write to GICR_PWRR register to power redistributor
 * for operating through the core (GICR_PWRR.RDAG = 0)
 */
#define PWRR_ON     (0 << PWRR_RDPD_SHIFT)
#define PWRR_OFF    (1 << PWRR_RDPD_SHIFT)

/* GIC-600 specific accessor functions */
static void gicr_write_pwrr(uintptr_t base, unsigned int val)
{
    GIC_WRITE(base + GICR_PWRR, val & PWRR_RDPD);
}

static uint32_t gicr_read_pwrr(uintptr_t base)
{
    return GIC_READ(base + GICR_PWRR);
}

static void gicr_wait_group_not_in_transit(uintptr_t base)
{
    /* Check group not transitioning: RDGPD == RDGPO */
    while (((gicr_read_pwrr(base) & PWRR_RDGPD) >> PWRR_RDGPD_SHIFT) !=
            ((gicr_read_pwrr(base) & PWRR_RDGPO) >> PWRR_RDGPO_SHIFT))
        ;
}

static void gic600_pwr_on(uintptr_t base)
{
    do {    /* Wait until group not transitioning */
        gicr_wait_group_not_in_transit(base);

        /* Power on redistributor */
        gicr_write_pwrr(base, PWRR_ON);

        /*
         * Wait until the power on state is reflected.
         * If RDPD == 0 then powered on.
         */
    } while ((gicr_read_pwrr(base) & PWRR_RDPD) != PWRR_ON);
}

static void gic600_pwr_off(uintptr_t base)
{

    /* Wait until group not transitioning */
    gicr_wait_group_not_in_transit(base);

    /* Power off redistributor */
    gicr_write_pwrr(base, PWRR_OFF);

    /*
     * If this is the last man, turning this redistributor frame off will
     * result in the group itself being powered off and RDGPD = 1.
     * In that case, wait as long as it's in transition, or has aborted
     * the transition altogether for any reason.
     */
    if ((gicr_read_pwrr(base) & PWRR_RDGPD) != 0) {
        /* Wait until group not transitioning */
        gicr_wait_group_not_in_transit(base);
    }
}

/*
 * Power off GIC-600 redistributor
 */
void gicv3_rdistif_off(void)
{
    uintptr_t gicr_base;

#if ARCH_ARM64
    gicr_base = GIC_REDIS_BASE_PHY;
#else
    gicr_base = GIC_REDIS_BASE;
#endif
    assert(gicr_base);

    /* Attempt to power redistributor off */
    gic600_pwr_off(gicr_base);
}

/*
 * Power on GIC-600 redistributor
 */
void gicv3_rdistif_on(void)
{
    uintptr_t gicr_base;

#if ARCH_ARM64
    gicr_base = GIC_REDIS_BASE_PHY;
#else
    gicr_base = GIC_REDIS_BASE;
#endif
    assert(gicr_base);

    /* Power redistributor on */
    gic600_pwr_on(gicr_base);
}

/* vim: set ts=4 sw=4 expandtab: */
