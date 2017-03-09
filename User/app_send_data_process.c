/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "app_send_data_process.h"
#include "app_show_message_process.h"

#define CLICKER_SNED_DATA_STATUS_TYPE     10
#define CLICKER_PRE_DATA_STATUS_TYPE      11

task_tcb_typedef send_data_task;
volatile send_data_process_tcb_tydef send_data_process_tcb;

extern uint8_t spi_status_buffer[SPI_DATA_IRQ_BUFFER_BLOCK_COUNT][20];
extern uint8_t spi_status_write_index, spi_status_read_index, spi_status_count;
extern uint8_t P_Vresion[2];

uint8_t is_open_statistic = 0;
uint8_t retranmist_data_status = 0;

uint8_t clicker_send_data_status = 0;
static uint8_t pre_status = 0;
static uint8_t sum_clicker_count = 0;

extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];

extern Uart_MessageTypeDef backup_massage;
extern uint8_t sum_clicker_count;
/* 统计与重发过程所使用变量 */
// 在线状态检索
#define LOST_INDEX        0
#define OK_INDEX          1

uint8_t rf_online_index[2];

// 上报与重发
#define PRE_SUM_TABLE     0
#define PRE_ACK_TABLE     1
#define CUR_SUM_TABLE     2
#define CUR_ACK_TABLE     3

static uint8_t result_check_tables[2];
static uint8_t after_result_status;
static uint8_t retransmit_check_tables[4];;
static uint8_t after_retransmit_status;

static message_tcb_tydef    message_tcb ;
static retransmit_tcb_tydef retransmit_tcb;
static Uart_MessageTypeDef  revice_lost_massage,revice_ok_massage;
extern WhiteList_Typedef    wl;
extern Revicer_Typedef      revicer;

void retransmit_env_init( void );
/******************************************************************************
  Function:change_clicker_send_data_status
  Description:
		修改clicker_send_data_status的状态
  Input :
		clicker_send_data_status: clicker_send_data_status状态
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_retransmit_set_status(uint8_t new_status)
{
	retransmit_tcb.status = new_status;
	//printf("rf_retransmit_set_status = %d \r\n",rf_retransmit_status);
}

/******************************************************************************
  Function:change_clicker_send_data_status
  Description:
		修改clicker_send_data_status的状态
  Input :
		clicker_send_data_status: clicker_send_data_status状态
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_rf_retransmit_status(void)
{
	return retransmit_tcb.status;
}

/******************************************************************************
  Function:set_retranmist_data_status
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
void set_retranmist_data_status(uint8_t new_status)
{
	retranmist_data_status = new_status;
}
/******************************************************************************
  Function:create_status_message
  Description:
		产生一个状态信息指令
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
void create_status_message( void )
{
	spi_status_buffer[spi_status_write_index][0] = 0x61;
	memset(spi_status_buffer[spi_status_write_index]+1,0,12);
	spi_status_buffer[spi_status_write_index][13] = CLICKER_SNED_DATA_STATUS_TYPE;
	spi_status_buffer[spi_status_write_index][14] = 0;
	spi_status_buffer[spi_status_write_index][15] = 0xFF;
	spi_status_buffer[spi_status_write_index][16] = 0;
	spi_status_buffer[spi_status_write_index][17] = XOR_Cal(spi_status_buffer[spi_status_write_index]+1, 16);
	spi_status_buffer[spi_status_write_index][18] = 0x21;
	spi_status_buffer[spi_status_write_index][19] = clicker_send_data_status;
	{
		#ifdef OPEN_SEND_STATUS_MESSAGE_SHOW
		uint8_t *str,status;
		uint16_t AckTableLen,DataLen,Len;

		AckTableLen = spi_status_buffer[spi_status_write_index][14];
		DataLen     = spi_status_buffer[spi_status_write_index][14+AckTableLen+2];
		Len         = AckTableLen + DataLen + 19;		
		status = spi_status_buffer[spi_status_write_index]Len];
		switch( status )
		{
			case SEND_IDLE_STATUS:            str = "IDLE_STATUS";            break;
			case SEND_DATA1_STATUS:           str = "DATA1_STATUS";           break;
			case SEND_DATA1_UPDATE_STATUS:    str = "DATA1_UPDATE_STATUS";    break;
			case SEND_DATA2_STATUS:           str = "DATA2_STATUS";           break;
			case SEND_DATA2_SEND_OVER_STATUS: str = "DATA2_SEND_OVER_STATUS"; break;
			case SEND_DATA2_UPDATE_STATUS:    str = "DATA2_UPDATE_STATUS";    break;
			case SEND_DATA3_STATUS:           str = "DATA3_STATUS";           break;
			case SEND_DATA3_SEND_OVER_STATUS: str = "DATA3_SEND_OVER_STATUS"; break;
			case SEND_DATA3_UPDATE_STATUS:    str = "DATA3_UPDATE_STATUS";    break;
			case SEND_DATA4_STATUS:           str = "DATA4_STATUS";           break;
			case SEND_DATA4_UPDATE_STATUS:    str = "DATA4_UPDATE_STATUS";    break;
			default:break;
		}
		b_print("send_status = %s\r\n",str);
		{
			int i;
			b_print("%4d %2d write: ", buffer_get_buffer_status(SPI_REVICE_BUFFER),spi_status_count+1);
			for(i=0;i<Len;i++)
			{
				b_print("%2x ",spi_status_buffer[spi_status_write_index][i]);
			}
			b_print("%2x \r\n",status);
		}
		#endif
	}
	spi_status_write_index = (spi_status_write_index + 1)%SPI_DATA_IRQ_BUFFER_BLOCK_COUNT;
	spi_status_count++;
}

/******************************************************************************
  Function:change_clicker_send_data_status
  Description:
		修改clicker_send_data_status的状态
  Input :
		clicker_send_data_status: clicker_send_data_status状态
  Output:
  Return:
  Others:None
******************************************************************************/
void change_clicker_send_data_status( uint8_t newstatus )
{
	clicker_send_data_status = newstatus;
	create_status_message();
}


