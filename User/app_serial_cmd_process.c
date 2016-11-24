/**
  ******************************************************************************
  * @file   	app_send_data_process.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  * @Changelog :
  *    [1].Date   : 2016_8-26
	*        Author : sam.wu
	*        brief  : 尝试分离数据，降低代码的耦合度
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_send_data_process.h"
#include "app_card_process.h"
#include "app_systick_package_process.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t whitelist_print_index = 0;

extern uint8_t is_open_statistic;
extern uint8_t uart_tx_status;
extern nrf_communication_t nrf_communication;
       uint8_t serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
			 uint8_t serial_cmd_type = 0;
			 uint8_t err_cmd_type = 0;

/* 暂存题目信息，以备重发使用 */
Uart_MessageTypeDef backup_massage;

extern WhiteList_Typedef wl;
extern Revicer_Typedef   revicer;
extern Process_tcb_Typedef systick_process;

/* Private functions ---------------------------------------------------------*/
static void serial_send_data_to_pc(void);
static void serial_cmd_process(void);

void App_initialize_white_list( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_send_data_to_clicker_return( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_stop_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_operate_uids_to_whitelist( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_open_or_close_white_list( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
uint8_t App_return_whitelist_data( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage, uint8_t index);
void App_open_or_close_attendance_match( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_return_device_info( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_returnErr( Uart_MessageTypeDef *SMessage, uint8_t cmd_type, uint8_t err_type );
void App_uart_message_copy( Uart_MessageTypeDef *SrcMessage, Uart_MessageTypeDef *DstMessage );
void App_return_systick( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_send_process_parameter_set( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_open_systick_ack( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_card_match_single( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_card_match( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_start_or_stop_answer( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );

/******************************************************************************
  Function:App_seirial_cmd_process
  Description:
		串口进程处理函数
  Input :None
  Return:None
  Others:None
******************************************************************************/
void App_seirial_cmd_process(void)
{
	/* send process data to pc */
	serial_send_data_to_pc();

	/* serial cmd process */
	serial_cmd_process();
}

/******************************************************************************
  Function:serial_send_data_to_pc
  Description:
		串口发送指令函数
  Input :None
  Return:None
  Others:None
******************************************************************************/
static void serial_send_data_to_pc(void)
{
	if( uart_tx_status == 0)
	{
		/* enable interrupt Start send data*/
		USART_ITConfig(USART1pos, USART_IT_TXE, ENABLE);
  }
}

/******************************************************************************
  Function:serial_cmd_process
  Description:
		串口指令处理进程
  Input :None
  Return:None
  Others:None
******************************************************************************/
static void serial_cmd_process(void)
{
	static Uart_MessageTypeDef ReviceMessage,SendMessage;
	uint8_t buffer_status = 0;

  /* 系统空闲提取缓存指令 */
	if( serial_cmd_status == APP_SERIAL_CMD_STATUS_IDLE )
	{
		/* 获取接收缓存的状态 */
		buffer_status = buffer_get_buffer_status(REVICE_RINGBUFFER);

		/* 根据状态决定是否读取缓存指令 */
		if(BUFFEREMPTY == buffer_status)
			return;
		else
		{
			serial_ringbuffer_read_data(REVICE_RINGBUFFER, &ReviceMessage);
			serial_cmd_type = ReviceMessage.TYPE;
			serial_cmd_status = APP_SERIAL_CMD_STATUS_WORK;
		}
	}

	/* 系统不空闲解析指令，生产返回信息 */
	if( serial_cmd_status != APP_SERIAL_CMD_STATUS_IDLE )
	{
		/* 解析指令 */
		switch(serial_cmd_type)
		{
			/* 下发给答题器 */
			case 0x10:
				{
					App_send_data_to_clickers( &ReviceMessage, &SendMessage);
          #ifdef ENABLE_SEND_DATA_TO_PC
					if(ReviceMessage.DATA[6] == 0x15)
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IGNORE;
					else
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
          #else
					serial_cmd_status = APP_SERIAL_CMD_STATUS_IGNORE;
          #endif
				}
				break;

			/* 停止下发数据 */
			case 0x12:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_stop_send_data_to_clickers( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			/* 添加或者删除白名单 */
			case 0x20:
			case 0x21:
				{
					if(ReviceMessage.LEN != 4*ReviceMessage.DATA[0] + 1)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_operate_uids_to_whitelist( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			/* 初始化白名单 */
			case 0x22:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						/* 延迟：防止第一次的第一条指令为初始化时，后面的指令接收不完全 */
						DelayMs(100);
						App_initialize_white_list( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			/* 开启或者关闭白名单 */
			case 0x23:
			case 0x24:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_open_or_close_white_list( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			/* 开始或者关闭考勤,开始或者关闭配对 */
			case 0x25:
			case 0x27:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_open_or_close_attendance_match( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			case 0x28:
				{
					if(ReviceMessage.LEN != 20)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_card_match_single( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;
			case 0x41:
				{
					if((ReviceMessage.LEN != 1))
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_card_match( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			/* 打印当前白名单 */
			case 0x2B:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						whitelist_print_index = App_return_whitelist_data(
								&ReviceMessage, &SendMessage,whitelist_print_index );
						if( whitelist_print_index < wl.len )
						{
							serial_cmd_type = 0x2B;
							serial_cmd_status = APP_SERIAL_CMD_STATUS_WORK;
						}
						else
						{
							serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
							whitelist_print_index = 0;
						}
					}
				}
				break;


			/* 获取设备信息 */
			case 0x2C:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_return_device_info( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			case 0x2D:
				{
					if(ReviceMessage.LEN != 1)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_open_systick_ack( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			/* 返回心跳在线状态 */
			case 0x2E:
				{
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_return_systick( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			case 0x40:
				{
					if(ReviceMessage.LEN != 1)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_start_or_stop_answer( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
					}
				}
				break;

			case 0xA0:
				{
					if(ReviceMessage.LEN != 9)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						App_send_process_parameter_set( &ReviceMessage, &SendMessage);
						#ifdef ENABLE_SEND_DATA_TO_PC
						serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
						#else
					  serial_cmd_status = APP_SERIAL_CMD_STATUS_IGNORE;
						#endif
					}
				}
				break;

			case APP_CTR_DATALEN_ERR:
				{
					App_returnErr(&SendMessage,err_cmd_type,APP_CTR_DATALEN_ERR);
					serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
				}
				break;

			case APP_CTR_UNKNOWN:
				{
					App_returnErr(&SendMessage,err_cmd_type,APP_CTR_UNKNOWN);
					serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
				}
				break;

			/* 无法识别的指令 */
			default:
				{
					err_cmd_type = serial_cmd_type;
					serial_cmd_type = 0xff;
					serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
				}
				break;
		}
	}

	/* 执行完的指令存入发送缓存:指令没有出错 */
	if(serial_cmd_status != APP_SERIAL_CMD_STATUS_ERR)
	{
		if(serial_cmd_status == APP_SERIAL_CMD_STATUS_IGNORE)
		{
			serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
			return;
		}

		if(serial_cmd_status == APP_SERIAL_CMD_STATUS_WORK_IGNORE)
		{
			return;
		}

		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			if(serial_cmd_type != 0x2f)
				serial_ringbuffer_write_data(SEND_RINGBUFFER,&SendMessage);
		}
	}
}

void app_debuglog_dump(uint8_t * p_buffer, uint32_t len)
{
	uint32_t index = 0;

    for (index = 0; index <  len; index++)
    {
        DebugLog("%02X ", p_buffer[index]);
    }
    DebugLog("\r\n");
}

void app_debuglog_dump_no_space(uint8_t * p_buffer, uint32_t len)
{
	uint32_t index = 0;

    for (index = 0; index <  len; index++)
    {
        DebugLog("%02X", p_buffer[index]);
    }
    DebugLog("\r\n");
}

/******************************************************************************
  Function:App_send_data_to_clickers
  Description:
		将指令发送到答题器
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint16_t i = 0;
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);
	uint8_t temp = 0;
	uint8_t status = 0;

	uint8_t send_data_status = get_clicker_send_data_status() ;
	uint8_t single_data_status = get_single_send_data_status();

	status = send_data_status | single_data_status;

	/* 获取:包封装的答题器->数据长度 */
	rf_var.tx_len = RMessage->LEN;

	/* 获取：包封装的答题器->数据内容 */
	memcpy(rf_var.tx_buf, (uint8_t *)(RMessage->DATA), RMessage->LEN);

	if( status == 0 )
	{
		/* 获取下发数据: 决定是否暂存数据 */
		switch( RMessage->DATA[6] )
		{
			case 0x10:
			case 0x11:
				{
					/* 暂存题目 */
					backup_massage.HEADER = 0x5C;
					backup_massage.TYPE = RMessage->TYPE;
					memcpy(SMessage->SIGN, RMessage->SIGN, 4);
					backup_massage.LEN = RMessage->LEN;
					memcpy( backup_massage.DATA, (uint8_t *)(RMessage->DATA), RMessage->LEN );
					backup_massage.XOR = RMessage->XOR;
					backup_massage.END = 0xCA;
				}
				break;

			default: break;
		}
	}

	if((RMessage->DATA[1] != 0) || (RMessage->DATA[2] != 0) ||
		 (RMessage->DATA[3] != 0) || (RMessage->DATA[4] != 0))
	{
		/* single send data */
		is_open_statistic = 1;
	  memcpy(Single_send_data_process.uid,RMessage->DATA+1,4);
	}
	else
	{
		is_open_statistic = 0;
		memcpy(Send_data_process.uid,RMessage->DATA+1,4);
	}

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x03;

	if( status == 0)
	{
		*( pdata + ( i++ ) ) = 0x00; // ok
	}
	else
	{
		*( pdata + ( i++ ) ) = 0x01; // busy
	}

	*( pdata + ( i++ ) ) = wl.switch_status;
	*( pdata + ( i++ ) ) = wl.len;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;

	if( status == 0 )
	{
		/* 准备发送数据管理块 */
		send_data_env_init();

		/* 下发计数加1 */
		revicer.sen_num++;

		/* 发送前导帧 */
		memcpy(nrf_communication.dtq_uid, RMessage->DATA+1, 4);
		nrf_transmit_start( &temp, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,  SEND_PRE_DELAY100US, SEND_DATA1_SUM_TABLE);

		/* 发送数据帧 */
		nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL, SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE );

		if( is_open_statistic == 0 )
		{
			/* 启动发送数据状态机 */
			change_clicker_send_data_status( SEND_DATA1_STATUS );
		}
		else
		{
			/* 启动单独发送数据状态机 */
			change_single_send_data_status(1);
		}

		/* 清除心跳包定时时间 */
		sw_clear_timer(&systick_package_timer);
	}
}

/******************************************************************************
  Function:App_initialize_white_list
  Description:
	  初始化白名单
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_initialize_white_list( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);
	uint8_t i = 0;
	uint8_t whitelist_init_status = 0;

	whitelist_init_status = initialize_white_list();

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = 0x22;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x01;

	if(whitelist_init_status == OPERATION_SUCCESS)
	{
		*( pdata + ( i++ ) ) = 0;
	}
	else
	{
		*( pdata + ( i++ ) ) = 1;
	}

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;

}

/******************************************************************************
  Function:App_open_or_close_white_list
  Description:
	  初始化白名单
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_open_or_close_white_list( Uart_MessageTypeDef *RMessage,
				Uart_MessageTypeDef *SMessage )
{
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);
	uint8_t i = 0;
	bool openstatus = false;

	SMessage->HEADER = 0x5C;

	if(RMessage->TYPE == 0x23)
		wl.switch_status = 1;
	else
		wl.switch_status = 0;

	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x01;

	openstatus = store_switch_status_to_fee(wl.switch_status);

	if(openstatus == OPERATION_SUCCESS)
	{
		*( pdata + ( i++ ) ) = 0;
	}
	else
	{
		*( pdata + ( i++ ) ) = 1;
	}

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;

}

/******************************************************************************
  Function:App_send_data_to_clickers
  Description:
		将指令发送到答题器
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_stop_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint16_t i = 0;
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);

	memset(rf_var.tx_buf, 0x00, rf_var.tx_len);
	rf_var.tx_len = 0x00;

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x03;

	{
		send_data_env_init();

		memset(nrf_communication.dtq_uid,0, 4);
		memset( rf_var.tx_buf, 0,rf_var.tx_len);

		change_clicker_send_data_status( 0 );
	}

	*( pdata + ( i++ ) ) = 0x00;
	*( pdata + ( i++ ) ) = wl.switch_status;
	*( pdata + ( i++ ) ) = wl.len;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}


/******************************************************************************
  Function:App_operate_uids_to_whitelist
  Description:
		添加UID到白名单
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_operate_uids_to_whitelist( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	/* 获取需要添加的UID的个数:最大为10*/
	uint8_t UidNum = RMessage->DATA[0];
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);
	uint8_t TemUid[4];
	uint8_t i = 0,j = 0,k = 0,uidpos;
	uint8_t opestatus = 0;
	uint8_t NewUidNum = 0;
	uint8_t UidAddStatus[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	SMessage->HEADER = 0x5C;

	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 10;

	for(j = 0; j < UidNum; j++)
	{
		TemUid[0] = RMessage->DATA[1+j*4];
		TemUid[1] = RMessage->DATA[2+j*4];
		TemUid[2] = RMessage->DATA[3+j*4];
		TemUid[3] = RMessage->DATA[4+j*4];

		if(RMessage->TYPE == 0x20)
			opestatus = add_uid_to_white_list(TemUid,&uidpos);

		if(RMessage->TYPE == 0x21)
			opestatus = delete_uid_from_white_list(TemUid);

		if(opestatus == OPERATION_ERR)
		{
			UidAddStatus[j/8] |= 1<<((k++)%8); // fail
		}
		else
		{
			UidAddStatus[j/8] &= ~1<<((k++)%8); // success
			NewUidNum++;
		}
	}

	*( pdata + ( i++ ) ) = NewUidNum;

	for(j=0;j<8;j++)
	{
		*( pdata + ( i++ ) ) = UidAddStatus[j];
	}

	*( pdata + ( i++ ) ) = wl.len;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;

}

/******************************************************************************
  Function:App_return_whitelist_data
  Description:
		打印白名单信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
		index：打印白名单的起始位置
  Return:
    uid_p:输出的白名单最后的位置
  Others:None
******************************************************************************/
uint8_t App_return_whitelist_data( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage, uint8_t index)
{
	uint8_t *pdata = (uint8_t *)(SMessage->DATA+1);
	uint8_t uid_p = index,uid_count = 0;
	uint8_t tempuid[4] = {0,0,0,0};

	SMessage->HEADER = 0x5C;

	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	while((uid_count*5<UART_NBUF-6)&&(uid_p<120))
	{
		if(OPERATION_SUCCESS == get_index_of_uid(uid_p,tempuid))
		{
			*pdata++ = uid_p;
			*pdata++ = tempuid[0];
			*pdata++ = tempuid[1];
			*pdata++ = tempuid[2];
			*pdata++ = tempuid[3];
			uid_count++;
		}
		uid_p++;
	}

	SMessage->DATA[0] = wl.len;
	SMessage->LEN = uid_count*5+1;
	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), SMessage->LEN+6);
	SMessage->END = 0xCA;

	return uid_p;
}

/******************************************************************************
  Function:App_open_or_close_attendance
  Description:
		开启或者关闭考勤
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_open_or_close_attendance_match( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;

	SMessage->HEADER = 0x5C;

	switch(RMessage->TYPE)
	{
		case 0x25: wl.attendance_sttaus = ON;  break;
		case 0x27: wl.attendance_sttaus = OFF; break;
		default:                               break;
	}

	Card_process.cmd_type = RMessage->TYPE;
	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	memcpy(Card_process.uid,RMessage->SIGN,4);

	SMessage->LEN = 0x01;
	SMessage->DATA[i++] = 0;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_return_device_info
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_return_device_info( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t temp_count = 0,i = 0,j=0;
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);

	SMessage->HEADER = 0x5C;

	if(RMessage->TYPE == 0x28)
	{
		wl.match_status = ON;
	}
	else
	{
		wl.match_status = OFF;
	}

	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	for(temp_count=0,i=0;(temp_count<4)&&(i<8);temp_count++,i=i+2)
	{
			*( pdata + ( j++ ) )=(jsq_uid[i]<<4|jsq_uid[i+1]);
	}

	for(temp_count=0;temp_count<3;temp_count++)
	{
			*( pdata + ( j++ ) )=software[temp_count];
	}

	for(temp_count=0,i=0;(temp_count<15)&&(i<30);temp_count++,i=i+2)
	{
			*( pdata + ( j++ ) )=(hardware[i]<<4)|(hardware[i+1]);
	}

	for(temp_count=0,i=0;(temp_count<8)&&(i<16);temp_count++,i=i+2)
	{
			*( pdata + ( j++ ) )=(company[i]<<4)|(company[i+1]);
	}

	SMessage->LEN = j;
	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), j+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_return_device_info
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_return_systick( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;

	SMessage->HEADER = 0x5C;

	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x04;

	SMessage->DATA[i++] = (jsq_uid[1]&0x0F)|((jsq_uid[0]<<4)&0xF0);
	SMessage->DATA[i++] = (jsq_uid[3]&0x0F)|((jsq_uid[2]<<4)&0xF0);
	SMessage->DATA[i++] = (jsq_uid[5]&0x0F)|((jsq_uid[4]<<4)&0xF0);
	SMessage->DATA[i++] = (jsq_uid[7]&0x0F)|((jsq_uid[6]<<4)&0xF0);

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;

	/* update RTC timer */
	{
		system_rtc_timer.year = *(uint16_t *)(RMessage->DATA);
		system_rtc_timer.mon  = RMessage->DATA[2];
		system_rtc_timer.date = RMessage->DATA[3];
		system_rtc_timer.hour = RMessage->DATA[4];
		system_rtc_timer.min  = RMessage->DATA[5];
		system_rtc_timer.sec  = RMessage->DATA[6];
	}
}

/******************************************************************************
  Function:App_send_process_parameter_set
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_send_process_parameter_set( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;
	uint8_t err = 0;
	send_data_process_tcb_tydef temp_tcb;

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	SMessage->LEN = 0x01;

	/* parameter check */
	{
		temp_tcb.pre_data_count   = RMessage->DATA[0];
		DEBUG_SET_SEND_DATA_PARAMETER("pre_data_count = %d\r\n",temp_tcb.pre_data_count);
		temp_tcb.pre_data_delay100us = *((uint16_t *)(RMessage->DATA+1));
		DEBUG_SET_SEND_DATA_PARAMETER("pre_data_delay100us = %d\r\n",temp_tcb.pre_data_delay100us);
		temp_tcb.data_count       = RMessage->DATA[3];
		DEBUG_SET_SEND_DATA_PARAMETER("data_count = %d\r\n",temp_tcb.data_count);
		temp_tcb.data_delay100us     = *((uint16_t *)(RMessage->DATA+4));;
		DEBUG_SET_SEND_DATA_PARAMETER("data_delay100us = %d\r\n",temp_tcb.data_delay100us);
		temp_tcb.rand_delayms     = *((uint16_t *)(RMessage->DATA+6));;
		DEBUG_SET_SEND_DATA_PARAMETER("rand_delayms = %d\r\n",temp_tcb.rand_delayms);
		temp_tcb.retransmit_count     = *((uint16_t *)(RMessage->DATA+8));;
		DEBUG_SET_SEND_DATA_PARAMETER("retransmit_count = %d\r\n",temp_tcb.retransmit_count+3);

		if( temp_tcb.pre_data_count == 0 )
			err |= 1<<0;
		if( temp_tcb.pre_data_delay100us < 10 )
			err |= 1<<1;
		if( temp_tcb.data_count < 2 )
			err |= 1<<2;
		if( temp_tcb.data_delay100us < 20 )
			err |= 1<<3;
		if( temp_tcb.rand_delayms < 120)
			err |= 1<<4;
		if( temp_tcb.retransmit_count >= 10 )
			err |= 1<<5;
	}

	if( err == 0 )
	{
		send_data_process_tcb.pre_data_count      = temp_tcb.pre_data_count;
		send_data_process_tcb.pre_data_delay100us = temp_tcb.pre_data_delay100us;
		send_data_process_tcb.data_count          = temp_tcb.data_count;
		send_data_process_tcb.data_delay100us     = temp_tcb.data_delay100us;
		send_data_process_tcb.rand_delayms        = temp_tcb.rand_delayms;
		send_data_process_tcb.retransmit_count    = temp_tcb.retransmit_count;
		SMessage->DATA[i++] = 0;
	}
	else
	{
		SMessage->DATA[i++] = err;
	}
	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_open_systick_ack
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_open_systick_ack( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;
	uint8_t err = 0;

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x01;

	if( RMessage->DATA[0] <= 1 )
	{
		systick_set_ack_funcction(RMessage->DATA[0]);
	  memcpy(systick_process.uid, RMessage->SIGN, 4);
		systick_process.cmd_type = RMessage->TYPE;
		/* parameter check */
		SMessage->DATA[i++] = 0;
	}
	else
	{
		err = 1;
	  memset(systick_process.uid, 0, 4);
		systick_process.cmd_type = 0;
		SMessage->DATA[i++] = err;
	}

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_card_match
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_card_match_single( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;

	SMessage->HEADER = 0x5C;
	Card_process.cmd_type = RMessage->TYPE;
	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	memcpy(Card_process.uid,RMessage->SIGN,4);

	SMessage->LEN = 0x01;

	memcpy(Card_process.studentid,RMessage->DATA,20);
	Card_process.match_single = 1;
	wl.match_status = ON;
	SMessage->DATA[i++] = 0;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_card_match
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_card_match( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;

	SMessage->HEADER = 0x5C;
	Card_process.cmd_type = RMessage->TYPE;
	SMessage->TYPE = RMessage->TYPE;

	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	memcpy(Card_process.uid,RMessage->SIGN,4);

	SMessage->LEN = 0x01;

	if( RMessage->DATA[0] < 2 )
	{
		memset(Card_process.studentid,0x00,20);
		Card_process.match_single = 0;
		SMessage->DATA[i++] = 0;
		if( RMessage->DATA[0] == 1 )
		{
			wl.match_status = ON;
		}
		else
		{
			wl.match_status = OFF;
		}
	}
	else
	{
		memset(Card_process.studentid,0x00,20);
		Card_process.match_single = 0;
		SMessage->DATA[i++] = 1;
		wl.match_status = OFF;
	}

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_start_or_stop_answer
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_start_or_stop_answer( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage )
{
	uint8_t i = 0;

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;

	memcpy(Card_process.uid,RMessage->SIGN,4);

	SMessage->LEN = 0x01;

	if( RMessage->DATA[0] < 2 )
	{
		wl.start = RMessage->DATA[0];
		SMessage->DATA[i++] = 0;

		if( wl.start == OFF )
		{
			memset(&backup_massage,0,sizeof(Uart_MessageTypeDef));
		}
	}
	else
	{
		wl.start = OFF;
		SMessage->DATA[i++] = 1;
	}

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/******************************************************************************
  Function:App_returnErr
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
void App_returnErr( Uart_MessageTypeDef *SMessage, uint8_t cmd_type, uint8_t err_type )
{
	uint8_t i = 0;
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);

	SMessage->HEADER = 0x5C;
	SMessage->TYPE   = err_type;

	memset(SMessage->SIGN, 0xFF, 4);

	SMessage->LEN = 2;

	/* 操作失败 */
	*( pdata + ( i++ ) ) = 0x01;
	/* 错误类型 */
	*( pdata + ( i++ ) ) = cmd_type;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}

/**************************************END OF FILE****************************/

