#ifndef __POWER_H
#define	__POWER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "define.h"
#include "nrf_delay.h"


typedef struct 
{
	uint8_t				sys_state;	
	bool				state_change_flg;
	bool				request_data_flg;		//开机/唤醒后请求数据标志
	uint8_t				request_data_num;		//开机/唤醒后请求数据次数，超过约定后自动关机
	uint8_t 			request_state;			//唤醒请求状态，开机请求还是唤醒请求
	uint16_t			sys_off_count;			//系统关机计数，1秒+1，45min后关机
	
	void				(*off)(void);
	void				(*on)(void);
	void				(*sleep)(void);
	void				(*wakeup)(void);
	
	void				(*send_state)(uint8_t sys_state);

}power_struct_t;


extern power_struct_t				*POWER;

void POWER_init(void);

#endif 

