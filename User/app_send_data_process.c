/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "cJSON.h"
#include "app_send_data_process.h"
#include "app_show_message_process.h"

task_tcb_typedef send_data_task;
volatile send_data_process_tcb_tydef send_data_process_tcb;
static uint8_t send_data_status = 0;

extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];

extern WhiteList_Typedef    wl;
extern Revicer_Typedef      revicer;

void retransmit_env_init( void );

void set_send_data_status( uint8_t new_status )
{
	send_data_status = new_status;
//printf("send_data_status = %d\r\n",send_data_status);
}

uint8_t get_send_data_status( void )
{
	return send_data_status;
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

void Parse_time_to_str( char *str )
{
	char* pdata = str;
	char str1[10];
	/*system_rtc_timer:year*/
	memset(str1,0,4);
	sprintf(str1, "%04d" , system_rtc_timer.year);
	memcpy(pdata,str1,4);
	pdata = pdata + 4;
	*pdata = '-';
	pdata++;

	/*system_rtc_timer:mon*/
	memset(str1,0,4);
	sprintf(str1, "%02d" , system_rtc_timer.mon);
	memcpy(pdata,str1,2);
	pdata = pdata + 2;
	*pdata  = '-';
	pdata++;

	/*system_rtc_timer:date*/
	memset(str1,0,4);
	sprintf(str1, "%02d" , system_rtc_timer.date);
	memcpy(pdata,str1,2);
	pdata = pdata + 2;
	*pdata  = ' ';
	pdata++;

	/*system_rtc_timer:hour*/
	memset(str1,0,4);
	sprintf(str1, "%02d" , system_rtc_timer.hour);
	memcpy(pdata,str1,2);
	pdata = pdata + 2;
	*pdata  = ':';
	pdata++;

	/*system_rtc_timer:min*/
	memset(str1,0,4);
	sprintf(str1, "%02d" , system_rtc_timer.min);
	memcpy(pdata,str1,2);
	pdata = pdata + 2;
	*pdata  = ':';
	pdata++;

	/*system_rtc_timer:sec*/
	memset(str1,0,4);
	sprintf(str1, "%02d" , system_rtc_timer.sec);
	memcpy(pdata,str1,2);
	pdata = pdata + 2;
	*pdata  = ' ';
}

static void update_data_to_buffer( uint8_t *Message )
{
	typedef struct
	{
		uint8_t type;
		uint8_t id;
		uint8_t range;
	}answer_info_typedef;

	uint16_t uidpos;
	uint8_t  Cmdtype;
	uint16_t AckTableLen,DataLen;
	answer_info_typedef answer_temp = {0,0,0};

	AckTableLen = Message[14];
	DataLen     = Message[14+AckTableLen+2];
	Cmdtype     = Message[14+AckTableLen+1];

	if( DataLen > 0 )
	{
		uint16_t r_index = 0;
		uint8_t  is_last_data_full = 0;

		/* ��ȡ���ݵ���ʼ��ַ */
		uint8_t *prdata = Message+14+AckTableLen+2+1;

		if(( Cmdtype == 0x10 ) || ( Cmdtype == 0x24 ))
		{
			if( wl.start == ON )
			{
				uint8_t Is_whitelist_uid = search_uid_in_white_list(Message+5,&uidpos);
				if(Message[12] != wl.uids[uidpos].rev_num)//�յ�������Ч����
				{
					char str[20];
					uint8_t ClickerAnswerTime[CLICKER_TIMER_STR_LEN];
					
					b_print("{\r\n");
					b_print("  \'fun\': \'update_answer_list\',\r\n");
					memset(str,0,20);
					sprintf(str, "%010u" , *(uint32_t *)( wl.uids[uidpos].uid));
					b_print("  \'card_id\': \'%s\',\r\n", str );
					memset(ClickerAnswerTime,0x00,CLICKER_TIMER_STR_LEN);
					Parse_time_to_str((char *)ClickerAnswerTime);
					b_print("  \'update_time\': \'%s\',\r\n",(char *) ClickerAnswerTime );
					b_print("  \'answers\': [\r\n");
					
					if( Cmdtype == 0x10 )
					{
						char answer_type[2];
						char answer_range[7];
						char answer_id[3];
						
						while( r_index < DataLen-2 )
						{
							b_print("    {");
							if(is_last_data_full == 0)
							{
								answer_temp.type  = prdata[r_index] & 0x0F;
								answer_temp.id    = ((prdata[r_index] & 0xF0) >> 4)   | ((prdata[r_index+1] & 0x0F) << 4);
								answer_temp.range = ((prdata[r_index+1] & 0xF0) >> 4) | ((prdata[r_index+2] & 0x0F) << 4);
								r_index = r_index + 2;
								is_last_data_full = 1;
							}
							else
							{
								answer_temp.type  = (prdata[r_index] & 0xF0) >> 4;
								answer_temp.id    = prdata[r_index+1];
								answer_temp.range = prdata[r_index+2];
								r_index = r_index + 3;
								is_last_data_full = 0;
							}
							
							memset(answer_range,0x00,7);
							memset(answer_type, 0x00,2);
							memset(answer_id,   0x00,3);
							
							sprintf(answer_id, "%d" , answer_temp.id);
							
							switch( answer_temp.type )
							{
								case 0: 
								{
									uint8_t answer = (answer_temp.range)&0x3F;
									uint8_t *pdata = (uint8_t *)answer_range;
									switch(answer)
									{
										case 0x01: *pdata = 'A'; break;
										case 0x02: *pdata = 'B'; break;
										case 0x04: *pdata = 'C'; break;
										case 0x08: *pdata = 'D'; break;
										case 0x10: *pdata = 'E'; break;
										case 0x20: *pdata = 'F'; break;
										default: break;
									}
									memcpy(answer_type,"s",sizeof("s"));
								}
								break;

								case 1: 
								{
									uint8_t answer = (answer_temp.range)&0x3F;
									uint8_t *pdata = (uint8_t *)answer_range;
									
									if((answer&0x01) == 0x01)
									{
										*pdata = 'A';
										pdata = pdata + 1;
									}

									if((answer&0x02) == 0x02)
									{
										*pdata = 'B';
										pdata = pdata + 1;
									}

									if((answer&0x04) == 0x04)
									{
										*pdata = 'C';
										pdata = pdata + 1;
									}

									if((answer&0x08) == 0x08)
									{
										*pdata = 'D';
										pdata = pdata + 1;
									}

									if((answer&0x10) == 0x10)
									{
										*pdata = 'E';
										pdata = pdata + 1;
									}
									if((answer&0x20) == 0x20)
									{
										*pdata = 'F';
										pdata = pdata + 1;
									}

									memcpy(answer_type,"m",sizeof("m"));
								}
								break;

								case 2: 
								{
									uint8_t answer = (answer_temp.range)&0x3F;
									
									switch(answer)
									{
										case 0x01: // true
										{
											memcpy(answer_range,"true",sizeof("true"));
										}
										break;
										case 0x02: // false
										{
											memcpy(answer_range,"false",sizeof("false"));
										}
										break;
										default: break;
									}
										
									memcpy(answer_type,"j",sizeof("j"));
								}
								break;

								case 3: 
								{
									sprintf(answer_range, "%d" , answer_temp.range);
									memcpy(answer_type,"d",sizeof("d"));
								}
								break;
								
								default: break;
							}
							b_print("\'type\': \'%s\', ",answer_type);
							b_print("\'id\': \'%2s\', ",answer_id);
							b_print("\'answer\': \'%7s\' ",answer_range);
							if( r_index < DataLen-2 )
								b_print("},\r\n");
							else
								b_print("}\r\n");
						}
					}
					
					b_print("  ]\r\n");
					b_print("}\r\n");

					if( Cmdtype == 0x24 )
					{
						
					}
		
					/* ���½�������֡������� */
					wl.uids[uidpos].rev_seq = Message[11];
					wl.uids[uidpos].rev_num = Message[12];	
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

		/* �ж��Ƿ�Ϊ״̬֡ */
		if(spi_message_type != 0x0A)
		{
			/* ���������� */
			Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

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
					DEBUG_BUFFER_DTATA_LOG("[DATA] uid:%02x%02x%02x%02x, ",\
						*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
					DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x\r\n",(uint8_t)*(spi_message+11),\
						(uint8_t)*(spi_message+12));

					if( wl.start == ON )
					{
						nrf_transmit_parameter_t transmit_config;
						/* �ظ�ACK */
						memcpy(transmit_config.dist,spi_message+5, 4 );
						transmit_config.package_type   = NRF_DATA_IS_ACK;
						transmit_config.transmit_count = 2;
						transmit_config.delay100us     = 20;
						transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
						transmit_config.data_buf       = NULL;
						transmit_config.data_len       = 0;
						nrf_transmit_start( &transmit_config );

						/* ��Ч���ݸ浽PC */
						update_data_to_buffer( spi_message );
					}
				}
				/* �յ�����Ack */
				else if(spi_message_type == NRF_DATA_IS_ACK)
				{
					DEBUG_BUFFER_DTATA_LOG("[ACK] uid:%02x%02x%02x%02x, ",\
						*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
					DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
						(uint8_t)*(spi_message+12));

					if( get_send_data_status() != 0 )
					{
						uint8_t Is_reviceed_uid = get_index_of_white_list_pos_status(SEND_DATA_ACK_TABLE,uidpos);
						if( Is_reviceed_uid == 0 )
						{
							clear_index_of_white_list_pos(SEND_PRE_TABLE,uidpos);
							set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
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

		}
	}
	else
	{
		// ok to update to pc
	}
	return (clicker_send_data_status);
}

void App_retransmit_data( uint8_t is_new_pack )
{		
	nrf_transmit_parameter_t transmit_config;
	memset(transmit_config.dist,0, 4);
	transmit_config.package_type   = NRF_DATA_IS_PRE;
	transmit_config.transmit_count = SEND_PRE_COUNT;
	transmit_config.delay100us     = SEND_PRE_DELAY100US;
	transmit_config.is_pac_add     = is_new_pack;
	transmit_config.data_buf       = NULL;
	transmit_config.data_len       = 0;
	transmit_config.sel_table      = SEND_PRE_TABLE;
	nrf_transmit_start( &transmit_config );
	
	/* ��������֡ */
	memset(transmit_config.dist,0, 4);
	transmit_config.package_type   = NRF_DATA_IS_USEFUL;
	transmit_config.transmit_count = SEND_DATA_COUNT;
	transmit_config.delay100us     = SEND_DATA_DELAY100US;
	transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
	transmit_config.data_buf       = rf_var.tx_buf; 
	transmit_config.data_len       = rf_var.tx_len;
	transmit_config.sel_table      = SEND_DATA_ACK_TABLE;
	nrf_transmit_start( &transmit_config );
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
	spi_process_revice_data();
	
	if(( send_data_status == SEND_500MS_DATA_STATUS ) || 
		 ( send_data_status == SEND_2S_DATA_STATUS))
	{
		/* ����ǰ��֡ */
		whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );

		App_retransmit_data(send_data_process_tcb.is_pack_add);
		if(send_data_process_tcb.is_pack_add == PACKAGE_NUM_ADD)
			send_data_process_tcb.is_pack_add = PACKAGE_NUM_SAM;

		send_data_status = send_data_status + 1;
	}
}

void retransmit_2s_timer_callback( void )
{
	send_data_status = SEND_2S_DATA_STATUS;
}

void retransmit_500ms_timer_callback( void )
{
	static uint8_t send_count = 0;

	send_count++;
	send_data_status = SEND_500MS_DATA_STATUS;

	if( send_count == 8 )
	{
		send_data_status = SEND_2S_DATA_STATUS;
		send_count = 0;
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

	/* create send data timer */
	sw_create_timer(&retransmit_500ms_timer ,        // timer
	                500,                             // timeout (ms)
	                SEND_500MS_TIMER_START_STATUS,   // start status
	                SEND_500MS_TIMER_TIMEOUT_STATUS, // timeout status
	                &(send_data_status),             // status mechine
	                retransmit_500ms_timer_callback);// timerout callback 
	
	sw_create_timer(&retransmit_2s_timer    ,
	                2000, 
	                SEND_2S_TIMER_START_STATUS, 
	                SEND_2S_TIMER_TIMEOUT_STATUS,
	                &(send_data_status), 
	                retransmit_2s_timer_callback);

	/* ���������ų�ʼΪһ����0��ֵ�����0���������������ı�־ */
	revicer.sen_num = 0x55;
}
