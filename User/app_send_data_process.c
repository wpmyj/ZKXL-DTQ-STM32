/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_send_data_process.h"

#define CLICKER_SNED_DATA_STATUS_TYPE     10
#define CLICKER_PRE_DATA_STATUS_TYPE      11

static uint8_t clicker_send_data_status = 0;
static uint8_t pre_status = 0;
uint8_t sum_clicker_count = 0;

extern nrf_communication_t nrf_communication;
extern uint16_t white_list_use_onlne_table[10][8];

extern clicker_t clickers[120];
extern Uart_MessageTypeDef backup_massage;
extern uint8_t dtq_to_jsq_sequence;
extern uint8_t jsq_to_dtq_sequence;
extern uint8_t dtq_to_jsq_packnum;
extern uint8_t jsq_to_dtq_packnum;
extern uint8_t sign_buffer[4];
extern uint8_t sum_clicker_count;

/* 统计与重发过程所使用变量 */
// 在线状态检索
#define LOST_INDEX        0
#define OK_INDEX          1

uint8_t rf_online_index[2]        = { 0, 0 };

// 上报与重发
#define PRE_SUM_TABLE     0
#define PRE_ACK_TABLE     1
#define CUR_SUM_TABLE     2
#define CUR_ACK_TABLE     3

uint8_t result_check_tables[2]     = { 0, 0 };
uint8_t after_result_status        = 0;
uint8_t retransmit_check_tables[4] = { 0, 0, 0, 0};
uint8_t after_retransmit_status    = 0;

message_tcb_tydef    message_tcb = { 0, 1, 1, 0, 0 };
retransmit_tcb_tydef retransmit_tcb   = {
	0,
	0,
	{ 0 , 0 ,0 , 0},
	0,
	0,
};

Uart_MessageTypeDef revice_lost_massage,revice_ok_massage;

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
#ifdef OPEN_SEND_STATUS_SHOW
	uint8_t *str;
#endif
	uint8_t spi_status_message[17];
	clicker_send_data_status = newstatus;
#ifdef OPEN_SEND_STATUS_SHOW
	switch(clicker_send_data_status)
	{
		case SEND_IDLE_STATUS: str = "SEND_IDLE_STATUS"; break;
		case SEND_DATA1_STATUS: str = "SEND_DATA1_STATUS"; break;
		case SEND_DATA1_UPDATE_STATUS: str = "SEND_DATA1_UPDATE_STATUS"; break;
		case SEND_DATA2_STATUS: str = "SEND_DATA2_STATUS"; break;
		case SEND_DATA2_SEND_OVER_STATUS: str = "SEND_DATA2_SEND_OVER_STATUS"; break;
		case SEND_DATA2_UPDATE_STATUS: str = "SEND_DATA2_UPDATE_STATUS"; break;
		case SEND_DATA3_STATUS: str = "SEND_DATA3_STATUS"; break;
		case SEND_DATA3_SEND_OVER_STATUS: str = "SEND_DATA3_SEND_OVER_STATUS"; break;
		case SEND_DATA3_UPDATE_STATUS: str = "SEND_DATA3_UPDATE_STATUS"; break;
		case SEND_DATA4_STATUS: str = "SEND_DATA4_STATUS"; break;
		case SEND_DATA4_UPDATE_STATUS: str = "SEND_DATA4_UPDATE_STATUS"; break;
		default:break;
	}
	printf("\r\nclicker_send_data_status = %s\r\n",str);
