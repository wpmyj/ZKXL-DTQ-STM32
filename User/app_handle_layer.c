/**
  ******************************************************************************
  * @file   	app_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "whitelist.h"

/* Private variables ---------------------------------------------------------*/

extern	uint8_t   sign_buffer[4];
				uint8_t		Buf_CtrPosToApp[250];		// pos下发指令缓冲
				uint8_t		Buf_AppToCtrPos[250];		// 应用层上报指令缓冲区

				uint16_t	Length_CtrPosToApp;				      // pos下发指令长度
				uint16_t	Length_AppToCtrPos;				      // 应用层上报指令长度
				uint8_t   whitelist_print_index = 0;

/* Private functions ---------------------------------------------------------*/
uint8_t FindICCard(void);
bool initialize_white_list( void );
bool delete_uid_from_white_list(uint8_t *g_uid);
bool add_uid_to_white_list(uint8_t *g_uid, uint8_t *position);
bool search_uid_in_white_list(uint8_t *g_uid, uint8_t *position);
void clear_white_list_tx_flag(void);
bool uidcmp(uint8_t *uid1, uint8_t *uid2);
static void Buzze_Control(void);
void add_sign_to_buffer(uint16_t *LenPos, uint8_t sign[]);


void App_returnInsertState(uint8_t sw1, uint8_t sw2);
void App_returnDeleteState(uint8_t sw1, uint8_t sw2);
void App_returnInitializeState(void);
void App_returnWhiteListSwitchState(Switch_State SWS);
void App_returnAttendanceSwitchState(Switch_State SWS);
void App_returnMatchSwitchState(Switch_State SWS);
void App_returnErr(uint8_t cmd_type, uint8_t err_type);

void    App_serial_transport_to_nrf51822(void);
void    App_return_data_to_clickers(void);
void    App_return_data_to_topic(void);
void    App_return_device_info(void);;
uint8_t App_return_whitelist_data(uint8_t index);

void    App_seirial_cmd_process(void);
void    App_rf_check_process(void);
void    App_card_process(void);

/******************************************************************************
  Function:app_handle_layer
  Description:
		App 轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void app_handle_layer(void)
{
		/* nrf51822 Communication processing process */
		App_rf_check_process();
		
		/* serial cmd processing process */
		App_seirial_cmd_process();
		
		/* MI Card processing process */
		App_card_process();
}

