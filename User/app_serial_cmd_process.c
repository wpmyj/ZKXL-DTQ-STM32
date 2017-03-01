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
StateMechineTcb_Typedef uart_rev_status,uart_sen_status;
uint8_t P_Vresion[2] = { 0x00, 0x02 };
//Timer_typedef uart_irq_rev_timer;

extern StateMechineTcb_Typedef default_state_mechine_tcb;
extern uint8_t is_open_statistic;
//extern uint8_t uart_tx_status;
extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[16][8];
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

uint8_t App_send_data_to_clickers( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
uint8_t App_operate_uids_to_whitelist( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
uint8_t App_return_device_info( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
uint8_t App_clicker_parameter_set( Uart_MessageTypeDef *RMessage, Uart_MessageTypeDef *SMessage );
uint8_t App_returnErr( Uart_MessageTypeDef *sMessage, uint8_t cmd_type, uint8_t err_type );
/******************************************************************************
  Function:App_seiral_process_init
  Description:
  Input :None
  Return:None
  Others:None
******************************************************************************/
void App_seiral_process_init( void )
{
	/* init uart_rev_status  */
	memcpy(&uart_rev_status,&default_state_mechine_tcb,
	       sizeof(StateMechineTcb_Typedef));
	memcpy(uart_rev_status.state.desc,"uart_rev_status",
				sizeof("uart_rev_status"));
	uart_rev_status.set_status(&(uart_rev_status.state),UartHEAD);
	
  /* init uart_irq_send_status */	
	memcpy(&uart_sen_status,&default_state_mechine_tcb,
	       sizeof(StateMechineTcb_Typedef));
	memcpy(uart_sen_status.state.desc,"uart_irq_send_status",
				sizeof("uart_irq_send_status"));
}

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
	if((uart_sen_status.get_status(&(uart_sen_status.state))) == 0)
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
			serial_cmd_type = ReviceMessage.CMDTYPE;
			revicer.uart_pac_num = ReviceMessage.PACNUM;
			revicer.uart_seq_num = ReviceMessage.SEQNUM;
			serial_cmd_status = APP_SERIAL_CMD_STATUS_WORK;
		}
	}

	/* 系统不空闲解析指令，生产返回信息 */
	if( serial_cmd_status != APP_SERIAL_CMD_STATUS_IDLE )
	{
		/* 解析指令 */
		switch(serial_cmd_type)
		{
			/* 下发数据给答题器 */
			case 0x10:
				{
					if(ReviceMessage.LEN == 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						serial_cmd_status = App_send_data_to_clickers( &ReviceMessage, &SendMessage);;
					}
				}
				break;

			/* 接收器设置指令 */
			case 0x20:
				{
					if(ReviceMessage.LEN == 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						serial_cmd_status = App_clicker_parameter_set( &ReviceMessage, &SendMessage);
					}
				}
				break;

			/* 操作白名单 */
			case 0x30:
				{
					if(ReviceMessage.LEN == 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						serial_cmd_status = App_operate_uids_to_whitelist( &ReviceMessage, &SendMessage);;
					}
				}
				break;

			/* 心跳指令 */
			case 0x40:
				{
					if(ReviceMessage.LEN == 0)
					{
						err_cmd_type = serial_cmd_type;
						serial_cmd_type = APP_CTR_DATALEN_ERR;
						serial_cmd_status = APP_SERIAL_CMD_STATUS_ERR;
					}
					else
					{
						serial_cmd_status = App_return_device_info( &ReviceMessage, &SendMessage);;
					}
				}
				break;

			case APP_CTR_DATALEN_ERR:
				{
					serial_cmd_status = App_returnErr(&SendMessage,err_cmd_type,APP_CTR_DATALEN_ERR);;
				}
				break;

			case APP_CTR_UNKNOWN:
				{
					serial_cmd_status = App_returnErr(&SendMessage,err_cmd_type,APP_CTR_UNKNOWN);;
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
uint8_t App_send_data_to_clickers( Uart_MessageTypeDef *rMessage, Uart_MessageTypeDef *sMessage )
{
	uint8_t  status = 0, err;

	typedef struct
	{
		uint8_t TASKTYPE;
		uint8_t DSTID[4];
		uint8_t TASKNUM;
	}TransmitInfo_Tydef;
	
	typedef struct
	{
		uint8_t TASKTYPE;
		uint8_t SENDDATA[2];
	}TransmitCtl_Tydef;

	typedef struct
	{
		uint8_t DATATYPE;
		uint8_t TASKDATA;
		uint8_t SENDDATA;
	}TransmitData_Tydef;

	/* 解析指令 */
	TransmitInfo_Tydef *pRdata = (TransmitInfo_Tydef *)(rMessage->DATA);
	uint8_t  *pSdata = (uint8_t *)rf_var.tx_buf;
	{

		uint16_t rdata_index = 0, sdata_index = 0;
  	uint8_t  is_last_data_full = 0;

		rdata_index = sizeof(TransmitInfo_Tydef);

		if(pRdata->TASKTYPE == 0x01) // DATA
		{
			TransmitData_Tydef *data;

			while( rdata_index < *(uint16_t *)rMessage->LEN )
			{
				data = (TransmitData_Tydef *)(rMessage->DATA + rdata_index);

		    /* 新版协议解析格式 */                                                    
				if(is_last_data_full == 0)
				{
					*(pSdata+(sdata_index++)) = ((data->DATATYPE) & 0x0F   ) | ((data->TASKDATA & 0x0F) << 4);
					*(pSdata+(sdata_index++)) = ((data->TASKDATA & 0xF0)>>4) | ((data->SENDDATA & 0x0F) << 4);
					*(pSdata+(sdata_index))   = (data->SENDDATA & 0xF0)>>4;
					is_last_data_full = 1;
				}
				else
				{
					*(pSdata+(sdata_index))   = *(pSdata+(sdata_index)) | ((data->DATATYPE & 0x0F) << 4);
					sdata_index++;
					*(pSdata+(sdata_index++)) = data->TASKDATA ;
					*(pSdata+(sdata_index++)) = data->SENDDATA ;
					is_last_data_full = 0;
				}

				rdata_index = rdata_index + sizeof(TransmitData_Tydef);
			}
		}
		else if(pRdata->TASKTYPE == 0x02) // CTL
		{
			TransmitCtl_Tydef *ctl_data;

			while( rdata_index < *(uint16_t *)rMessage->LEN )
			{
				ctl_data = (TransmitCtl_Tydef *)(rMessage->DATA + rdata_index);
				switch(ctl_data->TASKTYPE)
				{
					case 0x01: // 关机 
					{
						*(pSdata+(sdata_index++)) = 0x01;
					}
					break;
					case 0x02: // 清屏
					break;
					case 0x03: // 获取电量
					break;
					default: 
					break;
				}
				
				rdata_index = rdata_index + sizeof(TransmitCtl_Tydef);
			}
			
			
		}

		rf_var.tx_len = sdata_index+1 ;
	}

	/* 生成返回数据 */
	{
		sMessage->HEAD = UART_SOF;
		sMessage->DEVICE  = 0x01;
		memcpy(sMessage->VERSION,P_Vresion,2);
		memcpy(sMessage->SRCID,revicer.uid,UID_LEN);
		memcpy(sMessage->DSTID,rMessage->SRCID,UID_LEN);
		sMessage->SEQNUM = revicer.uart_seq_num++;
		sMessage->PACNUM = rMessage->PACNUM;
		sMessage->PACKTYPE = REVICER_PACKAGE_ACK;
		sMessage->CMDTYPE = rMessage->CMDTYPE;
		memset(sMessage->REVICED,0xAA,2);
		*(uint16_t *)(sMessage->LEN) = 0x01;

		status  = get_clicker_send_data_status() ;
		status |= get_single_send_data_status();
		
		if( status == 0)
			sMessage->DATA[0] = 0x00; // ok
		else
			sMessage->DATA[0] = 0x01; // busy

		sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DEVICE)), 
		*(uint16_t *)(sMessage->LEN)+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
		sMessage->END = 0xCA;
	}

	/* 发送数据 */
	if( status == 0 )
	{
		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
		send_data_env_init();
		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);
		
		memcpy(nrf_data.dtq_uid, pRdata->DSTID, 4);

		memcpy(transmit_config.dist,pRdata->DSTID, 4);
		transmit_config.package_type   = NRF_DATA_IS_PRE;
		transmit_config.transmit_count = SEND_PRE_COUNT;
		transmit_config.delay100us     = SEND_PRE_DELAY100US;
		transmit_config.is_pac_add     = PACKAGE_NUM_ADD;
		transmit_config.data_buf       = NULL;
		transmit_config.data_len       = 0;
		transmit_config.is_add_table   = 1;
		transmit_config.sel_table      = SEND_PRE_TABLE;
		nrf_transmit_start( &transmit_config );

		/* 发送数据帧 */
		transmit_config.package_type   = NRF_DATA_IS_USEFUL;
		transmit_config.transmit_count = SEND_DATA_COUNT;
		transmit_config.delay100us     = SEND_DATA_DELAY100US;
		transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
		transmit_config.data_buf       = rf_var.tx_buf;
		transmit_config.data_len       = rf_var.tx_len;
		transmit_config.is_add_table   = 1;
		transmit_config.sel_table      = SEND_DATA_ACK_TABLE;
		nrf_transmit_start( &transmit_config );

		/* 启动发送数据状态机 */
		change_clicker_send_data_status( SEND_DATA1_STATUS );

		/* 清除心跳包定时时间 */
		sw_clear_timer(&systick_package_timer);
	}

	err = APP_SERIAL_CMD_STATUS_IDLE;

	return err;
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
uint8_t App_operate_uids_to_whitelist( Uart_MessageTypeDef *rMessage, Uart_MessageTypeDef *sMessage )
{
	uint8_t *rpdata;
	uint8_t i = 0,j = 0,k = 0,opestatus = 0,Err = 0;
	uint8_t NewUidNum = 0;
	uint8_t UidAddStatus[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t TemUid[4];
	UidTask_CTL_Typedef UidCmd;

	sMessage->HEAD = UART_SOF;
	sMessage->DEVICE = 0x01;
	memcpy(sMessage->VERSION,P_Vresion,2);
	memcpy(sMessage->DSTID,rMessage->SRCID,UID_LEN);
	memcpy(sMessage->SRCID,revicer.uid,UID_LEN);
	sMessage->PACNUM = rMessage->PACNUM;
	sMessage->SEQNUM = revicer.uart_seq_num++;
	sMessage->CMDTYPE = rMessage->CMDTYPE + 1;
	memset(sMessage->REVICED,0xAA,2);
	sMessage->DATA[0]  = rMessage->DATA[0];

	UidCmd = (UidTask_CTL_Typedef)rMessage->DATA[0];
	rpdata = rMessage->DATA+1;

	switch( UidCmd )
	{
		case U_BIND_ON:
		case U_BIND_OFF:
			{
				uint8_t *spdata = (uint8_t *)(sMessage->DATA);
				if( UidCmd == U_BIND_ON )
				{
					uint8_t card_current_status = 0;
					card_current_status = rf_get_card_status();
					if(card_current_status != 0)
					{
						*( spdata + ( i++ ) ) = 1;
					}
					else
					{
						*( spdata + ( i++ ) ) = 0;
						wl.match_status = ON;
						rf_set_card_status(1);
					}
				}

				if( UidCmd == U_BIND_OFF )
				{
					wl.match_status = OFF;
					rf_set_card_status(0);
					*( spdata + ( i++ ) ) = 0;
				}

				*(uint16_t *)sMessage->LEN = i;
				sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DEVICE)), 
					*(uint16_t *)(sMessage->LEN)+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
				sMessage->END = 0xCA;
				return APP_SERIAL_CMD_STATUS_IDLE;
				//return APP_SERIAL_CMD_STATUS_IGNORE;
			}

		case U_CLEAR:
			{
				uint8_t *spdata = (uint8_t *)(sMessage->DATA);

				Err = initialize_white_list();

				if( Err == OPERATION_SUCCESS )
				{
					*( spdata + ( i++ ) ) = 0;
				}
				else
				{
					*( spdata + ( i++ ) ) = 1;
				}

				sMessage->SEQNUM = revicer.uart_seq_num++;
				*(uint16_t *)sMessage->LEN = i;
				sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DEVICE)), 
					*(uint16_t *)(sMessage->LEN)+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
				sMessage->END = 0xCA;
				return APP_SERIAL_CMD_STATUS_IDLE;
			}
			
		case U_DEL:
			{
				uint8_t UidNum = (*(uint16_t *)(rMessage->LEN)-1)/4;
				uint8_t *spdata = (uint8_t *)(sMessage->DATA);

				for(j = 0; j < UidNum; j++)
				{
					uint8_t TemUid[4];
					TemUid[0] = *rpdata++;
					TemUid[1] = *rpdata++;
					TemUid[2] = *rpdata++;
					TemUid[3] = *rpdata++;

					opestatus = delete_uid_from_white_list(TemUid);

					if(opestatus == OPERATION_ERR)
					{
						UidAddStatus[j/8] |= 1<<((k++)%8); 
					}
					else
					{
						UidAddStatus[j/8] &= ~1<<((k++)%8); 
						NewUidNum++;
					}
				}

				/* return OK_COUNT */
				*( spdata + ( i++ ) ) = NewUidNum;
				/* return WL_LEN */
				*( spdata + ( i++ ) ) = wl.len;
				/* return DETAIL */
				for(j=0;j<8;j++)
				{
					*( spdata + ( i++ ) ) = UidAddStatus[j];
				}

				*(uint16_t *)sMessage->LEN = i+1;
				sMessage->SEQNUM = revicer.uart_seq_num++;
				sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DEVICE)), 
					i+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
				sMessage->END = 0xCA;
				return APP_SERIAL_CMD_STATUS_IDLE;
			}

		case U_SHOW:
			{
				uint8_t result,uid_p;
				uint8_t *spdata = sMessage->DATA+2;
				while(NewUidNum*5<UART_NBUF-6)
				{
					if(OPERATION_SUCCESS == get_index_of_uid(uid_p,TemUid))
					{
						*spdata++ = uid_p;
						*spdata++ = TemUid[0];
						*spdata++ = TemUid[1];
						*spdata++ = TemUid[2];
						*spdata++ = TemUid[3];
						NewUidNum++;
					}
					uid_p++;
				}
		
				if(uid_p<120)
				{
					sMessage->SEQNUM = revicer.uart_seq_num++;
					result =  APP_SERIAL_CMD_STATUS_WORK;
				}
				else
				{
					uid_p = 0;
					sMessage->SEQNUM = revicer.uart_seq_num++;
					result = APP_SERIAL_CMD_STATUS_IDLE;
				}
				
				sMessage->DATA[1] = wl.len;
				*(uint16_t *)sMessage->LEN = NewUidNum*5+1+1;
				sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DEVICE)), 
					*(uint16_t *)(sMessage->LEN)+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
				sMessage->END = 0xCA;
				NewUidNum = 0;
				
				return result;
			}

		default:
			{
				err_cmd_type = serial_cmd_type;
				serial_cmd_type = 0xff;
				return APP_SERIAL_CMD_STATUS_ERR;
			}
	}
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
uint8_t App_return_device_info( Uart_MessageTypeDef *rMessage, Uart_MessageTypeDef *sMessage )
{
	uint8_t temp_count = 0,i = 0,j=0;
	uint8_t err = 0;

	uint8_t *pdata = (uint8_t *)(sMessage->DATA);

	sMessage->HEAD = UART_SOF;
	sMessage->DEVICE = 0x01;
	memcpy(sMessage->VERSION,P_Vresion,2);
	memcpy(sMessage->DSTID,rMessage->SRCID,UID_LEN);
	memcpy(sMessage->SRCID,revicer.uid,UID_LEN);
	sMessage->PACNUM = rMessage->PACNUM;
	sMessage->SEQNUM = revicer.uart_seq_num++;
	sMessage->CMDTYPE = rMessage->CMDTYPE + 1;
	memcpy(sMessage->REVICED,rMessage->REVICED,2);

	/* parameter check and update RTC timer */
	{
		system_rtc_timer.year = *(uint16_t *)(rMessage->DATA);
		if(system_rtc_timer.year < 2016)
		{
			err |= 1<<0;
		}
		system_rtc_timer.mon  = rMessage->DATA[2];
		if(system_rtc_timer.mon > 12)
		{
			err |= 1<<1;
		}
		system_rtc_timer.date = rMessage->DATA[3];
		if(system_rtc_timer.date > 31)
		{
			err |= 1<<2;
		}
		system_rtc_timer.hour = rMessage->DATA[4];
		if(system_rtc_timer.hour > 24)
		{
			err |= 1<<3;
		}
		system_rtc_timer.min  = rMessage->DATA[5];
		if(system_rtc_timer.min > 60)
		{
			err |= 1<<4;
		}
		system_rtc_timer.sec  = rMessage->DATA[6];
		if(system_rtc_timer.sec > 60)
		{
			err |= 1<<5;
		}
	}

	/* return data to pc */
	if(err == 0)
	{
		*( pdata + ( j++ ) ) = err;
		
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
	}
	else
	{
		memset(&system_rtc_timer,0x00,sizeof(RTC_timer_Typedef));
		*( pdata + ( j++ ) ) = err;
	}
	sMessage->PACKTYPE = REVICER_PACKAGE_ACK;
	*(uint16_t *)sMessage->LEN = j;
	sMessage->XOR = XOR_Cal(&(sMessage->DEVICE), 
		j+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
	sMessage->END = 0xCA;
	
	return 0;
}


/******************************************************************************
  Function:App_clicker_parameter_set
  Description:
		打印设备信息
  Input :
		RMessage:串口接收指令的消息指针
		SMessage:串口发送指令的消息指针
  Return:
  Others:None
******************************************************************************/
uint8_t App_clicker_parameter_set( Uart_MessageTypeDef *rMessage, Uart_MessageTypeDef *sMessage )
{
	uint8_t i   = 0;
	uint8_t err = 0;
	Clicker_CTL_Typedf ClickerCmd;

	uint8_t *spdata = (uint8_t *)(sMessage->DATA+1);
	uint8_t *rpdata = (uint8_t *)(rMessage->DATA+1);

	sMessage->HEAD = UART_SOF;
	memcpy(sMessage->DSTID,rMessage->SRCID,UID_LEN);
	memcpy(sMessage->SRCID,rMessage->DSTID,UID_LEN);
	sMessage->PACNUM = rMessage->PACNUM;
	sMessage->SEQNUM = revicer.uart_seq_num++;
	sMessage->CMDTYPE = rMessage->CMDTYPE;
	memcpy(sMessage->REVICED,rMessage->REVICED,2);
	sMessage->DATA[0]  = rMessage->DATA[0];

	/* 获取上位机指令 */
	ClickerCmd = (Clicker_CTL_Typedf)(rMessage->DATA[0]);

	switch( ClickerCmd )
	{
		/* 开关机指令 */
		case G_OFF:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) != 2 )
			{
				err = 1 << 0;
			}

      /* 参数合理性校验 */
			if( *rpdata > 2 )
			{
				err = 1 << 1;
			}
			else
			{
				// add power set code
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;

    /* 获取电池电量 */
		case G_POWER:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) != 1 )
			{
				err = 1 << 0;
			}
			else
			{
				// add power set code
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;

		/* 显示操作 */
		case G_CL_P:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) != 2 )
			{
				err = 1 << 0;
			}
			else
			{
				// add power set code
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;

		/* 设置信道 */
		case N_CH:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) != 3 )
			{
				err = 1 << 0;
			}

			/* 参数合理性校验 */
			if(( *rpdata > 128) || ( *(rpdata+1) > 128)  || 
				 ( *rpdata == 0) || ( *(rpdata+1) == 0))
			{
				err = 1 << 1;
			}
			else
			{
				// add power set code
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;
		
		/* 设置发送与接收数据参数 */
		case N_TIME:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) != 5 )
			{
				err = 1 << 0;
			}
			/* 参数合理性校验 */
			if(( *rpdata < 1) || ( *(rpdata+1) < 1) || (*(uint16_t *)(rpdata+2) < 120))
			{
				err = 1 << 1;
			}
			else
			{
				// add power set code
				// RxOn = *rpdata;
				// RxOff = *(rpdata+1);
				// randdelay = *(uint16_t *)(rpdata+2);
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;

		case N_READ_ID:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN)!= 2 )
			{
				err = 1 << 0;
			}
			
			/* 参数合理性校验 */
			if( *rpdata > 2 )
			{
				err = 1 << 1;
			}
			else
			{
				//if( *rpdata == 1 )
					//rf_set_card_status(1);
				//if( *rpdata == 2 )
					//rf_set_card_status(0);
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;
		
		case N_WR_EE:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) < 5 )
			{
				err = 1 << 0;
			}
			else
			{
				// add power set code
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;
		
		case N_RD_EE:
		{
			/* 帧长度检验 */
			if( *(uint16_t *)(rMessage->LEN) < 5 )
			{
				err = 1 << 0;
			}
			else
			{
				// add power set code
			}
			*(spdata+i++) = err;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;

		default :
		{
			*(spdata+i++) = APP_CTR_UNKNOWN;
			*(uint16_t *)(sMessage->LEN) = 2;
		}
		break;
	}

	sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DSTID)), 
		*(uint16_t *)sMessage->LEN + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
	sMessage->END = 0xCA;
return 0;
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
uint8_t App_returnErr( Uart_MessageTypeDef *sMessage, uint8_t cmd_type, uint8_t err_type )
{
	uint8_t i = 0;
	uint8_t *pdata = (uint8_t *)(sMessage->DATA);

	sMessage->HEAD = UART_SOF;
	memset(sMessage->DSTID,0xff,UID_LEN);
	memset(sMessage->DSTID,0xff,UID_LEN);
	sMessage->SEQNUM = 0xff;
	sMessage->PACNUM = 0xff;
	sMessage->PACKTYPE = REVICER_PACKAGE_ACK;
	sMessage->CMDTYPE = REVICER_CLICKER_ERR;
	memset(sMessage->REVICED,0xff,2);

	*(uint16_t *)(sMessage->LEN) = 2;

	/* 操作失败 */
	*( pdata + ( i++ ) ) = 0x01;
	/* 错误类型 */
	*( pdata + ( i++ ) ) = cmd_type;

	sMessage->XOR = XOR_Cal((uint8_t *)(&(sMessage->DSTID)), i+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
	sMessage->END = 0xCA;
	
	return 0;
}

/**************************************END OF FILE****************************/