#endif
	spi_status_message[0] = 0x61;
	memset(spi_status_message+1,0,10);
	spi_status_message[11] = CLICKER_SNED_DATA_STATUS_TYPE;
	memset(spi_status_message+12,0,3);
	spi_status_message[15] = CLICKER_SNED_DATA_STATUS_TYPE;
	spi_status_message[16] = 0x21;
	spi_write_data_to_buffer(SPI_REVICE_BUFFER,spi_status_message, newstatus);
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
	clear_white_list_table(2);
	clear_white_list_table(3);
	clear_white_list_table(4);
	clear_white_list_table(5);
	clear_white_list_table(6);
	clear_white_list_table(7);
	clear_white_list_table(8);
	clear_white_list_table(9);
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	printf("\r\nSum count:%d\r\n",sum_clicker_count);
#endif
	sum_clicker_count = 0;
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
		white_list_use_onlne_table[table2][i] = white_list_use_onlne_table[table1][i] |
			white_list_use_onlne_table[table2][i];
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
void clicker_send_data_statistics( uint8_t send_data_status, uint8_t uidpos )
{
	switch(send_data_status)
	{
		case SEND_DATA1_STATUS :          set_index_of_white_list_pos(SEND_DATA1_ACK_TABLE,uidpos); break;
		case SEND_DATA2_STATUS :
		case SEND_DATA2_SEND_OVER_STATUS: set_index_of_white_list_pos(SEND_DATA2_ACK_TABLE,uidpos); break;
		case SEND_DATA3_STATUS :
		case SEND_DATA3_SEND_OVER_STATUS: set_index_of_white_list_pos(SEND_DATA3_ACK_TABLE,uidpos); break;
		case SEND_DATA4_STATUS :          set_index_of_white_list_pos(SEND_DATA4_ACK_TABLE,uidpos); break;
		default:break;
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
	static  uint8_t spi_message[255];
	static  uint8_t spi_message_type = 0;
	bool    Is_whitelist_uid = OPERATION_ERR;
	uint8_t uidpos = 0;
	uint8_t clicker_send_data_status = 0;

	if(buffer_get_buffer_status(SPI_REVICE_BUFFER) != BUFFEREMPTY)
	{
		memset(spi_message,0,255);
		spi_read_data_from_buffer( SPI_REVICE_BUFFER, spi_message );
		clicker_send_data_status = spi_message[spi_message[14]+17];
		spi_message_type = spi_message[11];

		/* 判断是否为状态帧 */
		if(spi_message_type != 0x0A)
		{
			/* 检索白名单 */
			Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

			if(clickers[uidpos].use == 0)
			{
				memcpy(clickers[uidpos].uid, spi_message+5, 4);
				clickers[uidpos].use = 1;
				clickers[uidpos].first = 1;
			}
			else
			{
				clickers[uidpos].first = 0;
			}

			/* 检测是白名单 */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				uint8_t systick_current_status = 0;

				/* 获取心跳包状态 */
				systick_current_status = rf_get_systick_status();

				/* 填充索引表 */
				if(systick_current_status == 1)
				{
					set_index_of_white_list_pos(1,uidpos);
				}

				/* 统计发送状态 */
				clicker_send_data_statistics( clicker_send_data_status, uidpos );
			}

			if(1 == get_rf_retransmit_status())
			{
				if(spi_message[5] == retransmit_tcb.uid[0] &&
					 spi_message[6] == retransmit_tcb.uid[1]
					)
				{
					rf_retransmit_set_status(2);
				}
			}

			/* 白名单开关状态 */
			if(white_on_off == OFF)
			{
				/* 关闭白名单是不过滤白名单 */
				Is_whitelist_uid = OPERATION_SUCCESS;
			}

			/* 过滤通过 */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				/* get uid */
				memcpy(sign_buffer   ,spi_message+5 ,4);
				//memcpy(spi_message.dtq_uid,spi_message+5 ,4);

				/* 收到的是ACK */
				if(spi_message[11] == NRF_DATA_IS_ACK)
				{
					/* 返回ACK的包号和上次发送的是否相同 */
					if(spi_message[10] == jsq_to_dtq_packnum)
					{
#ifdef OPEN_ACK_SHOW
						printf("[ACK] uid:%02x%02x%02x%02x, ",
							*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),
							*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
						printf("seq:%2x, pac:%2x\r\n",(uint8_t)*(nrf_communication.receive_buf+9),
							(uint8_t)*(nrf_communication.receive_buf+10));
#endif
					}
					else//收到的是有效数据
					{
	//				printf("[DATA] uid:%02x%02x%02x%02x, ",
	//					*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),
	//					*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
	//				printf("seq:%2x, pac:%2x\r\n",(uint8_t)*(nrf_communication.receive_buf+9),
	//					(uint8_t)*(nrf_communication.receive_buf+10));

						/* 重复接收的数据，返回包号和上次一样的ACK */
						if(clickers[uidpos].prepacknum != spi_message[10])
						{
							/* 统计丢包 */
							if( clickers[uidpos].use == 1 )
							{
								//float lostrate = 0.0;

								if(clickers[uidpos].first == 0)
								{
									if( spi_message[10] > clickers[uidpos].prepacknum )
										clickers[uidpos].lost_package_num += spi_message[10] - clickers[uidpos].prepacknum -1 ;

									if( spi_message[10] < clickers[uidpos].prepacknum )
										clickers[uidpos].lost_package_num += spi_message[10] + 255 - clickers[uidpos].prepacknum ;
								}
								else
								{
									clickers[uidpos].lost_package_num = 0;
								}

	//							/* 统计收到包数 */
	//						clickers[uidpos].revice_package_num++;
	//						printf("clickers : %02x%02x%02x%02x, pre:%2x, cur:%2x revice = %08x, lost = %08x, \r\n",
	//						clickers[uidpos].uid[0],
	//						clickers[uidpos].uid[1],
	//						clickers[uidpos].uid[2],
	//						clickers[uidpos].uid[3],
	//						clickers[uidpos].prepacknum,
	//						nrf_communication.receive_buf[10],
	//						clickers[uidpos].revice_package_num,
	//						clickers[uidpos].lost_package_num
	//						);
								clickers[uidpos].prepacknum = spi_message[10];
							}
							/* 有效数据复制到缓存 */
							//rf_move_data_to_buffer(&nrf_communication);
							/* 更新接收数据帧号与包号 */
							dtq_to_jsq_sequence = spi_message[9];
							dtq_to_jsq_packnum = spi_message[10];
							/* 回复ACK */
							nrf_transmit_start(&dtq_to_jsq_sequence,0,NRF_DATA_IS_ACK, 1, 0, SEND_DATA_ACK_TABLE);
							/* 用户接收到数据处理函数 */
							my_nrf_receive_success_handler();
						}
					}
				}
			}
		}
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
bool checkout_online_uids(uint8_t src_table, uint8_t check_table, uint8_t mode, uint8_t *buffer,uint8_t *len)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t index = 0;

	for(i=rf_online_index[mode];(i<120)&&(*len<240);i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(src_table,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(check_table,i);
			if(is_online_pos == mode)
			{
				get_index_of_uid(i,buffer);
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("[%3d]:%02x%02x%02x%02x ",i,*buffer, *(buffer+1), *(buffer+2), *(buffer+3) );
#endif
				buffer = buffer+4;
				*len = *len + 4;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				if(((index++)+1) % 5 == 0)
					printf("\n");
#endif
			}
		}
	}

	if(i==120)
	{
		rf_online_index[mode] = 0;
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
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第1次发送统计结果：");
#endif
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				after_result_status = SEND_DATA2_STATUS;
			}
			break;

		case SEND_DATA2_UPDATE_STATUS:
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第2次发送统计结果：");
#endif
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				after_result_status = SEND_DATA3_STATUS;
			}
			break;
		case SEND_DATA3_UPDATE_STATUS:
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第3次发送统计结果：");
#endif
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				after_result_status = SEND_DATA4_STATUS;
				//after_result_status = SEND_IDLE_STATUS;
			}
			break;
		case SEND_DATA4_UPDATE_STATUS:
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第4次发送统计结果：");
#endif
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
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n\r\n[1].retransmit:\r\n");
#endif
				retransmit_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
				retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				retransmit_check_tables[CUR_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				retransmit_check_tables[CUR_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				after_retransmit_status             = SEND_DATA2_SEND_OVER_STATUS;
			}
			break;

		case SEND_DATA3_STATUS:
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n\r\n[2].retransmit:\r\n");
#endif
				retransmit_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				retransmit_check_tables[CUR_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				retransmit_check_tables[CUR_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				after_retransmit_status             = SEND_DATA3_SEND_OVER_STATUS;
			}
			break;

		default:
			{
				retransmit_check_tables[PRE_SUM_TABLE] = 0;
				retransmit_check_tables[PRE_ACK_TABLE] = 0;
				retransmit_check_tables[CUR_SUM_TABLE] = 0;
				retransmit_check_tables[CUR_ACK_TABLE] = 0;
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
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t puid[4];
	uint8_t index = 0;
	uint8_t clickernum = 0;

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
				printf("[%3d]:%02x%02x%02x%02x ",i,puid[0],puid[1],puid[2],puid[3]);
				if(((index++)+1) % 5 == 0)
					printf("\n");
#endif
			}
		}
	}
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	printf("\n");
#endif
	return clickernum;
}

