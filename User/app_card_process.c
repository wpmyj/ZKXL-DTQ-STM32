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

static uint8_t cmd_process_status = 0;
static uint8_t len;
Uart_MessageTypeDef card_message;
uint8_t is_white_list_uid = 0, uid_pos = 0xFF,ndef_xor = 0;
Process_tcb_Typedef Card_process;

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
	cmd_process_status = new_status;
	//printf("<%s> cmd_process_status = %d \r\n",__func__,cmd_process_status);
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
	return cmd_process_status ;
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
		uid_pos  = 0xFF;
		ndef_xor = 0;
		/* 打开13.56M天线 */
		PcdAntennaOn();
		memset(g_cardType, 0, 40);
		/* reqA指令 :请求A卡，返回卡类型，不同类型卡对应不同的UID长度 */
		if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)
		{
			if( (g_cardType[0] & 0x40) == 0x40)
			{	uid_len = 8;	}
			else
			{	uid_len = 4;	}
			rf_set_card_status(2);
		}
		return;
	}

	if( card_current_status == 2 )
	{
		/* 防碰撞1 */
		if(PcdAnticoll(PICC_ANTICOLL1, g_cSNR) == MI_OK )
			rf_set_card_status(3);
		else
			rf_set_card_status(1);
		return;
	}

	if( card_current_status == 3 )
	{
		/* 选卡1 */
		memset(respon, 0, BUF_LEN);
		if (MI_OK == PcdSelect1(g_cSNR, respon, &len))
		{
			if((uid_len == 8) && ((respon[0] & 0x04) == 0x04))
				rf_set_card_status(4);
			else if(uid_len == 4)
				rf_set_card_status(6);
		}
		else
			rf_set_card_status(1);

		return;
	}

	if( card_current_status == 4 )
	{
		/* 防碰撞2 */
		memset(respon, 0, BUF_LEN);
		if(MI_OK == PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]))
			rf_set_card_status(5);
		else
			rf_set_card_status(1);
		return;
	}

	if( card_current_status == 5 )
	{
		/* 选卡2 */
		if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
		{
				rf_set_card_status(6);
		}
		else
			rf_set_card_status(1);
		return;
	}

	if( card_current_status == 6 )
	{
		/* 寻卡成功,选择应用 */
		if(SelectApplication() == MI_OK)
			rf_set_card_status(7);
		else
			rf_set_card_status(1);
		return;
	}

	if( card_current_status == 7 )
	{
		/* 读取数据 */
		if(ReadNDEFfile(NDEF_DataWrite, &NDEF_Len) == MI_OK)
		{
			if(wl.attendance_sttaus == ON)
				rf_set_card_status(10);
			if(wl.match_status == ON)
				rf_set_card_status(8);
		}
		else
			rf_set_card_status(1);
		return;
	}

	if( card_current_status == 8 )
	{
		/* 操作白名单,分配序号 */
		if(wl.match_status == ON)
		{
			is_white_list_uid = add_uid_to_white_list(g_cSNR+4,&uid_pos);
			if( Card_process.cmd_type == 0x28 )
			{
			  memcpy(NDEF_DataWrite+7,Card_process.studentid,20);
			}
			ndef_xor           = XOR_Cal(NDEF_DataWrite+1,26);
			NDEF_DataWrite[6]  = uid_pos;
			NDEF_DataWrite[27] = ndef_xor;
		}

		if(is_white_list_uid != OPERATION_ERR)
		{
			// OK
			rf_set_card_status(9);
		}
		else
		{
			// Err FD
			App_returnErr(&card_message,Card_process.cmd_type,0xFD);
			rf_set_card_status(11);
		}
		return;
	}

	if( card_current_status == 9 )
	{
		/* 写入NDEF文件 */
		if(WriteNDEFfile((uint8_t *)&NDEF_DataWrite) == MI_OK)
		{
			/* 读出验证 */
			if(ReadNDEFfile(NDEF_DataRead, &NDEF_Len) == MI_OK)
			{
				if((NDEF_DataRead[6] == uid_pos) && (NDEF_DataRead[27] == ndef_xor))
				{
					rf_set_card_status(10);
				}
				else
				{
					rf_set_card_status(1);
				}
			}
			else
			{
				rf_set_card_status(1);
			}
		}
		else
		{
			rf_set_card_status(1);
		}

		if(wl.match_status == ON)
		{
			if(Card_process.match_single == 1)
				wl.match_status = OFF;
		}
		return;
	}

	if( card_current_status == 10 )
	{
		/* 产生返回数据 */
		switch(Card_process.cmd_type)
		{
			case 0x25: card_message.TYPE   = 0x26; break;
			case 0x28: card_message.TYPE   = 0x29; break;
			case 0x41: card_message.TYPE   = 0x42; break;
			default:                               break;
		}
		card_message.HEADER = 0x5C;
		memcpy(card_message.SIGN,Card_process.uid,4);
		card_message.LEN     = 25;
		memset(card_message.DATA,0x00,25);
		card_message.DATA[0] = uid_pos;
		memcpy(card_message.DATA+1,g_cSNR+4,4);
		memcpy(card_message.DATA+5,NDEF_DataRead+7,20);
		card_message.XOR = XOR_Cal(&card_message.TYPE,31);
		card_message.END  = 0xCA;
		rf_set_card_status(11);
	}

	if( card_current_status == 11 )
	{
		/* 返回数据 */
		if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			serial_ringbuffer_write_data(SEND_RINGBUFFER,&card_message);
			memset(NDEF_DataRead,00,50);
		}
		if(is_white_list_uid != OPERATION_ERR)
		{
			rf_set_card_status(12);
		}
		else
		{
			rf_set_card_status(14);
		}
	}

	if( card_current_status == 12 )
	{
		/* 发送中断信号 */
		SendInterrupt();
		rf_set_card_status(13);
		/* 打开蜂鸣器 */
		BEEP_EN();
	}

	if( card_current_status == 14 )
	{
		/* 去除选择 */
		Deselect();
		/* 命令卡进入休眠状态 */
		PcdHalt();
		/* 关闭蜂鸣器 */
		BEEP_DISEN();
		if(Card_process.match_single == 1)
		{
			rf_set_card_status(0);
			Card_process.match_single = 0;
		}
		else
			rf_set_card_status(1);
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
	sw_create_timer(&card_buzzer_timer , 300, 13, 14,&(cmd_process_status), NULL);
}
