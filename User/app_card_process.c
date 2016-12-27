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
#include "pos_handle_layer.h"
#include "rc500_handle_layer.h"
#include "app_card_process.h"

extern WhiteList_Typedef wl;
extern __IO uint32_t PowerOnTime;
Process_tcb_Typedef Card_process;

static Uart_MessageTypeDef card_message;
static uint8_t card_process_status = 0, wtrte_flash_ok = 0;
static uint8_t uid_pos = 0xFF, card_message_err = 0;
static uint8_t find_card_ok = 0;
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
	//printf("<%s> cmd_process_status = %d \r\n",__func__,card_process_status);
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
		uint8_t is_white_list_uid = 0,ndef_xor = 0;
		if( FindICCard() == MI_OK )
		{
			if(find_card_ok == 1)
			{
				sw_clear_timer(&card_second_find_timer);
				return;
			}
			if(find_card_ok == 2)
			{
				find_card_ok = 0;
			}

			/* 配对指令 */
			if( wl.match_status == ON )
			{
				is_white_list_uid = add_uid_to_white_list(g_cSNR+4,&uid_pos);

				if(is_white_list_uid != OPERATION_ERR)
				{
					card_message_err = 1;
					if( Card_process.cmd_type == 0x28 )
					{
						memcpy(NDEF_DataWrite+7,Card_process.studentid,20);
					}
					ndef_xor           = XOR_Cal(NDEF_DataWrite+1,26);
					NDEF_DataWrite[6]  = uid_pos;
					NDEF_DataWrite[27] = ndef_xor;

					if( FindICCardAndUpdateData() == MI_OK )
					{
						wtrte_flash_ok = 1;
					}
				}
				else
				{
					card_message_err = 2;
					wtrte_flash_ok = 1;
				}
			}
	
      /* 考勤指令 */
			if( wl.attendance_sttaus == ON )
			{
				is_white_list_uid = search_uid_in_white_list(g_cSNR+4,&uid_pos);
				if(is_white_list_uid == OPERATION_ERR)
				{
					uid_pos = 0xFF;
				}
				card_message_err = 1;
				wtrte_flash_ok = 1;
			}

			if( wtrte_flash_ok == 1 )
			{
				rf_set_card_status(2);
			}
		}

		/* 命令卡进入休眠状态 */
		PcdHalt();
		return;
	}

	if( card_current_status == 2 )
	{
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
			memcpy(card_message.SIGN,Card_process.uid,4);
			card_message.LEN     = 25;
			memset(card_message.DATA,0x00,25);
			card_message.DATA[0] = uid_pos;
			memcpy(card_message.DATA+1,g_cSNR+4,4);
			memcpy(card_message.DATA+5,NDEF_DataRead+7,20);
			if( Card_process.cmd_type == 0x25 )
			{
				memcpy(card_message.DATA+5,NDEF_DataWrite+7,20);
			}
			card_message.XOR = XOR_Cal(&card_message.TYPE,31);
			card_message.END  = 0xCA;	
		}

		if( card_message_err == 2 )
		{
			App_returnErr(&card_message,Card_process.cmd_type,0xFD);
		}

		if(card_message_err != 0)
		{
			/* 执行完的指令存入发送缓存 */
			if( wtrte_flash_ok == 1 )
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&card_message);
					memset(NDEF_DataRead,00,50);
				}
			}
		}

		/* 打开蜂鸣器 */
		if(wtrte_flash_ok == 1)
		{
			if( card_message_err != 2 )
			{
				BEEP_EN();
			}
		}
		rf_set_card_status(3);
	}

	if( card_current_status == 4 )
	{
		/* 关闭蜂鸣器 */
		BEEP_DISEN();

		if(wl.match_status == ON)
		{
			if(Card_process.match_single == 1)
			{
				wl.match_status = OFF;
				rf_set_card_status(0);
				return;
			}
		}
		rf_set_card_status(1);
		find_card_ok = 1;

		return;
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
	sw_create_timer(&card_buzzer_timer    , 200, 3, 4,&(card_process_status), NULL);
	sw_create_timer(&card_second_find_timer, 20, 1, 2,&(find_card_ok), NULL);
}