/******************************************************************************
  Function:App_seirial_cmd_process
  Description:
		App 串口指令处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_seirial_cmd_process(void)
{
	uint8_t temp_count = 0;
	
	switch(flag_App_or_Ctr)
	{	
		/* send clickers's data to topic */
		case 0x02:		   
				{
						App_return_data_to_topic();
						memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
						rf_var.rx_len = 0x00;
						flag_App_or_Ctr = 0x00;
						App_to_CtrPosReq =true;	
				}			
				break;
				
		/* 上传下发成功信息 */	
		case 0x03:		
				Length_AppToCtrPos = 0x00;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x5C;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x11;
				add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x04;
				memcpy(&Buf_AppToCtrPos[7], &rf_var.rx_buf[1], 4);
				Buf_AppToCtrPos[11] = XOR_Cal(&Buf_AppToCtrPos[1], 10);
				Buf_AppToCtrPos[12] = 0xCA;
				Length_AppToCtrPos = 13;
				App_to_CtrPosReq =true;
				flag_App_or_Ctr = 0x00;
			break;	
		
		case 0x04:		//停止下发指令
				rf_var.flag_txing = false;
				memset(rf_var.tx_buf, 0x00, rf_var.tx_len);
				rf_var.tx_len = 0x00;
				//clear_white_list_tx_flag();
			
				Length_AppToCtrPos = 0x00;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x5C;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x12;
				add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x03;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x00;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = white_on_off;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = white_len;
				Buf_AppToCtrPos[Length_AppToCtrPos++] = XOR_Cal(&Buf_AppToCtrPos[1], 9);
				Buf_AppToCtrPos[Length_AppToCtrPos++] = 0xCA;
				App_to_CtrPosReq =true;
			
				flag_App_or_Ctr = 0x00;
			break;	
		
		/* 添加白名单 */
		case 0x11:		
				while(temp_count != Buf_CtrPosToApp[0])
				{
					if(add_uid_to_white_list(&Buf_CtrPosToApp[4*temp_count + 1], &uid_p))
					{
						temp_count++;
						continue;
					}
					else
						break;
				}
				
				if(temp_count == Buf_CtrPosToApp[0])
					App_returnInsertState(0x00,white_len);
				else
					App_returnInsertState(temp_count + 1,white_len);
				
				flag_App_or_Ctr = 0x00;
			break;
		
		/* 删除白名单 */
		case 0x12:		
				while(temp_count != Buf_CtrPosToApp[0])
				{
					if(delete_uid_from_white_list(&Buf_CtrPosToApp[4*temp_count + 1]))
					{	
						temp_count++;
						continue;
					}
					else
						break;
				}
				
				if(temp_count == Buf_CtrPosToApp[0])
					App_returnDeleteState(0x00,white_len);
				else
					App_returnDeleteState(temp_count + 1,white_len);
				
				flag_App_or_Ctr = 0x00;
		break;
				
		case 0x13:		//初始化白名单
				initialize_white_list();
				App_returnInitializeState();
				flag_App_or_Ctr = 0x00;	
		break;
		
		case 0x14:		//开启白名单
				white_on_off = ON;
				App_returnWhiteListSwitchState(ON);
				flag_App_or_Ctr = 0x00;	
		break;
		
		case 0x15:		//关闭白名单
			white_on_off = OFF;
			App_returnWhiteListSwitchState(OFF);
			flag_App_or_Ctr = 0x00;	
		break;
			
		case 0x16:		//开始考勤
				attendance_on_off = ON;
				App_returnAttendanceSwitchState(ON);
				flag_App_or_Ctr = 0x00;	
		break;
			
		case 0x17:		//停止考勤
				attendance_on_off = OFF;
				App_returnAttendanceSwitchState(OFF);
				flag_App_or_Ctr = 0x00;
			break;
			
		case 0x18:		//开始配对
			match_on_off = ON;
			match_number = 1;
			initialize_white_list();
			App_returnMatchSwitchState(ON);
			flag_App_or_Ctr = 0x00;
		break;
		
		case 0x19:		//停止配对
			match_on_off = OFF;
			App_returnMatchSwitchState(OFF);
			flag_App_or_Ctr = 0x00;
		break;
		
		/* 打印当前白名单 */
		case 0x1a:      
				{		
						whitelist_print_index = 
					         App_return_whitelist_data( whitelist_print_index );
						if( whitelist_print_index < white_len )
						{
							flag_App_or_Ctr = 0x1a;
						}
						else
						{
							flag_App_or_Ctr = 0x00;
							whitelist_print_index = 0;
						}
						App_to_CtrPosReq =true;
				}
				break;
				
		/* 打印设备信息	 */	
		case 0x1b:  
				{
						App_return_device_info();
						flag_App_or_Ctr = 0x00;
						App_to_CtrPosReq =true;
				}
				break;
				
		case 0x0d:		// 指令检验异或错误	
				if(!App_to_CtrPosReq)
				{
					App_returnErr(Buf_CtrPosToApp[1], 0xE0);
					flag_App_or_Ctr = 0x00;
				}
		break;
		
		case 0xfe:		// 命令长度不合法
				if(!App_to_CtrPosReq)
				{
					App_returnErr(Buf_CtrPosToApp[1],0xE1);			// 返回长度错误
					App_to_CtrPosReq = true;										// pos指令请求
					flag_App_or_Ctr = 0x00;											// 应用层空闲
				}
		break;
			
		case 0xff:		// 未定义命令处理
				if(!App_to_CtrPosReq)
				{
					App_returnErr(Buf_CtrPosToApp[1],0xE2);
					App_to_CtrPosReq = true;
					flag_App_or_Ctr = 0x00;
				}		
		break;
		
		default:		// 默认，空闲
				flag_App_or_Ctr = 0x00;
		break;			
	}
	
	/* 将处理结果存入缓存区 */
	if(App_to_CtrPosReq)
	{		
		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			serial_ringbuffer_write_data1(SEND_RINGBUFFER,Buf_AppToCtrPos);
			App_to_CtrPosReq = false;
		}
	}
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
	if((delay_nms == 0)&&((attendance_on_off == ON) || match_on_off == ON))
	{
		delay_nms = 200;							//每秒寻卡5次
		if(FindICCard() == MI_OK)
		{
			if(match_on_off)						//如果是配对开启
			{
				if(search_uid_in_white_list(&g_cSNR[4], &uid_p))
				{
					
					Buf_AppToCtrPos[0] = 0x5C;
					Buf_AppToCtrPos[1] = 0x29;
				  Buf_AppToCtrPos[2] = sign_buffer[0];
				  Buf_AppToCtrPos[3] = sign_buffer[1];
				  Buf_AppToCtrPos[4] = sign_buffer[2];
				  Buf_AppToCtrPos[5] = sign_buffer[3];
					Buf_AppToCtrPos[6] = 0x05;
					memcpy(&Buf_AppToCtrPos[7], &g_cSNR[4],4);
					Buf_AppToCtrPos[11] = uid_p;
					Buf_AppToCtrPos[12] = XOR_Cal(&Buf_AppToCtrPos[1],11);
					Buf_AppToCtrPos[13] = 0xCA;
					Length_AppToCtrPos = 14;
				}
				else if(add_uid_to_white_list(&g_cSNR[4], &uid_p))
				{
					Buf_AppToCtrPos[0] = 0x5C;
					Buf_AppToCtrPos[1] = 0x29;
					Buf_AppToCtrPos[2] = sign_buffer[0];
					Buf_AppToCtrPos[3] = sign_buffer[1];
					Buf_AppToCtrPos[4] = sign_buffer[2];
					Buf_AppToCtrPos[5] = sign_buffer[3];					
					Buf_AppToCtrPos[6] = 0x05;
					memcpy(&Buf_AppToCtrPos[7], &g_cSNR[4],4);
					Buf_AppToCtrPos[11] = uid_p;
					Buf_AppToCtrPos[12] = XOR_Cal(&Buf_AppToCtrPos[1],11);
					Buf_AppToCtrPos[13] = 0xCA;
					Length_AppToCtrPos = 14;
				}
				else
				{
					App_returnErr(0x29,0xE3);
				}
				App_to_CtrPosReq = true;				
			}
			else									//如果是考勤开启
			{
				//串口返回UID
				Buf_AppToCtrPos[0] = 0x5C;
				Buf_AppToCtrPos[1] = 0x26;
				Buf_AppToCtrPos[2] = sign_buffer[0];
				Buf_AppToCtrPos[3] = sign_buffer[1];
				Buf_AppToCtrPos[4] = sign_buffer[2];
				Buf_AppToCtrPos[5] = sign_buffer[3];
				Buf_AppToCtrPos[6] = 0x04;
				memcpy(&Buf_AppToCtrPos[7], &g_cSNR[4],4);
				Buf_AppToCtrPos[11] = XOR_Cal(&Buf_AppToCtrPos[1],10);
				Buf_AppToCtrPos[12] = 0xCA;
				Length_AppToCtrPos = 0x0D;
				App_to_CtrPosReq = true;
			}
			
			//蜂鸣器响300ms
			time_for_buzzer_on = 10;
			time_for_buzzer_off = 300;
			
			//不重复寻卡
			PcdHalt();
		}
	}
	
	if( App_to_CtrPosReq )
	{
		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			serial_ringbuffer_write_data1(SEND_RINGBUFFER,Buf_AppToCtrPos);
			App_to_CtrPosReq = false;
		}
	}
	Buzze_Control();	// 等待蜂鸣器关闭
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_rf_check_process(void)
{
	/* send topic's data to clickers */
	if(flag_App_or_Ctr == 0x01)	
	{			
			//App_serial_transport_to_nrf51822();
			App_return_data_to_clickers();
			flag_App_or_Ctr = 0x00;
	}
	
	/* check rf_var status flag  */
	if(rf_var.flag_tx_ok)
	{
		flag_App_or_Ctr = 0x03;
		rf_var.flag_tx_ok = false;
	}
	
	if(rf_var.flag_rx_ok)
	{
		flag_App_or_Ctr = 0x02;
		rf_var.flag_rx_ok = false;
	}	
}


