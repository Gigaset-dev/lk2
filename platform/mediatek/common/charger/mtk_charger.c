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
#include <atm.h>
#include <debug.h>
#include <kernel/thread.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <mt_logo.h>
#include <mtk_battery.h>
#include <mtk_charger.h>
#include <platform.h>
#include <platform/boot_mode.h>
#include <platform/leds.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <pmic_auxadc.h>
#include <pmic_keys.h>
#include <set_fdt.h>
#include <sysenv.h>
#include <trace.h>
#include <gpio_api.h>
#include "mtk_charger_intf.h"
#include "sm5602_fg.h"

#define GPIO_INT_SIM2			GPIO24
//#define GPIO_VBUS_SWITCH		GPIO43
#define GPIO_DIO1567_SWITCH		GPIO154

#define LOCAL_TRACE 0

#define BOOT_TAG_CHR_INFO         0x88610018
#define BATTERY_ON_THRESHOLD      2500
#define CHARGER_ON_THRESHOLD      2500
#define MAX_SLEEP_LOOP            20
#define VBUS_CHECK_COUNT          3
#define LOOP_MAX_COUNT            86400

#define OFF_MODE_CHARGE "off-mode-charge"

static struct charger_custom_data chr_cust_data;
static int charger_type;

static void prize_get_gpio_state(void)
{
	int ret = 0;
	ret = mt_get_gpio_in(GPIO_INT_SIM2);
	dprintf(CRITICAL,"gezi %s:esim id:%d\n",__func__,ret);
//	ret = mt_get_gpio_out(GPIO_VBUS_SWITCH);
//	dprintf(CRITICAL,"gezi %s:vbus switch:%d\n",__func__,ret);
	
	if(!ret)
	{
		mt_set_gpio_mode(GPIO_DIO1567_SWITCH, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_DIO1567_SWITCH, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_DIO1567_SWITCH, GPIO_OUT_ZERO);
	}
	else
	{
		mt_set_gpio_mode(GPIO_DIO1567_SWITCH, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_DIO1567_SWITCH, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_DIO1567_SWITCH, GPIO_OUT_ONE);
	}
	
}

static uint32_t pl_boottags_get_charger_type(void)
{
    return charger_type;
}

static void pl_boottags_charger_type_hook(struct boot_tag *tag)
{
    memcpy(&charger_type, &tag->data, sizeof(charger_type));
}
PL_BOOTTAGS_INIT_HOOK(charger_type, BOOT_TAG_CHR_INFO, pl_boottags_charger_type_hook);

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_ATM
static void set_charger_atm(void *fdt)
{
    int offset = 0;
    bool atm_is_enabled = false;

    if (fdt != NULL) {
        offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,charger");

        if (offset >= 0) {
            atm_is_enabled = (get_atm_enable_status() != 0);
            if (atm_is_enabled)
                fdt_setprop(fdt, offset, "atm_is_enabled", &atm_is_enabled,
                    sizeof(atm_is_enabled));

            dprintf(CRITICAL, "%s: atm_is_enabled:%d to dtsi!\n",
                __func__, atm_is_enabled);
        } else {
            dprintf(CRITICAL, "%s: charger: no device tree!\n", __func__);
        }
    } else {
        dprintf(CRITICAL, "%s: fdt is null!\n", __func__);
    }
}

SET_FDT_INIT_HOOK(set_charger_atm, set_charger_atm);
#endif

void set_off_mode_charge_status(int enable_charge)
{
    static char *env_buf;

    env_buf = (char *)malloc(sizeof(int));
    if (!env_buf) {
        dprintf(CRITICAL, "off-mode-charge malloc failed\n");
        return;
    }
    memset(env_buf, 0x00, sizeof(int));

    sprintf(env_buf, "%d", enable_charge);
    if (set_env(OFF_MODE_CHARGE, env_buf) != 0)
        dprintf(CRITICAL, "set off-mode-charge failed\n");

    free(env_buf);
    return;
}

