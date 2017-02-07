#ifndef __ADC_H
#define	__ADC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "adc.h"
#include "nrf_drv_adc.h"
#include "nrf.h"
#include "nordic_common.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "define.h"




typedef struct 
{
	bool				ok_flg;					
	bool				update_flg;
	bool				sample_count;
	
	void				(*start)(void);
	void				(*update)(void);
	void				(*read_buf)(nrf_adc_value_t *buf);
}adc_struct_t;


extern adc_struct_t				*ADC;

void ADC_init(void);


#endif 

