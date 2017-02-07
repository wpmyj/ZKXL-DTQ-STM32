/** @file key_scan.h
 *
 * @author 
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2012 ZKXL. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 		2015.11.28
 * $Revision: 	V1000
 *
 */
 
#ifndef _APP_HANDLER_H_
#define _APP_HANDLER_H_

//#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "variable.h"
#include "my_radio.h"
#include "my_misc_fun.h"

typedef enum 
{
	SINGLE_QUESTION_ACT_COMMAND		= 0x10,			//单题下发指令
	MULTIPLE_QUESTION_ACT_COMMAND	= 0x11,			//多题下发指令
	MUTUAL_VOTE_ACT_COMMAND			= 0x12,			//互动投票指令
	SYS_OFF_ACT_COMMAND				= 0xFF,			//系统关机指令
	RECEIVE_OK_ACT_COMMAND			= 0x14,			//成功接收指令
	HEARTBEAT_PACKET_ACT_COMMAND	= 0x15,			//心跳包指令
	GROUP_GRADE_ACT_COMMAND			= 0x13,			//小组评分指令
	ACTIVITY_END_ACT_COMMAND    	= 0x17,			//活动结束指令
	STUDENT_ID_MATCH_ACT_COMMAND	= 0x18,			//学号与UID配对
	EMPTY_PACKET_ACT_COMMAND		= 0x19			//空包
}app_act_command_enum;	

typedef enum 
{
	KEY_A_1				= 1 ,
	KEY_B_2				= 2 ,
	KEY_C_3				= 3 ,
	KEY_D_4				= 4 ,
	KEY_E_5				= 5 ,
	KEY_F_6				= 6 ,
	KEY_G_7				= 7 ,
	KEY_H_8				= 8 ,
	KEY_I_9				= 9 ,
	KEY_J_0				= 10,
	KEY_SYS_POWER		= 11,
	KEY_FN              = 12,
	KEY_CLEAR			= 13,
	KEY_SEND			= 14,
	KEY_LAST_QUESTION   = 15,	
	KEY_NEXT_QUESTION	= 16
}key_act_command_enum;	



/* 外部函数声明---------------------------------------------------------*/

void button_handler2(uint8_t key_value);
void app_handler(void);
void clear_input_information( void );
void init_input_information( void );

 
#endif	//_APP_HANDLER_H_
 