int get_off_mode_charge_status(void)
{
     char *charge_buf = NULL;
     int enable_charge = 1;

     charge_buf = get_env(OFF_MODE_CHARGE);

     if (charge_buf == NULL) {
          set_off_mode_charge_status(1);
          return 1;
     }
     enable_charge = atoi(charge_buf);
     return enable_charge;
}

static bool chr_fdt_getprop_bool(const void *fdt, int nodeoffset,
                        const char *name)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (data)
        return true;
    else
        return false;
}

static unsigned int chr_fdt_getprop_u32(const void *fdt, int nodeoffset,
                                 const char *name)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        return fdt32_to_cpu(*(unsigned int *)data);
    else
        return 0;
}

static void init_charger_custom_data(void)
{
    chr_cust_data.disable_charger = false;
    chr_cust_data.power_path_support = true;
    chr_cust_data.enable_pe_plus = true;
    chr_cust_data.boot_battery_voltage = 3450;
    chr_cust_data.max_charger_voltage = 6500;
    chr_cust_data.min_charger_voltage = 4000;
    chr_cust_data.fast_charge_voltage = 3000;

    /* charging current */
    chr_cust_data.usb_charger_current = 500;
    chr_cust_data.ac_charger_current = 2050;
    chr_cust_data.ac_charger_input_current = 3200;
    chr_cust_data.non_std_ac_charger_current = 500;
    chr_cust_data.charging_host_charger_current = 500;
    chr_cust_data.ta_ac_charger_current = 3000;
    chr_cust_data.pd_charger_current = 500;

    /* temperature protection ref sw jeita */
    chr_cust_data.temp_t4_threshold = 50;
    chr_cust_data.temp_t3_threshold = 45;
    chr_cust_data.temp_t1_threshold = 0;

    /* charging anime */
    chr_cust_data.enable_anime = true;
    chr_cust_data.led_brightness = LED_DIM;
    chr_cust_data.blinking_times = 6;
    chr_cust_data.blinking_period = 1500;

    /* vbus resistance */
    chr_cust_data.r_charger_1 = 330;
    chr_cust_data.r_charger_2 = 39;

    /*vsys threshold*/
    chr_cust_data.min_vsys_voltage = 4000;
    chr_cust_data.enable_check_vsys = false;

    chr_cust_data.enable_power_measure = false;

    chr_cust_data.battery_volt_is_low = false;
}

