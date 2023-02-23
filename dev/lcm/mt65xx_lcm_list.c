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

#include <lcm_drv.h>
//#include <platform/disp_drv_platform.h>

/* used to identify float ID PIN status */
#define LCD_HW_ID_STATUS_LOW      0
#define LCD_HW_ID_STATUS_HIGH     1
#define LCD_HW_ID_STATUS_FLOAT 0x02
#define LCD_HW_ID_STATUS_ERROR  0x03

#define LCD_DEBUG(fmt)  dprintf(CRITICAL, fmt)

LCM_DRIVER *lcm_driver_list[] = {

//prize hjw  for panel 20220606 start
#if defined(TD4310_FHD2160_DSI_VDO_DZX)
	&td4310_fhd2160_dsi_vdo_dzx_lcm_drv,
#endif

#if defined(R63417_FHD_DSI_CMD_SHARP55)
	&r63417_fhd_dsi_cmd_sharp55_lcm_drv,
#endif
#if defined(ILI9882Q_HDP_DSI_VDO_TRULY6517)
	&ili9882q_hdp_dsi_vdo_truly6517_lcm_drv,
#endif
#if defined(ILI9882N_HDP_DSI_VDO_TRULY6517)
	&ili9882n_hdp_dsi_vdo_truly6517_lcm_drv,
#endif
#if defined(HX8399C_FHD_DSI_VDO_565)
	&hx8399c_fhd_dsi_vdo_565_lcm_drv,
#endif
#if defined(FT8756_HDP_DSI_VDO_CSOT67)
	&ft8756_hdp_dsi_vdo_csot67_lcm_drv,
#endif
#if defined(FT8756_HDP_DSI_VDO_CSOT67_WCL)
	&ft8756_hdp_dsi_vdo_csot67_wcl_lcm_drv,
#endif
#if defined(FT8756_HDP_DSI_VDO_CSOT67_XBD)
	&ft8756_hdp_dsi_vdo_csot67_xbd_lcm_drv,
#endif
//prize hjw  for panel 20220606 end
#if defined(HX83112B_FHDP_DSI_CMD_AUO_RT4801)
    &hx83112b_fhdp_dsi_cmd_auo_rt4801_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_120HZ_JDI_DPHY)
    &nt36672e_fhdp_dsi_vdo_120hz_jdi_dphy_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_144HZ_JDI_DPHY)
    &nt36672e_fhdp_dsi_vdo_144hz_jdi_dphy_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_60HZ_JDI_DPHY)
    &nt36672e_fhdp_dsi_vdo_60hz_jdi_dphy_lcm_drv,
#endif

#if defined(NT37701A_FHDP_TIANMA_DSI_CMD_DPHY_144HZ)
    &nt37701a_fhdp_tianma_dsi_cmd_dphy_144hz_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_120HZ_JDI_DPHY_HFP)
    &nt36672e_fhdp_dsi_vdo_120hz_jdi_dphy_hfp_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_144HZ_JDI_DPHY_HFP)
    &nt36672e_fhdp_dsi_vdo_144hz_jdi_dphy_hfp_lcm_drv,
#endif

#if defined(NT37701_FHDP_HX_DSI_CMD_DPHY)
    &nt37701_fhdp_hx_dsi_cmd_dphy_lcm_drv,
#endif

#if defined(NT37701_FHDP_HX_DSI_CMD_DPHY_120HZ)
    &nt37701_fhdp_hx_dsi_cmd_dphy_120hz_lcm_drv,
#endif

#if defined(TD4330_FHDP_DSI_VDO_AUO_RT5081)
    &td4330_fhdp_dsi_vdo_auo_rt5081_lcm_drv,
#endif

#if defined(TD4330_FHDP_DSI_CMD_AUO_RT5081)
    &td4330_fhdp_dsi_cmd_auo_rt5081_lcm_drv,
#endif

#if defined(FT8756_FHDP_DSI_VDO_AUO_RT4801)
    &ft8756_fhdp_dsi_vdo_auo_rt4801_lcm_drv,
#endif

#if defined(NT36672C_FHDP_DSI_VDO_CPHY_90HZ_TIANMA_RT4801)
    &nt36672c_fhdp_dsi_vdo_cphy_90hz_tianma_rt4801_lcm_drv,
#endif

#if defined(ANA6705_FHDP_SAMSUNG_DSI_CMD_DPHY)
    &ana6705_fhdp_samsung_dsi_cmd_dphy_lcm_drv,
#endif

#if defined(ANA6705_FHDP_SAMSUNG_DSI_CMD_DPHY_V2)
    &ana6705_fhdp_samsung_dsi_cmd_dphy_lcm_drv_v2,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358)
    &nt35595_fhd_dsi_cmd_truly_nt50358_lcm_drv,
#endif

#if defined(R66451_FHDP_DSI_CMD_TIANMA)
    &r66451_fhdp_dsi_cmd_tianma_lcm_drv,
