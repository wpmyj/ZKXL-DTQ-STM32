#ifndef __CLOCK_H
#define	__CLOCK_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_drv_clock.h"
#include "define.h"

typedef struct 
{
	bool				flg;					
	
}clock_struct_t;


extern clock_struct_t				*CLOCK;

void CLK_HFCLKStart(void);
void HFCLK_stop(void);
void CLK_LFCLKStart(void);
void LFCLK_stop(void);
void CLK_RTCCalibrate(void);
void RTC_calibrate_handler(nrf_drv_clock_evt_type_t event);

void CLOCK_Init(void);



#endif 