/******************************************************************************
  Function:get_clicker_send_data_status
  Description:
		获取clicker_send_data状态
  Input :
		clicker_send_data: clicker_send_data的新状态
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_clicker_send_data_status( void )
{
	return clicker_send_data_status;
}

/******************************************************************************
  Function:get_clicker_send_data_status
  Description:
		获取clicker_send_data状态
  Input :
		clicker_send_data: clicker_send_data的新状态
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t spi_buffer_status_check(uint8_t status)
{
	uint8_t current_status = status;

	if( current_status == 0 )
	{
		if(buffer_get_buffer_status(SPI_RBUF) == BUF_EMPTY)
		{
			if((clicker_send_data_status != pre_status) && (clicker_send_data_status != 0))
			{
				pre_status = clicker_send_data_status;
			}
		}
		switch( pre_status )
		{
			case SEND_DATA1_STATUS          : current_status = SEND_DATA1_STATUS;          break;
			case SEND_DATA2_STATUS          : current_status = SEND_DATA2_STATUS;          break;
			case SEND_DATA2_SEND_OVER_STATUS: current_status = SEND_DATA2_SEND_OVER_STATUS;break;
			case SEND_DATA3_STATUS          : current_status = SEND_DATA3_STATUS;          break;
			case SEND_DATA3_SEND_OVER_STATUS: current_status = SEND_DATA3_SEND_OVER_STATUS;break;
			case SEND_DATA4_STATUS          : current_status = SEND_DATA4_STATUS;          break;
			default:  break;
		}
	}

	pre_status = current_status;

	return current_status;
}

/******************************************************************************
  Function:clear_uid_check_table
  Description:
		清空所有状态索引表
  Input :
		clicker_num：答题器的数
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_uid_check_table( void )
{
	clear_white_list_table(3);
	clear_white_list_table(4);
	clear_white_list_table(5);
	clear_white_list_table(6);
	clear_white_list_table(7);
	clear_white_list_table(8);
	clear_white_list_table(9);
	DEBUG_STATISTICS_LOG("\r\nSum count:%d\r\n",sum_clicker_count);
	sum_clicker_count = 0;
	{
		retranmist_data_status = 1;
		sw_clear_timer(&request_data_timer);
	}
}

/******************************************************************************
  Function:whitelist_checktable_or
  Description:
		白名单在线状态表或函数,累加在线状态
  Input :
		table1：之前状态表1
		table2：之前状态表2
  Output:
		table3：累加之后的状态表
  Return:
  Others:None
******************************************************************************/
void whitelist_checktable_or(uint8_t table1, uint8_t table2)
{
	uint8_t i = 0;
	for(i=0;i<8;i++)
	{
		list_tcb_table[table2][i] = list_tcb_table[table1][i] |
			list_tcb_table[table2][i];
	}
}

/******************************************************************************
  Function:whitelist_checktable_and
  Description:
		白名单在线状态表与函数,剔除在线状态
  Input :
		table1：之前状态表1
		table2：之前状态表2
  Output:
		table：与操作之后的表格
  Return:
  Others:None
******************************************************************************/
void whitelist_checktable_and(uint8_t table1, uint8_t table2, uint8_t table)
{
	uint8_t i = 0;
	for(i=0;i<8;i++)
	{
		list_tcb_table[table][i] = list_tcb_table[table1][i] & (~list_tcb_table[table2][i]);
	}
}