/* 重发函数 */
void retansmit_data( uint8_t status )
{
	if(( status == SEND_DATA2_STATUS ) || ( status == SEND_DATA3_STATUS ))
	{
		get_retransmit_messsage( status );

		checkout_retransmit_clickers( retransmit_check_tables[PRE_SUM_TABLE] ,retransmit_check_tables[PRE_ACK_TABLE],
		                     retransmit_check_tables[CUR_SUM_TABLE] );
		/* 发送前导帧 */
		memset(nrf_communication.dtq_uid, 0, 4);
		nrf_transmit_start( nrf_communication.dtq_uid, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,
		                    SEND_PRE_DELAY100US, retransmit_check_tables[CUR_SUM_TABLE]);
		/* 发送数据帧 */
		memset(nrf_communication.dtq_uid, 0, 4);

		whitelist_checktable_or(retransmit_check_tables[PRE_ACK_TABLE],SEND_DATA_ACK_TABLE);

		nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL,
		                    SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE );

		/* 跟新状态，开始2次统计 */
		change_clicker_send_data_status( after_retransmit_status );
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
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
		printf("\r\nlost:\r\n");
#endif
		/* 返回失败的UID */
		while( message_tcb.Is_lost_over != 0)
		{
			message_tcb.Is_lost_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 0,
				revice_lost_massage.DATA,&(revice_lost_massage.LEN));
			message_tcb.lostuidlen += revice_lost_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_lost_massage.HEADER = 0x5C;
			memset(revice_lost_massage.SIGN,0,4);
			revice_lost_massage.TYPE = 0x30;
			revice_lost_massage.XOR = XOR_Cal((uint8_t *)(&(revice_lost_massage.TYPE)), revice_lost_massage.LEN+6);
			revice_lost_massage.END = 0xCA;
			if(revice_lost_massage.LEN != 0)
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_lost_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,revice_lost_massage.LEN);
			revice_lost_massage.LEN = 0;
		}
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
		printf("\r\nok:\r\n");
