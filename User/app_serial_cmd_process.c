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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "cJSON.h"
#include "app_send_data_process.h"
#include "app_card_process.h"
#include "app_show_message_process.h"

typedef  void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];

/* 暂存题目信息，以备重发使用 */
uint8_t json_read_index = 0;
uint8_t dtq_self_inspection_flg = 0;

extern wl_typedef       wl;
extern revicer_typedef  revicer;
extern task_tcb_typedef card_task;
/* Private functions ---------------------------------------------------------*/

const static serial_cmd_typedef cmd_list[] = {
{"clear_wl",       sizeof("clear_wl"),       serial_cmd_clear_uid_list},
{"bind",           sizeof("bind")-1,         serial_cmd_bind_operation},
{"answer_stop",    sizeof("answer_stop"),    serial_cmd_answer_stop   },
{"get_device_info",sizeof("get_device_info"),serial_cmd_get_device_no },
{"set_channel",    sizeof("set_channel"),    serial_cmd_set_channel   },
{"set_tx_power",   sizeof("set_tx_power"),   serial_cmd_set_tx_power  },
{"check_config",   sizeof("check_config"),   serial_cmd_check_config  },
{"set_student_id", sizeof("set_student_id"), serial_cmd_set_student_id},
{"one_key_off",    sizeof("one_key_off"),    serial_cmd_one_key_off   },
{"bootloader",     sizeof("bootloader"),     serial_cmd_bootloader    },
{"24g_attendance", sizeof("attendance_24g"), serial_cmd_attendance_24g},
{"dtq_self_inspection",sizeof("dtq_self_inspection"),serial_cmd_self_inspection},
{"NO_USE",         sizeof("NO_USE"),         NULL                     }
};

const static json_item_typedef answer_item_list[] = {
{"fun",      sizeof("fun"),       ANSWER_STATUS_FUN},
{"time",     sizeof("time"),      ANSWER_STATUS_TIME},
{"raise_hand",sizeof("raise_hand"), ANSWER_STATUS_HAND},
{"attendance",sizeof("attendance"), ANSWER_STATUS_SIGN},
{"questions",sizeof("questions"), ANSWER_STATUS_QUESTION},
{"type",     sizeof("type"),      ANSWER_STATUS_DATA_TYPE},
{"id",       sizeof("id"),        ANSWER_STATUS_DATA_ID},
{"range",    sizeof("range"),     ANSWER_STATUS_DATA_RANGE},
{"over",     sizeof("over"),      0xFF}
};

const static json_item_typedef import_item_list[] = {
{"fun",      sizeof("fun"),       IMPORT_STATUS_FUN},
{"addr",     sizeof("addr"),      IMPORT_STATUS_ADDR},
{"tx_ch",    sizeof("tx_ch"),     IMPORT_STATUS_TX_CH},
{"rx_ch",    sizeof("rx_ch"),     IMPORT_STATUS_RX_CH},
{"tx_power", sizeof("tx_power"),  IMPORT_STATUS_TX_POWER},
{"list",     sizeof("list"),      IMPORT_STATUS_LIST},
{"upos",     sizeof("upos"),      IMPORT_STATUS_UPOS},
{"uid",      sizeof("uid"),       IMPORT_STATUS_UID},
{"over",     sizeof("over"),      0xFF}
};

static void serial_send_data_to_pc(void);
static void serial_cmd_process(void);
void exchange_json_format( char *out, char old_format, char new_format);