static int init_cust_data_from_dt(void *fdt)
{
    int offset, val;

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,lk_charger");

    if (offset >= 0) {
        val = chr_fdt_getprop_bool(fdt, offset, "disable_charger");
        if (val)
            chr_cust_data.disable_charger = true;
        else
            chr_cust_data.disable_charger = false;

        val = chr_fdt_getprop_bool(fdt, offset, "enable_pe_plus");
        if (val)
            chr_cust_data.enable_pe_plus = true;
        else
            chr_cust_data.enable_pe_plus = false;

        val = chr_fdt_getprop_bool(fdt, offset, "power_path_support");
        if (val)
            chr_cust_data.power_path_support = true;
        else
            chr_cust_data.power_path_support = false;

        val = chr_fdt_getprop_u32(fdt, offset, "boot_battery_voltage");
        if (val)
            chr_cust_data.boot_battery_voltage = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "max_charger_voltage");
        if (val)
            chr_cust_data.max_charger_voltage = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "min_charger_voltage");
        if (val)
            chr_cust_data.min_charger_voltage = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "fast_charge_voltage");
        if (val)
            chr_cust_data.fast_charge_voltage = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "usb_charger_current");
        if (val)
            chr_cust_data.usb_charger_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "ac_charger_current");
        if (val)
            chr_cust_data.ac_charger_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "ac_charger_input_current");
        if (val)
            chr_cust_data.ac_charger_input_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "non_std_ac_charger_current");
        if (val)
            chr_cust_data.non_std_ac_charger_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "charging_host_charger_current");
        if (val)
            chr_cust_data.charging_host_charger_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "ta_ac_charger_current");
        if (val)
            chr_cust_data.ta_ac_charger_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "pd_charger_current");
        if (val)
            chr_cust_data.pd_charger_current = val / 1000;

        val = chr_fdt_getprop_u32(fdt, offset, "temp_t4_threshold");
        if (val)
            chr_cust_data.temp_t4_threshold = val;

        val = chr_fdt_getprop_u32(fdt, offset, "temp_t3_threshold");
        if (val)
            chr_cust_data.temp_t3_threshold = val;

        val = chr_fdt_getprop_u32(fdt, offset, "temp_t1_threshold");
        if (val)
            chr_cust_data.temp_t1_threshold = val;

        val = chr_fdt_getprop_bool(fdt, offset, "enable_anime");
        if (val)
            chr_cust_data.enable_anime = true;
        else
            chr_cust_data.enable_anime = false;

        val = chr_fdt_getprop_u32(fdt, offset, "led_brightness");
        if (val)
            chr_cust_data.led_brightness = val;

        val = chr_fdt_getprop_u32(fdt, offset, "blinking_times");
        if (val)
            chr_cust_data.blinking_times = val;

        val = chr_fdt_getprop_u32(fdt, offset, "blinking_period");
        if (val)
            chr_cust_data.blinking_period = val;

        val = chr_fdt_getprop_u32(fdt, offset, "r_charger_1");
        if (val)
            chr_cust_data.r_charger_1 = val;

        val = chr_fdt_getprop_u32(fdt, offset, "r_charger_2");
        if (val)
            chr_cust_data.r_charger_2 = val;

        val = chr_fdt_getprop_bool(fdt, offset, "enable_check_vsys");
        if (val)
            chr_cust_data.enable_check_vsys = true;
        else
            chr_cust_data.enable_check_vsys = false;

        val = chr_fdt_getprop_bool(fdt, offset, "enable_power_measure");
        if (val)
            chr_cust_data.enable_power_measure = true;
        else
            chr_cust_data.enable_power_measure = false;

        LTRACEF("%s: chroff:%d,pe:%d,powpath:%d,vchrmax:%d,vchrmin:%d,vfast:%d\n", __func__,
            chr_cust_data.disable_charger, chr_cust_data.enable_pe_plus,
            chr_cust_data.power_path_support, chr_cust_data.max_charger_voltage,
            chr_cust_data.min_charger_voltage, chr_cust_data.fast_charge_voltage);
        LTRACEF("%s: usb:%d,ac:%d %d,nac:%d,cdp:%d,ta:%d,pd:%d,t:%d %d %d\n", __func__,
            chr_cust_data.usb_charger_current, chr_cust_data.ac_charger_current,
            chr_cust_data.ac_charger_input_current, chr_cust_data.non_std_ac_charger_current,
            chr_cust_data.charging_host_charger_current, chr_cust_data.ta_ac_charger_current,
            chr_cust_data.pd_charger_current, chr_cust_data.temp_t4_threshold,
            chr_cust_data.temp_t3_threshold, chr_cust_data.temp_t1_threshold);

        return 0;
    } else {
        dprintf(INFO, "%s: lk_charger is not found in dts!\n", __func__);
        return 1;
    }
}

void charger_init(void *fdt)
{
    int ret = 0;

    ret = mtk_charger_init();
    if (ret < 0)
        LTRACEF("%s: mtk_charger_init failed, ret = %d\n", __func__, ret);

    /* set default values */
    init_charger_custom_data();

    if (fdt) {
        if (fdt_check_header(fdt) != 0)
            panic("Bad DTB header.\n");

        /* override default values by dts */
        init_cust_data_from_dt(fdt);
    } else
        dprintf(CRITICAL, "[chg] fdt null, fall back to default\n");

    LTRACEF("%s: done\n", __func__);
}

void charger_enable_charging(bool enable)
{
    int ret = 0;

    ret = mtk_charger_enable_charging(enable);
    if (ret < 0)
        LTRACEF("%s: enable/disable charging failed, ret = %d\n", __func__, ret);
    else {
        if (enable)
            LTRACEF("%s: enable charging\n", __func__);
        else
            LTRACEF("%s: disable charging\n", __func__);
    }
}

void charger_enable_power_path(bool enable)
{
    int ret = 0;

    ret = mtk_charger_enable_power_path(enable);
    if (ret < 0)
        LTRACEF("%s: enable/disable power path failed, ret = %d\n", __func__, ret);
}

