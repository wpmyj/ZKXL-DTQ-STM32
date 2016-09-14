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
extern uint8_t card_cmd_type ;

uint8_t rf_clickers_sign[4];
uint8_t rf_outline_index = 0;
extern uint8_t rf_back_sign[4];
Uart_MessageTypeDef rf_systick_massage = {
	0x5C,                 // HEADER
	0x2D,                 // TYPE
	0x00,0x00,0x00,0x00,  // UID
	0x00,                 // LEN
	
	0x00,0x00,0x00,0x00,      // ID
	
	0x00,                 // XOR
	0xCA,                 // END
};

void App_clickers_systick_process(void);
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
	
		/*clickers systick process */
		App_clickers_systick_process();
	
}

/******************************************************************************
  Function:checkout_outline_uid
  Description:
		提取不在线状态的答题器UID
  Input :
  Return:
  Others:None
******************************************************************************/
bool checkout_outline_uid(uint8_t *puid,uint8_t *len)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	
	for(i=rf_outline_index;(i<120)&&(*len<240);i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(0,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(1,i);
			if(is_online_pos == 0)
			{
				get_index_of_uid(i,puid);
				puid = puid+4;
				*len = *len + 4;
			}
		}
	}
	
	if(i==120)
	{
		rf_outline_index = 0;
		return 0;
	}
	else
	{
		rf_outline_index = i;
		return 1;

	}
	
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_systick_process(void)
{
	Uart_MessageTypeDef ReviceMessage;
	uint8_t buffer_status = 0;
	uint8_t systick_current_status = 0;
	
	/* 获取当前的systick的状态 */
	systick_current_status = rf_get_systick_status();
	
	/* 10s 时间到 发送新的心跳包到答题器 */
	if(systick_current_status == 2)
	{
		
		get_next_uid_of_white_list(rf_clickers_sign);
		
		ReviceMessage.HEADER = 0x5C;
		ReviceMessage.TYPE   = 0x10;
		memset(ReviceMessage.SIGN,0x00,4);
		ReviceMessage.LEN = 0x0A;
		
		ReviceMessage.DATA[0] = 0x5A;
		ReviceMessage.DATA[1] = rf_clickers_sign[0];
		ReviceMessage.DATA[2] = rf_clickers_sign[1];
		ReviceMessage.DATA[3] = rf_clickers_sign[2];
 		ReviceMessage.DATA[4] = rf_clickers_sign[3];
		ReviceMessage.DATA[5] = 0x00;
		ReviceMessage.DATA[6] = 0x15;
		ReviceMessage.DATA[7] = 0x00;
		ReviceMessage.DATA[8] = XOR_Cal(ReviceMessage.DATA+1, 7);
		ReviceMessage.DATA[9] = 0xCA;
		
		ReviceMessage.XOR = XOR_Cal((uint8_t *)(&(ReviceMessage.TYPE)), 9+6);	
		ReviceMessage.XOR = 0xCA;
		
		/* 获取接收缓存的状态 */
		buffer_status = buffer_get_buffer_status(REVICE_RINGBUFFER);
		
		/* 根据状态决定是否读取缓存指令 */
		if(BUFFERFULL == buffer_status)
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			serial_ringbuffer_write_data(REVICE_RINGBUFFER,&ReviceMessage);
			clear_white_list_online_table();
			rf_change_systick_status(3);
		}	
		
	}
	
	/* 发送数据之后 */
	if(systick_current_status == 4)
	{
		/* 填充心跳包 */
		uint8_t Is_over = 0;
		Is_over = checkout_outline_uid(rf_systick_massage.DATA,&(rf_systick_massage.LEN));
		rf_systick_massage.XOR =  XOR_Cal((uint8_t *)(&(rf_systick_massage.TYPE)), rf_systick_massage.LEN+6);
		rf_systick_massage.END = 0xCA;
		
		/* 上传在线状态 */
		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_systick_massage);
		}
		rf_systick_massage.LEN = 0;
		
		if(Is_over == 0)
		{
			rf_change_systick_status(1);
		}

	}
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
		
		/* 获取消息的有效长度 */
		rf_message.LEN = rf_var.rx_buf[7] + 10;

		for (i=0;i<rf_message.LEN;i++)
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
	uint8_t cmd_process_status = 0;
	
	if((delay_nms == 0)&&((attendance_on_off == ON) || match_on_off == ON))
	{
		delay_nms = 200;
		if(FindICCard() == MI_OK)
		{
			/* 处理数据 */
			if(attendance_on_off)						
			{
				is_white_list_uid = add_uid_to_white_list(g_cSNR+4,&uid_p);

				if(is_white_list_uid != OPERATION_ERR)
				{
          // OK
					cmd_process_status = 1;
					rf_change_systick_status(1);
					memcpy(rf_clickers_sign,g_cSNR+4,4);
				}
				else
				{
					// Err 0x29 E3
					cmd_process_status = 1;
					App_returnErr(&card_message,0x29,0xE3);
				}
			}
			else
			{
				cmd_process_status = 1;
			}
			
			if(cmd_process_status == 1)
			{
				/* 封装协议  */
				{
					card_message.HEADER = 0x5C;
					switch(card_cmd_type)
					{
						case 0x25: card_message.TYPE   = 0x26; break;
						case 0x28: card_message.TYPE   = 0x29; break;
						default:                               break;
					}
					memcpy(card_message.SIGN,uart_card_cmd_sign,4);
					card_message.LEN    = 0x04;
					memcpy(card_message.DATA,g_cSNR+4,4);
					card_message.XOR = XOR_Cal(&card_message.TYPE,10);
					card_message.END  = 0xCA;
				}
		
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
			
			//写入配对时将UID传给答题器
			write_RF_config();
			
			//不重复寻卡
			PcdHalt();
		}
	}	
//	Buzze_Control();
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