/******************************************************************************
  Function:App_seirial_cmd_process
  Description:
		串口进程处理函数
  Input :None
  Return:None
  Others:None
******************************************************************************/
void serial_cmd_process(void)
{
	if( rjson_count > 0 )
	{
		char header[30];
		char *pdata = (char *)uart_irq_revice_massage[json_read_index];
		/* 增加对'的支持 */
		exchange_json_format( pdata, '\'', '\"' );
		memcpy(header,pdata+8,14);

		if( strncmp( header, "answer_start", sizeof("answer_start")-1)== 0 )
		{
			serial_cmd_answer_start( pdata );
		}
		else if( strncmp( header, "import_config", sizeof("import_config")-1)== 0 )
		{
			serial_cmd_import_config( pdata );
		}
		else
		{
			cJSON *json;
			json = cJSON_Parse((char *)uart_irq_revice_massage[json_read_index]);
			if (!json)
			{
				//b_print("Error before: [%s]\n",cJSON_GetErrorPtr());
				  b_print("{\r\n");
	        b_print("  \"fun\": \"Error\",\r\n");
					b_print("  \"description\": \"json syntax error!\"\r\n");
					b_print("}\r\n");
			}
			else
			{
				uint8_t i = 0, is_know_cmd = 0;
				char *p_cmd_str = cJSON_GetObjectItem(json, "fun")->valuestring;

				while(cmd_list[i].cmd_fun != NULL)
				{
					if(strncmp(p_cmd_str, cmd_list[i].cmd_str,
						 cmd_list[i].cmd_len)== 0)
					{
						cmd_list[i].cmd_fun(json);
						is_know_cmd = 1;
					}
					i++;
				}

				if(is_know_cmd == 0)
				{
					b_print("{\r\n");
	        b_print("  \"fun\": \"Error\",\r\n");
					b_print("  \"description\": \"unknow cmd!\"\r\n");
					b_print("}\r\n");
				}
			}
			cJSON_Delete(json);
		}

		rjson_count--;
		memset( pdata, 0, JSON_BUFFER_LEN );
		json_read_index = (json_read_index + 1) % JSON_ITEM_MAX;
	}
}

void parse_str_to_time( char *str )
{
	char str1[10];
	/*system_rtc_timer:year*/
	memset(str1,0,10);
	memcpy(str1,str,4);
	system_rtc_timer.year = atoi( str1 );
	/*system_rtc_timer:mon*/
	memset(str1,0,10);
	memcpy(str1,str+5,2);
	system_rtc_timer.mon = atoi( str1 );
	/*system_rtc_timer:date*/
	memset(str1,0,10);
	memcpy(str1,str+8,2);
	system_rtc_timer.date = atoi( str1 );
	/*system_rtc_timer:hour*/
	memset(str1,0,10);
	memcpy(str1,str+11,2);
	system_rtc_timer.hour = atoi( str1 );
	/*system_rtc_timer:min*/
	memset(str1,0,10);
	memcpy(str1,str+14,2);
	system_rtc_timer.min = atoi( str1 );
	/*system_rtc_timer:sec*/
	memset(str1,0,10);
	memcpy(str1,str+17,2);
	system_rtc_timer.sec = atoi( str1 );
//printf("Parse:num  = %d type = %d \r\n",num,type);
//printf("Parse:year = %d \r\n",system_rtc_timer.year);
//printf("Parse:mon  = %d \r\n",system_rtc_timer.mon);
//printf("Parse:mon  = %d \r\n",system_rtc_timer.date);	
//printf("Parse:hour = %d \r\n",system_rtc_timer.hour);
//printf("Parse:min  = %d \r\n",system_rtc_timer.min);	
//printf("Parse:sec  = %d \r\n",system_rtc_timer.sec);
}

void exchange_json_format( char *out, char old_format, char new_format)
{
	char *pdata = out;

	while(*pdata != '\0')
	{
		if(*pdata == old_format)
		{
			*pdata = new_format;
		}
		pdata++;
	}
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
//	if((uart_sen_status.get_status(&(uart_sen_status.state))) == 0)
//	{
//		/* enable interrupt Start send data*/
//		USART_ITConfig(USART1pos, USART_IT_TXE, ENABLE);
//  }
}


void serial_cmd_clear_uid_list(const cJSON *object)
{
	uint8_t result = 0;

	result = initialize_white_list();
	EE_WriteVariable(CPU_ADDR_CLONE_FLAG,0);
	get_mcu_uid();
	
	b_print("{\r\n");
	b_print("  \"fun\": \"clear_wl\",\r\n");

	if(OPERATION_SUCCESS == result)
		b_print("  \"result\": \"0\"\r\n");
	else
		b_print("  \"result\": \"-1\"\r\n");

	b_print("}\r\n");
}