/******************************************************************************
  Function:clicker_send_data_statistics
  Description:
		App RF 统计函数
  Input :
  Return:
  Others:None
******************************************************************************/
void clicker_send_data_statistics( uint8_t send_data_status, uint16_t uidpos )
{
	switch(send_data_status)
	{
		case SEND_DATA1_STATUS :
			set_index_of_white_list_pos(SEND_DATA1_ACK_TABLE,uidpos);
			set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
		break;
		case SEND_DATA2_STATUS :
		case SEND_DATA2_SEND_OVER_STATUS: 
			set_index_of_white_list_pos(SEND_DATA2_ACK_TABLE,uidpos);
			set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
		break;
		case SEND_DATA3_STATUS :
		case SEND_DATA3_SEND_OVER_STATUS: 
			set_index_of_white_list_pos(SEND_DATA3_ACK_TABLE,uidpos);
			set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
		break;
		case SEND_DATA4_STATUS :
			set_index_of_white_list_pos(SEND_DATA4_ACK_TABLE,uidpos);
			set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
		break;
		default:break;
	}
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 消息缓存处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void rf_move_data_to_buffer( uint8_t *Message )
{
	Uart_MessageTypeDef rf_message;

	uint8_t  q_num = 0;
	uint16_t uidpos;
	uint8_t  Cmdtype;
	uint16_t AckTableLen,DataLen,Len;

	AckTableLen = Message[14];
	DataLen     = Message[14+AckTableLen+2];
	Len         = AckTableLen + DataLen + 19;
	Cmdtype     = Message[14+AckTableLen+1];
	
	if(DataLen>0)
	{
		uint16_t s_index = 0, r_index = 0;
		uint8_t is_last_data_full = 0;
		uint8_t *prdata; 

		rf_message.HEAD    = 0x5C;
		rf_message.CMDTYPE = 0x12;
		rf_message.DEVICE  = 0x01;
		memcpy(rf_message.VERSION,P_Vresion,2);
		memcpy(rf_message.DSTID,send_data_task.srcid,UID_LEN);
		memcpy(rf_message.SRCID,revicer.uid,UID_LEN);
		memset(rf_message.REVICED,0xAA,2);

		/* 获取消息的有效长度 */
		*(uint16_t *)rf_message.LEN = Len;

		/* 获取数据的起始地址 */
		prdata = Message+14+AckTableLen+2+1;

		if( Cmdtype == 0x10 )
		{
			rf_message.DATA[0] = 0x01;
			memcpy(rf_message.DATA+1,Message+5,UID_LEN);
			s_index = 6;

			while( s_index < DataLen +6 )
			{                                                 
				if(is_last_data_full == 0)
				{
					rf_message.DATA[s_index++] = prdata[r_index] & 0x0F;
					rf_message.DATA[s_index++] = ((prdata[r_index] & 0xF0) >> 4)   | ((prdata[r_index+1] & 0x0F) << 4);
					rf_message.DATA[s_index++] = ((prdata[r_index+1] & 0xF0) >> 4) | ((prdata[r_index+2] & 0x0F) << 4);
					r_index = r_index + 2;
					is_last_data_full = 1;
				}
				else
				{
					rf_message.DATA[s_index++] = (prdata[r_index] & 0xF0) >> 4;
					rf_message.DATA[s_index++] = prdata[r_index+1];
					rf_message.DATA[s_index++] = prdata[r_index+2];
					r_index = r_index + 3;
					is_last_data_full = 0;
				}
				q_num++;
			}
			rf_message.DATA[5] = q_num;
			*(uint16_t *)rf_message.LEN = s_index;
		}
		
		if( Cmdtype == 0x24 )
		{
			rf_message.DATA[0] = 0x02;
			prdata = Message+14+AckTableLen+2+1;
			memcpy(rf_message.DATA+1,Message+5,UID_LEN);
			s_index = 5;
			rf_message.DATA[s_index++] = 0x01;
			rf_message.DATA[s_index++] = 0x03;
			if(*(uint16_t *)(prdata+2) < 3300)
				memcpy(rf_message.DATA+s_index,prdata+2,2);
			else
				memset(rf_message.DATA+s_index,0x00,2);
			s_index = s_index + 2;
			*(uint16_t *)rf_message.LEN = s_index;
			//printf("[%02x%02x%02x%02x]:",Message[5],Message[6],Message[7],Message[8]);
			//printf("IsReviceData:%1d RSSI:%3d Battery:%4d \r\n",*(prdata),*(prdata+1),*(uint16_t *)(prdata+2));
		}

		rf_message.XOR =  XOR_Cal((uint8_t *)(&(rf_message.DSTID)), *(uint16_t *)rf_message.LEN+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
		rf_message.END = 0xCA;

		if(( Cmdtype == 0x10 ) || ( Cmdtype == 0x24 ))
		{
			if( wl.start == ON )
			{
				uint8_t Is_whitelist_uid = search_uid_in_white_list(Message+5,&uidpos);
				if(Message[12] != wl.uids[uidpos].rev_num)//收到的是有效数据
				{
					/* 存入缓存 */
					if(BUF_FULL != buffer_get_buffer_status(UART_SBUF))
					{
						serial_ringbuffer_write_data(UART_SBUF,&rf_message);
						/* 更新接收数据帧号与包号 */
						wl.uids[uidpos].rev_seq = Message[11];
						wl.uids[uidpos].rev_num = Message[12];	
					}
				}
			}
		}
	}
}

/******************************************************************************
  Function:spi_process_revice_data
  Description:
		RF SPI buffer 处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t spi_process_revice_data( void )
{
	uint8_t  spi_message[255];
	uint8_t  spi_message_type = 0;
	uint8_t  Is_whitelist_uid = OPERATION_ERR;
	uint16_t uidpos = 0xFFFF;
	uint8_t  clicker_send_data_status = 0;

	if(buffer_get_buffer_status(SPI_RBUF) != BUF_EMPTY)
	{
		uint16_t AckTableLen,DataLen,Len;

		memset(spi_message,0,255);
		spi_read_data_from_buffer( SPI_RBUF, spi_message );
		AckTableLen = spi_message[14];
		DataLen     = spi_message[14+AckTableLen+2];
		Len         = AckTableLen + DataLen + 19;
		clicker_send_data_status = spi_message[Len];
		spi_message_type = spi_message[13];

		#ifdef OPEN_BUFFER_DATA_SHOW
		{
			int i;
			DEBUG_BUFFER_ACK_LOG("%4d ", buffer_get_buffer_status(SPI_RBUF));
			DEBUG_BUFFER_ACK_LOG("Buffer Read :");
			for(i=5;i<9;i++)
				DEBUG_BUFFER_ACK_LOG("%02x",spi_message[i]);
		}
		#endif

		/* 判断是否为状态帧 */
		if(spi_message_type != 0x0A)
		{
			/* 检索白名单 */
			Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

			/* 白名单开关状态 */
			if(wl.switch_status == OFF)
			{
				/* 关闭白名单是不过滤白名单 */
				Is_whitelist_uid = OPERATION_SUCCESS;
			}

			/* 过滤通过 */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				/* 收到的是Data */
				if(spi_message_type == NRF_DATA_IS_USEFUL)
				{
					//printf("[DATA] uid:%02x%02x%02x%02x, ",\
						*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
					//printf("seq:%2x, pac:%2x\r\n",(uint8_t)*(spi_message+11),\
						(uint8_t)*(spi_message+12));

					if( wl.start == ON )
					{
						nrf_transmit_parameter_t transmit_config;
						/* 回复ACK */
						memcpy(transmit_config.dist,spi_message+5, 4 );
						transmit_config.package_type   = NRF_DATA_IS_ACK;
						transmit_config.transmit_count = 2;
						transmit_config.delay100us     = 20;
						transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
						transmit_config.data_buf       = NULL;
						transmit_config.data_len       = 0;
						nrf_transmit_start( &transmit_config );

						/* 有效数据告到PC */
						rf_move_data_to_buffer( spi_message );
					}
				}
				/* 收到的是Ack */
				else if(spi_message_type == NRF_DATA_IS_ACK)
				{
					//printf("[ACK] uid:%02x%02x%02x%02x, ",\
						*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
					//printf("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
						(uint8_t)*(spi_message+12));

					/* 发送定时统计*/
					if( is_open_statistic == 0 )
					{
						clicker_send_data_statistics( clicker_send_data_status, uidpos );
					}
					else
					{
						set_index_of_white_list_pos(SINGLE_SEND_DATA_ACK_TABLE,uidpos);
					}

          /* 请求数据定时统计 */
					if( retranmist_data_status == 1 )
					{
						uint8_t Is_reviceed_uid = get_index_of_white_list_pos_status(SEND_DATA_ACK_TABLE,uidpos);
						if( Is_reviceed_uid == 0 )
						{
							Uart_MessageTypeDef result_message;
							uint8_t *pdata = (uint8_t *)(result_message.DATA+2);
							
							/* 填充UID信息 */
							*((uint16_t *)pdata)= uidpos;
							pdata = pdata + 2;
							memcpy(pdata,spi_message+5,4);
							
							/* 填充包信息 */
							result_message.HEAD = UART_SOF;
							result_message.DEVICE = 0x01;
							memcpy(result_message.VERSION,P_Vresion,2);
							memcpy(result_message.SRCID,revicer.uid,UID_LEN);
							memcpy(result_message.DSTID,send_data_task.srcid,UID_LEN);
							memset(result_message.REVICED,0xAA,2);
							result_message.CMDTYPE = 0x11;
							*(uint16_t *)(result_message.LEN) = 6;
							*(uint16_t *)(result_message.LEN) = *(uint16_t *)(result_message.LEN)+2;
							result_message.DATA[0] = 0x00;
							result_message.DATA[1] = 0;
							result_message.XOR = XOR_Cal((uint8_t *)(&(result_message.DEVICE)), 
							*(uint16_t *)(result_message.LEN) + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA + 2);
							result_message.END = 0xCA;

							if(BUF_FULL != buffer_get_buffer_status(UART_SBUF))
							{
								clear_index_of_white_list_pos(SEND_PRE_TABLE,uidpos);
								set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
								serial_ringbuffer_write_data(UART_SBUF,&result_message);
							}
						}
					}
				}
				else
				{
					DEBUG_BUFFER_DTATA_LOG("[STATUS]:%d \r\n",spi_message[spi_message[15]+18]);
				}
			}
		}
		else
		{
			#ifdef OPEN_SEND_STATUS_SHOW
			int i;
			b_print("%4d    read2: ", buffer_get_buffer_status(SPI_REVICE_BUFFER));
			for(i=0;i<17;i++)
			{
				b_print("%2x ",spi_message[i]);
			}
			b_print("%2x \r\n",spi_message[17]);
			#endif
		}
	}
	else
	{
		// ok to update to pc
	}
	return (clicker_send_data_status);
}