#endif

#if defined(R66451_FHDP_DSI_CMD_TIANMA_WA)
    &r66451_fhdp_dsi_cmd_tianma_wa_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_120HZ_TIANMA_HFP)
    &nt36672e_fhdp_dsi_vdo_120hz_tianma_hfp_lcm_drv,
#endif

#if defined(NT36672E_FHDP_DSI_VDO_120HZ_JDI_CPHY)
    &nt36672e_fhdp_dsi_vdo_120hz_jdi_cphy_lcm_drv,
#endif
//prize add by lipengpeng 20220811 start 
#if defined(NT36672EGMS_FHDP_DSI_VDO_120HZ_JDI_CPHY)
    &nt36672egms_fhdp_dsi_vdo_120hz_jdi_cphy_lcm_drv,
#endif
//prize add by lipengpeng 20220811 end 
#if defined(ANA6705_WQHD_SAMSUNG_DSI_CMD_DPHY)
    &ana6705_wqhd_samsung_dsi_cmd_dphy_lcm_drv,
#endif

#if defined(ANA6705_WQHD_SAMSUNG_DSI_CMD_DPHY_MSYNC2)
    &ana6705_wqhd_samsung_dsi_cmd_dphy_msync2_lcm_drv,
#endif

#if defined(S68FC01_FHDP_VDO_SAMSUNG_AOD)
    &s68fc01_fhdp_vdo_samsung_aod_lcm_drv,
#endif

#if defined(S6E3HAE_FHD_CMD_SAMSUNG_120HZ_R4831A)
    &s6e3hae_fhd_cmd_samsung_120hz_r4831a_lcm_drv,
#endif

#if defined(S6E3HAE_WQHD_CMD_SAMSUNG_120HZ)
    &s6e3hae_wqhd_cmd_samsung_120hz_lcm_drv,
#endif

#if defined(K6895V1_64_95T_ALPHA)
    &k6895v1_64_95t_alpha_lcm_drv,
#endif

#if defined(NT37801_FHDPLUS_CMD_120HZ_R4831A)
    &nt37801_fhdplus_cmd_120hz_r4831a_lcm_drv,
#endif

};

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
unsigned char lcm_name_list[][128] = {
#if defined(HX8392A_DSI_CMD)
    "hx8392a_dsi_cmd",
#endif

#if defined(HX8392A_DSI_VDO)
    "hx8392a_vdo_cmd",
#endif

#if defined(OTM9608_QHD_DSI_CMD)
    "otm9608a_qhd_dsi_cmd",
#endif

#if defined(OTM9608_QHD_DSI_VDO)
    "otm9608a_qhd_dsi_vdo",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358)
    "r63417_fhd_dsi_cmd_truly_nt50358_drv",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_HDPLUS)
    "r63417_fhd_dsi_cmd_truly_nt50358_hdplus_drv",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_FWVGA)
    "r63417_fhd_dsi_cmd_truly_nt50358_fwvga_drv",
#endif

#if defined(R63417_FHD_DSI_VDO_TRULY_NT50358)
    "r63417_fhd_dsi_vdo_truly_nt50358_drv",
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_CMD_OK_WS3142)
    "r63419_wqhd_truly_phantom_2k_cmd_ok",
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_NT50358)
    "nt35695_fhd_dsi_cmd_truly_nt50358_drv",
#endif

#if defined(NT35521_HD_DSI_VDO_TRULY_RT5081)
    "nt35521_hd_dsi_vdo_truly_rt5081_drv",
#endif

#if defined(K6895V1_64_95T_ALPHA)
    "k6895v1_64_95t_alpha_lcm_drv"
#endif


};
#endif

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition)?1 :  -1]

unsigned int lcm_count = sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *));
#if defined(NT35520_HD720_DSI_CMD_TM) | defined(NT35520_HD720_DSI_CMD_BOE) | defined(NT35521_HD720_DSI_VDO_BOE) | defined(NT35521_HD720_DSI_VIDEO_TM)
extern void mdelay(unsigned long msec);
static unsigned char lcd_id_pins_value = 0xFF;