void serial_cmd_bind_operation(const cJSON *object)
{
	uint8_t card_status = 0;
	char *p_cmd_str = cJSON_GetObjectItem(object, "fun")->valuestring;
	
	card_status = rf_get_card_status();
	b_print("{\r\n");

	if(strncmp( p_cmd_str, "bind_start", 10) == 0)
	{
		wl.match_status = ON;
		rf_set_card_status(1);
		b_print("  \"fun\": \"bind_start\",\r\n");
		if( card_status == 0 )
			b_print("  \"result\": \"0\"\r\n");
		else
			b_print("  \"result\": \"-1\"\r\n");
	}

	if(strncmp( p_cmd_str, "bind_stop", 9 ) == 0)
	{
		wl.match_status = OFF;
		rf_set_card_status(0);
		PcdHalt();
		PcdAntennaOff();
		b_print("  \"fun\": \"bind_stop\",\r\n");
		b_print("  \"result\": \"0\"\r\n");
	}
	b_print("}\r\n");
}

void serial_cmd_get_device_no(const cJSON *object)
{
	int8_t tx_power = 0,attend_tx_ch = 0;
	char str[20];
	uint8_t i,is_pos_use = 0;
	uint8_t count = 0;

	b_print("{\r\n");
	b_print("  \"fun\": \"get_device_info\",\r\n");
	memset(str,0,20);
	sprintf(str, "%010u" , *(uint32_t *)(revicer.uid));
	b_print("  \"device_id\": \"%s\",\r\n",str);
	b_print("  \"software_version\": \"v%d.%d.%d\",\r\n",software[0],software[1],software[2]);
	b_print("  \"hardware_version\": \"%s\",\r\n",hardware);
	b_print("  \"company\": \"zkxltech\",\r\n");
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_TX);
	b_print("  \"tx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_RX);
	b_print("  \"rx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	tx_power = clicker_set.N_TX_POWER;
	sprintf(str, "%d" , tx_power);
	b_print("  \"tx_power\": \"%s\",\r\n",str);
	if( clicker_set.N_24G_ATTEND & 0x80 )
	{
		b_print("  \"attendance_status\": \"on\",\r\n");
		memset(str,0,10);
		attend_tx_ch = clicker_set.N_24G_ATTEND & 0x7F;
		sprintf(str, "%d" , attend_tx_ch);
		b_print("  \"attendance_tx_ch\": \"%s\",\r\n",str);
	}
	else
		b_print("  \"attendance_status\": \"off\",\r\n");
	b_print("  \"list\": [\r\n");

	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		if( is_pos_use == 1 )
		{
			count++;
			b_print("    {");
			memset(str,0,20);
			sprintf(str, "%d" , i);
			b_print("  \"upos\": \"%s\",", str );
			memset(str,0,20);
			sprintf(str, "%010u" , *(uint32_t *)( wl.uids[i].uid));
			b_print("  \"uid\": \"%s\"", str );
			if( count < wl.len )
				b_print(" },\r\n");
			else
				b_print(" }\r\n");
		}
	}
	b_print("  ]\r\n");
	b_print(" }\r\n");
}

void serial_cmd_one_key_off(const cJSON *object)
{
	uint8_t sdata_index = 0;
	uint8_t *pSdata;

	/* 准备发送数据 */
	pSdata = (uint8_t *)rf_var.tx_buf;
	*(pSdata+(sdata_index++)) = 0x01;
	rf_var.cmd = 0x25;
	rf_var.tx_len = sdata_index+1 ;
	
	/* 发送数据 */
	{
		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);
		
		memset(nrf_data.dtq_uid,    0x00, 4);
		memset(transmit_config.dist,0x00, 4);

		send_data_process_tcb.is_pack_add = PACKAGE_NUM_ADD;

		/* 启动发送数据状态机 */
		set_send_data_status( SEND_500MS_DATA_STATUS );
	}

	b_print("{\r\n");
	b_print("  \"fun\": \"one_key_off\",\r\n");
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
}