void App_returnInsertState(uint8_t sw1, uint8_t sw2)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x20;
	add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x03;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = Buf_CtrPosToApp[0];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw1;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw2;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],9);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnDeleteState(uint8_t sw1, uint8_t sw2)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x21;
	add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x03;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = Buf_CtrPosToApp[0];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw1;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw2;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],9);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnInitializeState(void)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x22;
	add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);	//异或结果
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnWhiteListSwitchState(Switch_State SWS)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	if(SWS == ON)
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x23;
	else
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x24;
	add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);	
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}


void App_returnAttendanceSwitchState(Switch_State SWS)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	if(SWS == ON)
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x25;
	else
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x27;
	add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnMatchSwitchState(Switch_State SWS)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	if(SWS == ON)
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x28;
	else
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x2A;
	add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnErr(uint8_t cmd_type, uint8_t err_type)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = cmd_type;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = err_type;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],3);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void Buzze_Control(void)
{
	if(time_for_buzzer_on == 1)//蜂鸣器控制
	{
		BEEP_EN();
		time_for_buzzer_on = 0;
	}
	if(time_for_buzzer_off == 0)
	{
		BEEP_DISEN();
	}
}

/******************************************************************************
  Function:App_serial_transport_to_nrf51822
  Description:
       上位机发送的数据透传给答题器
  Input:None
  Return:
  Others:None
******************************************************************************/
void App_serial_transport_to_nrf51822(void)
{
	  memcpy(Buf_AppToCtrPos, Buf_CtrPosToApp, Length_CtrPosToApp);
		Length_AppToCtrPos = Length_CtrPosToApp;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = XOR_Cal(&Buf_AppToCtrPos[1], Length_CtrPosToApp);
		Buf_AppToCtrPos[Length_AppToCtrPos++] = 0xCA;
	
	  memcpy(rf_var.tx_buf, Buf_AppToCtrPos, Length_AppToCtrPos);
		rf_var.tx_len = Length_AppToCtrPos;
		rf_var.flag_txing = true;
	
		App_to_CtrPosReq =true;
	
	  /* 有数据下发且未曾下发过 */
		if(rf_var.flag_txing)	
		{
			my_nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL);

			rf_var.flag_tx_ok = true;
		}
}