/******************************************************************************
Function:       which_lcd_module_triple
  Description:    read LCD ID PIN status,could identify three status:highlowfloat
  Input:           none
  Output:         none
  Return:         LCD ID1|ID0 value
  Others:
******************************************************************************/
unsigned char which_lcd_module_triple(void)
{
    unsigned char high_read0 = 0;
    unsigned char low_read0 = 0;
    unsigned char high_read1 = 0;
    unsigned char low_read1 = 0;
    unsigned char lcd_id0 = 0;
    unsigned char lcd_id1 = 0;
    unsigned char lcd_id = 0;
    /* Solve Coverity scan warning : check return value */
    unsigned int ret = 0;
    /* only recognise once */
    if (0xFF != lcd_id_pins_value) {
        return lcd_id_pins_value;
    }
    /* Solve Coverity scan warning : check return value */
    ret = mt_set_gpio_mode(GPIO_DISP_ID0_PIN, GPIO_MODE_00);
    if (0 != ret) {
        LCD_DEBUG("ID0 mt_set_gpio_mode fail\n");
    }
    ret = mt_set_gpio_dir(GPIO_DISP_ID0_PIN, GPIO_DIR_IN);
    if (0 != ret) {
        LCD_DEBUG("ID0 mt_set_gpio_dir fail\n");
    }
    ret = mt_set_gpio_pull_enable(GPIO_DISP_ID0_PIN, GPIO_PULL_ENABLE);
    if (0 != ret) {
        LCD_DEBUG("ID0 mt_set_gpio_pull_enable fail\n");
    }
    ret = mt_set_gpio_mode(GPIO_DISP_ID1_PIN, GPIO_MODE_00);
    if (0 != ret) {
        LCD_DEBUG("ID1 mt_set_gpio_mode fail\n");
    }
    ret = mt_set_gpio_dir(GPIO_DISP_ID1_PIN, GPIO_DIR_IN);
    if (0 != ret) {
        LCD_DEBUG("ID1 mt_set_gpio_dir fail\n");
    }
    ret = mt_set_gpio_pull_enable(GPIO_DISP_ID1_PIN, GPIO_PULL_ENABLE);
    if (0 != ret) {
        LCD_DEBUG("ID1 mt_set_gpio_pull_enable fail\n");
    }
    /* pull down ID0 ID1 PIN */
    ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
    if (0 != ret) {
        LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");
    }
    ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
    if (0 != ret) {
        LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");
    }
    /* delay 100ms , for discharging capacitance */
    mdelay(100);
    /* get ID0 ID1 status */
    low_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
    low_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);
    /* pull up ID0 ID1 PIN */
    ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
    if (0 != ret) {
        LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");
    }
    ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
    if (0 != ret) {
        LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");
    }
    /* delay 100ms , for charging capacitance */
    mdelay(100);
    /* get ID0 ID1 status */
    high_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
    high_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);

    if (low_read0 != high_read0) {
        /*float status , pull down ID0 ,to prevent electric leakage */
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
        if (0 != ret) {
            LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");
        }
        lcd_id0 = LCD_HW_ID_STATUS_FLOAT;
    } else if ((LCD_HW_ID_STATUS_LOW == low_read0) && (LCD_HW_ID_STATUS_LOW == high_read0)) {
        /*low status , pull down ID0 ,to prevent electric leakage */
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
        if (0 != ret) {
            LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");
        }
        lcd_id0 = LCD_HW_ID_STATUS_LOW;
    } else if ((LCD_HW_ID_STATUS_HIGH == low_read0) && (LCD_HW_ID_STATUS_HIGH == high_read0)) {
        /*high status , pull up ID0 ,to prevent electric leakage */
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
        if (0 != ret) {
            LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");
        }
        lcd_id0 = LCD_HW_ID_STATUS_HIGH;
    } else {
        LCD_DEBUG(" Read LCD_id0 error\n");
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DISABLE);
        if (0 != ret) {
            LCD_DEBUG("ID0 mt_set_gpio_pull_select->Disbale fail\n");
        }
        lcd_id0 = LCD_HW_ID_STATUS_ERROR;
    }


    if (low_read1 != high_read1) {
        /*float status , pull down ID1 ,to prevent electric leakage */
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
        if (0 != ret) {
            LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");
        }
        lcd_id1 = LCD_HW_ID_STATUS_FLOAT;
    } else if ((LCD_HW_ID_STATUS_LOW == low_read1) && (LCD_HW_ID_STATUS_LOW == high_read1)) {
        /*low status , pull down ID1 ,to prevent electric leakage */
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
        if (0 != ret) {
            LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");
        }
        lcd_id1 = LCD_HW_ID_STATUS_LOW;
    } else if ((LCD_HW_ID_STATUS_HIGH == low_read1) && (LCD_HW_ID_STATUS_HIGH == high_read1)) {
        /*high status , pull up ID1 ,to prevent electric leakage */
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
        if (0 != ret) {
            LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");
        }
        lcd_id1 = LCD_HW_ID_STATUS_HIGH;
    } else {

        LCD_DEBUG(" Read LCD_id1 error\n");
        ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DISABLE);
        if (0 != ret) {
            LCD_DEBUG("ID1 mt_set_gpio_pull_select->Disable fail\n");
        }
        lcd_id1 = LCD_HW_ID_STATUS_ERROR;
    }
    dprintf(CRITICAL, "which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
    dprintf(CRITICAL, "which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
    lcd_id = lcd_id0 | (lcd_id1 << 2);

    dprintf(CRITICAL, "which_lcd_module_triple,lcd_id:%d\n", lcd_id);

    lcd_id_pins_value = lcd_id;
    return lcd_id;
}
#endif
