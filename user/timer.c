/*-------------------------------- Include File -----------------------------------------*/
#include "timer.h"
/*-------------------------------- Macro definition -------------------------------------*/
//#define TIMER_DEBUG
#ifdef TIMER_DEBUG
#define timer_debug  debug_printf   
#else  
#define timer_debug(...)                    
#endif 

//定时器宏定义
#define APP_TIMER_PRESCALER     0
#define APP_TIMER_OP_QUEUE_SIZE 14

/* 通信层定时器 */
#define TEMP_TIMEOUT_INTERVAL     					APP_TIMER_TICKS(2000,APP_TIMER_PRESCALER)
#define RETURN_ACK_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(500,APP_TIMER_PRESCALER)
#define RX_WINDOW_ON_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(6,APP_TIMER_PRESCALER)
#define RX_WINDOW_OFF_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(50,APP_TIMER_PRESCALER)
#define TX_OVERTIME_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(15,APP_TIMER_PRESCALER) 
#define LCD_UPDATE_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(100,APP_TIMER_PRESCALER) 
#define RETRANSMIT_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(50,APP_TIMER_PRESCALER) 
#define TRANSMIT_RESULT_TIMEOUT_INTERVAL     		APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER) 	
#define POWERDOWN_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(30700,APP_TIMER_PRESCALER) 	//本应是30S，但是程序跑会有延时，所以+700ms补偿
#define BUTTON_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(50,APP_TIMER_PRESCALER) 	
#define SYSTEM_OFF_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER) 	
#define WAIT_DATA_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER) 	
#define POWER_ON_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(2000,APP_TIMER_PRESCALER) 	


/*-------------------------------- Local Variable ----------------------------------------*/
APP_TIMER_DEF(temp_timer_id);					/* 测试用 */
APP_TIMER_DEF(return_ack_timer_id); 			/* 随机延时回ACK */
APP_TIMER_DEF(rx_window_timer_id);				/* RX模式时间窗控制 */
APP_TIMER_DEF(tx_overtime_timer_id);			/* 发送超时 */
APP_TIMER_DEF(lcd_update_timer_id);				/* LCD定时刷新 */
APP_TIMER_DEF(retransmit_timer_id);				/* 重发延时 */
APP_TIMER_DEF(transmit_result_timer_id);		/* LCD显示发送结果时间 */
APP_TIMER_DEF(powerdown_timer_id);				/* 待机状态 */
APP_TIMER_DEF(button_timer_id);					/* 按键消抖 */
APP_TIMER_DEF(system_off_timer_id);			    /* 45min关机定时器 */
APP_TIMER_DEF(wait_data_timer_id);			    /* 根据收到的前导帧后序号，确定RX窗打开时间 */
APP_TIMER_DEF(power_on_timer_id);			    /* 长按开机定时器 */


timer_struct_t				timer_struct;
									   
/*-------------------------------- Gocal Variable ----------------------------------------*/
timer_struct_t				*TIMER = &timer_struct;
					   
/*-------------------------------- Function Declaration ----------------------------------*/

/*-------------------------------- Revision record ---------------------------------------*/

void TIMER_init(void)
{
	uint32_t err_code;
	
	/* 定时器创建 */
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
	err_code = app_timer_create(&temp_timer_id,APP_TIMER_MODE_REPEATED,TIMER_TmpHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&return_ack_timer_id,APP_TIMER_MODE_REPEATED,TIMER_ReturnAckHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&rx_window_timer_id,APP_TIMER_MODE_REPEATED,TIMER_RxWindowHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&tx_overtime_timer_id,APP_TIMER_MODE_REPEATED,TIMER_TxOvertimeHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&lcd_update_timer_id,APP_TIMER_MODE_REPEATED,TIMER_LcdUpdateHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&retransmit_timer_id,APP_TIMER_MODE_REPEATED,TIMER_RetransmitHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&transmit_result_timer_id,APP_TIMER_MODE_REPEATED,TIMER_TxResultHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&powerdown_timer_id,APP_TIMER_MODE_REPEATED,TIMER_SysSleepHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&button_timer_id,APP_TIMER_MODE_REPEATED,TIMER_ButtonHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&system_off_timer_id,APP_TIMER_MODE_REPEATED,TIMER_SysOffHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&wait_data_timer_id,APP_TIMER_MODE_REPEATED,TIMER_WaitDataHandler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&power_on_timer_id,APP_TIMER_MODE_SINGLE_SHOT,TIMER_PowerOnHandler);	//仅一次
	APP_ERROR_CHECK(err_code);
	
	
	/* TIMER struct init */
	TIMER->event_flg 			= 0x00;
}