int charger_check_battery_plugout_once(bool *plugout_once)
{
    return mtk_charger_check_battery_plugout_once(plugout_once);
}

static void chr_power_off(void)
{
    platform_halt(HALT_ACTION_SHUTDOWN, HALT_REASON_LOWVOLTAGE);
}

static int pmic_get_auxadc_value(int channel, enum auxadc_val_type type)
{
    int ret, val = 0;

    ret = pmic_auxadc_read_value_ext(channel, &val, type);
    if (ret < 0)
        return ret;
    return val;
}

static int get_bat_volt(void)
{
	int ret = 0;
	ret = fg_read_volt();
	if(ret > 0){
		return ret;
	}
	else{
		return pmic_get_auxadc_value(AUXADC_CHAN_BATADC, AUXADC_VAL_PROCESSED);
	}
}

static int get_vsys_volt(void)
{
    struct auxadc *main_pmic;
    int val = 0;

    main_pmic = pmic_auxadc_get_by_name("main_pmic");
    pmic_auxadc_read_value(main_pmic, AUXADC_CHAN_VSYSSNS, &val, AUXADC_VAL_PROCESSED);

    return val;
}

static unsigned int get_chr_volt(void)
{
    unsigned int val = 0;

    mtk_charger_get_vbus(&val);
    val /= 1000;

    return val;
}

static void select_charging_current_limit(int chr_type)
{
    int input_current_limit;
    int charging_current_limit;

    if (chr_type == STANDARD_HOST) {
        input_current_limit = chr_cust_data.usb_charger_current;
        charging_current_limit = chr_cust_data.usb_charger_current;
    } else if (chr_type == NONSTANDARD_CHARGER) {
        input_current_limit = chr_cust_data.non_std_ac_charger_current;
        charging_current_limit = chr_cust_data.non_std_ac_charger_current;
    } else if (chr_type == STANDARD_CHARGER) {
        input_current_limit = chr_cust_data.ac_charger_input_current;
        charging_current_limit = chr_cust_data.ac_charger_current;
    } else if (chr_type == CHARGING_HOST) {
        input_current_limit = chr_cust_data.charging_host_charger_current;
        charging_current_limit = chr_cust_data.charging_host_charger_current;
    } else {
        input_current_limit = chr_cust_data.usb_charger_current;
        charging_current_limit = chr_cust_data.usb_charger_current;
    }

    mtk_charger_set_aicr(input_current_limit);
    mtk_charger_set_ichg(charging_current_limit);
}

static void reset_default_charging_current_limit(void)
{
    int input_current_limit;
    int charging_current_limit;

    input_current_limit = chr_cust_data.usb_charger_current;
    charging_current_limit = chr_cust_data.usb_charger_current;

    mtk_charger_set_aicr(input_current_limit);
    mtk_charger_set_ichg(charging_current_limit);
}

static void show_plug_out_notify(void)
{
    leds_on();
    mt_disp_show_charger_ov_logo();
    spin(4000000);
    leds_off();
    mt_disp_clear_screen(0);
}

static void show_low_battery_notify(void)
{
    leds_set(chr_cust_data.led_brightness);
    mt_disp_show_plug_charger();
    spin(4000000);
    leds_off();
    mt_disp_clear_screen(0);
}

static void check_charger_volt(void)
{
    int i;
    int chr_volt;

    for (i = 0; i < VBUS_CHECK_COUNT; i++) {
        chr_volt = get_chr_volt();
        if (chr_volt < chr_cust_data.min_charger_voltage) {
            LTRACEF("%s: vbus %d is less than %dmv, power off\n", __func__,
                    chr_volt, chr_cust_data.min_charger_voltage);
            chr_power_off();
        }
    }

    if (chr_cust_data.power_path_support == true) {
        chr_volt = get_chr_volt();
        if (chr_volt > chr_cust_data.max_charger_voltage) {
            LTRACEF("%s: vbus %d over max voltage %d, power off\n",  __func__,
                    chr_volt, chr_cust_data.max_charger_voltage);
            show_plug_out_notify();
            chr_power_off();
        }
    }
}