/******************************************************************************
  Function:checkout_outline_uid
  Description:
		提取不在线状态的答题器UID
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t checkout_online_uids(uint8_t src_table, uint8_t check_table,
	                        uint8_t mode, uint8_t *buffer,uint8_t *len)
{
	uint16_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	static uint8_t index = 0;
#endif
	for(i=rf_online_index[mode];(i<120)&&(*len<239-5);i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(src_table,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(check_table,i);
			if(is_online_pos == mode)
			{
				*(uint16_t *)buffer = i;
				get_index_of_uid(i,buffer+2);
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				{
					DEBUG_UID_LOG("[%3d]:%02x%02x%02x%02x ",i, *(buffer+2),*(buffer+3), *(buffer+4), *(buffer+5));
					if(((index++)+1) % 6 == 0)
					{
						DEBUG_UID_LOG("\n");
					}
				}
#endif
				buffer = buffer+6;
				*len = *len + 6;
			}
		}
	}

	if(i==120)
	{
		rf_online_index[mode] = 0;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
		index = 0;
#endif
		return 0;
	}
	else
	{
		rf_online_index[mode] = i;
		return 1;
	}
}

void get_send_data_table_message(uint8_t status)
{
	switch( status )
	{
		case SEND_DATA1_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("Statistic : %d\r\n",revicer.data_statistic_count++);
				DEBUG_STATISTICS_LOG("First Statistic:\r\n");
				if(send_data_task.retransmit == 1)
				{
					whitelist_checktable_and(0, REQUEST_TABLE, SEND_PRE_TABLE);
					result_check_tables[PRE_SUM_TABLE] = SEND_PRE_TABLE;
					result_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				}
				else
				{
					result_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
					result_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				}
				after_result_status = SEND_DATA2_STATUS;
			}
			break;

		case SEND_DATA2_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("\r\nSecond Statistic:\r\n");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				after_result_status = SEND_DATA3_STATUS;
			}
			break;
		case SEND_DATA3_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("\r\nThird Statistic:\r\n");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				after_result_status = SEND_DATA4_STATUS;
			}
			break;
		case SEND_DATA4_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("\r\nFourth Statistic:");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA4_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA4_ACK_TABLE;
				after_result_status = SEND_IDLE_STATUS;
			}
			break;
		default:
			{
				result_check_tables[PRE_SUM_TABLE] = 0;
				result_check_tables[PRE_ACK_TABLE] = 0;
				after_result_status = 0;
			}
			break;
	}
}

void get_retransmit_messsage( uint8_t status )
{
	switch( status )
	{
		case SEND_DATA2_STATUS:
			{
				DEBUG_DATA_DETAIL_LOG("\r\n\r\n[1].retransmit:\r\n");
				if(send_data_task.retransmit == 1)
				{
					retransmit_check_tables[PRE_SUM_TABLE] = SEND_PRE_TABLE;
					retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				}
				else
				{
					retransmit_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
					retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				}
				retransmit_check_tables[CUR_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				retransmit_check_tables[CUR_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				after_retransmit_status = SEND_DATA2_SEND_OVER_STATUS;
			}
			break;

		case SEND_DATA3_STATUS:
			{
				DEBUG_DATA_DETAIL_LOG("\r\n\r\n[2].retransmit:\r\n");
				retransmit_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				retransmit_check_tables[CUR_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				retransmit_check_tables[CUR_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				after_retransmit_status                = SEND_DATA3_SEND_OVER_STATUS;
			}
			break;

		default:
			{
				retransmit_check_tables[PRE_SUM_TABLE] = 0;
				retransmit_check_tables[PRE_ACK_TABLE] = 0;
				retransmit_check_tables[CUR_SUM_TABLE] = 0;
				retransmit_check_tables[CUR_ACK_TABLE] = 0;
				after_retransmit_status                = 0;
			}
			break;
	}
}

/******************************************************************************
  Function:checkout_retransmit_clickers
  Description:
		答题器数据重发
  Input :
		presumtable  ：总的索引表
		preacktable  ：已经在线，无需重发的索引表
		cursumtable  ：下次统计的索引表
  Return:
  Others:None
******************************************************************************/
uint8_t checkout_retransmit_clickers(uint8_t presumtable, uint8_t preacktable, uint8_t cursumtable)
{
	uint16_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t puid[4];
	uint8_t clickernum = 0;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	uint16_t index = 0;
#endif
	for(i=0;i<120;i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(presumtable,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(preacktable,i);
			if(is_online_pos == 0)
			{
				get_index_of_uid(i,puid);
				set_index_of_white_list_pos(cursumtable,i);
				clickernum++;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				{
					DEBUG_UID_LOG("[%3d]:%02x%02x%02x%02x ",i,puid[0],puid[1],puid[2],puid[3]);
					if(((index++)+1) % 5 == 0)
						DEBUG_UID_LOG("\n");
				}
#endif
			}
		}
	}
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	DEBUG_UID_LOG("\n");
#endif
	return clickernum;
}

