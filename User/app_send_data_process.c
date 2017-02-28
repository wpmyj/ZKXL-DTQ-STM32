/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "app_send_data_process.h"
#include "app_show_message_process.h"
#include "app_systick_package_process.h"

#define CLICKER_SNED_DATA_STATUS_TYPE     10
#define CLICKER_PRE_DATA_STATUS_TYPE      11

Process_tcb_Typedef Send_data_process, Single_send_data_process;
volatile send_data_process_tcb_tydef send_data_process_tcb;

extern uint8_t spi_status_buffer[SPI_DATA_IRQ_BUFFER_BLOCK_COUNT][20];
extern uint8_t spi_status_write_index, spi_status_read_index, spi_status_count;
extern uint8_t P_Vresion[2];

uint8_t is_open_statistic = 0;
uint8_t single_send_data_status = 0;
uint8_t single_sned_data_count = 0;
uint8_t retranmist_data_status = 0;

uint8_t clicker_send_data_status = 0;
static uint8_t pre_status = 0;
static uint8_t sum_clicker_count = 0;

extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[16][8];

extern Uart_MessageTypeDef backup_massage;
extern uint8_t sum_clicker_count;
/* ͳ�����ط�������ʹ�ñ��� */
// ����״̬����
#define LOST_INDEX        0
#define OK_INDEX          1

uint8_t rf_online_index[2];

// �ϱ����ط�
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
		�޸�clicker_send_data_status��״̬
  Input :
		clicker_send_data_status: clicker_send_data_status״̬
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
		�޸�clicker_send_data_status��״̬
  Input :
		clicker_send_data_status: clicker_send_data_status״̬
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_rf_retransmit_status(void)
{
	return retransmit_tcb.status;
}

/******************************************************************************
  Function:create_status_message
  Description:
		����һ��״̬��Ϣָ��
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
void create_status_message( void )
{
	//printf("create_status_message:%02x\r\n",clicker_send_data_status);
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
		�޸�clicker_send_data_status��״̬
  Input :
		clicker_send_data_status: clicker_send_data_status״̬
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
		��ȡclicker_send_data״̬
  Input :
		clicker_send_data: clicker_send_data����״̬
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
		��ȡclicker_send_data״̬
  Input :
		clicker_send_data: clicker_send_data����״̬
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t spi_buffer_status_check(uint8_t status)
{
	uint8_t current_status = status;

	if( current_status == 0 )
	{
		if(buffer_get_buffer_status(SPI_REVICE_BUFFER) == BUFFEREMPTY)
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
		�������״̬������
  Input :
		clicker_num������������
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
		����������״̬�����,�ۼ�����״̬
  Input :
		table1��֮ǰ״̬��1
		table2��֮ǰ״̬��2
  Output:
		table3���ۼ�֮���״̬��
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
		����������״̬���뺯��,�޳�����״̬
  Input :
		table1��֮ǰ״̬��1
		table2��֮ǰ״̬��2
  Output:
		table�������֮��ı��
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
		App RF ͳ�ƺ���
  Input :
  Return:
  Others:None
******************************************************************************/
void clicker_send_data_statistics( uint8_t send_data_status, uint8_t uidpos )
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
		App RF ��Ϣ���洦����
  Input :
  Return:
  Others:None
