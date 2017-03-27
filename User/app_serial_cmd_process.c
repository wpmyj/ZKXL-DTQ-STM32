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

/* Private variables ---------------------------------------------------------*/
StateMechineTcb_Typedef uart_rev_status,uart_sen_status;
//uint8_t P_Vresion[2] = { 0x00, 0x02 };

extern StateMechineTcb_Typedef default_state_mechine_tcb;
extern uint8_t is_open_statistic;
extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];
       uint8_t serial_cmd_status = APP_SERIAL_CMD_STATUS_IDLE;
			 uint8_t serial_cmd_type = 0;
			 uint8_t err_cmd_type = 0;

/* 暂存题目信息，以备重发使用 */
//Uart_MessageTypeDef backup_massage;
extern uint8_t  uart_irq_revice_massage[2][300];
extern uint8_t revice_json_count;
uint8_t revice_json_read_index = 0;

extern WhiteList_Typedef wl;
extern Revicer_Typedef   revicer;
extern task_tcb_typedef  card_task;
/* Private functions ---------------------------------------------------------*/
static void serial_send_data_to_pc(void);
static void serial_cmd_process(void);

void serial_cmd_clear_uid_list(void);
void serial_cmd_bind_operation(const cJSON *object);
void serial_cmd_answer_start(const cJSON *object);
void serial_cmd_get_device_no(void);

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

		/* 增加对'的支持 */
		{
			char *pdata = (char *)uart_irq_revice_massage[revice_json_read_index];

			while(*pdata != '\0')
			{
				if(*pdata == '\'')
				{
					*pdata = '\"';
				}
				pdata++;
			}
		}
		
		json = cJSON_Parse((char *)uart_irq_revice_massage[revice_json_read_index]); 
		if (!json)  
		{
				printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
		} 
		else
		{
			/* clear_wl */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"clear_wl",8) == 0)
				serial_cmd_clear_uid_list();

			/* bind_operation */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"bind",4) == 0)
				serial_cmd_bind_operation(json);

			/* answer_start */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"answer_start",12) == 0)
				serial_cmd_answer_start(json);

			/* get_device_no */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"get_device_info",13) == 0)
				serial_cmd_get_device_no();
		}

		{
			cJSON_Delete(json);
			revice_json_count--;
			memset(uart_irq_revice_massage[revice_json_read_index],0,300);
			revice_json_read_index = (revice_json_read_index + 1) % 3;
		}
	}
}


void serial_cmd_clear_uid_list(void)
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
	{
		char *pdata = out;

		while(*pdata != '\0')
		{
			if(*pdata == '\"')
			{
				*pdata = '\'';
			}
			pdata++;
		}
	}
	printf("%s", out);
	cJSON_Delete(root);
	free(out); 
}

void serial_cmd_bind_operation(const cJSON *object)
{
	char *out;
	cJSON *root;
	uint8_t card_status = 0;
	
	root = cJSON_CreateObject();
	card_status = rf_get_card_status();
	
	if(strncmp(cJSON_GetObjectItem(object, "fun")->valuestring,"bind_start",10) == 0)
	{
		wl.match_status = ON;
		rf_set_card_status(1);
		cJSON_AddStringToObject(root, "fun", "bind_start" );
	}

	if(strncmp(cJSON_GetObjectItem(object, "fun")->valuestring,"bind_stop",9) == 0)
	{
		wl.match_status = OFF;
		rf_set_card_status(0);
		cJSON_AddStringToObject(root, "fun", "bind_stop" );
	}
	if( card_status == 0 )
		cJSON_AddStringToObject(root, "result", "0" );
	else
		cJSON_AddStringToObject(root, "result", "-1" );
	
	/* 打印返回 */
	out = cJSON_Print(root);
	{
		char *pdata = out;

		while(*pdata != '\0')
		{
			if(*pdata == '\"')
			{
				*pdata = '\'';
			}
			pdata++;
		}
	}
	printf("%s", out);
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
				char range_str[4];
				char type;
				answer_info_typedef answer_temp = {0,0,0};
				type = cJSON_GetObjectItem(question, "type")->valuestring[0];
				//printf("type = %s\r\n", cJSON_GetObjectItem(question, "type")->valuestring);
				answer_temp.id = atoi(cJSON_GetObjectItem(question, "id")->valuestring);
				//printf("id = %s\r\n", cJSON_GetObjectItem(question, "id")->valuestring);
				memcpy(range_str, cJSON_GetObjectItem(question, "range")->valuestring, 4);
				//printf("range = %s\r\n", cJSON_GetObjectItem(question, "range")->valuestring);
				
				switch( type )
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
						range_end  = range_str[2];
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
//	{
//		uint8_t i;
//		printf("rf_var.tx_buf:");
//		for(i=0;i<rf_var.tx_len;i++)
//		{
//			printf(" %02x",rf_var.tx_buf[i]);
//		}
//		printf("\r\n");
//	}
	}

	/* send data */
	//status = send_data_status ;
	send_data_status = 0;
	/* 发送数据 */
	if( send_data_status == 0 )
	{
		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
		//send_data_env_init();
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
	{
		char *pdata = out;

		while(*pdata != '\0')
		{
			if(*pdata == '\"')
			{
				*pdata = '\'';
			}
			pdata++;
		}
	}
	printf("%s", out);
	cJSON_Delete(root);
	free(out); 
}

void serial_cmd_get_device_no(void)
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
	{
		char *pdata = out;

		while(*pdata != '\0')
		{
			if(*pdata == '\"')
			{
				*pdata = '\'';
			}
			pdata++;
		}
	}
	printf("%s", out);
	cJSON_Delete(root);
	free(out); 
}

/**************************************END OF FILE****************************/