void serial_cmd_answer_stop(const cJSON *object)
{
	uint8_t sdata_index;
	uint8_t *pSdata;

	/* 准备发送数据 */
	pSdata = (uint8_t *)rf_var.tx_buf;
	*(pSdata+(sdata_index++)) = 0x01;
	rf_var.cmd = 0x11;
	rf_var.tx_len = sdata_index+1 ;
	
	/* 发送数据 */
	{
		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);
		
		memset(nrf_data.dtq_uid,    0x00, 4);
		memset(transmit_config.dist,0x00, 4);

		send_data_process_tcb.is_pack_add = PACKAGE_NUM_ADD;

		/* 启动发送数据状态机 */
		set_send_data_status( SEND_500MS_DATA_STATUS );
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"answer_stop\",\r\n");
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
}

void serial_cmd_set_channel(const cJSON *object)
{
	char str[3];
	uint8_t tx_ch = 0x02, rx_ch = 0x04;
	int8_t status;

	tx_ch = atoi(cJSON_GetObjectItem(object, "tx_ch")->valuestring);
	rx_ch = atoi(cJSON_GetObjectItem(object, "rx_ch")->valuestring);
	if((( tx_ch >= 1) && ( tx_ch <= 11)) && 
		 (( rx_ch >= 1) && ( rx_ch <= 11)) &&
	    ( tx_ch != rx_ch ))
	{
		clicker_set.N_CH_TX = tx_ch;
		clicker_set.N_CH_RX = rx_ch;

		/* 设置接收的信道：答题器与接收是反的 */
		status  = spi_set_cpu_rx_signal_ch(clicker_set.N_CH_TX);
		status |= spi_set_cpu_tx_signal_ch(clicker_set.N_CH_RX);
	}
	else
	{
		status = -1;
	}
	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"set_channel\",\r\n");
	sprintf(str, "%d" , (int8_t)(status));
	b_print("  \"result\": \"%s\"\r\n",str);
	b_print("}\r\n");
}

void serial_cmd_set_tx_power(const cJSON *object)
{
	char str[3];
	int8_t tx_power = 0x04;
	int8_t status;

	tx_power = atoi(cJSON_GetObjectItem(object, "tx_power")->valuestring);
	
	if(( tx_power >= 1) && ( tx_power <= 5))
	{
		clicker_set.N_TX_POWER = tx_power;
		EE_WriteVariable( CPU_TX_POWER_POS_OF_FEE , clicker_set.N_TX_POWER );
		status = 0;
	}
	else
	{
		status = -1;
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"set_tx_power\",\r\n");
	sprintf(str, "%d" , (int8_t)(status));
	b_print("  \"result\": \"%s\"\r\n",str);
	b_print("}\r\n");
}

void serial_cmd_set_student_id(const cJSON *object)
{
	cJSON *root;
	char *out;
	int8_t status;
	uint8_t i = 0;
	char str[3];
	uint8_t card_status = rf_get_card_status();

	char    *prdata = cJSON_GetObjectItem(object,"student_id")->valuestring;
	uint8_t len = strlen( prdata );

	uint8_t *pwdata = card_task.stdid;

	if((len > 0) && (len <= 20))
	{
		uint8_t wdata_index = 0,rdata_index = 0;
		
		memset(pwdata,0,10);
		
		while( rdata_index < len )
		{
			*pwdata = (((prdata[rdata_index  ]-'0') << 4) & 0xF0) | 
			           ((prdata[rdata_index+1]-'0') & 0x0F);
			pwdata++;
			wdata_index++;
			rdata_index = rdata_index + 2;
		}
		
		if( len % 2 )
			*(pwdata-1) |= 0x0F; 

		for(i=wdata_index; i<10; i++)
		{
			*pwdata = 0xFF;
			pwdata++;
		}
		if(card_status == 0)
		{
			wl.match_status = ON;
			wl.weite_std_id_status = ON;
			rf_set_card_status(1);
			status = 0;
		}
		else
		{
			status = -1;
		}
	}
	else
	{
		status = -2;
	}

	/* 打印返回 */
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "fun", "set_student_id" );
	sprintf(str, "%d" , (int8_t)(status));
	cJSON_AddStringToObject(root, "result", str );
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\"' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out);
}

