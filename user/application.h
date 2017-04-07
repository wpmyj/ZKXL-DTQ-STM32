#ifndef __APPLICATION_H
#define	__APPLICATION_H

#include "define.h"


#define APP_CMD_TYPE_NULL			0x00
#define APP_CMD_TYPE_POWER			0x01
#define APP_CMD_TYPE_QUESTION		0x02
#define APP_CMD_TYPE_CLEAR			0x03

typedef enum 
{
	SINGLE_QUESTION_COMMAND		= 0x10,			//�����·�ָ��
	MULTIPLE_QUESTION_COMMAND	= 0x11,			//�����·�ָ��
	MUTUAL_VOTE_COMMAND			= 0x12,			//����ͶƱָ��
	GROUP_GRADE_COMMAND			= 0x13,			//С������ָ��
	SYS_OFF_COMMAND				= 0x14,			//ϵͳ�ػ�ָ��
	START_ANSWER_COMMAND		= 0x16,			//��ʼ����ָ��
	ACTIVITY_END_COMMAND    	= 0x17,			//�����ָ��
	
	STUDENT_ID_MATCH_COMMAND	= 0x18,			//ѧ����UID���
	EMPTY_PACKET_COMMAND		= 0x19,			//�հ�
	
	RECEIVE_OK_COMMAND			= 0xF1,			//�ɹ�����ָ��
	HEARTBEAT_COMMAND			= 0x31			//������ָ��
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
	bool				answer_ing_flg;			//����Ŀ���·�ʱ�������ͼ�����Ч�������ģ�;
	bool				key_send_allow_flg;		//���ͼ��������ڷ�������ʱ�������ͼ���������ture:����false:������
	uint8_t				last_cmd_type;			//�յ�����һ��ָ������
	
	bool				DisplayVerFlg;			//��ʾ�汾����Ϣ��־
	
}application_struct_t;


extern application_struct_t				*APP;

 void APP_CmdHandler(void);
 void APP_KeyHandler(void);
void APP_init(void);


#endif

