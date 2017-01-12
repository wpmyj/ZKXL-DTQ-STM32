#ifndef __TIMER_H
#define	__TIMER_H

#include "app_timer.h"
#include "radio.h"
#include "my_misc_fun.h"
#include "ringbuffer.h"
#include "my_wdt.h"
#include "lcd.h"
#include "rtc.h"


#define RX_WINDOW_ADD_WAIT_ACK			(20)	
#define RX_WINDOW_ADD_WAIT_DATA			(20)

#define RX_WINDOW_ON					(3)		//RX窗打开时间
#define RX_WINDOW_OFF					(100)	//RX窗关闭时间

#define RETURN_ACK_DELAY				(300)	//随机回复ACK的最大时间

typedef enum 
{
	TEMP_TIMEER_EVENT			= 0x01,
	RETRANSMIT_TIMEER_EVENT		= 0x02,		//重发	
	RETURN_ACK_TIMEER_EVENT		= 0x04,		//随机延时回ACK	
	SYSTEM_TIMEER_EVENT			= 0x08,
	BATTERY_TIMEER_EVENT		= 0x10,
	RTC_TIMEER_EVENT			= 0x20,
	RF_SEND_FLAG_TIMEER_EVENT	= 0x40,
	WDT_TIMEER_EVENT			= 0x80
}timer_event_type;					//定时器类型，暂定8个


typedef struct 
{
	uint8_t				event_flg;			//刷新标志，各个定时器按位或
	
	void				(*event_handler)(void);
	
	void				(*tmp_start)(void);
	void				(*tmp_stop)(void);
	void				(*return_ack_start)(void);
	void				(*return_ack_stop)(void);
	void				(*rx_window_start)(void);
	void				(*rx_window_stop)(void);
	void				(*rx_window_add)(uint32_t time_ms);
	void				(*tx_overtime_start)(void);
	void				(*tx_overtime_stop)(void);
	
	void				(*lcd_update_start)(void);
	void				(*lcd_update_stop)(void);
	void				(*lcd_update_handler)(void * p_context);

	void				(*retransmit_start)(void);
	void				(*retransmit_stop)(void);
	void				(*transmit_result_start)(void);
	void				(*transmit_result_stop)(void);
	void				(*powerdown_start)(void);
	void				(*powerdown_stop)(void);
	void				(*button_start)(void);
	void				(*button_stop)(void);
	void				(*request_data_start)(void);
	void				(*request_data_stop)(void);
	void				(*system_off_start)(void);
	void				(*system_off_stop)(void);
	void				(*wait_data_start)(uint8_t time_ms);
	void				(*wait_data_stop)(void);
	void				(*power_on_start)(void);
	void				(*power_on_stop)(void);
}timer_struct_t;



extern timer_struct_t				*TIMER;


void TIMER_init(void);

#endif 


