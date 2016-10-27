/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_send_data_process.h"

#define CLICKER_SNED_DATA_STATUS_TYPE     10
#define CLICKER_PRE_DATA_STATUS_TYPE      11

static uint8_t clicker_send_data_status = 0;
static uint8_t pre_status = 0;
static uint8_t rf_retransmit_status = 0;
uint8_t sum_clicker_count = 0;

extern nrf_communication_t nrf_communication;
extern uint16_t white_list_use_onlne_table[10][8];
extern uint8_t retransmit_sum;
extern clicker_t clickers[120];
extern Uart_MessageTypeDef backup_massage;
extern uint8_t dtq_to_jsq_sequence;
extern uint8_t jsq_to_dtq_sequence;
extern uint8_t dtq_to_jsq_packnum;
extern uint8_t jsq_to_dtq_packnum;
extern uint8_t sign_buffer[4];
extern uint8_t retransmit_uid[4];
extern uint8_t sum_clicker_count;

uint8_t clicker_count = 0;
uint8_t Is_ok_over = 1, Is_lost_over = 1;
uint16_t lostuidlen = 0 ,okuidlen = 0 ;
uint8_t retransmit_count = 0;
uint8_t retransmit_sum = 0;
uint8_t retransmit_uid[4];
uint8_t retransmit_uid_pos = 0;

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

uint8_t uid_check_tables[2]       = { 0, 0 };
uint8_t uid_status_change         = 0;
uint8_t uid_retransmit_tables[4]  = { 0, 0, 0, 0};
uint8_t retransmit_status_change  = 0;

Uart_MessageTypeDef revice_lost_massage = {
	0x5C,                 // HEADER
	0x30,                 // TYPE
	0x00,0x00,0x00,0x00,  // UID
	0x00,                 // LEN

	0x00,0x00,0x00,0x00,      // ID

	0x00,                 // XOR
	0xCA,                 // END
};

Uart_MessageTypeDef revice_ok_massage = {
	0x5C,                 // HEADER
	0x30,                 // TYPE
	0x00,0x00,0x00,0x00,  // UID
	0x00,                 // LEN

	0x00,0x00,0x00,0x00,      // ID

	0x00,                 // XOR
	0xCA,                 // END
};

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
	rf_retransmit_status = new_status;
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
	return rf_retransmit_status;
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
	uint8_t spi_status_message[17];
	clicker_send_data_status = newstatus;