static void show_charging_anime(void)
{
    int i, j;
    bool stop = false;
    /* set LCD brightness */
    leds_set(chr_cust_data.led_brightness);

    /* set blinking times */
    for (i = 0; i < chr_cust_data.blinking_times && !stop; i++) {
        mtk_wdt_restart_timer();
        mt_disp_show_charging(i % 2);

        for (j = 0; j < 2 && !stop; j++) {
            /* set blinking period */
            spin(chr_cust_data.blinking_period * 1000);
            check_charger_volt();
            if (get_pmic_key_event_status(POWER_KEY)) {
                clr_pmic_key_event_status(POWER_KEY);
                stop = true;
            }
        }
    }
    leds_off();
    mt_disp_clear_screen(0);
    mtk_charger_reset_wdt();
}

static bool is_power_path_supported(void)
{
    return chr_cust_data.power_path_support;
}

static bool is_disable_charger(void)
{
    return chr_cust_data.disable_charger;
}

static bool is_battery_on(void)
{
    if (get_bat_volt() < BATTERY_ON_THRESHOLD)
        return false;
    else
        return true;
}

static bool is_low_battery(int val)
{
    bool bat_low = false;

    if (val < chr_cust_data.boot_battery_voltage)
        bat_low = true;
    else
        bat_low = false;

    /* The battery voltage low than boot voltage plus 200mV */
    if (val < (chr_cust_data.boot_battery_voltage + 200))
        chr_cust_data.battery_volt_is_low = true;

    dprintf(INFO, "%s: %d vbat:%d th:%d\n", __func__,
            bat_low, val, chr_cust_data.boot_battery_voltage);

    return bat_low;
}

/*
 * When is_battery_voltage_low() returns true, it means the battery
 * voltage is too low to enable dvfs at high frequency safely.
 */
bool is_battery_voltage_low(void)
{
    return chr_cust_data.battery_volt_is_low;
}

static void check_charging_status(void)
{
    int i, ret = 0;
    int bat_val = 0;
    int temperature = 0;
    int bat_current = 0;
    bool curr_sign = false;
    static bool is_first = true;
    int chr_type = pl_boottags_get_charger_type();

    dprintf(INFO, "%s: charger_type: %d\n", __func__, chr_type);

    ret = mtk_charger_enable_charging(false);
    if (ret < 0)
        LTRACEF("%s: disable charging failed, ret = %d\n", __func__, ret);

    bat_val = get_bat_volt();
    LTRACEF("%s: check VBAT=%d mV with %d mV\n", __func__,
            bat_val, chr_cust_data.boot_battery_voltage);

    clr_pmic_key_event_status(POWER_KEY);

    while (bat_val < chr_cust_data.boot_battery_voltage) {
        mtk_wdt_restart_timer();
        mtk_charger_reset_wdt();
        mtk_charger_check_charging_mode();

        if (LOCAL_TRACE)
            mtk_charger_dump_register();

        check_charger_volt();

        temperature = force_get_tbat(true);
        LTRACEF("%s: T=%d\n", __func__, temperature);
        if (temperature > chr_cust_data.temp_t4_threshold) {
            LTRACEF("%s: Battery over Temperature or NTC fail %d %d!!\n\r",
                    __func__, temperature, chr_cust_data.temp_t4_threshold);
            break;
        }

        if (bat_val < chr_cust_data.fast_charge_voltage ||
                temperature > chr_cust_data.temp_t3_threshold ||
                temperature < chr_cust_data.temp_t1_threshold)
            reset_default_charging_current_limit();
        else
            select_charging_current_limit(chr_type);

        ret = mtk_charger_enable_charging(true);
        if (ret < 0)
            LTRACEF("%s: enable charging failed, ret = %d\n", __func__, ret);

        if (chr_cust_data.enable_anime && bat_val > chr_cust_data.fast_charge_voltage) {
            if (is_first) {
                show_charging_anime();
                is_first = 0;
            }
            for (i = 0; i < MAX_SLEEP_LOOP; i++) {
                mtk_wdt_restart_timer();
                check_charger_volt();
                /* set polling period */
                spin(1000000);
                if (get_pmic_key_event_status(POWER_KEY)) {
                    clr_pmic_key_event_status(POWER_KEY);
                    show_charging_anime();
                }
            }
        } else {
            for (i = 0; i < MAX_SLEEP_LOOP; i++) {
                mtk_wdt_restart_timer();
                check_charger_volt();
                /* set polling period */
                spin(1000000);
            }
        }

        if (is_battery_on()) {
            gauge_get_current(&curr_sign, &bat_current);
            bat_current = bat_current / 10;
            dprintf(INFO, "%s: IBAT=%d\n", __func__, curr_sign ? bat_current : -1 * bat_current);
        } else {
            dprintf(INFO, "%s: no battery, exit charging\n", __func__);
            break;
        }

        if (chr_type == STANDARD_CHARGER && bat_val > chr_cust_data.fast_charge_voltage) {
            ret = mtk_charger_enable_charging(false);
            if (ret < 0)
                LTRACEF("%s: disable charging failed, ret = %d\n", __func__, ret);
        }

        bat_val = get_bat_volt();
        dprintf(INFO, "%s: check VBAT=%d mV with %d mV, start charging...\n", __func__,
                bat_val, chr_cust_data.boot_battery_voltage);
    }
    mtk_wdt_restart_timer();
    reset_default_charging_current_limit();
}