/******************************************************************************
  Function:check_is_revice_over
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t check_is_revice_over( void )
{
	uint16_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	for(i=0;i<120;i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(0,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(SEND_DATA_ACK_TABLE,i);
			if(is_online_pos == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}
/* 重发函数 */
void retansmit_data( uint8_t status )
{
	if(( status == SEND_DATA2_STATUS ) || ( status == SEND_DATA3_STATUS ))
	{
		nrf_transmit_parameter_t transmit_config;
		
		get_retransmit_messsage( status );
		checkout_retransmit_clickers( retransmit_check_tables[PRE_SUM_TABLE] ,retransmit_check_tables[PRE_ACK_TABLE],
		                     retransmit_check_tables[CUR_SUM_TABLE] );
		/* 发送前导帧 */
		whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );
		memset(transmit_config.dist,0, 4);
		transmit_config.package_type   = NRF_DATA_IS_PRE;
		transmit_config.transmit_count = SEND_PRE_COUNT;
		transmit_config.delay100us     = SEND_PRE_DELAY100US;
		transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
		transmit_config.data_buf       = NULL;
		transmit_config.data_len       = 0;
		transmit_config.sel_table      = SEND_PRE_TABLE;
		nrf_transmit_start( &transmit_config );
		
		/* 发送数据帧 */
		whitelist_checktable_or(retransmit_check_tables[PRE_ACK_TABLE],SEND_DATA_ACK_TABLE);

		memset(transmit_config.dist,0, 4);
		transmit_config.package_type   = NRF_DATA_IS_USEFUL;
		transmit_config.transmit_count = SEND_DATA_COUNT;
		transmit_config.delay100us     = SEND_DATA_DELAY100US;
		transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
		transmit_config.data_buf       = rf_var.tx_buf; 
		transmit_config.data_len       = rf_var.tx_len;
		transmit_config.sel_table      = SEND_DATA_ACK_TABLE;
		nrf_transmit_start( &transmit_config );


		/* 跟新状态，开始2次统计 */
		change_clicker_send_data_status( after_retransmit_status );

		if(after_retransmit_status == SEND_DATA3_SEND_OVER_STATUS)
		{
			retransmit_env_init();
		}
		return ;
	}
}

