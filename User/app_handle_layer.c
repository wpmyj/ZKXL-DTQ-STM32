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
extern Uart_MessageTypeDef pc_subject_massage;
extern nrf_communication_t nrf_communication;

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

Uart_MessageTypeDef revice_data_massage = {
	0x5C,                 // HEADER
	0x30,                 // TYPE
	0x00,0x00,0x00,0x00,  // UID
	0x00,                 // LEN

	0x00,0x00,0x00,0x00,      // ID

	0x00,                 // XOR
	0xCA,                 // END
};


void App_clickers_systick_process(void);
void App_clickers_send_data_process(void);
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
		/* serial cmd processing process */
		App_seirial_cmd_process();

		/* MI Card processing process */
		App_card_process();

		/*clickers systick process */
		//App_clickers_systick_process();

		/*clickers send data process */
		App_clickers_send_data_process();
}

/******************************************************************************
  Function:checkout_outline_uid
  Description:
		检查是否有新的答题器上线
  Input :
  Return:
  Others:None
******************************************************************************/
bool is_new_uid_online( void )
{
	uint8_t i;
	uint8_t is_online_pos_new = 0,is_online_pos_old = 0;

	for(i=0;i<120;i++)
	{
		is_online_pos_new = get_index_of_white_list_pos_status(1,i);
		is_online_pos_old = get_index_of_white_list_pos_status(2,i);

		if((is_online_pos_new == 1) && (is_online_pos_old == 0))
			return 1;
	}

	return 0;
}
/******************************************************************************
  Function:checkout_outline_uid
  Description:
		提取不在线状态的答题器UID
  Input :
  Return:
  Others:None
******************************************************************************/
bool checkout_outline_uid(uint8_t src_table, uint8_t check_table, uint8_t mode, uint8_t *puid,uint8_t *len)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t index = 0;

	for(i=rf_outline_index;(i<120)&&(*len<240);i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(src_table,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(check_table,i);
			if(is_online_pos == mode)
			{
				get_index_of_uid(i,puid);
				printf("[%2d]:%02x%02x%02x%02x ",i,*puid, *(puid+1), *(puid+2), *(puid+3) );
				puid = puid+4;
				*len = *len + 4;
				if(((index++)+1) % 5 == 0)
					printf("\n");
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
	if(systick_current_status == 4)
	{
		//get_next_uid_of_white_list(0,rf_clickers_sign);

		ReviceMessage.HEADER = 0x5C;
		ReviceMessage.TYPE   = 0x10;
		memset(ReviceMessage.SIGN,0x00,4);
		ReviceMessage.LEN = 0x0A;

		ReviceMessage.DATA[0] = 0x5A;
		ReviceMessage.DATA[1] = 0;//rf_clickers_sign[0];
		ReviceMessage.DATA[2] = 0;//rf_clickers_sign[1];
		ReviceMessage.DATA[3] = 0;//rf_clickers_sign[2];
 		ReviceMessage.DATA[4] = 0;//rf_clickers_sign[3];
		ReviceMessage.DATA[5] = 0x00;
		ReviceMessage.DATA[6] = 0x15;
		ReviceMessage.DATA[7] = 0x00;
		ReviceMessage.DATA[8] = XOR_Cal(ReviceMessage.DATA+1, 7);
		ReviceMessage.DATA[9] = 0xCA;

		ReviceMessage.XOR = XOR_Cal((uint8_t *)(&(ReviceMessage.TYPE)), 9+6);
		ReviceMessage.XOR = 0xCA;

		/* 获取接收缓存的状态 */
		buffer_status = buffer_get_buffer_status(REVICE_RINGBUFFER);

		if( get_pc_subject_status() == 1 )
		{
		  uint8_t is_new_uid = 0;

		  /* 检查是否有新的答题器上线 */
		  is_new_uid = is_new_uid_online();

		  if( is_new_uid == 1)
		  {
		    DebugLog("\n<%s> discover new uid \n\n",__func__);
		    if(BUFFERFULL != buffer_status)
		    {
		      serial_ringbuffer_write_data(REVICE_RINGBUFFER,&pc_subject_massage);
		      clear_white_list_online_table();
		    }
		  }
		  else
		  {
		    DebugLog("\n<%s> no new uid \n\n",__func__);
		    /* 根据状态决定是否读取缓存指令 */
		    if(BUFFERFULL != buffer_status)
		    {
		      serial_ringbuffer_write_data(REVICE_RINGBUFFER,&ReviceMessage);
		      clear_white_list_online_table();
		    }
		  }
		}
	}

	/* 发送数据之后 */
	if(systick_current_status == 2)
	{
		/* 填充心跳包 */
		uint8_t Is_over = 0;
		Is_over = checkout_outline_uid(0,1,1,rf_systick_massage.DATA,&(rf_systick_massage.LEN));
		rf_systick_massage.XOR =  XOR_Cal((uint8_t *)(&(rf_systick_massage.TYPE)), rf_systick_massage.LEN+6);
		rf_systick_massage.END = 0xCA;

		/* 上传在线状态 */
		if(rf_systick_massage.LEN != 0)
		{
			if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_systick_massage);
			}
			rf_systick_massage.LEN = 0;
		}

		if(Is_over == 0)
		{
			rf_change_systick_status(3);
		}
	}
}

