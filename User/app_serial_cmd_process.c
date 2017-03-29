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

/* Private variables ---------------------------------------------------------*/
extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];

/* 暂存题目信息，以备重发使用 */
uint8_t json_read_index = 0;

extern wl_typedef       wl;
extern revicer_typedef  revicer;
extern task_tcb_typedef card_task;
/* Private functions ---------------------------------------------------------*/

const static serial_cmd_typedef cmd_list[] = {
{"clear_wl",       sizeof("clear_wl"),       serial_cmd_clear_uid_list},
{"bind",           sizeof("bind")-1,           serial_cmd_bind_operation},
{"answer_start",   sizeof("answer_start"),   serial_cmd_answer_start  },
{"answer_stop",    sizeof("answer_stop"),    serial_cmd_answer_stop   },
{"get_device_info",sizeof("get_device_info"),serial_cmd_get_device_no },
{"set_channel",    sizeof("set_channel"),    serial_cmd_set_channel   },
{"set_tx_power",   sizeof("set_tx_power"),   serial_cmd_set_tx_power  },
{"set_student_id", sizeof("set_student_id"), serial_cmd_set_student_id},
{"NO_USE",         sizeof("NO_USE"),         NULL                     }
};

static void serial_send_data_to_pc(void);
static void serial_cmd_process(void);

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
	if( revice_json_count > 0 )
	{
		cJSON *json;
		char *pdata = (char *)uart_irq_revice_massage[json_read_index];
		/* 增加对'的支持 */
		exchange_json_format( pdata, '\'', '\"' );
		
		json = cJSON_Parse((char *)uart_irq_revice_massage[json_read_index]); 
		if (!json)  
		{
				b_print("Error before: [%s]\n",cJSON_GetErrorPtr());  
		} 
		else
		{
			uint8_t i = 0, is_know_cmd = 0;
			char *p_cmd_str = cJSON_GetObjectItem(json, "fun")->valuestring;

			while(cmd_list[i].cmd_fun != NULL)
			{
				if(strncmp(p_cmd_str, cmd_list[i].cmd_str, cmd_list[i].cmd_len)== 0)
				{
					cmd_list[i].cmd_fun(json);
					is_know_cmd = 1;
				}
				i++;
			}

			if(is_know_cmd == 0)
				printf("{\'result\': \'unknow cmd\'}\r\n");
		}

		cJSON_Delete(json);
		revice_json_count--;
		memset( pdata, 0, JSON_BUFFER_LEN );
		json_read_index = (json_read_index + 1) % JSON_ITEM_MAX;
	}
}

void serial_cmd_clear_uid_list(const cJSON *object)
{
	char *out;
	cJSON *root;
	uint8_t result = 0;

	root = cJSON_CreateObject();

	result = initialize_white_list();

	cJSON_AddStringToObject(root, "fun", "clear_wl" );
	if(OPERATION_SUCCESS == result)
	{
		cJSON_AddStringToObject(root, "result", "0" );
	}
	else
	{
		cJSON_AddStringToObject(root, "result", "1" );
	}

	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 
}

void serial_cmd_bind_operation(const cJSON *object)
{
	char *out;
	cJSON *root;
	uint8_t card_status = 0;
	char *p_cmd_str = cJSON_GetObjectItem(object, "fun")->valuestring;
	
	root = cJSON_CreateObject();
	card_status = rf_get_card_status();
	
	if(strncmp( p_cmd_str, "bind_start", 10) == 0)
	{
		wl.match_status = ON;
		rf_set_card_status(1);
		cJSON_AddStringToObject(root, "fun", "bind_start" );
		if( card_status == 0 )
			cJSON_AddStringToObject(root, "result", "0" );
		else
			cJSON_AddStringToObject(root, "result", "-1" );
	}

	if(strncmp( p_cmd_str, "bind_stop", 9 ) == 0)
	{
		wl.match_status = OFF;
		rf_set_card_status(0);
		PcdHalt();
		PcdAntennaOff();
		cJSON_AddStringToObject(root, "fun", "bind_stop" );
		cJSON_AddStringToObject(root, "result", "0" );
	}

	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 	
}