/* 统计函数 */
void send_data_result( uint8_t status )
{
	if(( status == SEND_DATA1_UPDATE_STATUS ) ||
		 ( status == SEND_DATA2_UPDATE_STATUS ) ||
		 ( status == SEND_DATA3_UPDATE_STATUS ) ||
	   ( status == SEND_DATA4_UPDATE_STATUS ))
	{

		get_send_data_table_message(status);

		DEBUG_DATA_DETAIL_LOG("\r\nlost:\r\n");

		/* 返回失败的UID */
		while( message_tcb.Is_lost_over != 0)
		{
			message_tcb.Is_lost_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 0,
				revice_lost_massage.DATA+2,revice_lost_massage.LEN);
			message_tcb.lostuidlen += *(uint16_t *)revice_lost_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_lost_massage.HEAD = UART_SOF;
			revice_lost_massage.DEVICE = 0x01;
			memcpy(revice_lost_massage.VERSION,P_Vresion,2);
			memcpy(revice_lost_massage.SRCID,revicer.uid,UID_LEN);
			memcpy(revice_lost_massage.DSTID,send_data_task.srcid,UID_LEN);
			memset(revice_lost_massage.REVICED,0xAA,2);
			revice_lost_massage.CMDTYPE = 0x11;
			*(uint16_t *)(revice_lost_massage.LEN) = *(uint16_t *)(revice_lost_massage.LEN)+2;
			revice_lost_massage.DATA[0] = 0x01;
			revice_lost_massage.DATA[1] = status / 3;
			revice_lost_massage.XOR = XOR_Cal((uint8_t *)(&(revice_lost_massage.DEVICE)), 
			*(uint16_t *)(revice_lost_massage.LEN) + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA + 2);
			revice_lost_massage.END = 0xCA;
			if(revice_lost_massage.LEN != 0)
			{
				if(BUF_FULL != buffer_get_buffer_status(UART_SBUF))
				{
					serial_ringbuffer_write_data(UART_SBUF,&revice_lost_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,*(uint16_t *)(revice_lost_massage.LEN));
			*(uint16_t *)revice_lost_massage.LEN = 0;
		}
		DEBUG_DATA_DETAIL_LOG("\r\nok:\r\n");
		message_tcb.clicker_count = 0;
		while(message_tcb.Is_ok_over != 0)
		{
			message_tcb.Is_ok_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 1,
				revice_ok_massage.DATA+2,revice_ok_massage.LEN);
			revice_ok_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_ok_massage.DEVICE)),
			                                 *(uint16_t *)revice_ok_massage.LEN+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
			revice_ok_massage.END = 0xCA;
			message_tcb.clicker_count += *(uint16_t *)revice_ok_massage.LEN/5;
			message_tcb.okuidlen += *(uint16_t *)revice_ok_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_ok_massage.HEAD = UART_SOF;
			revice_ok_massage.DEVICE = 0x01;
			memcpy(revice_ok_massage.VERSION,P_Vresion,2);
			memcpy(revice_ok_massage.SRCID,revicer.uid,UID_LEN);
			memcpy(revice_lost_massage.DSTID,send_data_task.srcid,UID_LEN);
			memset(revice_ok_massage.REVICED,0xAA,2);
			revice_ok_massage.CMDTYPE = 0x11;
			*(uint16_t *)revice_ok_massage.LEN = *(uint16_t *)revice_ok_massage.LEN+2;
			revice_ok_massage.DATA[0] = 0x00;
			revice_ok_massage.DATA[1] = status / 3;
			revice_ok_massage.XOR = XOR_Cal((uint8_t *)(&(revice_ok_massage.DEVICE)), 
			*(uint16_t *)revice_ok_massage.LEN+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA+2);
			revice_ok_massage.END = 0xCA;
			if( revice_ok_massage.LEN != 0)
			{
				if(BUF_FULL != buffer_get_buffer_status(UART_SBUF))
				{
					serial_ringbuffer_write_data(UART_SBUF,&revice_ok_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,*(uint16_t *)revice_lost_massage.LEN);
			*(uint16_t *)revice_ok_massage.LEN = 0;
		}
		DEBUG_STATISTICS_LOG("\r\ncount:%d\r\n",message_tcb.clicker_count);
		sum_clicker_count += message_tcb.clicker_count;
		message_tcb.clicker_count = 0;

		/* 上传在线状态 */
		if(message_tcb.lostuidlen != 0)
		{
			if( status == SEND_DATA3_UPDATE_STATUS )
			{
				uint8_t retransmit_clickers;
				DEBUG_DATA_DETAIL_LOG("\r\n\r\n[3].retransmit:\r\n");
				retransmit_clickers = checkout_retransmit_clickers(SEND_DATA3_SUM_TABLE,SEND_DATA3_ACK_TABLE,
																			SEND_DATA4_SUM_TABLE);
				if(retransmit_clickers > 0)
				{
					retransmit_tcb.sum = RETRANSMIT_SEND_DATA_COUNT;
				}
				else
				{
					retransmit_tcb.sum = 0;
				}
				whitelist_checktable_or(SEND_DATA3_ACK_TABLE,SEND_DATA_ACK_TABLE);
			}

			change_clicker_send_data_status( after_result_status );
			if(after_result_status == SEND_IDLE_STATUS)
				clear_uid_check_table();
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table();
		}
		message_tcb.okuidlen     = 0;
		message_tcb.lostuidlen   = 0;
		message_tcb.Is_lost_over = 1;
		message_tcb.Is_ok_over   = 1;
		return ;
	}
}