/******************************************************************************
  Function:clicker_send_data
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
//void clicker_send_data(uint8_t uid[],uint16_t delayms)
//{
//	memcpy(nrf_communication.dtq_uid,uid,4);
//	my_nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL,0);
//	DelayMs(delayms);
//}

void clicker_check_send_data(uint8_t sel_table, uint8_t uid_pos, uint8_t uid[], uint16_t delayms, uint8_t send_times)
{
	uint8_t is_online_pos = 0;
	uint8_t i = 0;
	
	for(i=0;i<send_times;i++)
	{
		is_online_pos = get_index_of_white_list_pos_status(sel_table,uid_pos);
		if(is_online_pos == 0)
		{
			memcpy(nrf_communication.dtq_uid,uid,4);
			printf("[%2d]:%02x%02x%02x%02x ",uid_pos,uid[0],uid[1],uid[2],uid[3]);
			my_nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL,0);
			DelayMs(delayms);
		}
	}
	if(send_times !=1 )
		printf("\n");
}

/******************************************************************************
  Function:clear_uid_check_table
  Description:
		清除统计表格的数据
  Input :
  Return:
  Others:None
******************************************************************************/
void clear_uid_check_table( uint8_t clicker_count )
{
	clear_white_list_table(3);
	clear_white_list_table(4);
	clear_white_list_table(5);
	clear_white_list_table(6);
	clear_white_list_table(7);
	clear_white_list_table(8);
	clear_white_list_table(9);
	printf("\r\nSum count:%d\r\n",clicker_count);
}

/******************************************************************************
  Function:clickers_retransmit
  Description:
		答题器数据重发
  Input :
		sumtable     ：总的索引表
		onlinetable  ：已经在线，无需重发的索引表
		nextsumtable ：下次统计的索引表
  Return:
  Others:None
******************************************************************************/
void clickers_retransmit(uint8_t sumtable, uint8_t onlinetable, uint8_t nextsumtable,uint8_t nextOnlinetable, uint16_t delayms, uint8_t send_times)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t puid[4];
	uint8_t index = 0;
	
	for(i=0;i<120;i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(sumtable,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(onlinetable,i);
			if(is_online_pos == 0)
			{
				get_index_of_uid(i,puid);
				set_index_of_white_list_pos(nextsumtable,i);
				
				/* 重发数据 */
				//clicker_send_data(puid,delayms);
				clicker_check_send_data(nextOnlinetable,i,puid,delayms,send_times);
				if(send_times == 1)
				{
					if(((index++)+1) % 5 == 0)
						printf("\n");
				}
			}
		}
	}
	printf("\n");
}

