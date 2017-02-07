#ifndef __APPLICATION_H
#define	__APPLICATION_H

#include "define.h"

typedef enum 
{
	SINGLE_QUESTION_COMMAND		= 0x10,			//单题下发指令
	MULTIPLE_QUESTION_COMMAND	= 0x11,			//多题下发指令
	MUTUAL_VOTE_COMMAND			= 0x12,			//互动投票指令
	GROUP_GRADE_COMMAND			= 0x13,			//小组评分指令
	SYS_OFF_COMMAND				= 0x14,			//系统关机指令
	START_ANSWER_COMMAND		= 0x16,			//开始抢答指令
	ACTIVITY_END_COMMAND    	= 0x17,			//活动结束指令
	
	STUDENT_ID_MATCH_COMMAND	= 0x18,			//学号与UID配对
	EMPTY_PACKET_COMMAND		= 0x19,			//空包
	
	RECEIVE_OK_COMMAND			= 0xF1,			//成功接收指令
	HEARTBEAT_COMMAND			= 0x31			//心跳包指令
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



typedef struct 
{	
	bool				answer_ing_flg;			//有题目包下发时，按发送键才有效（降功耗）;
	bool				key_send_allow_flg;		//发送键允许，正在发送数据时，按发送键不做处理；ture:允许，false:不允许
	
	void				(*command_handler)(void);		
	void				(*button_handler)(void);
}application_struct_t;


extern application_struct_t				*APP;

void APP_init(void);


#endif