/******************************************************************************
  Function:retransmit_data_to_next_clicker
  Description:
		第三次重发函数
  Input :
  Return:
  Others:None
******************************************************************************/
void retransmit_data_to_next_clicker( void )
{
	nrf_transmit_parameter_t transmit_config;
	whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );

	memset(transmit_config.dist,0, 4);
	transmit_config.package_type   = NRF_DATA_IS_PRE;
	transmit_config.transmit_count = SEND_PRE_COUNT;
	transmit_config.delay100us     = SEND_PRE_DELAY100US;
	transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
	transmit_config.data_buf       = NULL;
	transmit_config.data_len       = 0;
	transmit_config.sel_table      = SEND_PRE_TABLE;
	nrf_transmit_start( &transmit_config );


	whitelist_checktable_or(SEND_DATA4_ACK_TABLE,SEND_DATA_ACK_TABLE);
	
	memset(transmit_config.dist,0, 4);
	transmit_config.package_type   = NRF_DATA_IS_USEFUL;
	transmit_config.transmit_count = SEND_DATA_COUNT;
	transmit_config.delay100us     = SEND_DATA_DELAY100US;
	transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
	transmit_config.data_buf       = rf_var.tx_buf; 
	transmit_config.data_len       = rf_var.tx_len;
	transmit_config.sel_table      = SEND_DATA_ACK_TABLE;
	nrf_transmit_start( &transmit_config );

	rf_retransmit_set_status(1);
}

/******************************************************************************
  Function:retransmit_env_init
  Description:
		清除重发过程中的环境变量
  Input :
  Return:
  Others:None
******************************************************************************/
void retransmit_env_init( void )
{
	retransmit_tcb.count  = 0;
	retransmit_tcb.sum    = 0;
	retransmit_tcb.pos    = 0;
	retransmit_tcb.status = 0;
	memset(retransmit_tcb.uid,0,4);
	clear_current_uid_index();
}

/******************************************************************************
  Function:spi_write_temp_buffer_to_buffer
  Description:
		将零时缓存的数据存入到buffer中
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_write_temp_buffer_to_buffer()
{
	if((spi_status_count > 0) && (BUF_EMPTY == buffer_get_buffer_status(SPI_RBUF)))
	{
		uint16_t AckTableLen,DataLen,Len;

		AckTableLen = spi_status_buffer[spi_status_read_index][14];
		DataLen     = spi_status_buffer[spi_status_read_index][14+AckTableLen+2];
		Len         = AckTableLen + DataLen + 19;

		spi_write_data_to_buffer(SPI_RBUF,spi_status_buffer[spi_status_read_index],
				spi_status_buffer[spi_status_read_index][Len]);
		{
			#ifdef OPEN_SEND_STATUS_SHOW
			uint8_t *str,status;
			status = spi_status_buffer[spi_status_read_index][Len];
			switch( status )
			{
				case SEND_IDLE_STATUS:            str = "IDLE_STATUS";            break;
				case SEND_DATA1_STATUS:           str = "DATA1_STATUS";           break;
				case SEND_DATA1_UPDATE_STATUS:    str = "DATA1_UPDATE_STATUS";    break;
				case SEND_DATA2_STATUS:           str = "DATA2_STATUS";           break;
				case SEND_DATA2_SEND_OVER_STATUS: str = "DATA2_SEND_OVER_STATUS"; break;
				case SEND_DATA2_UPDATE_STATUS:    str = "DATA2_UPDATE_STATUS";    break;
				case SEND_DATA3_STATUS:           str = "DATA3_STATUS";           break;
				case SEND_DATA3_SEND_OVER_STATUS: str = "DATA3_SEND_OVER_STATUS"; break;
				case SEND_DATA3_UPDATE_STATUS:    str = "DATA3_UPDATE_STATUS";    break;
				case SEND_DATA4_STATUS:           str = "DATA4_STATUS";           break;
				case SEND_DATA4_UPDATE_STATUS:    str = "DATA4_UPDATE_STATUS";    break;
				default:break;
			}
			DEBUG_BUFFER_DTATA_LOG("send_status = %s\r\n",str);
			#endif
			#ifdef OPEN_SEND_STATUS_MESSAGE_SHOW
			{
				int i;
				DEBUG_BUFFER_DTATA_LOG("%4d %2d read1: ", buffer_get_buffer_status(SPI_REVICE_BUFFER),spi_status_count);
				for(i=0;i<Len;i++)
				{
					DEBUG_BUFFER_DTATA_LOG("%2x ",spi_status_buffer[spi_status_read_index][i]);
				}
				DEBUG_BUFFER_DTATA_LOG("%2x \r\n",status);
			}
			#endif
		}
		spi_status_read_index = (spi_status_read_index + 1) % SPI_DATA_IRQ_BUFFER_BLOCK_COUNT;
		spi_status_count--;
	}
}

/******************************************************************************
  Function:send_data_env_init
  Description:
		答题器发送处理逻辑函数
  Input :
  Return:
  Others:None
******************************************************************************/
void send_data_env_init(void)
{
	message_tcb.okuidlen     = 0;
	message_tcb.lostuidlen   = 0;
	message_tcb.Is_lost_over = 1;
	message_tcb.Is_ok_over   = 1;

	memset(result_check_tables,0,2);
	after_result_status = 0;
	memset(retransmit_check_tables,0,4);
	after_retransmit_status = 0;

	memset(rf_online_index,0,2);

	/* clear retransmit task control block*/
	memset(&retransmit_tcb,0,sizeof(retransmit_tcb_tydef));

	/* clear online check table */
	memset(list_tcb_table[SINGLE_SEND_DATA_ACK_TABLE],0,16);
	memset(list_tcb_table[SEND_DATA1_ACK_TABLE],0,16*13);

	/* clear count of clicker */
	sum_clicker_count = 0;

	/* clear send data status */
	clicker_send_data_status = 0;

	/* clear last status of send status */
	pre_status = 0;
	
	retranmist_data_status = 0;
	sw_clear_timer(&request_data_timer);
}