char *parse_json_item(char *pdata_str, char *key_str, char *value_str)
{
	uint8_t i = 0;
	char *pdata = pdata_str;
	
	while(*pdata != ':')
	{
		if(*pdata != '{')
		{
			if((*pdata != '"') && (*pdata != ','))
			{
			//printf("%c",*pdata);
				key_str[i++] = *pdata;
			}
		}
		pdata++;
	}
	key_str[i] = '\0';

	i = 0;
	pdata++;
	while((*pdata != ',') && (*pdata != '[') && (*pdata != '}'))
	{
		if(*pdata != '"')
		{
		//printf("%c",*pdata);
			value_str[i++] = *pdata;
		}
		pdata++;
	}
	pdata++;
	value_str[i] = '\0';
  //b_print("KEY:%7s  VALUE:%s \r\n",key_str,value_str);
	return (pdata);
}

void serial_cmd_answer_start(char *pdata_str)
{
	typedef struct
	{
		uint8_t type;
		uint8_t id;
		uint8_t range;
	}answer_info_typedef;
	
	/* prase data control */ 
	char *p_end,*p_next_start; 
	char value_str[25],key_str[10];
	uint8_t parse_data_status = 0;
	uint16_t len = strlen(pdata_str);
	uint8_t real_total = 0;

	/* send data control */
	uint8_t  *pSdata = (uint8_t *)rf_var.tx_buf+1;
	uint16_t sdata_index = 0;
  uint8_t  is_last_data_full = 0;
	answer_info_typedef answer_temp = {0,0,0};
	uint8_t send_data_status;
	uint8_t raise_hand_sign_in = 0;
	
	/* print result */
	char   result_str[3];
	int8_t result = 0;
	
	/* prase the first key and value */
	p_end = parse_json_item( pdata_str, key_str, value_str );

	while( (p_end - pdata_str) < len-3 )
	{
		uint8_t i = 0;
		p_next_start = p_end;
		
		/* prase next key and value, and get string status*/
		p_end = parse_json_item( p_next_start, key_str, value_str );
		while(answer_item_list[i].status != 0xFF)
		{
			if(strncmp( key_str, answer_item_list[i].key,
				          answer_item_list[i].key_str_len)== 0)
			{
				parse_data_status = answer_item_list[i].status;
				break;
			}
			i++;
		}
		
		/* process string status, get prase data */
		if( result != 0 )
			break ;
				
		switch( parse_data_status )
		{
			
			case ANSWER_STATUS_FUN: break;
			case ANSWER_STATUS_TIME:
					parse_str_to_time( value_str );
				break;
			case ANSWER_STATUS_QUESTION:
				break;
			case ANSWER_STATUS_HAND:
				{
					uint32_t temp = atoi( value_str );
					if( temp <= 1 )
					{
						if( temp == 0 )
							raise_hand_sign_in &= 0xFE;
						else
							raise_hand_sign_in |= 0x01;
						rf_var.tx_buf[0] = raise_hand_sign_in;
					}
				}
				break;
			case ANSWER_STATUS_SIGN:
				{
					uint32_t temp = atoi( value_str );
					if( temp <= 1 )
					{
						if( temp == 0 )
							raise_hand_sign_in &= 0xFD;
						else
							raise_hand_sign_in |= 0x02;
						rf_var.tx_buf[0] = raise_hand_sign_in;
					}
				}
				break;
			case ANSWER_STATUS_DATA_TYPE:
				switch( value_str[0] )
				{
					case 's': answer_temp.type = 0; break;
					case 'm': answer_temp.type = 1; break;
					case 'j': answer_temp.type = 2; break;
					case 'd': answer_temp.type = 3; break;
					case 'g': answer_temp.type = 4; break;
					default: break;
				}
				break;
			case ANSWER_STATUS_DATA_ID:
				answer_temp.id = atoi(value_str);
			break;
			case ANSWER_STATUS_DATA_RANGE:
				{
					char range_end;
					if( answer_temp.type == 2 )
						answer_temp.range = 0x03;
					else if( answer_temp.type == 4 )
						answer_temp.range = 0xFF;
					else
					{
						range_end  = value_str[2];
						if(( range_end >= 'A') && ( range_end <= 'G'))
						{
							uint8_t j;
							for(j=0;j<=range_end-'A';j++)
								answer_temp.range |= 1<<j; 
						}
						
						if(( range_end >= '0') && ( range_end <= '9'))
						{
								answer_temp.range = range_end - '0'; 
						}
					}
					real_total++;
//				printf("[%3d]:{ type  = %02x, ", real_total,answer_temp.type);
//				printf("id = %2d, ",answer_temp.id);
//				printf("range = %02x }\r\n",answer_temp.range);

					if( real_total > 80 )
					{
						result = -2;
						break;
					}

					if(is_last_data_full == 0)
					{
						*(pSdata+(sdata_index++)) = ((answer_temp.type) & 0x0F ) | 
						                            ((answer_temp.id & 0x0F) << 4);
						*(pSdata+(sdata_index++)) = ((answer_temp.id & 0xF0)>>4) | 
						                            ((answer_temp.range & 0x0F) << 4);
						*(pSdata+(sdata_index))   = (answer_temp.range & 0xF0)>>4;
						is_last_data_full = 1;
					}
					else
					{
						*(pSdata+(sdata_index))   = *(pSdata+(sdata_index)) | 
						                            ((answer_temp.type & 0x0F) << 4);
						sdata_index++;
						*(pSdata+(sdata_index++)) = answer_temp.id ;
						*(pSdata+(sdata_index++)) = answer_temp.range ;
						is_last_data_full = 0;
					}
				}
				break;
			default:
				break;
		}
	}
	
	/* set rf buffer len */
	rf_var.cmd = 0x10;
	if(is_last_data_full == 1)
		rf_var.tx_len = sdata_index+1 ;
	else
		rf_var.tx_len = sdata_index ;
	
	rf_var.tx_len = rf_var.tx_len + 1;
	send_data_status = get_send_data_status();

	/* 发送数据 */
	if(( send_data_status == SEND_IDLE_STATUS ) ||
		 ( send_data_status >= SEND_2S_DATA_STATUS))
	{
		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);
		
		memset(nrf_data.dtq_uid,    0x00, 4);
		memset(transmit_config.dist,0x00, 4);

		send_data_process_tcb.is_pack_add = PACKAGE_NUM_ADD;

		/* 启动发送数据状态机 */
		set_send_data_status( SEND_500MS_DATA_STATUS );
	}
	else
		result = -1;

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"answer_start\",\r\n");
	sprintf(result_str, "%d" , result);
	b_print("  \"result\": \"%s\"\r\n",result_str);
	b_print("}\r\n");
}


