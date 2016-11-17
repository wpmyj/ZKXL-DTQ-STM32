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

uint8_t app_card_status = 0;

extern WhiteList_Typedef wl;
extern uint8_t card_cmd_type ;
extern uint8_t uart_rf_cmd_sign[4],uart_card_cmd_sign[4];

void Buzze_Control(void);
void write_rf_config(uint8_t upos, uint8_t ndef_xor, uint8_t *flg);

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
	Uart_MessageTypeDef card_message;
	uint8_t is_white_list_uid = 0,uid_p = 0,ndef_xor = 0;
	uint8_t cmd_process_status = 0;
	uint8_t wtrte_flash_ok = 0;

	if((delay_nms == 0)&&((wl.attendance_sttaus == ON) || wl.match_status == ON))
	{
		delay_nms = 200;
		if(FindICCard() == MI_OK)
		{
			/* 处理数据 */
			if((wl.attendance_sttaus == ON) || (wl.match_status == ON))
			{
				is_white_list_uid = add_uid_to_white_list(g_cSNR+5,&uid_p);
				NDEF_DataWrite[6] = uid_p;
				ndef_xor          = XOR_Cal(NDEF_DataWrite+1,6);
				NDEF_DataWrite[7] = ndef_xor;
				
				if(is_white_list_uid != OPERATION_ERR)
				{
          // OK
					cmd_process_status = 1;
				}
				else
				{
					// Err 0x29 E3
					cmd_process_status = 2;
					App_returnErr(&card_message,0x26,0xFD);
				}
			}
			else
			{
				cmd_process_status = 1;
			}

			if(cmd_process_status == 1)
			{
				/* 封装协议  */
				{
					card_message.HEADER = 0x5C;
					switch(card_cmd_type)
					{
						case 0x25: card_message.TYPE   = 0x26; break;
						case 0x28: card_message.TYPE   = 0x29; break;
						default:                               break;
					}
					memcpy(card_message.SIGN,uart_card_cmd_sign,4);
					card_message.LEN     = 0x05;
					card_message.DATA[0] = uid_p;
					memcpy(card_message.DATA+1,g_cSNR+5,4);
					card_message.XOR = XOR_Cal(&card_message.TYPE,11);
					card_message.END  = 0xCA;
				}
			}

			if(is_white_list_uid != OPERATION_ERR)
			{
				if(wl.match_status == ON)
				{
					wtrte_flash_ok = 1;
					time_for_buzzer_on = 10;
					time_for_buzzer_off = 300;
				}
				
				if(wl.attendance_sttaus == ON)
				{
					//写入配对时将UID传给答题器
					write_rf_config(uid_p,ndef_xor,&wtrte_flash_ok);
				}
			}

			if(cmd_process_status != 0)
			{
				/* 执行完的指令存入发送缓存 */
				if( wtrte_flash_ok == 1 )
				{
					if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
					{
						serial_ringbuffer_write_data(SEND_RINGBUFFER,&card_message);
					}
				}
			}
			PcdHalt();
		}
	}
	Buzze_Control();
}


/*******************************************************************************
  * @brief  Initialize the Gpio port for system
  * @param  None
  * @retval None
*******************************************************************************/
void Buzze_Control(void)
{
	if(time_for_buzzer_on == 1)
	{
		BEEP_EN();
		time_for_buzzer_on = 0;
	}
	if(time_for_buzzer_off == 0)
	{
		BEEP_DISEN();
	}
}

/*******************************************************************************
  * @brief  write_RF_config
  * @param  None
  * @retval None
*******************************************************************************/
void write_rf_config(uint8_t upos, uint8_t ndef_xor, uint8_t *flg)
{
	if(SelectApplication() == MI_OK)		//选择应用
	{
		if(WriteNDEFfile((uint8_t *)&NDEF_DataWrite) == MI_OK)		  //写入NDEF文件
		{
			if(ReadNDEFfile(NDEF_DataRead, &NDEF_Len) == MI_OK)			//读出验证
			{
				if((NDEF_DataRead[6] == upos) && (NDEF_DataRead[7] == ndef_xor))
				{
					time_for_buzzer_on = 10;
					time_for_buzzer_off = 300;
					*flg = 1;
				}
			}
		}
		/* 去除选择 */
		Deselect();	
	}
}