/******************************************************************************
  Function:App_clickers_send_data_process
  Description:
		答题器发送处理逻辑函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_send_data_process( void )
{
	uint8_t spi_buffer_status = 0;
	uint8_t current_status = 0;

	/* 读取spi数据写入到 Buffer */
	spi_write_temp_buffer_to_buffer();

	/* 获取缓存状态 */
	spi_buffer_status = spi_process_revice_data();

	/* 修正当前状态 */
	current_status = spi_buffer_status_check(spi_buffer_status);

	/* 第三次上报之后，重新单独发送 */
	if( current_status == SEND_DATA4_STATUS ) // 10
	{
		uint8_t rf_retransmit_status = 0;

		rf_retransmit_status = get_rf_retransmit_status();

		if(retransmit_tcb.sum != 0)
		{
			if(rf_retransmit_status == 0)
			{
				retransmit_data_to_next_clicker();
			}
		}

		if(rf_retransmit_status == 2)
		{
			uint8_t is_revice_over;

			is_revice_over = check_is_revice_over();
			if(is_revice_over)
			{
				retransmit_tcb.count++;
				DEBUG_DATA_DETAIL_LOG("[%d].retransmit\r\n",retransmit_tcb.count + 3);

				if(retransmit_tcb.count == retransmit_tcb.sum)
				{
					retransmit_tcb.count = 0;
					change_clicker_send_data_status( SEND_DATA4_UPDATE_STATUS ); // 11
					retransmit_env_init();
				}
				else
				{
					retransmit_data_to_next_clicker();
				}
			}
			else
			{
				retransmit_tcb.count = 0;
				change_clicker_send_data_status( SEND_DATA4_UPDATE_STATUS ); // 11
				retransmit_env_init();
			}
		}
		return ;
	}

	/* 打印统计结果 */
	send_data_result( current_status );

	/* 上报之后，重新广播发送 */
	retansmit_data( current_status );
	
	if( retranmist_data_status == 2 ) 
	{
		nrf_transmit_parameter_t transmit_config;
		memcpy(list_tcb_table[REQUEST_TABLE],list_tcb_table[SEND_DATA_ACK_TABLE],16);

		/* 发送前导帧 */
		whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );

		memset(transmit_config.dist,0, 4);
		transmit_config.package_type   = NRF_DATA_IS_PRE;
		transmit_config.transmit_count = SEND_PRE_COUNT;
		transmit_config.delay100us     = SEND_PRE_DELAY100US;
		transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
		transmit_config.data_buf       = NULL;
		transmit_config.data_len       = 0;
		transmit_config.sel_table      = SEND_PRE_TABLE;
		nrf_transmit_start( &transmit_config );
		
		/* 发送数据帧 */
		memset(transmit_config.dist,0, 4);
		transmit_config.package_type   = NRF_DATA_IS_USEFUL;
		transmit_config.transmit_count = SEND_DATA_COUNT;
		transmit_config.delay100us     = SEND_DATA_DELAY100US;
		transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
		transmit_config.data_buf       = rf_var.tx_buf; 
		transmit_config.data_len       = rf_var.tx_len;
		transmit_config.sel_table      = SEND_DATA_ACK_TABLE;
		nrf_transmit_start( &transmit_config );
		
		retranmist_data_status = 1;
		sw_clear_timer(&request_data_timer);
	}
}

/******************************************************************************
  Function:send_data_process_timer_init
  Description:
		发送过程中的定时器初始化
  Input :
  Return:
  Others:None
******************************************************************************/
void send_data_process_timer_init( void )
{
	/* initialize send_data_process_tcb */
	{
		send_data_process_tcb.pre_data_count      = 110;
		send_data_process_tcb.pre_data_delay100us = 10;
		send_data_process_tcb.data_count          = 2;
		send_data_process_tcb.data_delay100us     = 50;
		send_data_process_tcb.rand_delayms        = 500;
		send_data_process_tcb.retransmit_count    = 5;
	}

	/* create send data process timer*/
	sw_create_timer(&send_data1_timer , SEND_DATA1_TIMEOUT, SEND_DATA1_STATUS,
		SEND_DATA1_UPDATE_STATUS,&(clicker_send_data_status), create_status_message);
	sw_create_timer(&send_data2_timer , SEND_DATA2_TIMEOUT, SEND_DATA2_SEND_OVER_STATUS,
		SEND_DATA2_UPDATE_STATUS,&(clicker_send_data_status), create_status_message);
	sw_create_timer(&send_data3_timer , SEND_DATA3_TIMEOUT, SEND_DATA3_SEND_OVER_STATUS,
		SEND_DATA3_UPDATE_STATUS,&(clicker_send_data_status), create_status_message);

  /* create retransmit timer */
	sw_create_timer(&retransmit_timer, SEND_DATA4_TIMEOUT, 1, 2, &(retransmit_tcb.status), NULL);

  /* create request timer */
	sw_create_timer(&request_data_timer, RETRANSMIT_DATA_TIME_UNIT, 1, 2, &retranmist_data_status, NULL);

}
