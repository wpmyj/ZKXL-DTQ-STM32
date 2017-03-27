/**
  ******************************************************************************
  * @file   	delay.h
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.27
  * @brief   	delay function
  ******************************************************************************
  */

#ifndef __DELAY_H_
#define __DELAY_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#define TIMER_COUNT_MAX    17

typedef struct
{
	uint8_t  *status;
	uint8_t  pos;
	uint32_t cnt;
	uint32_t timeout;
	uint8_t  start_status;
	uint8_t  timeout_status;
	void     (*timerout_event_handle)( void );
}Timer_typedef;

typedef	void  (*timerout_event_callback)( void );

typedef struct
{
	Timer_typedef *timer[TIMER_COUNT_MAX];
	uint8_t       (*registr_timer)( Timer_typedef *timer_id );
	uint8_t       (*unregistr_timer)( Timer_typedef *timer_id );
	uint8_t       timer_count;

	void          (*set_sattus)( Timer_typedef *timer_id,uint8_t new_status );
	uint8_t       (*get_status)( Timer_typedef *timer_id );

	void          (*set_cnt)( Timer_typedef *timer_id, uint32_t new_cnt);
	uint32_t      (*get_cnt)( Timer_typedef *timer_id );
	void          (*inc_cnt)( Timer_typedef *timer_id );
}timer_list_typedef;

typedef struct
{
	uint16_t ms;
	uint8_t  sec;
	uint8_t  min;
	uint8_t  hour;
	uint8_t  date;
	uint8_t  mon;
	uint16_t  year;
	uint8_t  sync_flg;
}RTC_timer_Typedef;

extern timer_list_typedef timer_list;
extern Timer_typedef retransmit_2s_timer,retransmit_500ms_timer,system_timer;
extern Timer_typedef spi_send_data_timer,spi_send_data_timer1;
extern RTC_timer_Typedef system_rtc_timer;
extern Timer_typedef card_buzzer_timer,card_second_find_timer;

/* Private functions ---------------------------------------------------------*/
void SysClockInit(void);
void TimingDelay_Decrement(void);
void IWDG_Configuration(void);

void Delay2us(uint32_t times);
void Delay3us(void);
void Delay10us(uint16_t times);
void DelayMs(__IO uint32_t nTime);

void Timer_list_handler(void);
void sw_timer_init(void);
void sw_create_timer( Timer_typedef *timer_id, uint32_t delayms, uint8_t statrt_status,
	                    uint8_t timout_status, uint8_t * status, timerout_event_callback callback);
void system_timer_init( void );
void sw_clear_timer( Timer_typedef *timer_id );
#endif //__DELAY_H_
/**************************************END OF FILE****************************/