void serial_cmd_check_config(const cJSON *object)
{
	int8_t tx_power = 0;
	int8_t attend_tx_ch = 0;
	char str[20];
	uint8_t i,is_pos_use = 0;
	uint8_t count = 0;

	b_print("{\r\n");
	b_print("  \"fun\": \"check_config\",\r\n");
	memset(str,0,20);
	sprintf(str, "%010u" , *(uint32_t *)(revicer.uid));
	b_print("  \"addr\": \"%s\",\r\n",str);
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_TX);
	b_print("  \"tx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_RX);
	b_print("  \"rx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	tx_power = clicker_set.N_TX_POWER;
	sprintf(str, "%d" , tx_power);
	b_print("  \"tx_power\": \"%s\",\r\n",str);

	if( clicker_set.N_24G_ATTEND & 0x80 )
	{
		b_print("  \"attendance_status\": \"on\",\r\n");
		memset(str,0,10);
		attend_tx_ch = clicker_set.N_24G_ATTEND & 0x7F;
		sprintf(str, "%d" , attend_tx_ch);
		b_print("  \"attendance_tx_ch\": \"%s\",\r\n",str);
	}
	else
		b_print("  \"attendance_status\": \"off\",\r\n");

	b_print("  \"list\": [\r\n");

	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		if( is_pos_use == 1 )
		{
			count++;
			b_print("    {");
			memset(str,0,20);
			sprintf(str, "%d" , i);
			b_print("  \"upos\": \"%s\",", str );
			memset(str,0,20);
			sprintf(str, "%010u" , *(uint32_t *)( wl.uids[i].uid));
			b_print("  \"uid\": \"%s\"", str );
			if( count < wl.len )
				b_print(" },\r\n");
			else
				b_print(" }\r\n");
		}
	}
	b_print("  ]\r\n");
	b_print("}\r\n");
}