#ifdef OPEN_SEND_STATUS_SHOW
	printf("<%s>clicker_send_data_status = %d\r\n",__func__,clicker_send_data_status);
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
				if(spi_message[5] == retransmit_uid[0] &&
					 spi_message[6] == retransmit_uid[1]
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
bool checkout_online_uids(uint8_t src_table, uint8_t check_table, uint8_t mode, uint8_t *puid,uint8_t *len)
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
				get_index_of_uid(i,puid);
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("[%3d]:%02x%02x%02x%02x ",i,*puid, *(puid+1), *(puid+2), *(puid+3) );
#endif
				puid = puid+4;
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

void clickers_set_retransmit_table(uint8_t sumtable, uint8_t onlinetable, uint8_t nextsumtable)
{
	uint8_t i, is_use_pos = 0,is_online_pos = 0;
	
	for(i=0;i<120;i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(sumtable,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(onlinetable,i);
			if(is_online_pos == 0)
			{
				get_index_of_white_list_pos_status(sumtable,i);
				set_index_of_white_list_pos(nextsumtable,i);
				retransmit_sum++;
			}
		}
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
				uid_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
				uid_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				uid_status_change = SEND_DATA2_STATUS;
			}
			break;
		
		case SEND_DATA2_UPDATE_STATUS: 
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第2次发送统计结果：");
#endif
				uid_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				uid_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				uid_status_change = SEND_DATA3_STATUS;
			}
			break;
		case SEND_DATA3_UPDATE_STATUS: 
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第3次发送统计结果：");
#endif
				uid_check_tables[PRE_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				uid_check_tables[PRE_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				uid_status_change = SEND_DATA4_STATUS;
				//uid_status_change = SEND_IDLE_STATUS;
			} 
			break;
		case SEND_DATA4_UPDATE_STATUS: 
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n第4次发送统计结果：");
#endif
				uid_check_tables[PRE_SUM_TABLE] = SEND_DATA4_SUM_TABLE;
				uid_check_tables[PRE_ACK_TABLE] = SEND_DATA4_ACK_TABLE;
				uid_status_change = SEND_IDLE_STATUS;
			}
			break;
		default:
			{
				uid_check_tables[PRE_SUM_TABLE] = 0;
				uid_check_tables[PRE_ACK_TABLE] = 0;
				uid_status_change = 0;
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
				uid_retransmit_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
				uid_retransmit_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				uid_retransmit_tables[CUR_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				uid_retransmit_tables[CUR_ACK_TABLE] = SEND_DATA2_ACK_TABLE;	
				retransmit_status_change             = SEND_DATA2_SEND_OVER_STATUS;
			}
			break;
	
		case SEND_DATA3_STATUS: 
			{
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				printf("\r\n\r\n[2].retransmit:\r\n");
#endif
				uid_retransmit_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				uid_retransmit_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				uid_retransmit_tables[CUR_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				uid_retransmit_tables[CUR_ACK_TABLE] = SEND_DATA3_ACK_TABLE;	
				retransmit_status_change             = SEND_DATA3_SEND_OVER_STATUS;
			}
			break;
		
		default:
			{
				uid_retransmit_tables[PRE_SUM_TABLE] = 0;
				uid_retransmit_tables[PRE_ACK_TABLE] = 0;
				uid_retransmit_tables[CUR_SUM_TABLE] = 0;
				uid_retransmit_tables[CUR_ACK_TABLE] = 0;	
			}
			break;
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
void clicker_check_send_data(uint8_t sel_table, uint8_t uid_pos, uint8_t uid[], uint16_t delayms)
{
	uint8_t is_online_pos = 0;
	
	is_online_pos = get_index_of_white_list_pos_status(sel_table,uid_pos);
	if(is_online_pos == 0)
	{
		/* 如果发送间隔时间短，就是用阻塞式发送：直接延时等待，减小缓冲区负担 */
		printf("[%3d]:%02x%02x%02x%02x ",uid_pos,uid[0],uid[1],uid[2],uid[3]);
	}
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
void clickers_retransmit(uint8_t sumtable, uint8_t onlinetable, uint8_t nextsumtable,uint8_t nextOnlinetable, uint16_t delayms)
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
				if(delayms<10)
					clicker_check_send_data(nextOnlinetable,i,puid,delayms);

				if(delayms>10)
					retransmit_sum++;

				if(((index++)+1) % 5 == 0)
					printf("\n");
			}
		}
	}
	printf("\n");
}

/* 重发函数 */
void retansmit_data( uint8_t status )
{
	if(( status == SEND_DATA2_STATUS ) || ( status == SEND_DATA3_STATUS ))
	{
		get_retransmit_messsage( status );
		
		clickers_retransmit( uid_retransmit_tables[PRE_SUM_TABLE] ,uid_retransmit_tables[PRE_ACK_TABLE],
		                     uid_retransmit_tables[CUR_SUM_TABLE] ,uid_retransmit_tables[CUR_ACK_TABLE],0);
		/* 发送前导帧 */
		memset(nrf_communication.dtq_uid, 0, 4);
		nrf_transmit_start( nrf_communication.dtq_uid, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT, 
		                    SEND_PRE_DELAY100US, uid_retransmit_tables[CUR_SUM_TABLE]);
		/* 发送数据帧 */
		memset(nrf_communication.dtq_uid, 0, 4);

		whitelist_checktable_or(uid_retransmit_tables[PRE_ACK_TABLE],SEND_DATA_ACK_TABLE);
		
		nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL, 
		                    SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE );

		/* 跟新状态，开始2次统计 */
		change_clicker_send_data_status( retransmit_status_change );
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
		printf("\r\nlost:\r\n");
		/* 返回失败的UID */
		while(Is_lost_over != 0)
		{
			Is_lost_over = checkout_online_uids( uid_check_tables[PRE_SUM_TABLE],uid_check_tables[PRE_ACK_TABLE], 0,
				revice_lost_massage.DATA,&(revice_lost_massage.LEN));
			lostuidlen = revice_lost_massage.LEN;
			revice_lost_massage.LEN = 0;
		}

		printf("\r\nok:\r\n");
		clicker_count = 0;
		while(Is_ok_over != 0)
		{
			Is_ok_over = checkout_online_uids( uid_check_tables[PRE_SUM_TABLE],uid_check_tables[PRE_ACK_TABLE], 1,
				revice_ok_massage.DATA,&(revice_ok_massage.LEN));
			revice_ok_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_ok_massage.TYPE)), 
			                                 revice_ok_massage.LEN+6);
			revice_ok_massage.END = 0xCA;
			clicker_count += revice_ok_massage.LEN/4;
			okuidlen = revice_ok_massage.LEN;
			revice_ok_massage.LEN = 0;
		}
		printf("\r\ncount:%d\r\n",clicker_count);
		sum_clicker_count += clicker_count;
		clicker_count = 0;
		/* 上传在线状态 */
		if(lostuidlen != 0)
		{
			if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				//serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_data_massage);
			}

			if((Is_lost_over == 0) && (Is_ok_over == 0))
			{
				if( status == SEND_DATA3_UPDATE_STATUS )
				{
					printf("\r\n\r\n[3].retransmit:\r\n");
					clickers_set_retransmit_table(SEND_DATA3_SUM_TABLE,SEND_DATA3_ACK_TABLE,
																				SEND_DATA4_SUM_TABLE);
				}
				change_clicker_send_data_status( uid_status_change ); // 10
				if(status == SEND_DATA4_UPDATE_STATUS)
					clear_uid_check_table();
				okuidlen = 0;
				lostuidlen = 0;
				Is_lost_over = 1;
				Is_ok_over = 1;
			}
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table();
			okuidlen = 0;
			lostuidlen = 0;
			Is_lost_over = 1;
			Is_ok_over = 1;
		}
	}
}