void serial_cmd_answer_start(const cJSON *object)
{
	char *out;
	cJSON *root;

	uint8_t send_data_status = 0 ;

	/* 填充内容 */
	root = cJSON_CreateObject();

	/* update time */
	parse_str_to_time(cJSON_GetObjectItem(object, "time")->valuestring);

	/* create data for clicker */
	{
		typedef struct
		{
			uint8_t type;
			uint8_t id;
			uint8_t range;
		}answer_info_typedef;

		uint8_t  *pSdata = (uint8_t *)rf_var.tx_buf;
		uint16_t sdata_index = 0;
  	uint8_t  is_last_data_full = 0;
		cJSON *question_array;

		question_array = cJSON_GetObjectItem(object, "questions");
	
		/* 解析题目数组 */
		{
			cJSON *question = question_array->child;
		
			/* get answer message */
			while(question != NULL)
			{
				char *p_question_range = cJSON_GetObjectItem(question,"range")->valuestring;
				char *p_question_type  = cJSON_GetObjectItem(question,"type")->valuestring;

				answer_info_typedef answer_temp = {0,0,0};
				answer_temp.id = atoi(cJSON_GetObjectItem(question, "id")->valuestring);
				
				switch( p_question_type[0] )
				{
					case 's': answer_temp.type = 0; break;
					case 'm': answer_temp.type = 1; break;
					case 'j': answer_temp.type = 2; break;
					case 'd': answer_temp.type = 3; break;
					default: break;
				}

				/* change an from dec to hex */
				{
					char range_end;
					
					if( answer_temp.type == 2 )
						answer_temp.range = 0x03;
					else
					{
						range_end  = p_question_range[2];
						if(( range_end >= 'A') && ( range_end <= 'G'))
						{
							uint8_t j;
							for(j=0;j<=range_end-'A';j++)
								answer_temp.range |= 1<<j; 
						}
						
						if(( range_end >= '0') && ( range_end <= '9'))
						{
								answer_temp.range |= range_end - '0'; 
						}
					}
				}
				//printf("type  = %02x\r\n", answer_temp.type);
				//printf("id    = %02x\r\n", answer_temp.id);
				//printf("range = %02x\r\n", answer_temp.range);

				if(is_last_data_full == 0)
				{
					*(pSdata+(sdata_index++)) = ((answer_temp.type) & 0x0F ) | ((answer_temp.id & 0x0F) << 4);
					*(pSdata+(sdata_index++)) = ((answer_temp.id & 0xF0)>>4) | ((answer_temp.range & 0x0F) << 4);
					*(pSdata+(sdata_index))   = (answer_temp.range & 0xF0)>>4;
					is_last_data_full = 1;
				}
				else
				{
					*(pSdata+(sdata_index))   = *(pSdata+(sdata_index)) | ((answer_temp.type & 0x0F) << 4);
					sdata_index++;
					*(pSdata+(sdata_index++)) = answer_temp.id ;
					*(pSdata+(sdata_index++)) = answer_temp.range ;
					is_last_data_full = 0;
				}
				question = question->next;
			}
		}
		
		rf_var.cmd = 0x10;
		rf_var.tx_len = sdata_index+1 ;
	}

	/* send data */
	send_data_status = get_send_data_status();
	cJSON_AddStringToObject(root, "fun", "answer_start" );
	send_data_status = 0;

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

		/* return data */	
		cJSON_AddStringToObject(root, "result", "0" );			
	}
	else
	{
		cJSON_AddStringToObject(root, "result", "1" );
	}

	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 
}

void serial_cmd_get_device_no(const cJSON *object)
{		
	char *out,str[20];
	cJSON *root;
	
	/* 填充内容 */
	root = cJSON_CreateObject();
	
	cJSON_AddStringToObject(root, "fun", "get_device_info" );
	memset(str,0,20);
	sprintf(str, "%010u" , *(uint32_t *)(revicer.uid));
	cJSON_AddStringToObject(root, "device_id", str );
	cJSON_AddStringToObject(root, "software_version", "v0.1.0" );
	cJSON_AddStringToObject(root, "hardware_version", "ZL-RP551-MAIN-F" );
	cJSON_AddStringToObject(root, "company", "zkxltech" );
	
	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 
}