/******************************************************************************
  @函数:temp_timer_id
  @描述:
  @输入:
  @输出:
  @调用:
******************************************************************************/
void TIMER_TmpStart(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(temp_timer_id,TEMP_TIMEOUT_INTERVAL,NULL);
//	err_code = app_timer_start(temp_timer_id,6554,NULL);	//手动计算的200ms定时器，测试用
	APP_ERROR_CHECK(err_code);
}


void TIMER_TmpStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(temp_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_TmpHandler(void * p_context)
{
	TIMER_TmpStop();
}



/******************************************************************************
  @函数:return_ack_timer_id
  @描述:
  @输入:
  @输出:
  @调用:
******************************************************************************/
void TIMER_ReturnAckStart(void)
{
	uint32_t err_code;
	uint8_t  random_num1,random_num2,random_num3,random_num4,random_num5;
	uint8_t  random_num6,random_num7,random_num8;
	uint32_t random_delay = 0;
	uint32_t random_value = 0;
	
	random_num1 = get_random_number();			//随机延时600ms
	random_num2 = get_random_number();
	random_num3 = get_random_number();
	random_num4 = get_random_number();
	random_num5 = get_random_number();
	random_num6 = get_random_number();
	random_num7 = get_random_number();
	random_num8 = get_random_number();
	
	random_value = random_num1 + random_num2 + random_num3 + random_num4 + random_num5 + random_num6 
			+ random_num7 + random_num8;
	random_delay = random_value * 5;	
	
	//systic 1tick = 30.517us 
	//官方要求tick最小值为5
	
	err_code = app_timer_start(return_ack_timer_id,random_delay,NULL);

	APP_ERROR_CHECK(err_code);
}


void TIMER_ReturnAckStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(return_ack_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_ReturnAckHandler(void * p_context)
{
	TIMER->event_flg |= RETURN_ACK_TIMEER_EVENT;
	TIMER_ReturnAckStop();
}

void TIMER_MyReturnAckHandler(void)
{
	TIMER_RxWindowStop();	 //发送数据时把RX窗口定时器关闭
	TIMER_TxOvertimeStart();  //发送超时定时器
	my_esb_mode_change(NRF_ESB_MODE_PTX);
	TRANSPORT.sequence_re_tx_num = 1;

	memcpy(tx_payload.data,TRANSPORT.ack,NRF_ACK_PROTOCOL_LEN);
	tx_payload.length = NRF_ACK_PROTOCOL_LEN;
	tx_payload.pipe = NRF_PIPE;
	tx_payload.noack = true;
	nrf_esb_flush_tx();
	nrf_esb_write_payload(&tx_payload);	
}


/******************************************************************************
  @函数:rx_window_timer_id
  @描述:
  @输入:
  @输出:
  @调用:
******************************************************************************/
void TIMER_RxWindowStart(void)
{
	uint32_t err_code;
	
	if(RADIO->rx_window_on_flg)	//当前RX打开，则关闭
	{
		POWER_DCDCDisable();
//		HFCLK_stop();   //RX关闭时，关掉外部时钟，降功耗
		nrf_esb_stop_rx();	
//		err_code = app_timer_start(rx_window_timer_id,RX_WINDOW_OFF_TIMEOUT_INTERVAL,NULL);
		err_code = app_timer_start(rx_window_timer_id,APP_TIMER_TICKS(RADIO->rx_window_off,APP_TIMER_PRESCALER),NULL);
		RADIO->rx_window_on_flg = false;	
		RADIO->rx_window_add_flg = false;			
	}
	else
	{
		POWER_DCDCEnable();

		my_esb_mode_change(NRF_ESB_MODE_PRX);
		nrf_esb_start_rx();	
//		err_code = app_timer_start(rx_window_timer_id,RX_WINDOW_ON_TIMEOUT_INTERVAL,NULL);
		err_code = app_timer_start(rx_window_timer_id,APP_TIMER_TICKS(RADIO->rx_window_on,APP_TIMER_PRESCALER),NULL);
		RADIO->rx_window_on_flg = true;
		RADIO->rx_window_add_flg = false;
	}
	APP_ERROR_CHECK(err_code);
}

//增加时间窗
void TIMER_RxWindowAdd(uint32_t time_ms)
{
	uint32_t err_code;
	

	POWER_DCDCEnable();
	
	my_esb_mode_change(NRF_ESB_MODE_PRX);
	nrf_esb_start_rx();	
	
	TIMER_RxWindowStop();
	err_code = app_timer_start(rx_window_timer_id,APP_TIMER_TICKS(time_ms,APP_TIMER_PRESCALER),NULL);
	APP_ERROR_CHECK(err_code);
	
	RADIO->rx_window_on_flg = true;
	RADIO->rx_window_add_flg = true;
}


void TIMER_RxWindowStop(void)
{

	uint32_t err_code;
	err_code = app_timer_stop(rx_window_timer_id);
	APP_ERROR_CHECK(err_code);
}

void TIMER_RxWindowHandler(void * p_context)
{
	RADIO->rx_window_wait_data = false;
	RADIO->rx_window_add_flg = false;
	TIMER_RxWindowStop();
	TIMER_RxWindowStart();
}


/******************************************************************************
  @函数:tx_overtime_timer_id
  @描述:
  @输入:
  @输出:
  @调用:内/外部
******************************************************************************/
void TIMER_TxOvertimeStart(void)
{
	uint32_t err_code;
	err_code = app_timer_start(tx_overtime_timer_id,TX_OVERTIME_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_TxOvertimeStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(tx_overtime_timer_id);
	APP_ERROR_CHECK(err_code);
}

void TIMER_TxOvertimeHandler(void * p_context)
{	
	TIMER_TxOvertimeStop();
	
	/* 如果发送超时后，转回RX模式，则重新开启RX定时器 */
	if(m_config_local.mode != NRF_ESB_MODE_PRX)
	{

		RADIO->rx_window_on_flg = false;
		RADIO->rx_window_add_flg = false;
		TIMER_RxWindowStart();
	}
}

/******************************************************************************
  @函数:lcd_update_timer_id
  @描述:
  @输入:
  @输出:
  @调用:内/外部
******************************************************************************/
void TIMER_LcdUpdateStart(void)
{
	uint32_t err_code;
	err_code = app_timer_start(lcd_update_timer_id,LCD_UPDATE_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}

void TIMER_LcdUpdateStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(lcd_update_timer_id);
	APP_ERROR_CHECK(err_code);
}

void TIMER_LcdUpdateHandler(void * p_context)
{
	static uint8_t rtc_timer = 0;		//RTC和LCD共用定时器
	static uint16_t adc_timer = 0;
	LCD->update_flg  = true;
//	if(++rtc_timer == 10)
//	{
//		RTC->update_flg = true;
//		rtc_timer = 0;
//	}
	
	//第一次采集时，加快采集速度
	if(ADC->sample_count)
	{
		if(++adc_timer >= 10)
		{
			ADC->update_flg = true;
			adc_timer = 0;
		}
	}
	else
	{
		ADC->update_flg = true;
	}

}

/******************************************************************************
  @函数:retransmit_timer_id
  @描述:
  @输入:
  @输出:
  @调用:内/外部
******************************************************************************/
void TIMER_RetransmitStart(void)
{
	uint32_t err_code;
	err_code = app_timer_start(retransmit_timer_id,RETRANSMIT_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}

void TIMER_RetransmitStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(retransmit_timer_id);
	APP_ERROR_CHECK(err_code);
}

void TIMER_RetransmitHandler(void * p_context)
{	
	TIMER->event_flg |= RETRANSMIT_TIMEER_EVENT;
}

void TIMER_MyRetransmitHandler(void)
{
	TRANSPORT.retransmit_num++;

	if( true == TRANSPORT.tx_ok_flag ) 					//收到有效ACK,发送成功
	{
		TRANSPORT.tx_ing_flag = false;
		TRANSPORT.retransmit_num = 0;					
		TIMER_RetransmitStop();						
		RADIO_TxSuccessHandler();					
		timer_debug("[timer]transmit succeed \r\n");
	}
	else if(TRANSPORT.retransmit_num > NRF_MAX_NUMBER_OF_RETRANSMITS )	//达到最大重发次数，发送失败
	{	
		TRANSPORT.tx_ing_flag = false;
		TRANSPORT.retransmit_num = 0;		
		TIMER_RetransmitStop();
		RADIO_TxFailedHandler();
		timer_debug("[timer]transmit failure \r\n");
	}
	else
	{
		
		TRANSPORT.data[9]++;		//重发包号不变，帧号+1
		TRANSPORT.data[NRF_ACK_PROTOCOL_LEN + rf_var.tx_len - 2] = XOR_Cal(TRANSPORT.data+1,TRANSPORT.data_len-3);
		RADIO_StartTx();
	}
}

/******************************************************************************
  @函数:transmit_result_timer_id
  @描述:
  @输入:
  @输出:
  @调用:内/外部
******************************************************************************/
void TIMER_TxResultStart(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(transmit_result_timer_id,TRANSMIT_RESULT_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_TxResultStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(transmit_result_timer_id);
	APP_ERROR_CHECK(err_code);
}

void TIMER_TxResultHandler(void * p_context)
{
	TIMER_TxResultStop();
	LCD_display_ok_failure(SEND_RESULT_CLEAR);
	APP->key_send_allow_flg = true;
}


/******************************************************************************
  @函数:TIMER_SysSleepStart
  @描述:
  @输入:
  @输出:
  @调用:内/外部
******************************************************************************/
void TIMER_SysSleepStart(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(powerdown_timer_id,POWERDOWN_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_SysSleepStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(powerdown_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_SysSleepHandler(void * p_context)
{
//	tmp_debug("TIMER_SysSleepHandler \r\n");
	TIMER_SysSleepStop();
	POWER_Sleep();
}



/******************************************************************************
  @函数:temp_timer_id
  @描述:
  @输入:
  @输出:
  @调用:
******************************************************************************/
void TIMER_ButtonStart(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(button_timer_id,BUTTON_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_ButtonStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(button_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_ButtonHandler(void * p_context)
{
	KEY->run_flg = true;
	//TIMER_ButtonStop();
}


void TIMER_SysOffStart(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(system_off_timer_id,SYSTEM_OFF_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_SysOffStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(system_off_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_SysOffHandler(void * p_context)
{
	if(POWER->sys_off_count >= 2700)	//60*45min
		POWER_Off();					//关机
	else
		POWER->sys_off_count++;
}


void TIMER_WaitDataStart(uint8_t time_ms)
{
	uint32_t err_code;
	
	err_code = app_timer_start(wait_data_timer_id,APP_TIMER_TICKS(time_ms,APP_TIMER_PRESCALER),NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_WaitDataStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(wait_data_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_WaitDataHandler(void * p_context)
{
//	tmp_debug("TIMER_WaitDataHandler \r\n");
	TIMER_WaitDataStop();							//停止定时器
	TIMER_RxWindowAdd(RADIO->rx_window_add);		//增加RX窗口，等待有效数据
}

void TIMER_PowerOnStart(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(power_on_timer_id,POWER_ON_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void TIMER_PowerOnStop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(power_on_timer_id);
	APP_ERROR_CHECK(err_code);
}


void TIMER_PowerOnHandler(void * p_context)
{
	POWER->KeyOnIngFlg = true;
}


void TIMER_MyEventHandler(void)
{
	if(TIMER->event_flg)
	{
		if(TIMER->event_flg & RETURN_ACK_TIMEER_EVENT)
		{
			TIMER->event_flg &= ~RETURN_ACK_TIMEER_EVENT;
			TIMER_MyReturnAckHandler();
		}
		if(TIMER->event_flg & RETRANSMIT_TIMEER_EVENT)
		{
			TIMER->event_flg &= ~RETRANSMIT_TIMEER_EVENT;
			TIMER_MyRetransmitHandler();
		}
	}
}














