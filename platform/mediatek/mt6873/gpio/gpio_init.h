/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include "cust_gpio_boot.h"

static struct gpio_pin gpio_array[MT_GPIO_BASE_MAX] = {
    {
        GPIO0_MODE,
        GPIO0_DIR,
        GPIO0_PULLEN,
        GPIO0_PULL,
        GPIO0_DATAOUT,
        GPIO0_SMT,
    },
    {
        GPIO1_MODE,
        GPIO1_DIR,
        GPIO1_PULLEN,
        GPIO1_PULL,
        GPIO1_DATAOUT,
        GPIO1_SMT,
    },
    {
        GPIO2_MODE,
        GPIO2_DIR,
        GPIO2_PULLEN,
        GPIO2_PULL,
        GPIO2_DATAOUT,
        GPIO2_SMT,
    },
    {
        GPIO3_MODE,
        GPIO3_DIR,
        GPIO3_PULLEN,
        GPIO3_PULL,
        GPIO3_DATAOUT,
        GPIO3_SMT,
    },
    {
        GPIO4_MODE,
        GPIO4_DIR,
        GPIO4_PULLEN,
        GPIO4_PULL,
        GPIO4_DATAOUT,
        GPIO4_SMT,
    },
    {
        GPIO5_MODE,
        GPIO5_DIR,
        GPIO5_PULLEN,
        GPIO5_PULL,
        GPIO5_DATAOUT,
        GPIO5_SMT,
    },
    {
        GPIO6_MODE,
        GPIO6_DIR,
        GPIO6_PULLEN,
        GPIO6_PULL,
        GPIO6_DATAOUT,
        GPIO6_SMT,
    },
    {
        GPIO7_MODE,
        GPIO7_DIR,
        GPIO7_PULLEN,
        GPIO7_PULL,
        GPIO7_DATAOUT,
        GPIO7_SMT,
    },
    {
        GPIO8_MODE,
        GPIO8_DIR,
        GPIO8_PULLEN,
        GPIO8_PULL,
        GPIO8_DATAOUT,
        GPIO8_SMT,
    },
    {
        GPIO9_MODE,
        GPIO9_DIR,
        GPIO9_PULLEN,
        GPIO9_PULL,
        GPIO9_DATAOUT,
        GPIO9_SMT,
    },
    {
        GPIO10_MODE,
        GPIO10_DIR,
        GPIO10_PULLEN,
        GPIO10_PULL,
        GPIO10_DATAOUT,
        GPIO10_SMT,
    },
    {
        GPIO11_MODE,
        GPIO11_DIR,
        GPIO11_PULLEN,
        GPIO11_PULL,
        GPIO11_DATAOUT,
        GPIO11_SMT,
    },
    {
        GPIO12_MODE,
        GPIO12_DIR,
        GPIO12_PULLEN,
        GPIO12_PULL,
        GPIO12_DATAOUT,
        GPIO12_SMT,
    },
    {
        GPIO13_MODE,
        GPIO13_DIR,
        GPIO13_PULLEN,
        GPIO13_PULL,
        GPIO13_DATAOUT,
        GPIO13_SMT,
    },
    {
        GPIO14_MODE,
        GPIO14_DIR,
        GPIO14_PULLEN,
        GPIO14_PULL,
        GPIO14_DATAOUT,
        GPIO14_SMT,
    },
    {
        GPIO15_MODE,
        GPIO15_DIR,
        GPIO15_PULLEN,
        GPIO15_PULL,
        GPIO15_DATAOUT,
        GPIO15_SMT,
    },
    {
        GPIO16_MODE,
        GPIO16_DIR,
        GPIO16_PULLEN,
        GPIO16_PULL,
        GPIO16_DATAOUT,
        GPIO16_SMT,
    },
    {
        GPIO17_MODE,
        GPIO17_DIR,
        GPIO17_PULLEN,
        GPIO17_PULL,
        GPIO17_DATAOUT,
        GPIO17_SMT,
    },
    {
        GPIO18_MODE,
        GPIO18_DIR,
        GPIO18_PULLEN,
        GPIO18_PULL,
        GPIO18_DATAOUT,
        GPIO18_SMT,
    },
    {
        GPIO19_MODE,
        GPIO19_DIR,
        GPIO19_PULLEN,
        GPIO19_PULL,
        GPIO19_DATAOUT,
        GPIO19_SMT,
    },
    {
        GPIO20_MODE,
        GPIO20_DIR,
        GPIO20_PULLEN,
        GPIO20_PULL,
        GPIO20_DATAOUT,
        GPIO20_SMT,
    },
    {
        GPIO21_MODE,
        GPIO21_DIR,
        GPIO21_PULLEN,
        GPIO21_PULL,
        GPIO21_DATAOUT,
        GPIO21_SMT,
    },
    {
        GPIO22_MODE,
        GPIO22_DIR,
        GPIO22_PULLEN,
        GPIO22_PULL,
        GPIO22_DATAOUT,
        GPIO22_SMT,
    },
    {
        GPIO23_MODE,
        GPIO23_DIR,
        GPIO23_PULLEN,
        GPIO23_PULL,
        GPIO23_DATAOUT,
        GPIO23_SMT,
    },
    {
        GPIO24_MODE,
        GPIO24_DIR,
        GPIO24_PULLEN,
        GPIO24_PULL,
        GPIO24_DATAOUT,
        GPIO24_SMT,
    },
    {
        GPIO25_MODE,
        GPIO25_DIR,
        GPIO25_PULLEN,
        GPIO25_PULL,
        GPIO25_DATAOUT,
        GPIO25_SMT,
    },
    {
        GPIO26_MODE,
        GPIO26_DIR,
        GPIO26_PULLEN,
        GPIO26_PULL,
        GPIO26_DATAOUT,
        GPIO26_SMT,
    },
    {
        GPIO27_MODE,
        GPIO27_DIR,
        GPIO27_PULLEN,
        GPIO27_PULL,
        GPIO27_DATAOUT,
        GPIO27_SMT,
    },
    {
        GPIO28_MODE,
        GPIO28_DIR,
        GPIO28_PULLEN,
        GPIO28_PULL,
        GPIO28_DATAOUT,
        GPIO28_SMT,
    },
    {
        GPIO29_MODE,
        GPIO29_DIR,
        GPIO29_PULLEN,
        GPIO29_PULL,
        GPIO29_DATAOUT,
        GPIO29_SMT,
    },
    {
        GPIO30_MODE,
        GPIO30_DIR,
        GPIO30_PULLEN,
        GPIO30_PULL,
        GPIO30_DATAOUT,
        GPIO30_SMT,
    },
    {
        GPIO31_MODE,
        GPIO31_DIR,
        GPIO31_PULLEN,
        GPIO31_PULL,
        GPIO31_DATAOUT,
        GPIO31_SMT,
    },
    {
        GPIO32_MODE,
        GPIO32_DIR,
        GPIO32_PULLEN,
        GPIO32_PULL,
        GPIO32_DATAOUT,
        GPIO32_SMT,
    },
    {
        GPIO33_MODE,
        GPIO33_DIR,
        GPIO33_PULLEN,
        GPIO33_PULL,
        GPIO33_DATAOUT,
        GPIO33_SMT,
    },
    {
        GPIO34_MODE,
        GPIO34_DIR,
        GPIO34_PULLEN,
        GPIO34_PULL,
        GPIO34_DATAOUT,
        GPIO34_SMT,
    },
    {
        GPIO35_MODE,
        GPIO35_DIR,
        GPIO35_PULLEN,
        GPIO35_PULL,
        GPIO35_DATAOUT,
        GPIO35_SMT,
    },
    {
        GPIO36_MODE,
        GPIO36_DIR,
        GPIO36_PULLEN,
        GPIO36_PULL,
        GPIO36_DATAOUT,
        GPIO36_SMT,
    },
    {
        GPIO37_MODE,
        GPIO37_DIR,
        GPIO37_PULLEN,
        GPIO37_PULL,
        GPIO37_DATAOUT,
        GPIO37_SMT,
    },
    {
        GPIO38_MODE,
        GPIO38_DIR,
        GPIO38_PULLEN,
        GPIO38_PULL,
        GPIO38_DATAOUT,
        GPIO38_SMT,
    },
    {
        GPIO39_MODE,
        GPIO39_DIR,
        GPIO39_PULLEN,
        GPIO39_PULL,
        GPIO39_DATAOUT,
        GPIO39_SMT,
    },
    {
        GPIO40_MODE,
        GPIO40_DIR,
        GPIO40_PULLEN,
        GPIO40_PULL,
        GPIO40_DATAOUT,
        GPIO40_SMT,
    },
    {
        GPIO41_MODE,
        GPIO41_DIR,
        GPIO41_PULLEN,
        GPIO41_PULL,
        GPIO41_DATAOUT,
        GPIO41_SMT,
    },
    {
        GPIO42_MODE,
        GPIO42_DIR,
        GPIO42_PULLEN,
        GPIO42_PULL,
        GPIO42_DATAOUT,
        GPIO42_SMT,
    },
    {
        GPIO43_MODE,
        GPIO43_DIR,
        GPIO43_PULLEN,
        GPIO43_PULL,
        GPIO43_DATAOUT,
        GPIO43_SMT,
    },
    {
        GPIO44_MODE,
        GPIO44_DIR,
        GPIO44_PULLEN,
        GPIO44_PULL,
        GPIO44_DATAOUT,
        GPIO44_SMT,
    },
    {
        GPIO45_MODE,
        GPIO45_DIR,
        GPIO45_PULLEN,
        GPIO45_PULL,
        GPIO45_DATAOUT,
        GPIO45_SMT,
    },
    {
        GPIO46_MODE,
        GPIO46_DIR,
        GPIO46_PULLEN,
        GPIO46_PULL,
        GPIO46_DATAOUT,
        GPIO46_SMT,
    },
    {
        GPIO47_MODE,
        GPIO47_DIR,
        GPIO47_PULLEN,
        GPIO47_PULL,
        GPIO47_DATAOUT,
        GPIO47_SMT,
    },
    {
        GPIO48_MODE,
        GPIO48_DIR,
        GPIO48_PULLEN,
        GPIO48_PULL,
        GPIO48_DATAOUT,
        GPIO48_SMT,
    },
    {
        GPIO49_MODE,
        GPIO49_DIR,
        GPIO49_PULLEN,
        GPIO49_PULL,
        GPIO49_DATAOUT,
        GPIO49_SMT,
    },
    {
        GPIO50_MODE,
        GPIO50_DIR,
        GPIO50_PULLEN,
        GPIO50_PULL,
        GPIO50_DATAOUT,
        GPIO50_SMT,
    },
    {
        GPIO51_MODE,
        GPIO51_DIR,
        GPIO51_PULLEN,
        GPIO51_PULL,
        GPIO51_DATAOUT,
        GPIO51_SMT,
    },
    {
        GPIO52_MODE,
        GPIO52_DIR,
        GPIO52_PULLEN,
        GPIO52_PULL,
        GPIO52_DATAOUT,
        GPIO52_SMT,
    },
    {
        GPIO53_MODE,
        GPIO53_DIR,
        GPIO53_PULLEN,
        GPIO53_PULL,
        GPIO53_DATAOUT,
        GPIO53_SMT,
    },
    {
        GPIO54_MODE,
        GPIO54_DIR,
        GPIO54_PULLEN,
        GPIO54_PULL,
        GPIO54_DATAOUT,
        GPIO54_SMT,
    },
    {
        GPIO55_MODE,
        GPIO55_DIR,
        GPIO55_PULLEN,
        GPIO55_PULL,
        GPIO55_DATAOUT,
        GPIO55_SMT,
    },
    {
        GPIO56_MODE,
        GPIO56_DIR,
        GPIO56_PULLEN,
        GPIO56_PULL,
        GPIO56_DATAOUT,
        GPIO56_SMT,
    },
    {
        GPIO57_MODE,
        GPIO57_DIR,
        GPIO57_PULLEN,
        GPIO57_PULL,
        GPIO57_DATAOUT,
        GPIO57_SMT,
    },
    {
        GPIO58_MODE,
        GPIO58_DIR,
        GPIO58_PULLEN,
        GPIO58_PULL,
        GPIO58_DATAOUT,
        GPIO58_SMT,
    },
    {
        GPIO59_MODE,
        GPIO59_DIR,
        GPIO59_PULLEN,
        GPIO59_PULL,
        GPIO59_DATAOUT,
        GPIO59_SMT,
    },
    {
        GPIO60_MODE,
        GPIO60_DIR,
        GPIO60_PULLEN,
        GPIO60_PULL,
        GPIO60_DATAOUT,
        GPIO60_SMT,
    },
    {
        GPIO61_MODE,
        GPIO61_DIR,
        GPIO61_PULLEN,
        GPIO61_PULL,
        GPIO61_DATAOUT,
        GPIO61_SMT,
    },
    {
        GPIO62_MODE,
        GPIO62_DIR,
        GPIO62_PULLEN,
        GPIO62_PULL,
        GPIO62_DATAOUT,
        GPIO62_SMT,
    },
    {
        GPIO63_MODE,
        GPIO63_DIR,
        GPIO63_PULLEN,
        GPIO63_PULL,
        GPIO63_DATAOUT,
        GPIO63_SMT,
    },
    {
        GPIO64_MODE,
        GPIO64_DIR,
        GPIO64_PULLEN,
        GPIO64_PULL,
        GPIO64_DATAOUT,
        GPIO64_SMT,
    },
    {
        GPIO65_MODE,
        GPIO65_DIR,
        GPIO65_PULLEN,
        GPIO65_PULL,
        GPIO65_DATAOUT,
        GPIO65_SMT,
    },
    {
        GPIO66_MODE,
        GPIO66_DIR,
        GPIO66_PULLEN,
        GPIO66_PULL,
        GPIO66_DATAOUT,
        GPIO66_SMT,
    },
    {
        GPIO67_MODE,
        GPIO67_DIR,
        GPIO67_PULLEN,
        GPIO67_PULL,
        GPIO67_DATAOUT,
        GPIO67_SMT,
    },
    {
        GPIO68_MODE,
        GPIO68_DIR,
        GPIO68_PULLEN,
        GPIO68_PULL,
        GPIO68_DATAOUT,
        GPIO68_SMT,
    },
    {
        GPIO69_MODE,
        GPIO69_DIR,
        GPIO69_PULLEN,
        GPIO69_PULL,
        GPIO69_DATAOUT,
        GPIO69_SMT,
    },
    {
        GPIO70_MODE,
        GPIO70_DIR,
        GPIO70_PULLEN,
        GPIO70_PULL,
        GPIO70_DATAOUT,
        GPIO70_SMT,
    },
    {
        GPIO71_MODE,
        GPIO71_DIR,
        GPIO71_PULLEN,
        GPIO71_PULL,
        GPIO71_DATAOUT,
        GPIO71_SMT,
    },
    {
        GPIO72_MODE,
        GPIO72_DIR,
        GPIO72_PULLEN,
        GPIO72_PULL,
        GPIO72_DATAOUT,
        GPIO72_SMT,
    },
    {
        GPIO73_MODE,
        GPIO73_DIR,
        GPIO73_PULLEN,
        GPIO73_PULL,
        GPIO73_DATAOUT,
        GPIO73_SMT,
    },
    {
        GPIO74_MODE,
        GPIO74_DIR,
        GPIO74_PULLEN,
        GPIO74_PULL,
        GPIO74_DATAOUT,
        GPIO74_SMT,
    },
    {
        GPIO75_MODE,
        GPIO75_DIR,
        GPIO75_PULLEN,
        GPIO75_PULL,
        GPIO75_DATAOUT,
        GPIO75_SMT,
    },
    {
        GPIO76_MODE,
        GPIO76_DIR,
        GPIO76_PULLEN,
        GPIO76_PULL,
        GPIO76_DATAOUT,
        GPIO76_SMT,
    },
    {
        GPIO77_MODE,
        GPIO77_DIR,
        GPIO77_PULLEN,
        GPIO77_PULL,
        GPIO77_DATAOUT,
        GPIO77_SMT,
    },
    {
        GPIO78_MODE,
        GPIO78_DIR,
        GPIO78_PULLEN,
        GPIO78_PULL,
        GPIO78_DATAOUT,
        GPIO78_SMT,
    },
    {
        GPIO79_MODE,
        GPIO79_DIR,
        GPIO79_PULLEN,
        GPIO79_PULL,
        GPIO79_DATAOUT,
        GPIO79_SMT,
    },
    {
        GPIO80_MODE,
        GPIO80_DIR,
        GPIO80_PULLEN,
        GPIO80_PULL,
        GPIO80_DATAOUT,
        GPIO80_SMT,
    },
    {
        GPIO81_MODE,
        GPIO81_DIR,
        GPIO81_PULLEN,
        GPIO81_PULL,
        GPIO81_DATAOUT,
        GPIO81_SMT,
    },
    {
        GPIO82_MODE,
        GPIO82_DIR,
        GPIO82_PULLEN,
        GPIO82_PULL,
        GPIO82_DATAOUT,
        GPIO82_SMT,
    },
    {
        GPIO83_MODE,
        GPIO83_DIR,
        GPIO83_PULLEN,
        GPIO83_PULL,
        GPIO83_DATAOUT,
        GPIO83_SMT,
    },
    {
        GPIO84_MODE,
        GPIO84_DIR,
        GPIO84_PULLEN,
        GPIO84_PULL,
        GPIO84_DATAOUT,
        GPIO84_SMT,
    },
    {
        GPIO85_MODE,
        GPIO85_DIR,
        GPIO85_PULLEN,
        GPIO85_PULL,
        GPIO85_DATAOUT,
        GPIO85_SMT,
    },
    {
        GPIO86_MODE,
        GPIO86_DIR,
        GPIO86_PULLEN,
        GPIO86_PULL,
        GPIO86_DATAOUT,
        GPIO86_SMT,
    },
    {
        GPIO87_MODE,
        GPIO87_DIR,
        GPIO87_PULLEN,
        GPIO87_PULL,
        GPIO87_DATAOUT,
        GPIO87_SMT,
    },
    {
        GPIO88_MODE,
        GPIO88_DIR,
        GPIO88_PULLEN,
        GPIO88_PULL,
        GPIO88_DATAOUT,
        GPIO88_SMT,
    },
    {
        GPIO89_MODE,
        GPIO89_DIR,
        GPIO89_PULLEN,
        GPIO89_PULL,
        GPIO89_DATAOUT,
        GPIO89_SMT,
    },
    {
        GPIO90_MODE,
        GPIO90_DIR,
        GPIO90_PULLEN,
        GPIO90_PULL,
        GPIO90_DATAOUT,
        GPIO90_SMT,
    },
    {
        GPIO91_MODE,
        GPIO91_DIR,
        GPIO91_PULLEN,
        GPIO91_PULL,
        GPIO91_DATAOUT,
        GPIO91_SMT,
    },
    {
        GPIO92_MODE,
        GPIO92_DIR,
        GPIO92_PULLEN,
        GPIO92_PULL,
        GPIO92_DATAOUT,
        GPIO92_SMT,
    },
    {
        GPIO93_MODE,
        GPIO93_DIR,
        GPIO93_PULLEN,
        GPIO93_PULL,
        GPIO93_DATAOUT,
        GPIO93_SMT,
    },
    {
        GPIO94_MODE,
        GPIO94_DIR,
        GPIO94_PULLEN,
        GPIO94_PULL,
        GPIO94_DATAOUT,
        GPIO94_SMT,
    },
    {
        GPIO95_MODE,
        GPIO95_DIR,
        GPIO95_PULLEN,
        GPIO95_PULL,
        GPIO95_DATAOUT,
        GPIO95_SMT,
    },
    {
        GPIO96_MODE,
        GPIO96_DIR,
        GPIO96_PULLEN,
        GPIO96_PULL,
        GPIO96_DATAOUT,
        GPIO96_SMT,
    },
    {
        GPIO97_MODE,
        GPIO97_DIR,
        GPIO97_PULLEN,
        GPIO97_PULL,
        GPIO97_DATAOUT,
        GPIO97_SMT,
    },
    {
        GPIO98_MODE,
        GPIO98_DIR,
        GPIO98_PULLEN,
        GPIO98_PULL,
        GPIO98_DATAOUT,
        GPIO98_SMT,
    },
    {
        GPIO99_MODE,
        GPIO99_DIR,
        GPIO99_PULLEN,
        GPIO99_PULL,
        GPIO99_DATAOUT,
        GPIO99_SMT,
    },
    {
        GPIO100_MODE,
        GPIO100_DIR,
        GPIO100_PULLEN,
        GPIO100_PULL,
        GPIO100_DATAOUT,
        GPIO100_SMT,
    },
    {
        GPIO101_MODE,
        GPIO101_DIR,
        GPIO101_PULLEN,
        GPIO101_PULL,
        GPIO101_DATAOUT,
        GPIO101_SMT,
    },
    {
        GPIO102_MODE,
        GPIO102_DIR,
        GPIO102_PULLEN,
        GPIO102_PULL,
        GPIO102_DATAOUT,
        GPIO102_SMT,
    },
    {
        GPIO103_MODE,
        GPIO103_DIR,
        GPIO103_PULLEN,
        GPIO103_PULL,
        GPIO103_DATAOUT,
        GPIO103_SMT,
    },
    {
        GPIO104_MODE,
        GPIO104_DIR,
        GPIO104_PULLEN,
        GPIO104_PULL,
        GPIO104_DATAOUT,
        GPIO104_SMT,
    },
    {
        GPIO105_MODE,
        GPIO105_DIR,
        GPIO105_PULLEN,
        GPIO105_PULL,
        GPIO105_DATAOUT,
        GPIO105_SMT,
    },
    {
        GPIO106_MODE,
        GPIO106_DIR,
        GPIO106_PULLEN,
        GPIO106_PULL,
        GPIO106_DATAOUT,
        GPIO106_SMT,
    },
    {
        GPIO107_MODE,
        GPIO107_DIR,
        GPIO107_PULLEN,
        GPIO107_PULL,
        GPIO107_DATAOUT,
        GPIO107_SMT,
    },
    {
        GPIO108_MODE,
        GPIO108_DIR,
        GPIO108_PULLEN,
        GPIO108_PULL,
        GPIO108_DATAOUT,
        GPIO108_SMT,
    },
    {
        GPIO109_MODE,
        GPIO109_DIR,
        GPIO109_PULLEN,
        GPIO109_PULL,
        GPIO109_DATAOUT,
        GPIO109_SMT,
    },
    {
        GPIO110_MODE,
        GPIO110_DIR,
        GPIO110_PULLEN,
        GPIO110_PULL,
        GPIO110_DATAOUT,
        GPIO110_SMT,
    },
    {
        GPIO111_MODE,
        GPIO111_DIR,
        GPIO111_PULLEN,
        GPIO111_PULL,
        GPIO111_DATAOUT,
        GPIO111_SMT,
    },
    {
        GPIO112_MODE,
        GPIO112_DIR,
        GPIO112_PULLEN,
        GPIO112_PULL,
        GPIO112_DATAOUT,
        GPIO112_SMT,
    },
    {
        GPIO113_MODE,
        GPIO113_DIR,
        GPIO113_PULLEN,
        GPIO113_PULL,
        GPIO113_DATAOUT,
        GPIO113_SMT,
    },
    {
        GPIO114_MODE,
        GPIO114_DIR,
        GPIO114_PULLEN,
        GPIO114_PULL,
        GPIO114_DATAOUT,
        GPIO114_SMT,
    },
    {
        GPIO115_MODE,
        GPIO115_DIR,
        GPIO115_PULLEN,
        GPIO115_PULL,
        GPIO115_DATAOUT,
        GPIO115_SMT,
    },
    {
        GPIO116_MODE,
        GPIO116_DIR,
        GPIO116_PULLEN,
        GPIO116_PULL,
        GPIO116_DATAOUT,
        GPIO116_SMT,
    },
    {
        GPIO117_MODE,
        GPIO117_DIR,
        GPIO117_PULLEN,
        GPIO117_PULL,
        GPIO117_DATAOUT,
        GPIO117_SMT,
    },
    {
        GPIO118_MODE,
        GPIO118_DIR,
        GPIO118_PULLEN,
        GPIO118_PULL,
        GPIO118_DATAOUT,
        GPIO118_SMT,
    },
    {
        GPIO119_MODE,
        GPIO119_DIR,
        GPIO119_PULLEN,
        GPIO119_PULL,
        GPIO119_DATAOUT,
        GPIO119_SMT,
    },
    {
        GPIO120_MODE,
        GPIO120_DIR,
        GPIO120_PULLEN,
        GPIO120_PULL,
        GPIO120_DATAOUT,
        GPIO120_SMT,
    },
    {
        GPIO121_MODE,
        GPIO121_DIR,
        GPIO121_PULLEN,
        GPIO121_PULL,
        GPIO121_DATAOUT,
        GPIO121_SMT,
    },
    {
        GPIO122_MODE,
        GPIO122_DIR,
        GPIO122_PULLEN,
        GPIO122_PULL,
        GPIO122_DATAOUT,
        GPIO122_SMT,
    },
    {
        GPIO123_MODE,
        GPIO123_DIR,
        GPIO123_PULLEN,
        GPIO123_PULL,
        GPIO123_DATAOUT,
        GPIO123_SMT,
    },
    {
        GPIO124_MODE,
        GPIO124_DIR,
        GPIO124_PULLEN,
        GPIO124_PULL,
        GPIO124_DATAOUT,
        GPIO124_SMT,
    },
    {
        GPIO125_MODE,
        GPIO125_DIR,
        GPIO125_PULLEN,
        GPIO125_PULL,
        GPIO125_DATAOUT,
        GPIO125_SMT,
    },
    {
        GPIO126_MODE,
        GPIO126_DIR,
        GPIO126_PULLEN,
        GPIO126_PULL,
        GPIO126_DATAOUT,
        GPIO126_SMT,
    },
    {
        GPIO127_MODE,
        GPIO127_DIR,
        GPIO127_PULLEN,
        GPIO127_PULL,
        GPIO127_DATAOUT,
        GPIO127_SMT,
    },
    {
        GPIO128_MODE,
        GPIO128_DIR,
        GPIO128_PULLEN,
        GPIO128_PULL,
        GPIO128_DATAOUT,
        GPIO128_SMT,
    },
    {
        GPIO129_MODE,
        GPIO129_DIR,
        GPIO129_PULLEN,
        GPIO129_PULL,
        GPIO129_DATAOUT,
        GPIO129_SMT,
    },
    {
        GPIO130_MODE,
        GPIO130_DIR,
        GPIO130_PULLEN,
        GPIO130_PULL,
        GPIO130_DATAOUT,
        GPIO130_SMT,
    },
    {
        GPIO131_MODE,
        GPIO131_DIR,
        GPIO131_PULLEN,
        GPIO131_PULL,
        GPIO131_DATAOUT,
        GPIO131_SMT,
    },
    {
        GPIO132_MODE,
        GPIO132_DIR,
        GPIO132_PULLEN,
        GPIO132_PULL,
        GPIO132_DATAOUT,
        GPIO132_SMT,
    },
    {
        GPIO133_MODE,
        GPIO133_DIR,
        GPIO133_PULLEN,
        GPIO133_PULL,
        GPIO133_DATAOUT,
        GPIO133_SMT,
    },
    {
        GPIO134_MODE,
        GPIO134_DIR,
        GPIO134_PULLEN,
        GPIO134_PULL,
        GPIO134_DATAOUT,
        GPIO134_SMT,
    },
    {
        GPIO135_MODE,
        GPIO135_DIR,
        GPIO135_PULLEN,
        GPIO135_PULL,
        GPIO135_DATAOUT,
        GPIO135_SMT,
    },
    {
        GPIO136_MODE,
        GPIO136_DIR,
        GPIO136_PULLEN,
        GPIO136_PULL,
        GPIO136_DATAOUT,
        GPIO136_SMT,
    },
    {
        GPIO137_MODE,
        GPIO137_DIR,
        GPIO137_PULLEN,
        GPIO137_PULL,
        GPIO137_DATAOUT,
        GPIO137_SMT,
    },
    {
        GPIO138_MODE,
        GPIO138_DIR,
        GPIO138_PULLEN,
        GPIO138_PULL,
        GPIO138_DATAOUT,
        GPIO138_SMT,
    },
    {
        GPIO139_MODE,
        GPIO139_DIR,
        GPIO139_PULLEN,
        GPIO139_PULL,
        GPIO139_DATAOUT,
        GPIO139_SMT,
    },
    {
        GPIO140_MODE,
        GPIO140_DIR,
        GPIO140_PULLEN,
        GPIO140_PULL,
        GPIO140_DATAOUT,
        GPIO140_SMT,
    },
    {
        GPIO141_MODE,
        GPIO141_DIR,
        GPIO141_PULLEN,
        GPIO141_PULL,
        GPIO141_DATAOUT,
        GPIO141_SMT,
    },
    {
        GPIO142_MODE,
        GPIO142_DIR,
        GPIO142_PULLEN,
        GPIO142_PULL,
        GPIO142_DATAOUT,
        GPIO142_SMT,
    },
    {
        GPIO143_MODE,
        GPIO143_DIR,
        GPIO143_PULLEN,
        GPIO143_PULL,
        GPIO143_DATAOUT,
        GPIO143_SMT,
    },
    {
        GPIO144_MODE,
        GPIO144_DIR,
        GPIO144_PULLEN,
        GPIO144_PULL,
        GPIO144_DATAOUT,
        GPIO144_SMT,
    },
    {
        GPIO145_MODE,
        GPIO145_DIR,
        GPIO145_PULLEN,
        GPIO145_PULL,
        GPIO145_DATAOUT,
        GPIO145_SMT,
    },
    {
        GPIO146_MODE,
        GPIO146_DIR,
        GPIO146_PULLEN,
        GPIO146_PULL,
        GPIO146_DATAOUT,
        GPIO146_SMT,
    },
    {
        GPIO147_MODE,
        GPIO147_DIR,
        GPIO147_PULLEN,
        GPIO147_PULL,
        GPIO147_DATAOUT,
        GPIO147_SMT,
    },
    {
        GPIO148_MODE,
        GPIO148_DIR,
        GPIO148_PULLEN,
        GPIO148_PULL,
        GPIO148_DATAOUT,
        GPIO148_SMT,
    },
    {
        GPIO149_MODE,
        GPIO149_DIR,
        GPIO149_PULLEN,
        GPIO149_PULL,
        GPIO149_DATAOUT,
        GPIO149_SMT,
    },
    {
        GPIO150_MODE,
        GPIO150_DIR,
        GPIO150_PULLEN,
        GPIO150_PULL,
        GPIO150_DATAOUT,
        GPIO150_SMT,
    },
    {
        GPIO151_MODE,
        GPIO151_DIR,
        GPIO151_PULLEN,
        GPIO151_PULL,
        GPIO151_DATAOUT,
        GPIO151_SMT,
    },
    {
        GPIO152_MODE,
        GPIO152_DIR,
        GPIO152_PULLEN,
        GPIO152_PULL,
        GPIO152_DATAOUT,
        GPIO152_SMT,
    },
    {
        GPIO153_MODE,
        GPIO153_DIR,
        GPIO153_PULLEN,
        GPIO153_PULL,
        GPIO153_DATAOUT,
        GPIO153_SMT,
    },
    {
        GPIO154_MODE,
        GPIO154_DIR,
        GPIO154_PULLEN,
        GPIO154_PULL,
        GPIO154_DATAOUT,
        GPIO154_SMT,
    },
    {
        GPIO155_MODE,
        GPIO155_DIR,
        GPIO155_PULLEN,
        GPIO155_PULL,
        GPIO155_DATAOUT,
        GPIO155_SMT,
    },
    {
        GPIO156_MODE,
        GPIO156_DIR,
        GPIO156_PULLEN,
        GPIO156_PULL,
        GPIO156_DATAOUT,
        GPIO156_SMT,
    },
    {
        GPIO157_MODE,
        GPIO157_DIR,
        GPIO157_PULLEN,
        GPIO157_PULL,
        GPIO157_DATAOUT,
        GPIO157_SMT,
    },
    {
        GPIO158_MODE,
        GPIO158_DIR,
        GPIO158_PULLEN,
        GPIO158_PULL,
        GPIO158_DATAOUT,
        GPIO158_SMT,
    },
    {
        GPIO159_MODE,
        GPIO159_DIR,
        GPIO159_PULLEN,
        GPIO159_PULL,
        GPIO159_DATAOUT,
        GPIO159_SMT,
    },
    {
        GPIO160_MODE,
        GPIO160_DIR,
        GPIO160_PULLEN,
        GPIO160_PULL,
        GPIO160_DATAOUT,
        GPIO160_SMT,
    },
    {
        GPIO161_MODE,
        GPIO161_DIR,
        GPIO161_PULLEN,
        GPIO161_PULL,
        GPIO161_DATAOUT,
        GPIO161_SMT,
    },
    {
        GPIO162_MODE,
        GPIO162_DIR,
        GPIO162_PULLEN,
        GPIO162_PULL,
        GPIO162_DATAOUT,
        GPIO162_SMT,
    },
    {
        GPIO163_MODE,
        GPIO163_DIR,
        GPIO163_PULLEN,
        GPIO163_PULL,
        GPIO163_DATAOUT,
        GPIO163_SMT,
    },
    {
        GPIO164_MODE,
        GPIO164_DIR,
        GPIO164_PULLEN,
        GPIO164_PULL,
        GPIO164_DATAOUT,
        GPIO164_SMT,
    },
    {
        GPIO165_MODE,
        GPIO165_DIR,
        GPIO165_PULLEN,
        GPIO165_PULL,
        GPIO165_DATAOUT,
        GPIO165_SMT,
    },
    {
        GPIO166_MODE,
        GPIO166_DIR,
        GPIO166_PULLEN,
        GPIO166_PULL,
        GPIO166_DATAOUT,
        GPIO166_SMT,
    },
    {
        GPIO167_MODE,
        GPIO167_DIR,
        GPIO167_PULLEN,
        GPIO167_PULL,
        GPIO167_DATAOUT,
        GPIO167_SMT,
    },
    {
        GPIO168_MODE,
        GPIO168_DIR,
        GPIO168_PULLEN,
        GPIO168_PULL,
        GPIO168_DATAOUT,
        GPIO168_SMT,
    },
    {
        GPIO169_MODE,
        GPIO169_DIR,
        GPIO169_PULLEN,
        GPIO169_PULL,
        GPIO169_DATAOUT,
        GPIO169_SMT,
    },
    {
        GPIO170_MODE,
        GPIO170_DIR,
        GPIO170_PULLEN,
        GPIO170_PULL,
        GPIO170_DATAOUT,
        GPIO170_SMT,
    },
    {
        GPIO171_MODE,
        GPIO171_DIR,
        GPIO171_PULLEN,
        GPIO171_PULL,
        GPIO171_DATAOUT,
        GPIO171_SMT,
    },
    {
        GPIO172_MODE,
        GPIO172_DIR,
        GPIO172_PULLEN,
        GPIO172_PULL,
        GPIO172_DATAOUT,
        GPIO172_SMT,
    },
    {
        GPIO173_MODE,
        GPIO173_DIR,
        GPIO173_PULLEN,
        GPIO173_PULL,
        GPIO173_DATAOUT,
        GPIO173_SMT,
    },
    {
        GPIO174_MODE,
        GPIO174_DIR,
        GPIO174_PULLEN,
        GPIO174_PULL,
        GPIO174_DATAOUT,
        GPIO174_SMT,
    },
    {
        GPIO175_MODE,
        GPIO175_DIR,
        GPIO175_PULLEN,
        GPIO175_PULL,
        GPIO175_DATAOUT,
        GPIO175_SMT,
    },
    {
        GPIO176_MODE,
        GPIO176_DIR,
        GPIO176_PULLEN,
        GPIO176_PULL,
        GPIO176_DATAOUT,
        GPIO176_SMT,
    },
    {
        GPIO177_MODE,
        GPIO177_DIR,
        GPIO177_PULLEN,
        GPIO177_PULL,
        GPIO177_DATAOUT,
        GPIO177_SMT,
    },
    {
        GPIO178_MODE,
        GPIO178_DIR,
        GPIO178_PULLEN,
        GPIO178_PULL,
        GPIO178_DATAOUT,
        GPIO178_SMT,
    },
    {
        GPIO179_MODE,
        GPIO179_DIR,
        GPIO179_PULLEN,
        GPIO179_PULL,
        GPIO179_DATAOUT,
        GPIO179_SMT,
    },
    {
        GPIO180_MODE,
        GPIO180_DIR,
        GPIO180_PULLEN,
        GPIO180_PULL,
        GPIO180_DATAOUT,
        GPIO180_SMT,
    },
    {
        GPIO181_MODE,
        GPIO181_DIR,
        GPIO181_PULLEN,
        GPIO181_PULL,
        GPIO181_DATAOUT,
        GPIO181_SMT,
    },
    {
        GPIO182_MODE,
        GPIO182_DIR,
        GPIO182_PULLEN,
        GPIO182_PULL,
        GPIO182_DATAOUT,
        GPIO182_SMT,
    },
    {
        GPIO183_MODE,
        GPIO183_DIR,
        GPIO183_PULLEN,
        GPIO183_PULL,
        GPIO183_DATAOUT,
        GPIO183_SMT,
    },
    {
        GPIO184_MODE,
        GPIO184_DIR,
        GPIO184_PULLEN,
        GPIO184_PULL,
        GPIO184_DATAOUT,
        GPIO184_SMT,
    },
    {
        GPIO185_MODE,
        GPIO185_DIR,
        GPIO185_PULLEN,
        GPIO185_PULL,
        GPIO185_DATAOUT,
        GPIO185_SMT,
    },
    {
        GPIO186_MODE,
        GPIO186_DIR,
        GPIO186_PULLEN,
        GPIO186_PULL,
        GPIO186_DATAOUT,
        GPIO186_SMT,
    },
    {
        GPIO187_MODE,
        GPIO187_DIR,
        GPIO187_PULLEN,
        GPIO187_PULL,
        GPIO187_DATAOUT,
        GPIO187_SMT,
    },
    {
        GPIO188_MODE,
        GPIO188_DIR,
        GPIO188_PULLEN,
        GPIO188_PULL,
        GPIO188_DATAOUT,
        GPIO188_SMT,
    },
    {
        GPIO189_MODE,
        GPIO189_DIR,
        GPIO189_PULLEN,
        GPIO189_PULL,
        GPIO189_DATAOUT,
        GPIO189_SMT,
    },
    {
        GPIO190_MODE,
        GPIO190_DIR,
        GPIO190_PULLEN,
        GPIO190_PULL,
        GPIO190_DATAOUT,
        GPIO190_SMT,
    },
    {
        GPIO191_MODE,
        GPIO191_DIR,
        GPIO191_PULLEN,
        GPIO191_PULL,
        GPIO191_DATAOUT,
        GPIO191_SMT,
    },
    {
        GPIO192_MODE,
        GPIO192_DIR,
        GPIO192_PULLEN,
        GPIO192_PULL,
        GPIO192_DATAOUT,
        GPIO192_SMT,
    },
    {
        GPIO193_MODE,
        GPIO193_DIR,
        GPIO193_PULLEN,
        GPIO193_PULL,
        GPIO193_DATAOUT,
        GPIO193_SMT,
    },
    {
        GPIO194_MODE,
        GPIO194_DIR,
        GPIO194_PULLEN,
        GPIO194_PULL,
        GPIO194_DATAOUT,
        GPIO194_SMT,
    },
    {
        GPIO195_MODE,
        GPIO195_DIR,
        GPIO195_PULLEN,
        GPIO195_PULL,
        GPIO195_DATAOUT,
        GPIO195_SMT,
    },
    {
        GPIO196_MODE,
        GPIO196_DIR,
        GPIO196_PULLEN,
        GPIO196_PULL,
        GPIO196_DATAOUT,
        GPIO196_SMT,
    },
    {
        GPIO197_MODE,
        GPIO197_DIR,
        GPIO197_PULLEN,
        GPIO197_PULL,
        GPIO197_DATAOUT,
        GPIO197_SMT,
    },
    {
        GPIO198_MODE,
        GPIO198_DIR,
        GPIO198_PULLEN,
        GPIO198_PULL,
        GPIO198_DATAOUT,
        GPIO198_SMT,
    },
    {
        GPIO199_MODE,
        GPIO199_DIR,
        GPIO199_PULLEN,
        GPIO199_PULL,
        GPIO199_DATAOUT,
        GPIO199_SMT,
    },
    {
        GPIO200_MODE,
        GPIO200_DIR,
        GPIO200_PULLEN,
        GPIO200_PULL,
        GPIO200_DATAOUT,
        GPIO200_SMT,
    },
    {
        GPIO201_MODE,
        GPIO201_DIR,
        GPIO201_PULLEN,
        GPIO201_PULL,
        GPIO201_DATAOUT,
        GPIO201_SMT,
    },
    {
        GPIO202_MODE,
        GPIO202_DIR,
        GPIO202_PULLEN,
        GPIO202_PULL,
        GPIO202_DATAOUT,
        GPIO202_SMT,
    },
    {
        GPIO203_MODE,
        GPIO203_DIR,
        GPIO203_PULLEN,
        GPIO203_PULL,
        GPIO203_DATAOUT,
        GPIO203_SMT,
    },
    {
        GPIO204_MODE,
        GPIO204_DIR,
        GPIO204_PULLEN,
        GPIO204_PULL,
        GPIO204_DATAOUT,
        GPIO204_SMT,
    },
    {
        GPIO205_MODE,
        GPIO205_DIR,
        GPIO205_PULLEN,
        GPIO205_PULL,
        GPIO205_DATAOUT,
        GPIO205_SMT,
    },
    {
        GPIO206_MODE,
        GPIO206_DIR,
        GPIO206_PULLEN,
        GPIO206_PULL,
        GPIO206_DATAOUT,
        GPIO206_SMT,
    },
    {
        GPIO207_MODE,
        GPIO207_DIR,
        GPIO207_PULLEN,
        GPIO207_PULL,
        GPIO207_DATAOUT,
        GPIO207_SMT,
    },
    {
        GPIO208_MODE,
        GPIO208_DIR,
        GPIO208_PULLEN,
        GPIO208_PULL,
        GPIO208_DATAOUT,
        GPIO208_SMT,
    },
    {
        GPIO209_MODE,
        GPIO209_DIR,
        GPIO209_PULLEN,
        GPIO209_PULL,
        GPIO209_DATAOUT,
        GPIO209_SMT,
    },
    {
        GPIO210_MODE,
        GPIO210_DIR,
        GPIO210_PULLEN,
        GPIO210_PULL,
        GPIO210_DATAOUT,
        GPIO210_SMT,
    },
    {
        GPIO211_MODE,
        GPIO211_DIR,
        GPIO211_PULLEN,
        GPIO211_PULL,
        GPIO211_DATAOUT,
        GPIO211_SMT,
    },
    {
        GPIO212_MODE,
        GPIO212_DIR,
        GPIO212_PULLEN,
        GPIO212_PULL,
        GPIO212_DATAOUT,
        GPIO212_SMT,
    },
    {
        GPIO213_MODE,
        GPIO213_DIR,
        GPIO213_PULLEN,
        GPIO213_PULL,
        GPIO213_DATAOUT,
        GPIO213_SMT,
    },
    {
        GPIO214_MODE,
        GPIO214_DIR,
        GPIO214_PULLEN,
        GPIO214_PULL,
        GPIO214_DATAOUT,
        GPIO214_SMT,
    },
    {
        GPIO215_MODE,
        GPIO215_DIR,
        GPIO215_PULLEN,
        GPIO215_PULL,
        GPIO215_DATAOUT,
        GPIO215_SMT,
    },
    {
        GPIO216_MODE,
        GPIO216_DIR,
        GPIO216_PULLEN,
        GPIO216_PULL,
        GPIO216_DATAOUT,
        GPIO216_SMT,
    },
    {
        GPIO217_MODE,
        GPIO217_DIR,
        GPIO217_PULLEN,
        GPIO217_PULL,
        GPIO217_DATAOUT,
        GPIO217_SMT,
    },
    {
        GPIO218_MODE,
        GPIO218_DIR,
        GPIO218_PULLEN,
        GPIO218_PULL,
        GPIO218_DATAOUT,
        GPIO218_SMT,
    },
    {
        GPIO219_MODE,
        GPIO219_DIR,
        GPIO219_PULLEN,
        GPIO219_PULL,
        GPIO219_DATAOUT,
        GPIO219_SMT,
    },
};