/******************************************************************************
  Function:App_return_data_to_clickers
  Description:
       上位机发送的数据发送给答题器
  Input:None
  Return:
  Others:None
******************************************************************************/
void App_return_data_to_clickers(void)
{
	  uint8_t uidpos = 0;

		memcpy(rf_var.tx_buf, Buf_CtrPosToApp, Length_CtrPosToApp);
		rf_var.tx_len = Length_CtrPosToApp;
		rf_var.flag_txing = true;

		Length_AppToCtrPos = 0x00;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x5C;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x10;
		add_sign_to_buffer(&Length_AppToCtrPos,sign_buffer);
		Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x03;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x00;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = white_on_off;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = white_len;
		Buf_AppToCtrPos[Length_AppToCtrPos++] = XOR_Cal(&Buf_AppToCtrPos[1], 9);
		Buf_AppToCtrPos[Length_AppToCtrPos++] = 0xCA;

		App_to_CtrPosReq =true;

		if(App_to_CtrPosReq)
		{
			if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
			{
				DebugLog("Serial Send Buffer is full! \r\n");
			}
			else
			{
				serial_ringbuffer_write_data1(SEND_RINGBUFFER,Buf_AppToCtrPos);
				App_to_CtrPosReq = false;
			}
		}	
		/* 有数据下发且未曾下发过 */
		if(rf_var.flag_txing)	
		{
			my_nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL);
		
			rf_var.flag_tx_ok = true;
		}
}

/******************************************************************************
  Function:App_return_data_to_topic
  Description:
  Input:None
  Return:
  Others:None
******************************************************************************/
void App_return_data_to_topic(void)
{
		uint8_t temp_count = 0;
	
		Length_AppToCtrPos = rf_var.rx_len+0x07;  
		Buf_AppToCtrPos[0] = 0x5C;
		Buf_AppToCtrPos[1] = 0x10;
		Buf_AppToCtrPos[2] = sign_buffer[0];
		Buf_AppToCtrPos[3] = sign_buffer[1];
		Buf_AppToCtrPos[4] = sign_buffer[2];
		Buf_AppToCtrPos[5] = sign_buffer[3];
		Buf_AppToCtrPos[6] = rf_var.rx_len;

		for (temp_count=0;temp_count<rf_var.rx_len+1;temp_count++)
		{
			Buf_AppToCtrPos[temp_count+7]=rf_var.rx_buf[temp_count];
#ifdef ENABLE_RF_DATA_SHOW
			printf("%2X ",rf_var.rx_buf[temp_count]);
			if((temp_count+1)%20 == 0 )
				printf("\r\n");			
#endif
		}
		Buf_AppToCtrPos[rf_var.rx_len+7] = XOR_Cal(&Buf_AppToCtrPos[1], rf_var.rx_len+7);		
		Buf_AppToCtrPos[rf_var.rx_len+8] = 0xCA;
}