void serial_cmd_import_config(char *pdata_str)
{
	typedef struct
		{
			uint8_t type;
			uint8_t id;
			uint8_t range;
		}answer_info_typedef;
	
	/* prase data control */ 
	char *p_end,*p_next_start; 
	char value_str[25],key_str[10];
	uint8_t parse_data_status = 0;
	uint16_t len = strlen(pdata_str);
	uint16_t upos;
	uint32_t uid;
	uint8_t count = 0;

	/* print result */
	char   result_str[3];
	int8_t result = 0;

	result = initialize_white_list();

	/* prase the first key and value */
	p_end = parse_json_item( pdata_str, key_str, value_str );

	while( (p_end - pdata_str) < len-3 )
	{
		uint8_t i = 0;
		p_next_start = p_end;
		
		/* prase next key and value, and get string status*/
		p_end = parse_json_item( p_next_start, key_str, value_str );
		while(import_item_list[i].status != 0xFF)
		{
			if(strncmp( key_str, import_item_list[i].key,
				          import_item_list[i].key_str_len)== 0)
			{
				parse_data_status = import_item_list[i].status;
				break;
			}
			i++;
		}

		/* process string status, get prase data */
		if( result != 0 )
			break ;
		
		switch( parse_data_status )
		{
			
			case IMPORT_STATUS_FUN:
				break;
			case IMPORT_STATUS_ADDR:
				{
					uint32_t uid;
					uid = atof(value_str);
					memcpy(revicer.uid,(uint8_t *)&uid,4);
					EE_WriteVariable(CPU_ADDR_CLONE_FLAG,1);
					EE_WriteVariable(CPU_CLONE_ADDR+0,revicer.uid[0]);
					EE_WriteVariable(CPU_CLONE_ADDR+1,revicer.uid[1]);
					EE_WriteVariable(CPU_CLONE_ADDR+2,revicer.uid[2]);
					EE_WriteVariable(CPU_CLONE_ADDR+3,revicer.uid[3]);
				}
				break;
			case IMPORT_STATUS_TX_CH:
				{
					uint8_t tx_ch;
					tx_ch = atoi(value_str);

					if(( tx_ch > 1) && ( tx_ch < 11))
					{
						clicker_set.N_CH_TX = tx_ch;

						/* 设置接收的信道：答题器与接收是反的 */
						spi_set_cpu_rx_signal_ch(clicker_set.N_CH_TX);
						result = 0;
					}
					else
					{
						result = -1;
					}
				}
				break;
			case IMPORT_STATUS_RX_CH:
				{
					uint8_t rx_ch = atoi(value_str);
					if((( rx_ch > 1) && ( rx_ch < 11)))
					{
						clicker_set.N_CH_RX = rx_ch;

						/* 设置接收的信道：答题器与接收是反的 */
						spi_set_cpu_tx_signal_ch(clicker_set.N_CH_RX);
						result = 0;
					}
					else
					{
						result = -1;
					}
				}
				break;
			case IMPORT_STATUS_TX_POWER:
				{
					uint8_t tx_power = atoi(value_str);
					if(( tx_power >= 1) && ( tx_power <= 5))
						{
							clicker_set.N_TX_POWER = tx_power;
							EE_WriteVariable( CPU_TX_POWER_POS_OF_FEE , clicker_set.N_TX_POWER );
							result = 0;
						}
						else
						{
							result = -1;
						}
				}
				break;
			case IMPORT_STATUS_UPOS: 
				{
					upos = atoi(value_str);
					if(upos >= MAX_WHITE_LEN)
						result = -1;
				}
				break;
			case IMPORT_STATUS_UID: 
				{
					uint8_t *pdata,is_white_list_uid;
					uint16_t uid_pos;
					if(count <= MAX_WHITE_LEN)
					{
						uid = atof(value_str);
						pdata = (uint8_t *)&uid;
						is_white_list_uid = search_uid_in_white_list((uint8_t *)&uid,&uid_pos);
						if(is_white_list_uid == OPERATION_ERR)
						{
							add_index_of_uid(upos,pdata);
							count++;
						}
					}
					else
					{
						result = -1;
					}
				}
				break;
			default:
				break;
		}
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"import_config\",\r\n");
	sprintf(result_str, "%d" , result);
	b_print("  \"result\": \"%s\"\r\n",result_str);
	b_print("}\r\n");
}

