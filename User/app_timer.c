/**
  ******************************************************************************
  * @file   	delay.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.27
  * @brief   	delay function
  ******************************************************************************
  */

#include "app_timer.h"
#include "main.h"
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
__IO uint32_t PowerOnTime;
timer_list_typedef timer_list;
uint8_t system_timer_status;

Timer_typedef retransmit_timer,system_timer;
Timer_typedef send_data1_timer,send_data2_timer,send_data3_timer;
Timer_typedef request_data_timer;
Timer_typedef spi_send_data_timer,spi_send_data_timer1;
Timer_typedef systick_package_timer;
Timer_typedef card_buzzer_timer,card_second_find_timer;
RTC_timer_Typedef system_rtc_timer;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);

/*******************************************************************************
  * @brief  Init system clock
  * @param  None
  * @retval None
*******************************************************************************/
void SysClockInit(void)
{
	SystemInit();
	SystemCoreClockUpdate();

  if(SysTick_Config(SystemCoreClock / 1000))
  {
    /* Capture error */
    while (1);
  }

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ClockSecuritySystemCmd(ENABLE);
}

/*******************************************************************************
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
*******************************************************************************/
void DelayMs(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/*******************************************************************************
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
*******************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
	PowerOnTime++;
}

/*******************************************************************************
  * @brief  watchdog init.
  * @param  None
  * @retval None
*******************************************************************************/
void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	/* 写入0x5555,用于允许狗狗寄存器写入功能 */
	IWDG_SetPrescaler(IWDG_Prescaler_256);				/* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
	IWDG_SetReload(312);													/* 喂狗时间 2s/6.4MS=312 .注意不能大于0xfff*/
	IWDG_ReloadCounter();													/* 喂狗*/
	IWDG_Enable();																/* 使能*/
}

/*******************************************************************************
  * @brief  delay 2us
  * @param  None
  * @retval None
*******************************************************************************/
void Delay2us(uint32_t times)
{
	uint16_t i,j;
	for(i = 0; i < times; i++)
	{
		for(j = 0;j < 12;j++)
		{
			__nop();
		}
	}
}

/*******************************************************************************
  * @brief  delay 3us
  * @param  None
  * @retval None
*******************************************************************************/
void Delay3us(void)
{
	uint16_t i;
	for(i = 0;i < 17;i++)
	{
		__nop();
	}
}

/*******************************************************************************
  * @brief  delay 10us
  * @param  None
  * @retval None
*******************************************************************************/
void Delay10us(uint16_t times)
{
	uint16_t i,j;
	for(i = 0; i < times; i++)
	{
		for(j = 0;j < 70;j++)
		{
			__nop();
		}
	}
}