/******************************************************************************
  Function:App_return_whitelist_data
  Description:
		打印白名单信息
  Input :
		index：打印白名单的起始位置
  Return:
    uid_p:输出的白名单最后的位置
  Others:None
******************************************************************************/
uint8_t App_return_whitelist_data(uint8_t index)
{
	uint8_t temp_count = 0;
	uint8_t uid_p = index;
	uint8_t tempuid[4];
	
	Buf_AppToCtrPos[0] = 0x5C;
	Buf_AppToCtrPos[1] = 0x2B;
	Buf_AppToCtrPos[2] = sign_buffer[0];
	Buf_AppToCtrPos[3] = sign_buffer[1];
	Buf_AppToCtrPos[4] = sign_buffer[2];
	Buf_AppToCtrPos[5] = sign_buffer[3];
	
	for(temp_count=7;(temp_count<UART_NBUF-6)&&(uid_p<white_len);
	    temp_count=temp_count+4)
	{
		get_index_of_uid(uid_p,tempuid);
		Buf_AppToCtrPos[temp_count+1]=tempuid[0];
		Buf_AppToCtrPos[temp_count+2]=tempuid[1];
		Buf_AppToCtrPos[temp_count+3]=tempuid[2];
		Buf_AppToCtrPos[temp_count+4]=tempuid[3];
		uid_p++;
	}
	
	Buf_AppToCtrPos[6] = (uid_p-index)*4+1;
	Buf_AppToCtrPos[7] = uid_p;
	
	Buf_AppToCtrPos[(uid_p-index)*4+8] = XOR_Cal(&Buf_AppToCtrPos[1], (uid_p-index)*4+7);
	Buf_AppToCtrPos[(uid_p-index)*4+9] = 0xCA;
	
	Length_AppToCtrPos = (uid_p-index)*4+10;
	
	return uid_p;
}

/******************************************************************************
  Function:App_return_device_info
  Description:
		打印设备信息
  Input :
  Return:
  Others:None
******************************************************************************/
void App_return_device_info(void)
{	
	uint8_t temp_count = 0,i;

	Length_AppToCtrPos = 0x27;  
	Buf_AppToCtrPos[0] = 0x5C;
	Buf_AppToCtrPos[1] = 0x2C;
	Buf_AppToCtrPos[2] = sign_buffer[0];
	Buf_AppToCtrPos[3] = sign_buffer[1];
	Buf_AppToCtrPos[4] = sign_buffer[2];
	Buf_AppToCtrPos[5] = sign_buffer[3];	
	Buf_AppToCtrPos[6] = 0x34;
	for(temp_count=0,i=0;(temp_count<4)&&(i<8);temp_count++,i=i+2)
	{
			Buf_AppToCtrPos[temp_count+7]=(jsq_uid[i]<<4|jsq_uid[i+1]);
	}

	for(temp_count=0;temp_count<3;temp_count++)
	{
			Buf_AppToCtrPos[temp_count+11]=software[temp_count];
	}

	for(temp_count=0,i=0;(temp_count<15)&&(i<30);temp_count++,i=i+2)
	{
			Buf_AppToCtrPos[temp_count+14]=(hardware[i]<<4)|(hardware[i+1]);
	}

	for(temp_count=0,i=0;(temp_count<8)&&(i<16);temp_count++,i=i+2)
	{
			Buf_AppToCtrPos[temp_count+29]=(company[i]<<4)|(company[i+1]);
	}
	Buf_AppToCtrPos[37] = XOR_Cal(&Buf_AppToCtrPos[1],36);
	Buf_AppToCtrPos[38] = 0xCA;
}

/******************************************************************************
  Function:add_sign_to_buffer
  Description:
		将签名写入 Buf_AppToCtrPos buffer
  Input :
	  LenPos:写入的起始位置
		sign[]:UID签名数字组
  Return:
  Others:None
******************************************************************************/
void add_sign_to_buffer(uint16_t *LenPos, uint8_t sign[])
{
	Buf_AppToCtrPos[(*LenPos)++] = sign_buffer[0];
	Buf_AppToCtrPos[(*LenPos)++] = sign_buffer[1];
	Buf_AppToCtrPos[(*LenPos)++] = sign_buffer[2];
	Buf_AppToCtrPos[(*LenPos)++] = sign_buffer[3];
}
/**************************************END OF FILE****************************/
