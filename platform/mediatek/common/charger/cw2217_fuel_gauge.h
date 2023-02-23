/*****************************************************************************
*
* Filename:
* ---------
*   cw2017_fuel_guau.h
*
* Project:
* --------
*   Android
*
* Description:
* ------------
*   cw2017 header file
*
* Author:
* -------
*
****************************************************************************/

#ifndef _CW2217_SW_H_
#define _CW2217_SW_H_

typedef struct tagSTRUCT_CW_BATTERY {
	/*IC value*/
    unsigned int voltage;	
    unsigned int capacity;
	int temp;

	unsigned long cycle_count;
	unsigned char SOH;
	long current;
	
	/*Get from charger power supply*/
	unsigned int charger_mode;
}STRUCT_CW_BATTERY;

//extern STRUCT_CW_BATTERY   cw_bat;

int cw221x_get_chip_id(int *chip_id);
int cw221x_sleep(void);
int cw221x_get_vol(void);
int cw221x_get_capacity(void);
int cw221x_get_temp(void);
int cw221x_get_current(long *lp_current);
int cw221x_get_cycle_count(unsigned long *lp_count);
int cw221x_get_soh(unsigned char *lp_soh);
int cw221x_init(void);


#endif // _CW2017_SW_H_