/******************************************************************************
  Function:Timer_list_handler
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void Timer_list_handler(void)
{
	uint8_t i;
	for(i=0;i<timer_list.timer_count;i++)
	{
		if(timer_list.timer[i] != 0)
		{
			if(timer_list.timer[i]->start_status == *timer_list.timer[i]->status)
			{
				timer_list.inc_cnt(timer_list.timer[i]);
				if(timer_list.get_cnt(timer_list.timer[i]) == timer_list.timer[i]->timeout)
				{
					timer_list.set_sattus(timer_list.timer[i],timer_list.timer[i]->timeout_status);
					timer_list.set_cnt(timer_list.timer[i],0);
					if(timer_list.timer[i]->timerout_event_handle != 0)
						timer_list.timer[i]->timerout_event_handle();
				}
			}
		}
	}
}

/******************************************************************************
  Function:sw_register_timer
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t sw_register_timer( Timer_typedef *timer_id )
{
	if(timer_list.timer_count < TIMER_COUNT_MAX)
	{
		timer_list.timer[timer_list.timer_count++] = timer_id;
		timer_id->pos = timer_list.timer_count;
		return 0;
	}
	else
	{
		return 1;
	}
}

/******************************************************************************
  Function:sw_unregister_timer
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t sw_unregister_timer( Timer_typedef *timer_id )
{
	timer_list.timer[timer_id->pos] = 0;
	return 0;
}

/******************************************************************************
  Function:sw_set_sattus
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sw_set_sattus( Timer_typedef *timer_id, uint8_t new_status )
{
	*timer_id->status = new_status;
}

/******************************************************************************
  Function:sw_get_status
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t sw_get_status( Timer_typedef *timer_id )
{
	return *(timer_id->status);
}

/******************************************************************************
  Function:sw_set_cnt
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sw_set_cnt( Timer_typedef *timer_id, uint32_t new_cnt)
{
	timer_id->cnt = new_cnt;
}

/******************************************************************************
  Function:sw_get_cnt
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint32_t sw_get_cnt( Timer_typedef *timer_id )
{
	return (timer_id->cnt);
}

/******************************************************************************
  Function:sw_inc_cnt
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sw_inc_cnt( Timer_typedef *timer_id )
{
	timer_id->cnt++;
}

/******************************************************************************
  Function:sw_timer_init
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sw_timer_init( void )
{
	uint8_t i;

	for(i=0;i<TIMER_COUNT_MAX;i++)
		timer_list.timer[i] = 0;

	timer_list.timer_count = 0;

	timer_list.registr_timer   = sw_register_timer;
	timer_list.unregistr_timer = sw_unregister_timer;
	timer_list.set_sattus      = sw_set_sattus;
	timer_list.get_status      = sw_get_status;
	timer_list.set_cnt         = sw_set_cnt;
	timer_list.get_cnt         = sw_get_cnt;
	timer_list.inc_cnt         = sw_inc_cnt;
}

/******************************************************************************
  Function:create_timer
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sw_create_timer( Timer_typedef *timer_id, uint32_t delayms, uint8_t statrt_status,
	                    uint8_t timout_status, uint8_t * status, timerout_event_callback callback)
{
	timer_id->timeout        = delayms;
	timer_id->start_status   = statrt_status;
	timer_id->timeout_status = timout_status;
	timer_id->status         = status;
	timer_id->timerout_event_handle = callback;
	timer_list.registr_timer(timer_id);
}

/******************************************************************************
  Function:sw_clear_timer
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sw_clear_timer( Timer_typedef *timer_id )
{
	timer_list.set_cnt( timer_id, 0 );
}

/******************************************************************************
  Function:send_data_process_timer_init
  Description:
		系统状态定时器回调函数
  Input :
  Return:
  Others:None
******************************************************************************/
void systemtick_timeout_callback( void )
{
	system_timer_status = 0;

	system_rtc_timer.ms += 100;
	if( system_rtc_timer.ms >= 1000 )
	{
		system_rtc_timer.ms = 0;
#ifdef ENABLE_WATCHDOG
		IWDG_ReloadCounter();
#endif
		system_rtc_timer.sec++;
		if( system_rtc_timer.sec >= 60 )
		{
			system_rtc_timer.sec = 0;
			system_rtc_timer.min++;
			if( system_rtc_timer.min >= 60 )
			{
				system_rtc_timer.min = 0;
				system_rtc_timer.hour++;
				if( system_rtc_timer.hour >= 24 )
				{
					system_rtc_timer.hour = 0;
				}
			}
		}
	}
}

/******************************************************************************
  Function:system_timer_init
  Description:
		系统状态定时器
  Input :
  Return:
  Others:None
******************************************************************************/
void system_timer_init( void )
{
	sw_create_timer(&system_timer , 100, 0, 1,&(system_timer_status), systemtick_timeout_callback);

	system_rtc_timer.year = 2016;
	system_rtc_timer.mon  = 0;
	system_rtc_timer.date = 0;
	system_rtc_timer.hour = 0;
	system_rtc_timer.min  = 0;
	system_rtc_timer.sec  = 0;
	system_rtc_timer.ms   = 0;
	system_rtc_timer.sync_flg = 0;
}

/**************************************END OF FILE****************************/