/* 第三次重发函数 */
void retransmit_data_to_next_clicker( uint8_t Is_next_uid, uint8_t *pos )
{
	if(Is_next_uid == 1)
	{
		get_next_uid_of_white_list( SEND_DATA4_SUM_TABLE, retransmit_uid );

		search_uid_in_white_list( retransmit_uid, pos );
	}

	printf("[%3d]:%02x%02x%02x%02x ",*pos,retransmit_uid[0],retransmit_uid[1],
																					retransmit_uid[2],retransmit_uid[3]);

	memcpy(rf_var.tx_buf, (uint8_t *)(backup_massage.DATA), backup_massage.LEN);
	memcpy(nrf_communication.dtq_uid,retransmit_uid,4);
	nrf_transmit_start(rf_var.tx_buf,0,NRF_DATA_IS_PRE,SEND_PRE_COUNT,
	                   SEND_PRE_DELAY100US,SEND_DATA4_SUM_TABLE);
	whitelist_checktable_or(SEND_DATA3_ACK_TABLE,SEND_DATA_ACK_TABLE);

	nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL,SEND_DATA_COUNT,
	                   SEND_DATA_DELAY100US,SEND_DATA_ACK_TABLE);

	rf_retransmit_set_status(1);
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

		if(rf_retransmit_status == 0)
		{
			retransmit_data_to_next_clicker( 1, &retransmit_uid_pos );

		}

		if(rf_retransmit_status == 2)
		{
			printf("ok\r\n");
			clickers[retransmit_uid_pos].retransmit_count = 0;
			retransmit_count++;
			rf_retransmit_set_status(0);

			if(retransmit_count == retransmit_sum)
			{
				change_clicker_send_data_status(SEND_DATA4_UPDATE_STATUS); // 11
				retransmit_uid_pos = 0;
			}
		}

		if(rf_retransmit_status == 3)
		{
			printf("fail\r\n");
			clickers[retransmit_uid_pos].retransmit_count++;
			rf_retransmit_set_status(0);

			if(clickers[retransmit_uid_pos].retransmit_count == 3)
			{
				retransmit_count++;
				clickers[retransmit_uid_pos].retransmit_count = 0;
				
				if(retransmit_count == retransmit_sum)
				{
					change_clicker_send_data_status( SEND_DATA4_UPDATE_STATUS ); // 11
					retransmit_uid_pos = 0;
				}
			}
			else
			{
				retransmit_data_to_next_clicker(0,&retransmit_uid_pos);
			}
		}
	}
	
	/* 打印统计结果 */
	send_data_result( current_status );

	/* 上报之后，重新广播发送 */
  retansmit_data( current_status );
}
