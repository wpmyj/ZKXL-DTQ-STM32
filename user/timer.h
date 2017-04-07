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
}timer_struct_t;



extern timer_struct_t				*TIMER;

void TIMER_TmpStart(void);
void TIMER_TmpStop(void);
void TIMER_TmpHandler(void * p_context);

void TIMER_ReturnAckStart(void);
void TIMER_ReturnAckStop(void);
void TIMER_ReturnAckHandler(void * p_context);
void TIMER_MyReturnAckHandler(void);

void TIMER_RxWindowStart(void);
void TIMER_RxWindowStop(void);
void TIMER_RxWindowAdd(uint32_t time_ms);
void TIMER_RxWindowHandler(void * p_context);

void TIMER_TxOvertimeStart(void);
void TIMER_TxOvertimeStop(void);
void TIMER_TxOvertimeHandler(void * p_context);

void TIMER_LcdUpdateStart(void);
void TIMER_LcdUpdateStop(void);
void TIMER_LcdUpdateHandler(void * p_context);

void TIMER_RetransmitStart(void);
void TIMER_RetransmitStop(void);
void TIMER_RetransmitHandler(void * p_context);
void TIMER_MyRetransmitHandler(void);

void TIMER_TxResultStart(void);
void TIMER_TxResultStop(void);
void TIMER_TxResultHandler(void * p_context);

void TIMER_SysSleepStart(void);
void TIMER_SysSleepStop(void);
void TIMER_SysSleepHandler(void * p_context);

void TIMER_ButtonStart(void);
void TIMER_ButtonStop(void);
void TIMER_ButtonHandler(void * p_context);

void TIMER_SysOffStart(void);
void TIMER_SysOffStop(void);
void TIMER_SysOffHandler(void * p_context);

void TIMER_WaitDataStart(uint8_t time_ms);
void TIMER_WaitDataStop(void);
void TIMER_WaitDataHandler(void * p_context);

void TIMER_PowerOnStart(void);
void TIMER_PowerOnStop(void);
void TIMER_PowerOnHandler(void * p_context);

void TIMER_MyEventHandler(void);

void TIMER_init(void);

#endif 


