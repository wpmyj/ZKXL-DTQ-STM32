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
#include "app_show_message_process.h"

/* uid */
extern Revicer_Typedef   revicer;
extern RTC_timer_Typedef system_rtc_timer;
extern uint16_t list_tcb_table[16][8];
extern nrf_communication_t nrf_data;
extern WhiteList_Typedef wl;
extern uint8_t ClickerAnswerTime[MAX_WHITE_LEN][21];

/* 暂存题目信息，以备重发使用 */
Uart_MessageTypeDef backup_massage;
extern uint8_t  uart_irq_revice_massage[3][300];
extern uint8_t revice_json_count;
uint8_t revice_json_read_index = 0;


void Parse_str_to_time( char *str )
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
	if( revice_json_count > 0 )
	{
		cJSON *json;
		json = cJSON_Parse((char *)uart_irq_revice_massage[revice_json_read_index]); 
		if (!json)  
		{
				printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
		} 
		else
		{
			/* start */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"start",5) == 0)
			{
				cJSON *root;
				uint8_t num  = 0;
				uint8_t type = 0;
				uint8_t status = 0;

				uint8_t send_data_status = get_clicker_send_data_status() ;
				uint8_t single_data_status = get_single_send_data_status();

				/* 填充内容 */
				root = cJSON_CreateObject();

				/* update time */
				Parse_str_to_time(cJSON_GetObjectItem(json, "time")->valuestring);

				/* create data for clicker */
				{
					uint8_t i,answer = 0;
					uint8_t header[7] = { 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11};

					num  = atoi(cJSON_GetObjectItem(json, "num")->valuestring);
					type = atoi(cJSON_GetObjectItem(json, "type")->valuestring);

					switch(type)
					{
						case 1 : answer= 0x7F; break;
						case 2 : answer= 0x7F; break;
						case 3 : answer= 0xC3; break;
						default:  break;
					}
					
					backup_massage.HEADER  = 0x5C;
					backup_massage.TYPE    = 0x10;
					backup_massage.LEN     = num*2+1 + 10;
					memcpy(backup_massage.DATA,header,7);
					backup_massage.DATA[7] = num*2+1;
					backup_massage.DATA[8] = num;
					for(i=0;i<num*2;)
					{
						backup_massage.DATA[9+i] = 1+i;
						backup_massage.DATA[10+i] = answer;
						i = i + 2;
					}
					backup_massage.DATA[8+num*2 + 1] = XOR_Cal((uint8_t *)(backup_massage.DATA+1), backup_massage.DATA[7]+7);
					backup_massage.DATA[8+num*2 + 2] = 0xCA;
					backup_massage.XOR = XOR_Cal((uint8_t *)(&(backup_massage.TYPE)), backup_massage.LEN+6);
					backup_massage.END = 0xCA;
				}

				/* send data */
				status = send_data_status | single_data_status;
				if( status == 0 )
				{
					{
						uint8_t temp = 0;
						
						memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);
						/* 准备发送数据管理块 */
						send_data_env_init();
						
						whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );

						/* 获取:包封装的答题器->数据长度 */
						rf_var.tx_len = backup_massage.LEN;

						/* 获取：包封装的答题器->数据内容 */
						memcpy(rf_var.tx_buf, (uint8_t *)(backup_massage.DATA), backup_massage.LEN);

						/* 发送前导帧 */
						nrf_transmit_start( &temp, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,
								SEND_PRE_DELAY100US, SEND_PRE_TABLE,PACKAGE_NUM_ADD);
						/* 发送数据帧 */
						nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL,
								SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE,PACKAGE_NUM_ADD);
						/* 启动发送数据状态机 */
						change_clicker_send_data_status( SEND_DATA1_STATUS );
						/* 清除心跳包定时时间 */
						sw_clear_timer(&systick_package_timer);
					}

					/* return data */	
					cJSON_AddNumberToObject(root, "result", 0 );			
				}
				else
				{
					cJSON_AddNumberToObject(root, "result", 1 );
				}

				/* 打印返回 */
				printf("%s",cJSON_Print(root));
				cJSON_Delete(root);
			}

			/* get_device_no */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"get_device_no",13) == 0)
			{
				cJSON *root;
				
				/* 填充内容 */
				root = cJSON_CreateObject();
				cJSON_AddNumberToObject(root, "no", *(uint32_t *)(revicer.uid) );
				
				/* 打印返回 */
				printf("%s", cJSON_Print(root));
				cJSON_Delete(root);
			}

			/* getlist */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"getlist",7) == 0)
			{
				cJSON *root;
				uint8_t i = 0;
				uint8_t is_use_pos = 0,is_online_pos = 0;
				/* 填充内容 */
				root = cJSON_CreateObject();
				
				for( i = 0; (i<MAX_WHITE_LEN); i++)
				{
					is_use_pos = get_index_of_white_list_pos_status(SEND_DATA1_SUM_TABLE,i);
					if(is_use_pos == 1)
					{
						is_online_pos = get_index_of_white_list_pos_status(CLICKER_ANSWER_TABLE,i);
						if(is_online_pos == 1)
						{
							uint8_t answer_len = 0;
							uint8_t answer_num = 0;
							//char    str[20];
							cJSON_AddNumberToObject(root, "cardId", *(uint32_t *)(wl.uids[i].uid) );
							cJSON_AddStringToObject(root, "uptime",(char *) ClickerAnswerTime[i] );
							//answer_num = ClickerAnswerTime[i][9];
							//answer_len = 10 + answer_num*2;
						}
					}
				
				}
				
				/* 打印返回 */
				printf("%s", cJSON_Print(root));
				cJSON_Delete(root);
			}
		}

		if(json)
		{
			cJSON_Delete(json);
			revice_json_count--;
			memset(uart_irq_revice_massage[revice_json_read_index],0,300);
			revice_json_read_index = (revice_json_read_index + 1) % 3;
		}
	}
}
