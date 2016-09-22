/**
  ******************************************************************************
  * @file   	pos_handle_layer.c
  * @author  	Tian erjun
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

/* Private variables ---------------------------------------------------------*/	 
extern Uart_MessageTypeDef uart_irq_send_massage;
extern uint8_t uart_tx_status;
       uint8_t serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;	
			 uint8_t serial_cmd_type = 0;
			 uint8_t err_cmd_type = 0;
	     uint8_t sign_buffer[4];
			 uint8_t whitelist_print_index = 0;
			 uint8_t card_cmd_type = 0;
			 
uint8_t uart_rf_cmd_sign[4],uart_card_cmd_sign[4];				 
			 
			 
/* Private functions ---------------------------------------------------------*/
static void serial_send_data_to_pc(void);
static void serial_cmd_process(void);
//static void serial_transmission_to_nrf51822(void);

void App_initialize_white_list( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_stop_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_operate_uids_to_whitelist( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_open_or_close_white_list( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
uint8_t App_return_whitelist_data( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage, uint8_t index);
void App_open_or_close_attendance_match( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_return_device_info( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
void App_returnErr( Uart_MessageTypeDef *SMessage, uint8_t cmd_type, uint8_t err_type );
void App_uart_message_copy( Uart_MessageTypeDef *SrcMessage, Uart_MessageTypeDef *DstMessage );
void App_return_systick( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );

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
	
	//serial_transmission_to_nrf51822();
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
#ifdef ENABLE_DEBUG_LOG	
	uint8_t *pdata;
	uint8_t i,uart_tx_cnt = 0;
	
	if(BUFFEREMPTY == buffer_get_buffer_status(SEND_RINGBUFFER))
	{
		USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
		return;
	}
	else
	{
		serial_ringbuffer_read_data(SEND_RINGBUFFER, &uart_irq_send_massage);
		
		pdata = (uint8_t *)(uart_irq_send_massage.DATA);
		uart_tx_cnt = uart_irq_send_massage.LEN;
		
		printf("message->header = %2X \r\n",uart_irq_send_massage.HEADER);
		printf("message->type   = %2X \r\n",uart_irq_send_massage.TYPE);
		printf("message->sign   = %X%X%X%X \r\n",
				uart_irq_send_massage.SIGN[0],
				uart_irq_send_massage.SIGN[1],
				uart_irq_send_massage.SIGN[2],
				uart_irq_send_massage.SIGN[3]);
		printf("message->len    = %2X\r\n",uart_irq_send_massage.LEN);
		printf("message->data   = ");
		for(i=0;i<uart_tx_cnt;i++)
		{
			printf("%2X ",*pdata);
			pdata++;
		}
		printf(" \r\n");
		printf("message->xor    = %2X\r\n",uart_irq_send_massage.XOR);
		printf("message->end    = %2X\r\n",uart_irq_send_massage.END);
		
		printf(" \r\n");
		
	}
#else
#ifdef ENABLE_OUTPUT_MODE_NORMOL
	uint8_t *pdata;
	uint8_t i,uart_tx_cnt = 0;
	
	if(BUFFEREMPTY == buffer_get_buffer_status(SEND_RINGBUFFER))
	{
		return;
	}
	else
	{
		serial_ringbuffer_read_data(SEND_RINGBUFFER, &uart_irq_send_massage);
		
		pdata = (uint8_t *)(uart_irq_send_massage.DATA);
		uart_tx_cnt = uart_irq_send_massage.LEN;
		
		uart_send_char(uart_irq_send_massage.HEADER);
		uart_send_char(uart_irq_send_massage.TYPE);
		
		uart_send_char(uart_irq_send_massage.SIGN[0]);
		uart_send_char(uart_irq_send_massage.SIGN[1]);
		uart_send_char(uart_irq_send_massage.SIGN[2]);
		uart_send_char(uart_irq_send_massage.SIGN[3]);
		
		uart_send_char(uart_irq_send_massage.LEN);
		
		for(i=0;i<uart_tx_cnt;i++)
		{
			uart_send_char(*pdata);
			pdata++;
		}
		uart_send_char(uart_irq_send_massage.XOR);
		uart_send_char(uart_irq_send_massage.END);
	}
#else
	if( uart_tx_status == 0)
	{
		/* enable interrupt Start send data*/
		USART_ITConfig(USART1pos, USART_IT_TXE, ENABLE);
  }
#endif
#endif //ENABLE_DEBUG_LOG
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
					memcpy(uart_rf_cmd_sign,ReviceMessage.SIGN,4);
					App_send_data_to_clickers( &ReviceMessage, &SendMessage);
					serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
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
						serial_cmd_status = APP_CTR_IDLE;
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
						serial_cmd_status = APP_CTR_IDLE;
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
						serial_cmd_status = APP_CTR_IDLE;
					}
				}
				break;	
				
			/* 开始或者关闭考勤,开始或者关闭配对 */
			case 0x25:	
			case 0x27:					
			case 0x28:	
			case 0x2A:
				{				
					if(ReviceMessage.LEN != 0)
					{
						err_cmd_type = serial_cmd_type;	
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;	
					}
					else
					{
						memcpy(uart_card_cmd_sign,ReviceMessage.SIGN,4);
						App_open_or_close_attendance_match( &ReviceMessage, &SendMessage);
						serial_cmd_status = APP_CTR_IDLE;
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
						/* 延迟：增加操作Fee的时间间隔，防止读取数据出错 */
						DelayMs(30);
						whitelist_print_index = App_return_whitelist_data( 
								&ReviceMessage, &SendMessage,whitelist_print_index );
						if( whitelist_print_index < white_len )
						{
							serial_cmd_type = 0x2B;
							serial_cmd_status = APP_SERIAL_CMD_STATUS_WORK;
						}
						else
						{
							serial_cmd_status = APP_CTR_IDLE;
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
						serial_cmd_status = APP_CTR_IDLE;
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
						serial_cmd_status = APP_CTR_IDLE;
					}
				}
				break;
				
			case APP_CTR_DATALEN_ERR:
				{
					App_returnErr(&SendMessage,err_cmd_type,APP_CTR_DATALEN_ERR);
					serial_cmd_status = APP_CTR_IDLE;
				}
				break;
				
			case APP_CTR_UNKNOWN:
				{
					App_returnErr(&SendMessage,err_cmd_type,APP_CTR_UNKNOWN);
					serial_cmd_status = APP_CTR_IDLE;
				}
				break;
			
			/* 无法识别的指令 */
			default:	
				{
					serial_cmd_type = 0xff;	
					err_cmd_type = serial_cmd_type;	
					serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
				}
				break;
		}
	}
	
	/* 执行完的指令存入发送缓存:指令没有出错 */
	if(serial_cmd_status != APP_SERIAL_CMD_STATUS_ERR)
	{
		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
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
	
	/* 获取:包封装的答题器->数据长度 */
	rf_var.tx_len = RMessage->LEN;
	
	/* 获取：包封装的答题器->数据内容 */ 
	memcpy(rf_var.tx_buf, (uint8_t *)(RMessage->DATA), RMessage->LEN);
	
	/* 打开发送开关 */
	rf_var.flag_txing = true;

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x03;

	*( pdata + ( i++ ) ) = 0x00;
	*( pdata + ( i++ ) ) = white_on_off;
	*( pdata + ( i++ ) ) = white_len;

	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;

	/* 有数据下发且未曾下发过 */
	if(rf_var.flag_txing)	
	{
		my_nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL);
	
		rf_var.flag_tx_ok = true;
		rf_change_systick_status(1);
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
		white_on_off = ON;
	else
		white_on_off = OFF;
	
	SMessage->TYPE = RMessage->TYPE;
	
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	
	SMessage->LEN = 0x01;
	
	openstatus = store_switch_status_to_fee(white_on_off);
	
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

	rf_var.flag_txing = false;
	memset(rf_var.tx_buf, 0x00, rf_var.tx_len);
	rf_var.tx_len = 0x00;

	SMessage->HEADER = 0x5C;
	SMessage->TYPE = RMessage->TYPE;
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);

	SMessage->LEN = 0x03;

	*( pdata + ( i++ ) ) = 0x00;
	*( pdata + ( i++ ) ) = white_on_off;
	*( pdata + ( i++ ) ) = white_len;

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
	
	*( pdata + ( i++ ) ) = white_len;
	
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
	uint8_t i = 0;
	uint8_t *pdata = (uint8_t *)(SMessage->DATA);
	uint8_t uid_p = index;
	uint8_t tempuid[4] = {0,0,0,0};
	
	SMessage->HEADER = 0x5C;

	SMessage->TYPE = RMessage->TYPE;
	
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	
	pdata++;
	white_len = get_len_of_white_list();
	
	for(i=0;(i<UART_NBUF-6)&&(uid_p<white_len);i=i+4)
	{
		get_index_of_uid(uid_p,tempuid);

		*pdata++ = tempuid[0]; 
		*pdata++ = tempuid[1]; 
		*pdata++ = tempuid[2]; 
		*pdata++ = tempuid[3]; 
		uid_p++;
	}
	SMessage->DATA[0] = uid_p;
	SMessage->LEN = (uid_p-index)*4+1;
	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+7);
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
		case 0x25: attendance_on_off = ON;  break;
		case 0x27: attendance_on_off = OFF; break;
		case 0x28: match_on_off = ON;       break;
		case 0x2A: match_on_off = OFF;      break;
		default:                            break;
	}
	
	card_cmd_type = RMessage->TYPE;
	SMessage->TYPE = RMessage->TYPE;
		
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	
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
		match_on_off = ON;
		match_number = 1;
	}
	else
	{
		match_on_off = OFF;
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
	
	card_cmd_type = RMessage->TYPE;
	SMessage->TYPE = RMessage->TYPE;
		
	memcpy(SMessage->SIGN, RMessage->SIGN, 4);
	
	SMessage->LEN = 0x04;
	
	SMessage->DATA[i++] = (jsq_uid[1]&0x0F)|((jsq_uid[0]<<4)&0xF0);
	SMessage->DATA[i++] = (jsq_uid[3]&0x0F)|((jsq_uid[2]<<4)&0xF0);
	SMessage->DATA[i++] = (jsq_uid[5]&0x0F)|((jsq_uid[4]<<4)&0xF0);
	SMessage->DATA[i++] = (jsq_uid[7]&0x0F)|((jsq_uid[6]<<4)&0xF0);
	
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
	SMessage->TYPE   = cmd_type;
	
	memset(SMessage->SIGN, 0xFF, 4);
	
	SMessage->LEN = 2;
	
	/* 操作失败 */
	*( pdata + ( i++ ) ) = 0x01;
	/* 错误类型 */
	*( pdata + ( i++ ) ) = err_type;
	
	SMessage->XOR = XOR_Cal((uint8_t *)(&(SMessage->TYPE)), i+6);
	SMessage->END = 0xCA;
}




/**************************************END OF FILE****************************/