void serial_cmd_bootloader(const cJSON *object)
{
	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"bootloader\",\r\n");
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
	DelayMs(200);
	{
		uint32_t JumpAddress;
		pFunction JumpToBootloader;
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (0x8000000 + 4);
		JumpToBootloader = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) 0x8000000);
		JumpToBootloader();
	}
}

void serial_cmd_attendance_24g(const cJSON *object)
{
	char str[3];
	uint8_t attend = 0x00;
	int8_t  tx_ch = 81;
	int8_t status;

	attend = atoi(cJSON_GetObjectItem(object, "attendance_status")->valuestring);
	tx_ch  = atoi(cJSON_GetObjectItem(object, "attendance_tx_ch")->valuestring);
	
	if(( attend <= 1) && (( tx_ch > 0) && ( tx_ch < 127)))
	{
		if (attend == 1)
			clicker_set.N_24G_ATTEND = (uint8_t)tx_ch | 0x80;
		else
			clicker_set.N_24G_ATTEND = (uint8_t)tx_ch & 0x7F;

		EE_WriteVariable( CPU_24G_ATTENDANCE_OF_FEE , clicker_set.N_24G_ATTEND );
		status = 0;
	}
	else
	{
		status = -1;
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"24g_attendance\",\r\n");
	sprintf(str, "%d" , (int8_t)(status));
	b_print("  \"result\": \"%s\"\r\n",str);
	b_print("}\r\n");

}

void serial_cmd_self_inspection(const cJSON *object)
{
	uint8_t sdata_index = 0;
	uint8_t *pSdata;

	/* 准备发送数据 */
	pSdata = (uint8_t *)rf_var.tx_buf;
	*(pSdata+(sdata_index++)) = 0x00;
	*(pSdata+(sdata_index++)) = 0x00;
	rf_var.cmd = 0xF1;
	rf_var.tx_len = sdata_index+1 ;

	/* 发送数据 */
	{
		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);

		memset(nrf_data.dtq_uid,    0x00, 4);
		memset(transmit_config.dist,0x00, 4);

		send_data_process_tcb.is_pack_add = PACKAGE_NUM_ADD;

		/* 启动发送数据状态机 */
		set_send_data_status( SEND_500MS_DATA_STATUS );
		dtq_self_inspection_flg = 1;
	}

	b_print("{\r\n");
	b_print("  \"fun\": \"dtq_self_inspection\",\r\n");
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
}

/**************************************END OF FILE****************************/