static void check_low_battery(void)
{
    int bat_vol;
    int chr_vol;
    int vsys_vol;
	
	prize_get_gpio_state();
	
	fg_init();
	
    bat_vol = get_bat_volt();
    chr_vol = get_chr_volt();
    vsys_vol = get_vsys_volt();
    LTRACEF("%s: check vsys=%d mV\n", __func__, vsys_vol);
	
	dprintf(CRITICAL,"%s:bat_vol = %d,chr_vol = %d,vsys_vol = %d\n",__func__,bat_vol,chr_vol,vsys_vol);

    if (chr_cust_data.enable_check_vsys == true) {
        if (bat_vol < BATTERY_ON_THRESHOLD &&
            vsys_vol > chr_cust_data.min_vsys_voltage) {
            dprintf(INFO, "%s: bypass vbat check\n", __func__);
            return;
        }
    }

    LTRACEF("%s: vbat:%d vbus:%d\n", __func__, bat_vol, chr_vol);

    if (is_low_battery(bat_vol)) {
        if (chr_vol > CHARGER_ON_THRESHOLD){
            check_charging_status();
		}
        else {
            LTRACEF("%s: no charger, power off\n", __func__);
            show_low_battery_notify();
            chr_power_off();
        }
    }
}

static void power_measure_loop(void)
{
    int bat_current = 0, cnt = 0;
    bool curr_sign = false;

    while (cnt < LOOP_MAX_COUNT) {
        mtk_wdt_restart_timer();
        mtk_charger_reset_wdt();
        spin(1000000);
        dprintf(INFO, "%s: loop in lk2 charger start.\n", __func__);
        gauge_get_current(&curr_sign, &bat_current);
        bat_current = bat_current / 10;
        dprintf(INFO, "%s: IBAT=%d\n", __func__, curr_sign ? bat_current : -1 * bat_current);
        cnt++;
    }
}

void mtk_charger_start(void)
{
    if (chr_cust_data.enable_power_measure)
        power_measure_loop();

    if (chr_cust_data.disable_charger == false)
        check_low_battery();
    else
        dprintf(INFO, "%s: skip mtk_charger_start\n", __func__);

    LTRACEF("%s: end\n", __func__);
}

bool mtk_charger_kpoc_check(void)
{
    uint32_t boot_mode = platform_get_boot_mode();
    uint32_t boot_reason = platform_get_boot_reason();
    int off_mode_status = 1;

    LTRACEF("platform_get_boot_mode = %d\n", boot_mode);
    LTRACEF("platform_get_boot_reason = %d\n", boot_reason);

    off_mode_status = get_off_mode_charge_status();
    if (off_mode_status == 0)
        return false;

    if (boot_mode == NORMAL_BOOT && boot_reason == BR_USB)
        return true;

    return false;
}
