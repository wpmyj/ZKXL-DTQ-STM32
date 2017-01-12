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
	
	void				(*HFCLK_start)(void);
	void				(*HFCLK_stop)(void);
	void				(*LFCLK_start)(void);
	void				(*LFCLK_stop)(void);
	void				(*RTC_calibrate)(void);
}clock_struct_t;


extern clock_struct_t				*CLOCK;

void CLOCK_init(void);



#endif 

