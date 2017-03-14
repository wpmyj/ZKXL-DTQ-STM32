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

/* uid */
extern Revicer_Typedef   revicer;
extern RTC_timer_Typedef system_rtc_timer;
extern uint16_t list_tcb_table[16][8];

extern nrf_communication_t nrf_data;
extern WhiteList_Typedef wl;
extern uint8_t ClickerAnswerTime[MAX_WHITE_LEN][CLICKER_TIMER_STR_LEN];
extern uint8_t ClickerAnswerData[MAX_WHITE_LEN][CLICKER_ANSWER_STR_LEN];

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
			{
				char *out;
				cJSON *root;
				uint8_t result = 0;

				root = cJSON_CreateObject();

				result = initialize_white_list();

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
			
			/* bind */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"bind",4) == 0)
			{
				char *out;
				cJSON *root;
				uint8_t card_is_busy = 0;
				
				root = cJSON_CreateObject();
				card_is_busy = rf_get_card_status();
				
				if(strncmp(cJSON_GetObjectItem(json, "cmd")->valuestring,"start",5) == 0)
				{
					wl.match_status = ON;
					rf_set_card_status(1);
				}

				if(strncmp(cJSON_GetObjectItem(json, "cmd")->valuestring,"stop",4) == 0)
				{
					wl.match_status = OFF;
					rf_set_card_status(0);
				}

				cJSON_AddStringToObject(root, "result", "0" );
				
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

			/* start */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"start",5) == 0)
			{
				char *out;
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
						case 2 : answer= 0xBF; break;
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
						backup_massage.DATA[9+i] = 1+i/2;
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
						memset(ClickerAnswerTime,0,MAX_WHITE_LEN*CLICKER_TIMER_STR_LEN);
						memset(ClickerAnswerData,0,MAX_WHITE_LEN*CLICKER_ANSWER_STR_LEN);
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

			/* get_device_no */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"get_device_no",13) == 0)
			{
				char *out,str[20];
				cJSON *root;
				
				/* 填充内容 */
				root = cJSON_CreateObject();
				memset(str,0,20);
				sprintf(str, "%010u" , *(uint32_t *)(revicer.uid));
				cJSON_AddStringToObject(root, "no", str );
				
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

			/* getlist */
			if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"getlist",7) == 0)
			{
				char *out;
				cJSON *cards,*card;
				uint8_t revicer_answer_num = 0, print_answer_num = 0;
				uint8_t i = 0, j = 0;
				uint8_t is_use_pos = 0,is_online_pos = 0;
				
				/* 获取提交答案的答题器的个数 */
				for( i = 0; i<MAX_WHITE_LEN; i++)
				{
					is_use_pos = get_index_of_white_list_pos_status(SEND_DATA1_SUM_TABLE,i);
					if(is_use_pos == 1)
					{
						is_online_pos = get_index_of_white_list_pos_status(CLICKER_ANSWER_TABLE,i);
						if(is_online_pos == 1)
						{
							revicer_answer_num++;
							//printf("revicer_answer_num = %d\r\n",revicer_answer_num);
						}
					}
				}

				/* 填充内容 */
				cards = cJSON_CreateArray();
				printf("[");
				for( i = 0; i<MAX_WHITE_LEN; i++)
				{
					is_use_pos = get_index_of_white_list_pos_status(SEND_DATA1_SUM_TABLE,i);
					if(is_use_pos == 1)
					{
						is_online_pos = get_index_of_white_list_pos_status(CLICKER_ANSWER_TABLE,i);
						if(is_online_pos == 1)
						{
							char str[20];
							const char RoleTypeName[5]   = {0xD1, 0xA7, 0xC9, 0xFA} ;             // 学生
							const char DeviceTypeName[7] = {0xBD, 0xBB, 0xBB, 0xA5, 0xBF, 0xA8} ; // 交互卡
								
							cJSON_AddItemToArray(cards, card = cJSON_CreateObject());
							
							cJSON_AddStringToObject(card, "RoleTypeName", RoleTypeName );
					    cJSON_AddStringToObject(card, "DeviceTypeName", DeviceTypeName );
		
							memset(str,0,20);
							sprintf(str, "%010u" , *(uint32_t *)( wl.uids[i].uid));
							cJSON_AddStringToObject(card, "cardId", str );
							
							cJSON_AddStringToObject(card, "uptime",(char *) ClickerAnswerTime[i] );

							for(j=0;j<ClickerAnswerData[i][0]*2;)
							{
								const char AnswerTypeName[3][7] = {
									{0xB5, 0xA5, 0xD1, 0xA1, 0xCC, 0xE2 }, // 单选题
									{0xB6, 0xE0, 0xD1, 0xA1, 0xCC, 0xE2 }, // 多选题
									{0xC5, 0xD0, 0xB6, 0xCF, 0xCC, 0xE2 }  // 判断题
								};

								char item[10],str[20];
								char *pdata = str;

								memset(item,0,10);
								memset(str, 0,20);
								switch(ClickerAnswerData[i][2+j]&0xC0)
								{
									
									case 0x40:
										cJSON_AddStringToObject(card, "AnswerTypeName", AnswerTypeName[0]);
									break;
									case 0x80:
										cJSON_AddStringToObject(card, "AnswerTypeName", AnswerTypeName[1]);
									break;
									case 0xC0:
										cJSON_AddStringToObject(card, "AnswerTypeName", AnswerTypeName[2]);
									break;
									default:break;
								}

								sprintf(item, "q%d" , ClickerAnswerData[i][1+j]);

								switch(ClickerAnswerData[i][2+j]&0xC0)
								{
									case 0x40:
									{
											uint8_t answer = ClickerAnswerData[i][2+j]&0x3F;
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
											pdata = pdata + 1;
									}
									break;

									case 0x80:
									{
										if((ClickerAnswerData[i][2+j]&0x01) == 0x01)
										{
											*pdata = 'A';
											pdata = pdata + 1;
										}

										if((ClickerAnswerData[i][2+j]&0x02) == 0x02)
										{
											*pdata = 'B';
											pdata = pdata + 1;
										}

										if((ClickerAnswerData[i][2+j]&0x04) == 0x04)
										{
											*pdata = 'C';
											pdata = pdata + 1;
										}

										if((ClickerAnswerData[i][2+j]&0x08) == 0x08)
										{
											*pdata = 'D';
											pdata = pdata + 1;
										}

										if((ClickerAnswerData[i][2+j]&0x10) == 0x10)
										{
											*pdata = 'E';
											pdata = pdata + 1;
										}
										if((ClickerAnswerData[i][2+j]&0x20) == 0x20)
										{
											*pdata = 'F';
											pdata = pdata + 1;
										}
									}
									break;

									case 0xC0:
									{
										switch(ClickerAnswerData[i][2+j]&0x3F)
										{
											case 0x01: // 对
											{
												*pdata = 0xB6; pdata = pdata + 1;
												*pdata = 0xD4; pdata = pdata + 1;
											}
											break;
											case 0x02: // 错
											{
												*pdata = 0xB4; pdata = pdata + 1;
												*pdata = 0xED; pdata = pdata + 1;
											}
											break;
											default: break;
										}
									}
									break;
									
									default:
										break;
								}
								cJSON_AddStringToObject(card, item, str );
								j = j + 2;
							}
							/* 打印返回 */
							out = cJSON_Print(card);
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
							print_answer_num++;
							//printf("print_answer_num = %d\r\n",print_answer_num);
							if((print_answer_num == revicer_answer_num) && (print_answer_num != 0))
								printf("%s", out);
							else
								printf("%s,", out);
							free(out);
							
							clear_index_of_white_list_pos(CLICKER_ANSWER_TABLE,i);
						}
					}
				}
				printf("]");
				cJSON_Delete(cards);
			}
		}

		{
			cJSON_Delete(json);
			revice_json_count--;
			memset(uart_irq_revice_massage[revice_json_read_index],0,300);
			revice_json_read_index = (revice_json_read_index + 1) % 3;
		}
	}
}