#endif
		message_tcb.clicker_count = 0;
		while(message_tcb.Is_ok_over != 0)
		{
			message_tcb.Is_ok_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 1,
				revice_ok_massage.DATA,&(revice_ok_massage.LEN));
			revice_ok_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_ok_massage.TYPE)),
			                                 revice_ok_massage.LEN+6);
			revice_ok_massage.END = 0xCA;
			message_tcb.clicker_count += revice_ok_massage.LEN/4;
			message_tcb.okuidlen += revice_ok_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_ok_massage.HEADER = 0x5C;
			memset(revice_ok_massage.SIGN,0,4);
			revice_ok_massage.TYPE = 0x31;
			revice_ok_massage.XOR = XOR_Cal((uint8_t *)(&(revice_ok_massage.TYPE)), revice_ok_massage.LEN+6);
			revice_ok_massage.END = 0xCA;
			if( revice_ok_massage.LEN != 0)
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_ok_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,revice_lost_massage.LEN);
			revice_ok_massage.LEN = 0;
		}
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
		printf("\r\ncount:%d\r\n",message_tcb.clicker_count);
#endif
		sum_clicker_count += message_tcb.clicker_count;
		message_tcb.clicker_count = 0;

		/* 上传在线状态 */
		if(message_tcb.lostuidlen != 0)
		{
			if( status == SEND_DATA3_UPDATE_STATUS )
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n\r\n[3].retransmit:\r\n");
#endif
				retransmit_tcb.sum = checkout_retransmit_clickers(SEND_DATA3_SUM_TABLE,SEND_DATA3_ACK_TABLE,
																			SEND_DATA4_SUM_TABLE);
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
void retransmit_data_to_next_clicker( uint8_t Is_next_uid, uint8_t *pos )
{
	if( Is_next_uid == 1 )
	{
		get_next_uid_of_white_list( SEND_DATA4_SUM_TABLE, retransmit_tcb.uid, pos );
	}
#ifdef RETRANSMIT_DATA_DETAIL_MESSAGE_SHOW
	printf("[%3d]:%02x%02x%02x%02x ",*pos,retransmit_tcb.uid[0],retransmit_tcb.uid[1],
																					retransmit_tcb.uid[2],retransmit_tcb.uid[3]);
#endif
	memcpy(rf_var.tx_buf, (uint8_t *)(backup_massage.DATA), backup_massage.LEN);
	memcpy(nrf_communication.dtq_uid,retransmit_tcb.uid,4);
	nrf_transmit_start(rf_var.tx_buf,0,NRF_DATA_IS_PRE,SEND_PRE_COUNT,
	                   SEND_PRE_DELAY100US,SEND_DATA4_SUM_TABLE);

	whitelist_checktable_or(SEND_DATA4_ACK_TABLE,SEND_DATA_ACK_TABLE);

	nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL,SEND_DATA_COUNT,
	                   SEND_DATA_DELAY100US,SEND_DATA_ACK_TABLE);

	rf_retransmit_set_status(1);
}

