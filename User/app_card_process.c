/**
  ******************************************************************************
  * @file   	app_card_process.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_serial_cmd_process.h"
#include "app_card_process.h"

//#define SHOW_CARD_PROCESS_TIME
extern uint8_t g_cSNR[10];	
extern WhiteList_Typedef wl;
extern Revicer_Typedef   revicer;
Process_tcb_Typedef Card_process;

#ifdef SHOW_CARD_PROCESS_TIME
extern __IO uint32_t     PowerOnTime;
uint32_t StartTime,EndTime;
#endif

static Uart_MessageTypeDef card_message;
static uint8_t card_process_status = 0;
static uint8_t is_white_list_uid = 0, ndef_wr_xor = 0, ndef_rd_xor = 0xFF;
/* 返回卡类型 */
uint8_t g_cardType[40];	
uint8_t respon[BUF_LEN + 20];	
uint8_t NDEF_DataWrite[30];
uint8_t NDEF_DataRead[0xFF];
uint16_t NDEF_Len               = 0;
static uint8_t g_uid_len        = 0;
static uint8_t wtrte_flash_ok   = 0;
static uint8_t read_uid_pos     = 0xFF;
static uint8_t write_uid_pos    = 0xFF;
static uint8_t card_message_err = 0;
static uint8_t find_card_ok     = 0;
/******************************************************************************
  Function:rf_set_card_status
  Description:
		修改systick的状态
  Input :
		rf_status: systick的新状态
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_set_card_status(uint8_t new_status)
{
	card_process_status = new_status;
}

/******************************************************************************
  Function:rf_get_card_status
  Description:
		获取systick的状态
  Input :
  Output:systick的新状态
  Return:
  Others:None
******************************************************************************/
uint8_t rf_get_card_status(void)
{
	return card_process_status ;
}