/******************************************************************************
  Function:App_clickers_send_data_process
  Description:
		答题器发送处理逻辑函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_send_data_process(void)
{
	uint8_t clicker_send_data_current_status = 0;
	static uint8_t clicker_count = 0;
	uint8_t lostuidlen = 0;
	
	/* 获取当前的systick的状态 */
	clicker_send_data_current_status = get_clicker_send_data_status();
	
	/* 上报第一次接受失败的UID */
	if(clicker_send_data_current_status == 2)
	{
		/* 返回失败的UID */
		uint8_t Is_over = 0;
		printf("\r\n第1次发送统计结果：");
		printf("\r\nlost:\r\n");
		Is_over = checkout_outline_uid(0,3,0,revice_data_massage.DATA,&(revice_data_massage.LEN));
		lostuidlen = revice_data_massage.LEN;
				
		revice_data_massage.LEN = 0;
		printf("\r\nok:\r\n");
		Is_over = checkout_outline_uid(0,3,1,revice_data_massage.DATA,&(revice_data_massage.LEN));
		revice_data_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_data_massage.TYPE)), revice_data_massage.LEN+6);
		revice_data_massage.END = 0xCA;
		clicker_count = revice_data_massage.LEN/4;
		printf("\r\ncount:%d\r\n",revice_data_massage.LEN/4);
		revice_data_massage.LEN = 0;
		/* 上传在线状态 */
		if(lostuidlen != 0)
		{
			if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				//serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_data_massage);
			}

			
			if(Is_over == 0)
			{
				change_clicker_send_data_status(3);
				//change_clicker_send_data_status(0);
				//clear_uid_check_table();
			}
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table(clicker_count);
		}
	}
	
	/* 上报之后，重新单独发送 */
	if(clicker_send_data_current_status == 3)
	{
		printf("\r\n\r\n[1].retransmit:\r\n");
		clickers_retransmit(0,3,6,4,0,1);
		
		/* 跟新状态，开始2次统计 */
		change_clicker_send_data_status(4);
	}

	if(clicker_send_data_current_status == 5)
	{
		/* 返回失败的UID */
		uint8_t Is_over = 0;
		printf("\r\n第2次发送统计结果： ");
		printf("\r\nlost:\r\n");
		Is_over = checkout_outline_uid(6,4,0,revice_data_massage.DATA,&(revice_data_massage.LEN));
		lostuidlen = revice_data_massage.LEN;
				
		revice_data_massage.LEN = 0;
		printf("\r\nok:\r\n");
		Is_over = checkout_outline_uid(6,4,1,revice_data_massage.DATA,&(revice_data_massage.LEN));
		revice_data_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_data_massage.TYPE)), revice_data_massage.LEN+6);
		revice_data_massage.END = 0xCA;
		clicker_count += revice_data_massage.LEN/4;
		printf("\r\ncount:%d\r\n",revice_data_massage.LEN/4);
		revice_data_massage.LEN = 0;
		
		/* 上传在线状态 */
		if(lostuidlen != 0)
		{
			if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				//serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_data_massage);
			}

				
			if(Is_over == 0)
			{
				change_clicker_send_data_status(6);
			}
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table(clicker_count);
		}	
	}
	
	/* 上报之后，重新单独发送 */
	if(clicker_send_data_current_status == 6)
	{
		printf("\r\n\r\n[2].retransmit:\r\n");
		clickers_retransmit(6,4,7,5,0,1);

		/* 跟新状态，开始2次统计 */
		change_clicker_send_data_status(7);
	}
	
	if(clicker_send_data_current_status == 8)
	{
		/* 返回失败的UID */
		uint8_t Is_over = 0;
		printf("\r\n第3次发送统计结果： ");
		printf("\r\nlost:\r\n");
		Is_over = checkout_outline_uid(7,5,0,revice_data_massage.DATA,&(revice_data_massage.LEN));
		lostuidlen = revice_data_massage.LEN;
					
		revice_data_massage.LEN = 0;
		printf("\r\nok:\r\n");
		Is_over = checkout_outline_uid(7,5,1,revice_data_massage.DATA,&(revice_data_massage.LEN));
		revice_data_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_data_massage.TYPE)), revice_data_massage.LEN+6);
		revice_data_massage.END = 0xCA;
		clicker_count += revice_data_massage.LEN/4;
		printf("\r\ncount:%d\r\n",revice_data_massage.LEN/4);
		revice_data_massage.LEN = 0;
		
		/* 上传在线状态 */
		if(lostuidlen != 0)
		{
			if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				//serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_data_massage);
			}

			if(Is_over == 0)
			{
				change_clicker_send_data_status(9);
				//clear_uid_check_table();
			}
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table(clicker_count);
		}	
	}
	
	/* 第三次上报之后，重新单独发送 */
	if(clicker_send_data_current_status == 9)
	{
		printf("\r\n\r\n[3].retransmit:\r\n");
		clickers_retransmit(7,5,9,8,1200,3);

		/* 跟新状态，开始2次统计 */
		change_clicker_send_data_status(10);
	}
	
	if(clicker_send_data_current_status == 11)
	{
		/* 返回失败的UID */
		uint8_t Is_over = 0;
		printf("\r\n第4次发送统计结果： ");
		printf("\r\nlost:\r\n");
		Is_over = checkout_outline_uid(9,8,0,revice_data_massage.DATA,&(revice_data_massage.LEN));
		lostuidlen = revice_data_massage.LEN;
					
		revice_data_massage.LEN = 0;
		printf("\r\nok:\r\n");
		Is_over = checkout_outline_uid(9,8,1,revice_data_massage.DATA,&(revice_data_massage.LEN));
		revice_data_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_data_massage.TYPE)), revice_data_massage.LEN+6);
		revice_data_massage.END = 0xCA;
		clicker_count += revice_data_massage.LEN/4;
		printf("\r\ncount:%d\r\n",revice_data_massage.LEN/4);
		
		revice_data_massage.LEN = 0;
		
		/* 上传在线状态 */
		if(lostuidlen!= 0)
		{
			if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				//serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_data_massage);
			}
	
			if(Is_over == 0)
			{
				change_clicker_send_data_status(0);
				clear_uid_check_table(clicker_count);
			}
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table(clicker_count);
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
				is_white_list_uid = add_uid_to_white_list(g_cSNR+5,&uid_p);

				if(is_white_list_uid != OPERATION_ERR)
				{
          // OK
					cmd_process_status = 1;
				}
				else
				{
					// Err 0x29 E3
					cmd_process_status = 2;
					App_returnErr(&card_message,0x26,0xFD);
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
					memcpy(card_message.DATA,g_cSNR+5,4);
					card_message.XOR = XOR_Cal(&card_message.TYPE,10);
					card_message.END  = 0xCA;
				}
			}

			if(cmd_process_status != 0)
			{
				/* 执行完的指令存入发送缓存 */
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
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
