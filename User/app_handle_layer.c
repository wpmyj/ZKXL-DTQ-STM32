/**
  ******************************************************************************
  * @file   	app_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pos_handle_layer.h"
#include "rc500_handle_layer.h"

extern uint8_t uart_rf_cmd_sign[4],uart_card_cmd_sign[4];		

void App_rf_check_process(void);
void App_card_process(void);
void Buzze_Control(void);
	
/******************************************************************************
  Function:app_handle_layer
  Description:
		App 轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void app_handle_layer(void)
{
		/* nrf51822 Communication processing process */
		App_rf_check_process();
		
		/* serial cmd processing process */
		App_seirial_cmd_process();
		
		/* MI Card processing process */
		App_card_process();
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_rf_check_process(void)
{
	Uart_MessageTypeDef rf_message;
	uint8_t i = 0 ;
	
	if(rf_var.flag_rx_ok == true)
	{
		rf_message.HEADER = 0x5C;
		rf_message.TYPE = 0x10;
		
		memcpy(rf_message.SIGN,uart_rf_cmd_sign,4);
		
		rf_message.LEN = rf_var.rx_len;

		for (i=0;i<rf_var.rx_len+1;i++)
		{
			rf_message.DATA[i]=rf_var.rx_buf[i];
	#ifdef ENABLE_RF_DATA_SHOW
			printf("%2X ",rf_var.rx_buf[i]);
			if((i+1)%20 == 0 )
				printf("\r\n");			
	#endif
		}
		
		rf_message.XOR =  XOR_Cal((uint8_t *)(&(rf_message.TYPE)), i+6);		
		rf_message.END = 0xCA;
		
		/* 清空接收缓存 */
		memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
		rf_var.rx_len = 0x00;
		rf_var.flag_rx_ok = false;
		
		/* 执行完的指令存入发送缓存 */
		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_message);
		}	
	}
}

/******************************************************************************
  Function:App_card_process
  Description:
		App MI Card 轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_card_process(void)
{ 
	Uart_MessageTypeDef card_message;
	uint8_t is_white_list_uid = 0,uid_p = 0;
	
	if((delay_nms == 0)&&((attendance_on_off == ON) || match_on_off == ON))
	{
		delay_nms = 200;
		if(FindICCard() == MI_OK)
		{
			/* 处理数据 */
			if(attendance_on_off)						
			{
				is_white_list_uid = add_uid_to_white_list(g_cSNR,&uid_p);
				if(is_white_list_uid == OPERATION_SUCCESS)
				{
          // OK
				}
				else
				{
					// Err 0x29 E3
				}
			}
			else
			{

			}
			
			/* 封装协议  */
			{
				card_message.HEADER = 0x5C;
				card_message.TYPE   = 0x29;
				memcpy(card_message.SIGN,uart_card_cmd_sign,4);
				card_message.LEN    = 0x04;
				memcpy(card_message.DATA,g_cSNR,4);
				card_message.XOR = XOR_Cal(&card_message.TYPE,10);
				card_message.END  = 0xCA;
			}
			
			/* 缓存数据 */
			{
				/* 执行完的指令存入发送缓存 */
				if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					DebugLog("Serial Send Buffer is full! \r\n");
				}
				else
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&card_message);
				}	
			}
			//蜂鸣器响300ms
			time_for_buzzer_on = 10;
			time_for_buzzer_off = 300;
		}
		
		//写入配对时将UID传给答题器
		write_RF_config();
		
		//不重复寻卡
		PcdHalt();
	}
	Buzze_Control();	
}

/*******************************************************************************
  * @brief  Initialize the Gpio port for system
  * @param  None
  * @retval None
*******************************************************************************/
void Buzze_Control(void)
{
	if(time_for_buzzer_on == 1)
	{
		BEEP_EN();
		time_for_buzzer_on = 0;
	}
	if(time_for_buzzer_off == 0)
	{
		BEEP_DISEN();
	}
}