void serial_cmd_answer_stop(const cJSON *object)
{
	uint8_t sdata_index;
	uint8_t *pSdata;
	cJSON *root;
	char *out;
	
	/* 填充内容 */
	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "fun", "answer_stop" );
	
	pSdata = (uint8_t *)rf_var.tx_buf;

	/* 准备发送数据 */
	{
		*(pSdata+(sdata_index++)) = 0x01;
		rf_var.cmd = 0x11;
		rf_var.tx_len = sdata_index+1 ;
	}
	
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

	/* return data */	
	cJSON_AddStringToObject(root, "result", "0" );	

	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 	
}

void serial_cmd_set_channel(const cJSON *object)
{
	cJSON *root;
	char *out;
	char str[3];
	uint8_t tx_ch = 0x02, rx_ch = 0x04;
	int8_t status;

	tx_ch = atoi(cJSON_GetObjectItem(object, "tx_ch")->valuestring);
	rx_ch = atoi(cJSON_GetObjectItem(object, "rx_ch")->valuestring);
	if((( tx_ch > 1) && ( tx_ch < 11)) && 
		 (( rx_ch > 1) && ( rx_ch < 11)) &&
	    ( tx_ch != rx_ch ))
	{
		clicker_set.N_CH_TX = tx_ch;
		clicker_set.N_CH_RX = rx_ch;
				
		/* 设置接收的信道：答题器与接收是反的 */
		spi_set_cpu_rx_signal_ch(clicker_set.N_CH_TX);
		spi_set_cpu_tx_signal_ch(clicker_set.N_CH_RX);
		status = 0;
	}
	else
	{
		status = -1;
	}
	
	/* 填充内容 */
	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "fun", "set_channel" );
	sprintf(str, "%d" , (int8_t)(status));
	cJSON_AddStringToObject(root, "result", str );
	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 			
}

void serial_cmd_set_tx_power(const cJSON *object)
{
	cJSON *root;
	char *out;
	char str[3];
	uint8_t tx_power = 0x04;
	int8_t status;

	tx_power = atoi(cJSON_GetObjectItem(object, "tx_power")->valuestring);
	
	if(( tx_power > 1) && ( tx_power < 5))
	{
		switch( tx_power )
		{
			case 1:
				clicker_set.N_TX_POWER = -4;
			break;
			case 2:
				clicker_set.N_TX_POWER = -2;
			break;
			case 3:
				clicker_set.N_TX_POWER = 0;
			break;
			case 4:
				clicker_set.N_TX_POWER = 2;
			break;
			case 5:
				clicker_set.N_TX_POWER = 4;
			break;
			default:
				clicker_set.N_TX_POWER = 4;
			break;
		}
		EE_WriteVariable( CPU_TX_POWER_POS_OF_FEE , clicker_set.N_TX_POWER );
		status = 0;
	}
	else
	{
		status = -1;
	}
	
	/* 填充内容 */
	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "fun", "set_tx_power" );
	sprintf(str, "%d" , (int8_t)(status));
	cJSON_AddStringToObject(root, "result", str );
	/* 打印返回 */
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\'' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out); 			
}

void serial_cmd_set_student_id(const cJSON *object)
{
	cJSON *root;
	char *out;
	int8_t status;
	uint8_t i = 0;
	uint8_t len = strlen(cJSON_GetObjectItem(object, "student_id")->valuestring);
	char    *prdata = cJSON_GetObjectItem(object,    "student_id")->valuestring;
	uint8_t *pwdata = card_task.stdid;

	if((len > 0) && (len <= 20))
	{
		memset(pwdata,0,10);

		for(i=0;2*i<len;i++)
		{
			*pwdata = ((((prdata[2*i]-'0') << 4) & 0xF0) | 
			             ((prdata[2*i+1]-'0') & 0x0F));
			pwdata++;
		}

		wl.match_status = ON;
		wl.weite_std_id_status = ON;
		rf_set_card_status(1);
		status = 0;
	}
	else
	{
		char str[3];

		status = -1;
		root = cJSON_CreateObject();

		cJSON_AddStringToObject(root, "fun", "student_id" );
		sprintf(str, "%d" , (int8_t)(status));
		cJSON_AddStringToObject(root, "result", str );
		/* 打印返回 */
		out = cJSON_Print(root);
		exchange_json_format( out, '\"', '\'' );
		b_print("%s", out);
		cJSON_Delete(root);
		free(out); 		
	}	
}
/**************************************END OF FILE****************************/