/******************************************************************************
  Function:retransmit_env_clear
  Description:
		清除重发过程中的环境变量
  Input :
  Return:
  Others:None
******************************************************************************/
void retransmit_env_clear( void )
{
	retransmit_tcb.count = 0;
	retransmit_tcb.sum = 0;
	retransmit_tcb.pos = 0;
	retransmit_tcb.status = 0;
	memset(retransmit_tcb.uid,0,4);
	clear_current_uid_index();
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
				retransmit_data_to_next_clicker( 1, &retransmit_tcb.pos );

			}
		}

		if(rf_retransmit_status == 2)
		{
			clickers[retransmit_tcb.pos].retransmit_count = 0;
			retransmit_tcb.count++;
#ifdef RETRANSMIT_DATA_DETAIL_MESSAGE_SHOW
			printf("ok\r\n");
			printf("retransmit_tcb.count = %d retransmit_tcb.sum = %d\r\n",
			 retransmit_tcb.count, retransmit_tcb.sum );
#endif
			if(retransmit_tcb.count == retransmit_tcb.sum)
			{
				change_clicker_send_data_status(SEND_DATA4_UPDATE_STATUS); // 11
				retransmit_env_clear();
			}
			else
			{
				rf_retransmit_set_status(0);
			}
		}

		if(rf_retransmit_status == 3)
		{
#ifdef RETRANSMIT_DATA_DETAIL_MESSAGE_SHOW
			printf("fail\r\n");
#endif
			clickers[retransmit_tcb.pos].retransmit_count++;

			if(clickers[retransmit_tcb.pos].retransmit_count == 3)
			{
				retransmit_tcb.count++;
#ifdef RETRANSMIT_DATA_DETAIL_MESSAGE_SHOW
			printf("retransmit_tcb.count = %d retransmit_tcb.sum = %d\r\n",
			 retransmit_tcb.count, retransmit_tcb.sum );
#endif
				clickers[retransmit_tcb.pos].retransmit_count = 0;

				if(retransmit_tcb.count == retransmit_tcb.sum)
				{
					change_clicker_send_data_status( SEND_DATA4_UPDATE_STATUS ); // 11
					retransmit_env_clear();
				}
				else
				{
					rf_retransmit_set_status(0);
				}
			}
			else
			{
				retransmit_data_to_next_clicker(0,&retransmit_tcb.pos);
			}
		}
	}

	/* 打印统计结果 */
	send_data_result( current_status );

	/* 上报之后，重新广播发送 */
  retansmit_data( current_status );
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

	memset(&retransmit_tcb.uid,0,8);

	memset(result_check_tables,0,2);
	after_result_status = 0;
	memset(retransmit_check_tables,0,4);
	after_retransmit_status = 0;

	/* clear online check table */
	memset(white_list_use_onlne_table[2],0,16*8);
}