******************************************************************************/
void rf_move_data_to_buffer( uint8_t *Message )
{
	Uart_MessageTypeDef rf_message;

	uint8_t i = 0 ;
	uint8_t q_num = 0;
	uint8_t uidpos,Cmdtype;
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
		rf_message.CMDTYPE = 0x11;
		rf_message.DEVICE  = 0x01;
		memcpy(rf_message.VERSION,P_Vresion,2);
		memcpy(rf_message.SRCID,revicer.uid,UID_LEN);

		/* ��ȡ��Ϣ����Ч���� */
		*(uint16_t *)rf_message.LEN = Len;

		/* ��ȡ��Ŀ���ݵ���ʼ��ַ */
		prdata = Message+14+AckTableLen+2+1;

		if( Cmdtype == 0x10 )
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

		rf_message.XOR =  XOR_Cal((uint8_t *)(&(rf_message.DSTID)), s_index+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
		rf_message.END = 0xCA;

		if( Cmdtype == 0x10 )
		{
			if( wl.start == ON )
			{
				uint8_t Is_whitelist_uid = search_uid_in_white_list(Message+5,&uidpos);
				if(Message[12] != wl.uids[uidpos].rev_num)//�յ�������Ч����
				{
					/* ���뻺�� */
					if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
					{
						serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_message);
						/* ���½�������֡������� */
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
		RF SPI buffer ������
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t spi_process_revice_data( void )
{
	uint8_t spi_message[255];
	uint8_t spi_message_type = 0;
	bool    Is_whitelist_uid = OPERATION_ERR;
	uint8_t uidpos = 0xFF;
	uint8_t clicker_send_data_status = 0;

	if(buffer_get_buffer_status(SPI_REVICE_BUFFER) != BUFFEREMPTY)
	{
		uint16_t AckTableLen,DataLen,Len, i;

		memset(spi_message,0,255);
		spi_read_data_from_buffer( SPI_REVICE_BUFFER, spi_message );
		AckTableLen = spi_message[14];
		DataLen     = spi_message[14+AckTableLen+2];
		Len         = AckTableLen + DataLen + 19;
		clicker_send_data_status = spi_message[Len];
		spi_message_type = spi_message[13];

		#ifdef OPEN_BUFFER_DATA_SHOW
		{
			int i;
			DEBUG_BUFFER_ACK_LOG("%4d ", buffer_get_buffer_status(SPI_REVICE_BUFFER));
			DEBUG_BUFFER_ACK_LOG("Buffer Read :");
			for(i=5;i<9;i++)
				DEBUG_BUFFER_ACK_LOG("%02x",spi_message[i]);
		}
		#endif

		/* �ж��Ƿ�Ϊ״̬֡ */
		if(spi_message_type != 0x0A)
		{
			/* ���������� */
			Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

			if(wl.uids[uidpos].use == 0)
			{
				wl.uids[uidpos].use = 1;
				wl.uids[uidpos].firstrev = 1;
			}
			else
			{
				wl.uids[uidpos].firstrev = 0;
			}

			/* ����������״̬ */
			if(wl.switch_status == OFF)
			{
				/* �رհ������ǲ����˰����� */
				Is_whitelist_uid = OPERATION_SUCCESS;
			}

			/* ����ͨ�� */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				/* �յ�����Data */
				if(spi_message_type == NRF_DATA_IS_USEFUL)
				{
					/* ����ACK�İ��ź��ϴη��͵��Ƿ���ͬ */
					uint8_t temp;
					uint8_t Is_return_ack = 1;

					//printf("[DATA] uid:%02x%02x%02x%02x, ",\
						*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
					//printf("seq:%2x, pac:%2x\r\n",(uint8_t)*(spi_message+11),\
						(uint8_t)*(spi_message+12));

					if((spi_message[6+15] == 0x10) || (spi_message[6+15] == 0x11) ||
						 (spi_message[6+15] == 0x12) || (spi_message[6+15] == 0x13))
					{
						if( wl.start == ON )
							Is_return_ack = 1;
						else
							Is_return_ack = 0;
					}

					if( Is_return_ack )
					{
						/* �ظ�ACK */
						memcpy( nrf_data.dtq_uid, spi_message+5, 4 );
						nrf_transmit_start(&temp,0,NRF_DATA_IS_ACK, 2, 20, SEND_DATA_ACK_TABLE,PACKAGE_NUM_SAM);
					}

					/* ��Ч���ݸ浽PC */
					rf_move_data_to_buffer( spi_message );
				}
				/* �յ�����Ack */
				else if(spi_message_type == NRF_DATA_IS_ACK)
				{
					//printf("[ACK] uid:%02x%02x%02x%02x, ",\
						*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
					//printf("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
						(uint8_t)*(spi_message+12));

					/* ���Ͷ�ʱͳ��*/
					if( is_open_statistic == 0 )
					{
						clicker_send_data_statistics( clicker_send_data_status, uidpos );
					}
					else
					{
						set_index_of_white_list_pos(SINGLE_SEND_DATA_ACK_TABLE,uidpos);
					}

          /* �������ݶ�ʱͳ�� */
					if( retranmist_data_status == 1 )
					{
						uint8_t Is_reviceed_uid = get_index_of_white_list_pos_status(SEND_DATA_ACK_TABLE,uidpos);
						if( Is_reviceed_uid == 0 )
						{
							Uart_MessageTypeDef result_message;

//							result_message.TYPE   = 0x31;
//							result_message.HEADER = 0x5C;
//							memcpy(result_message.SIGN,backup_massage.SIGN,4);
//							result_message.LEN     = 0x06;
//							result_message.DATA[0] = 0x00;
//							result_message.DATA[1] = uidpos;
//							memcpy(result_message.DATA+2,spi_message+5,4);
//							result_message.XOR = XOR_Cal(&result_message.TYPE,12);
//							result_message.END  = 0xCA;

							if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
							{
								clear_index_of_white_list_pos(SEND_PRE_TABLE,uidpos);
								set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
								serial_ringbuffer_write_data(SEND_RINGBUFFER,&result_message);
							}
						}
					}

					/* �ظ����յ����ݣ����ذ��ź��ϴ�һ����*/
					if(wl.uids[uidpos].rev_num != spi_message[10])
					{
						/* ͳ�ƶ��� */
						if( wl.uids[uidpos].use == 1 )
						{
							if(wl.uids[uidpos].firstrev == 0)
							{
								if( spi_message[10] > wl.uids[uidpos].rev_num)
									wl.uids[uidpos].lost_package_num += spi_message[10] - wl.uids[uidpos].rev_num -1 ;

								if( spi_message[10] < wl.uids[uidpos].rev_num )
									wl.uids[uidpos].lost_package_num += spi_message[10] + 255 - wl.uids[uidpos].rev_num ;
							}
							else
							{
								wl.uids[uidpos].lost_package_num = 0;
							}

  						/* ͳ���յ����� */
							wl.uids[uidpos].recv_package_num++;
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
		��ȡ������״̬�Ĵ�����UID
  Input :
  Return:
  Others:None
******************************************************************************/
bool checkout_online_uids(uint8_t src_table, uint8_t check_table,
	                        uint8_t mode, uint8_t *buffer,uint8_t *len)
{
	uint8_t i;
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
				*buffer = i;
				get_index_of_uid(i,buffer+1);
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				{
					DEBUG_UID_LOG("[%3d]:%02x%02x%02x%02x ",i, *(buffer+1),*(buffer+2), *(buffer+3), *(buffer+4));
					if(((index++)+1) % 5 == 0)
					{
						DEBUG_UID_LOG("\n");
					}
				}
#endif
				buffer = buffer+5;
				*len = *len + 5;
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
				if(Send_data_process.retransmit == 1)
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
				if(Send_data_process.retransmit == 1)
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
		�����������ط�
  Input :
		presumtable  ���ܵ�������
		preacktable  ���Ѿ����ߣ������ط���������
		cursumtable  ���´�ͳ�Ƶ�������
  Return:
  Others:None
******************************************************************************/
uint8_t checkout_retransmit_clickers(uint8_t presumtable, uint8_t preacktable, uint8_t cursumtable)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t puid[4];
	uint8_t clickernum = 0;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	uint8_t index = 0;
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
	uint8_t i;
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
/* �ط����� */
void retansmit_data( uint8_t status )
{
	if(( status == SEND_DATA2_STATUS ) || ( status == SEND_DATA3_STATUS ))
	{
		get_retransmit_messsage( status );

		checkout_retransmit_clickers( retransmit_check_tables[PRE_SUM_TABLE] ,retransmit_check_tables[PRE_ACK_TABLE],
		                     retransmit_check_tables[CUR_SUM_TABLE] );
		/* ����ǰ��֡ */
		whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );
		memset(nrf_data.dtq_uid, 0, 4);
		nrf_transmit_start( nrf_data.dtq_uid, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,
		                    SEND_PRE_DELAY100US, SEND_PRE_TABLE,PACKAGE_NUM_SAM);
		/* ��������֡ */
		memset(nrf_data.dtq_uid, 0, 4);

		whitelist_checktable_or(retransmit_check_tables[PRE_ACK_TABLE],SEND_DATA_ACK_TABLE);

		nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL,
		                    SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE, PACKAGE_NUM_SAM );

		/* ����״̬����ʼ2��ͳ�� */
		change_clicker_send_data_status( after_retransmit_status );

		if(after_retransmit_status == SEND_DATA3_SEND_OVER_STATUS)
		{
			retransmit_env_init();
		}
		return ;
	}
}

/* ͳ�ƺ��� */
void send_data_result( uint8_t status )
{
	if(( status == SEND_DATA1_UPDATE_STATUS ) ||
		 ( status == SEND_DATA2_UPDATE_STATUS ) ||
		 ( status == SEND_DATA3_UPDATE_STATUS ) ||
	   ( status == SEND_DATA4_UPDATE_STATUS ))
	{

		get_send_data_table_message(status);

		DEBUG_DATA_DETAIL_LOG("\r\nlost:\r\n");

		/* ����ʧ�ܵ�UID */
		while( message_tcb.Is_lost_over != 0)
		{
			message_tcb.Is_lost_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 0,
				revice_lost_massage.DATA+1,revice_lost_massage.LEN);
			message_tcb.lostuidlen += *(uint16_t *)revice_lost_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_lost_massage.HEAD = UART_SOF;
			revice_lost_massage.DEVICE = 0x01;
			memcpy(revice_lost_massage.VERSION,P_Vresion,2);
			memcpy(revice_lost_massage.SRCID,revicer.uid,UID_LEN);
			memset(revice_lost_massage.DSTID,0,UID_LEN);
			revice_lost_massage.CMDTYPE = 0x12;
			*(uint16_t *)(revice_lost_massage.LEN) = *(uint16_t *)(revice_lost_massage.LEN)+1;
			revice_lost_massage.DATA[0] = status / 3;
			revice_lost_massage.XOR = XOR_Cal((uint8_t *)(&(revice_lost_massage.DEVICE)), 
			*(uint16_t *)(revice_lost_massage.LEN) + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA + 1);
			revice_lost_massage.END = 0xCA;
			if(revice_lost_massage.LEN != 0)
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_lost_massage);
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
				revice_ok_massage.DATA+1,revice_ok_massage.LEN);
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
			memset(revice_ok_massage.DSTID,0,UID_LEN);
			revice_ok_massage.CMDTYPE = 0x13;
			*(uint16_t *)revice_ok_massage.LEN = *(uint16_t *)revice_ok_massage.LEN+1;
			revice_ok_massage.DATA[0] = status / 3;
			revice_ok_massage.XOR = XOR_Cal((uint8_t *)(&(revice_ok_massage.DEVICE)), 
			*(uint16_t *)revice_ok_massage.LEN+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA+1);
			revice_ok_massage.END = 0xCA;
			if( revice_ok_massage.LEN != 0)
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_ok_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,*(uint16_t *)revice_lost_massage.LEN);
			*(uint16_t *)revice_ok_massage.LEN = 0;
		}
		DEBUG_STATISTICS_LOG("\r\ncount:%d\r\n",message_tcb.clicker_count);
		sum_clicker_count += message_tcb.clicker_count;
		message_tcb.clicker_count = 0;

		/* �ϴ�����״̬ */
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
		�������ط�����
  Input :
  Return:
  Others:None
******************************************************************************/
void retransmit_data_to_next_clicker( void )
{
	whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );
	nrf_transmit_start(rf_var.tx_buf,0,NRF_DATA_IS_PRE,SEND_PRE_COUNT,
	                   SEND_PRE_DELAY100US,SEND_PRE_TABLE,PACKAGE_NUM_SAM);

	whitelist_checktable_or(SEND_DATA4_ACK_TABLE,SEND_DATA_ACK_TABLE);

	nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL,SEND_DATA_COUNT,
	                   SEND_DATA_DELAY100US,SEND_DATA_ACK_TABLE,PACKAGE_NUM_SAM);

	rf_retransmit_set_status(1);
}

/******************************************************************************
  Function:retransmit_env_init
  Description:
		����ط������еĻ�������
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
		����ʱ��������ݴ��뵽buffer��
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_write_temp_buffer_to_buffer()
{
	if(BUFFEREMPTY != buffer_get_buffer_status(SPI_IRQ_BUFFER))
	{
		uint8_t spi_message[255];
		uint16_t AckTableLen,DataLen,Len;

		memset(spi_message,0,255);
		spi_read_data_from_buffer( SPI_IRQ_BUFFER, spi_message );

		AckTableLen = spi_message[14];
		DataLen     = spi_message[14+AckTableLen+2];
		Len         = AckTableLen + DataLen + 19;

		if(BUFFERFULL != buffer_get_buffer_status(SPI_REVICE_BUFFER))
		{
			spi_write_data_to_buffer(SPI_REVICE_BUFFER,spi_message, spi_message[Len]);
		}
	}

	if(BUFFERFULL != buffer_get_buffer_status(SPI_REVICE_BUFFER))
	{
		if((spi_status_count > 0) && (BUFFEREMPTY == buffer_get_buffer_status(SPI_IRQ_BUFFER)))
		{
			uint16_t AckTableLen,DataLen,Len, i;
			uint8_t *pdata;

			AckTableLen = spi_status_buffer[spi_status_read_index][14];
			DataLen     = spi_status_buffer[spi_status_read_index][14+AckTableLen+2];
			Len         = AckTableLen + DataLen + 19;

			spi_write_data_to_buffer(SPI_REVICE_BUFFER,spi_status_buffer[spi_status_read_index],
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
}

/******************************************************************************
  Function:send_data_env_init
  Description:
		���������ʹ����߼�����
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

	single_send_data_status = 0;
	single_sned_data_count  = 0;
	
	retranmist_data_status = 0;
	sw_clear_timer(&request_data_timer);
}

/******************************************************************************
  Function:change_single_send_data_status
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void change_single_send_data_status( uint8_t status )
{
	single_send_data_status = status;
}

/******************************************************************************
  Function:get_single_send_data_status
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t get_single_send_data_status( void )
{
	return single_send_data_status;
}

/******************************************************************************
  Function:single_send_data_result
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void single_send_data_result( uint8_t status, uint8_t step, uint8_t pos )
{
//	Uart_MessageTypeDef result_message;

//	if( status == 0 )
//		result_message.TYPE   = 0x31;
//	else
//		result_message.TYPE   = 0x30;

//	result_message.HEADER = 0x5C;
//	memcpy(result_message.SIGN,backup_massage.SIGN,4);
//	result_message.LEN     = 0x06;
//	result_message.DATA[0] = step;
//	result_message.DATA[1] = pos;
//	memcpy(result_message.DATA+2,Single_send_data_process.uid,4);
//	result_message.XOR = XOR_Cal(&result_message.TYPE,12);
//	result_message.END  = 0xCA;

//	if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
//	{
//		serial_ringbuffer_write_data(SEND_RINGBUFFER,&result_message);
//	}

}
/******************************************************************************
  Function:single_send_data_process
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_single_send_data_process( void )
{
	if( single_send_data_status == 2 )
	{
		uint8_t Is_whitelist_uid = 0, Is_revice = 0, upos = 0;
		uint8_t temp = 0;

		Is_whitelist_uid = search_uid_in_white_list( Single_send_data_process.uid, &upos );

		/* ����������״̬ */
		if(wl.switch_status == OFF)
		{
			/* �رհ������ǲ����˰����� */
			Is_whitelist_uid = OPERATION_SUCCESS;
		}

		if(Is_whitelist_uid == OPERATION_SUCCESS )
		{
			Is_revice = get_index_of_white_list_pos_status( SINGLE_SEND_DATA_ACK_TABLE, upos );

			if( Is_revice == 0 )
			{
				/* ����ǰ��֡ */
				whitelist_checktable_and( 0, SINGLE_SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );
				memcpy( nrf_data.dtq_uid, Single_send_data_process.uid, 4 );
				nrf_transmit_start( &temp, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,
														SEND_PRE_DELAY100US, SEND_PRE_TABLE,PACKAGE_NUM_SAM);
				/* ��������֡ */
				memcpy( nrf_data.dtq_uid, Single_send_data_process.uid, 4 );

				nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL,
														SEND_DATA_COUNT, SEND_DATA_DELAY100US, SINGLE_SEND_DATA_ACK_TABLE,PACKAGE_NUM_SAM);

				single_sned_data_count++;

				if( single_sned_data_count >= SINGLE_SEND_DATA_COUNT_MAX)
				{
					single_send_data_status = 0;
					DEBUG_STATISTICS_LOG("send over fail\r\n");
					clear_white_list_table(SINGLE_SEND_DATA_ACK_TABLE);
					#ifdef ENABLE_SEND_DATA_TO_PC
					single_send_data_result( 1, single_sned_data_count, upos );
					single_sned_data_count = 0;
					#endif
				}
				else
				{
					single_send_data_status = 1;
					DEBUG_STATISTICS_LOG("send count = %d\r\n",single_sned_data_count);
				}
			}
			else
			{
				single_send_data_status = 0;
				DEBUG_STATISTICS_LOG("OK \r\n");
				clear_white_list_table(SINGLE_SEND_DATA_ACK_TABLE);
				#ifdef ENABLE_SEND_DATA_TO_PC
				single_send_data_result( 0, single_sned_data_count, upos );
				single_sned_data_count = 0;
				#endif
			}
		}
		else
		{
			single_send_data_status = 0;
			DEBUG_STATISTICS_LOG("uid is not in white list fail\r\n");
			clear_white_list_table(SINGLE_SEND_DATA_ACK_TABLE);
			#ifdef ENABLE_SEND_DATA_TO_PC
			single_send_data_result( 1, single_sned_data_count, upos );
			single_sned_data_count = 0;
			#endif
		}
	}
}
/******************************************************************************
  Function:App_clickers_send_data_process
  Description:
		���������ʹ����߼�����
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_send_data_process( void )
{
	uint8_t spi_buffer_status = 0;
	uint8_t current_status = 0;

	/* ��ȡspi����д�뵽 Buffer */
	spi_write_temp_buffer_to_buffer();

	/* ��ȡ����״̬ */
	spi_buffer_status = spi_process_revice_data();

	/* ������ǰ״̬ */
	current_status = spi_buffer_status_check(spi_buffer_status);

	/* �������ϱ�֮�����µ������� */
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

	/* ��ӡͳ�ƽ�� */
	send_data_result( current_status );

	/* �ϱ�֮�����¹㲥���� */
	retansmit_data( current_status );
	
	if( retranmist_data_status == 2 ) 
	{
		memcpy(list_tcb_table[REQUEST_TABLE],list_tcb_table[SEND_DATA_ACK_TABLE],16);

		/* ����ǰ��֡ */
		whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );
		memset(nrf_data.dtq_uid, 0, 4);
		nrf_transmit_start( nrf_data.dtq_uid, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,
		                    SEND_PRE_DELAY100US, SEND_PRE_TABLE,PACKAGE_NUM_SAM);
		/* ��������֡ */
		memset(nrf_data.dtq_uid, 0, 4);
		nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL,
		                    SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE, PACKAGE_NUM_SAM );
		
		retranmist_data_status = 1;
		sw_clear_timer(&request_data_timer);
	}
}

/******************************************************************************
  Function:send_data_process_timer_init
  Description:
		���͹����еĶ�ʱ����ʼ��
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

	/* create single send data timer */
	sw_create_timer(&single_send_data_timer, SINGLE_SEND_DATA_TIMEOUT, 1, 2,
	                &(single_send_data_status), NULL);
}