/******************************************************************************
  Function:App_card_process
  Description:
		App MI Card 轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_card_process(void)
{
	/* 获取当前状态 */
	uint8_t card_current_status = 0;
	
	card_current_status = rf_get_card_status();

	if( card_current_status == 1 )
	{
		uint8_t status = 0;
		#ifdef SHOW_CARD_PROCESS_TIME
		StartTime = PowerOnTime;
		#endif

		PcdAntennaOn();
	  MRC500_DEBUG_START("PcdRequest \r\n");
		memset(g_cardType, 0, 40);
		/* reqA指令 :请求A卡，返回卡类型，不同类型卡对应不同的UID长度 */
		status = PcdRequest(PICC_REQIDL,g_cardType);
	  MRC500_DEBUG_END();
		if( status == MI_OK )
		{
			DEBUG_CARD_DEBUG_LOG("PcdRequest status = %d\r\n",status);
			if(find_card_ok == 1)
			{
				sw_clear_timer(&card_second_find_timer);
				return;
			}
			if(find_card_ok == 2)
			{
				find_card_ok = 0;
			}
		
			if( (g_cardType[0] & 0x40) == 0x40)
			{	
				g_uid_len = 8;	
			}
			else
			{	
				g_uid_len = 4;
			}
			DEBUG_CARD_DEBUG_LOG("uid len = %d\r\n",g_uid_len);
		}
		else
		{
			DEBUG_CARD_DEBUG_LOG("PcdRequest status = %d\r\n",status);
			return;
		}
		/* 防碰撞1 */
		status = PcdAnticoll(PICC_ANTICOLL1, g_cSNR);
		if( status != MI_OK )
		{
			return;
		}

		/* 选卡1 */
		memset(respon, 0, 10);
		status = PcdSelect1(g_cSNR, respon);
		if( status == MI_OK )
		{
			if((g_uid_len == 8) && ((respon[0] & 0x04) == 0x04))
			{
				DEBUG_CARD_DEBUG_LOG("PcdSelect1 status = %d\r\n",status);
				//MRC500_DEBUG_START("PICC_ANTICOLL2 \r\n");
				status = PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]);
				//MRC500_DEBUG_END();
				if( status != MI_OK)
				{
					return;
				}
				status = PcdSelect2(&g_cSNR[4], respon);
				if( status == MI_OK)
				{
					rf_set_card_status(2);
					DEBUG_CARD_DEBUG_LOG("PcdSelect2 status = %d\r\n",status);
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			return;
		}
		#ifdef SHOW_CARD_PROCESS_TIME
		EndTime = PowerOnTime - StartTime;
		printf("UseTime:PcdSelect2 = %d \r\n",EndTime);
		#endif
	}

	if( card_current_status == 2 )
	{
		uint8_t status = 0;
		/*选择应用*/
		status = SelectApplication();
		DEBUG_CARD_DEBUG_LOG("SelectApplication status = %d\r\n",status);
		if( status != MI_OK )
		{
			mfrc500_init();
			rf_set_card_status(1);
			return;
		}
		#ifdef SHOW_CARD_PROCESS_TIME
		EndTime = PowerOnTime - StartTime;
		printf("UseTime:SelectApplication = %d \r\n",EndTime);
		#endif
		/* 考勤指令 */
		if( wl.attendance_sttaus == ON )
		{
			status = ReadNDEFfile(NDEF_DataRead, &NDEF_Len);
			DEBUG_CARD_DEBUG_LOG("ReadNDEFfile status = %d\r\n",status);
			if( status != MI_OK )
			{
				memset(NDEF_DataRead,00,28);
				mfrc500_init();
				rf_set_card_status(1);
				return;
			}
			else
			{
				ndef_wr_xor = XOR_Cal(NDEF_DataRead+1,26);
				if(NDEF_DataRead[27] != ndef_wr_xor)
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<28;i++)
						DEBUG_CARD_DATA_LOG("%02x ",NDEF_DataRead[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					memset(NDEF_DataRead,00,28);
					DEBUG_CARD_DATA_LOG("NDEF_DataRead XOR ERROR!\r\n");
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<28;i++)
						DEBUG_CARD_DATA_LOG("%02x ",NDEF_DataRead[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					//memset(NDEF_DataRead,00,28);
					DEBUG_CARD_DATA_LOG("NDEF_DataRead Clear!\r\n");
				}
			}

			is_white_list_uid = search_uid_in_white_list(g_cSNR+4,&read_uid_pos);
			if(is_white_list_uid == OPERATION_ERR)
			{
				read_uid_pos = 0xFF;
			}
			card_message_err = 1;
			wtrte_flash_ok = 1;
			#ifdef SHOW_CARD_PROCESS_TIME
			EndTime = PowerOnTime - StartTime;
			printf("UseTime:ReadNDEFfile = %d \r\n",EndTime);
			#endif
			rf_set_card_status(3);
		}

		/* 配对指令 */
		if( wl.match_status == ON )
		{
			is_white_list_uid = add_uid_to_white_list(g_cSNR+4,&write_uid_pos);

			if(is_white_list_uid != OPERATION_ERR)
			{
				card_message_err  = 1;
				NDEF_DataWrite[0]  = 0;
				NDEF_DataWrite[1]  = 0x1C;
				memcpy(NDEF_DataWrite+2,revicer.uid,4);
				NDEF_DataWrite[6]  = write_uid_pos;
				if( Card_process.cmd_type == 0x28 )
				{
					memcpy(NDEF_DataWrite+7,Card_process.studentid,20);
				}
				if( Card_process.cmd_type == 0x41 )
				{
					memset(NDEF_DataWrite+7, 0x00, 20);
				}
				ndef_wr_xor        = XOR_Cal(NDEF_DataWrite+1,26);
				NDEF_DataWrite[27] = ndef_wr_xor;

				/* 重新写入数据检测 */
				status = ReadNDEFfile(NDEF_DataRead, &NDEF_Len);
				DEBUG_CARD_DEBUG_LOG("ReadNDEFfile0 status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:ReadNDEFfile0 = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(NDEF_DataRead ,00,28);
					memset(NDEF_DataWrite,00,28);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					uint8_t write_flag = 0;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<28;i++)
						DEBUG_CARD_DATA_LOG("%02x ",NDEF_DataRead[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					ndef_rd_xor        = XOR_Cal(NDEF_DataRead+1,26);

					/* 完全比较，否则存在异或校验的巧合*/
					for(i=6;i<28;i++)
					{
						if(NDEF_DataRead[i]  != NDEF_DataWrite[i])
							write_flag = 1;
					}

					if( write_flag )   
					{
						/* 需要重新写入数据 */
					}
					else
					{
						/* 无需写入新数据,直接进入下一流程 */
						card_message_err = 1;
						wtrte_flash_ok = 1;
						rf_set_card_status(3);
						return;
					}
				}

				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:WriteNDEFfile0 = %d \r\n",EndTime);
				#endif
				status = WriteNDEFfile((uint8_t *)&NDEF_DataWrite);
				DEBUG_CARD_DEBUG_LOG("WriteNDEFfile1 status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:WriteNDEFfile1 = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(NDEF_DataWrite,00,28);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataWrite:");
					for(i=0;i<28;i++)
						DEBUG_CARD_DATA_LOG("%02x ",NDEF_DataWrite[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
				}
				status = ReadNDEFfile(NDEF_DataRead, &NDEF_Len);
				DEBUG_CARD_DEBUG_LOG("ReadNDEFfile status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:ReadNDEFfile = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(NDEF_DataRead ,00,28);
					memset(NDEF_DataWrite,00,28);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<28;i++)
						DEBUG_CARD_DATA_LOG("%02x ",NDEF_DataRead[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					ndef_rd_xor        = XOR_Cal(NDEF_DataRead+1,26);
					if((NDEF_DataRead[6]  != NDEF_DataWrite[6])  || 
						 (NDEF_DataRead[27] != NDEF_DataWrite[27]) || 
					   (NDEF_DataRead[27] != ndef_rd_xor))
					{
						memset(NDEF_DataRead ,00,28);
						memset(NDEF_DataWrite,00,28);
						mfrc500_init();
						rf_set_card_status(1);
						return;
					}
				}

				status = SendInterrupt();
				DEBUG_CARD_DEBUG_LOG("SendInterrupt status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:SendInterrupt = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(NDEF_DataRead,00,28);
					memset(NDEF_DataWrite,00,28);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				wtrte_flash_ok = 1;
			}
			else
			{
				card_message_err = 2;
				wtrte_flash_ok = 1;
			}
			rf_set_card_status(3);
		}
	}

	if( card_current_status == 3 )
	{
		if(wtrte_flash_ok == 1)
		{
			if( card_message_err != 2 )
			{
				#ifdef OPEN_SILENT_MODE
				ledOn(LBLUE);
				#else
				BEEP_EN();
				#endif
				Deselect();
				PcdHalt();
				PcdAntennaOff();
			}
		}

		if( card_message_err == 1 )
		{
			card_message.HEADER = 0x5C;
			switch(Card_process.cmd_type)
			{
				case 0x25: card_message.TYPE   = 0x26; break;
				case 0x28: card_message.TYPE   = 0x29; break;
				case 0x41: card_message.TYPE   = 0x42; break;
				default:                               break;
			}
			memcpy(card_message.SIGN,Card_process.sign,4);
			card_message.LEN     = 25;
			memset(card_message.DATA,0x00,25);
			if( wl.attendance_sttaus == ON )
			{
				card_message.DATA[0] = read_uid_pos;
			}
			if( wl.match_status == ON )
			{
				card_message.DATA[0] = write_uid_pos;
			}
			memcpy(card_message.DATA+1,g_cSNR+4,4);
			memcpy(card_message.DATA+5,NDEF_DataRead+7,20);
			card_message.XOR = XOR_Cal(&card_message.TYPE,31);
			card_message.END  = 0xCA;	
		}
		if( card_message_err == 2 )
		{
			memcpy(card_message.SIGN,Card_process.sign,4);
			App_returnErr(&card_message,Card_process.cmd_type,0xFD);
		}

		if(card_message_err != 0)
		{
			if( wtrte_flash_ok == 1 )
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					#ifndef OPEN_CARD_DATA_SHOW 
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&card_message);
					DEBUG_CARD_DATA_LOG("NDEF_DataRead and NDEF_DataWrite Clear!\r\n");
					#endif
				}
			}
		}
		rf_set_card_status(4);
	}

	if( card_current_status == 5 )
	{
		#ifdef OPEN_SILENT_MODE
		ledOff(LBLUE);
		#else
		BEEP_DISEN();
		#endif
		rf_set_card_status(1);
		memset(NDEF_DataRead ,00,28);
		memset(NDEF_DataWrite,00,28);
		if( Card_process.cmd_type == 0x28 )
		{
			memcpy(NDEF_DataWrite+7,Card_process.studentid,20);
			wl.match_status = OFF;
			rf_set_card_status(0);
		}
		find_card_ok = 1;
		#ifdef SHOW_CARD_PROCESS_TIME
		EndTime = PowerOnTime - StartTime;
		printf("UseTime:SecondFindStart = %d \r\n",EndTime);
		#endif
	}
}

/******************************************************************************
  Function:systick_timer_init
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void card_timer_init( void )
{
	sw_create_timer(&card_buzzer_timer    , 150, 4, 5,&(card_process_status), NULL);
	sw_create_timer(&card_second_find_timer,100, 1, 2,&(find_card_ok), NULL);
}
