/**
  ******************************************************************************
  * @file   	variable.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	all variables called by other functions
  ******************************************************************************
  */
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "define.h"
#include "gpio.h"

typedef struct 
{
	uint8_t 			tx_len;			  					//2.4G发送长度
	uint8_t				tx_buf[RF_NBUF];					//2.4G发送缓存  
}RF_TypeDef;

typedef struct
{
	uint8_t 						hour;
	uint8_t 						minute;
	uint8_t 						second;
}time_t;

extern uint8_t 						NDEF_DataWrite[];
extern uint8_t 						NDEF_DataRead[];
extern uint16_t						NDEF_Len;

extern bool 						  gbf_hse_setup_fail;				//外部16M晶振起振标志
extern uint8_t 						time_for_buzzer_on;				//多长时间之后开始响
extern uint16_t 					time_for_buzzer_off;			//响了多长时间之后关

extern RF_TypeDef 				rf_var;							// 2.4G数据包缓冲
extern uint16_t						delay_nms;						// 中断延时变量
extern uint32_t 					timer_1ms;
extern time_t						  time;							//保持当前时间

extern bool							  flag_upload_uid_once;			//是否单次上传卡号标志
extern uint8_t						ReadNDEF_Step;					// 读取NDEF文件的步骤
extern uint8_t						FindCard_Step;					// 寻卡的步骤

extern uint8_t 						flag_App_or_Ctr;
extern uint16_t 					Length_CtrRC500ToApp;
extern uint16_t 					Length_AppToCtrRC500;
extern uint16_t 					Length_CtrPosToApp;
extern uint16_t 					Length_AppToCtrPos;
extern uint8_t 						Buf_CtrRC500ToApp[];
extern uint8_t 						Buf_AppToCtrRC500[];
extern uint8_t 						Buf_CtrPosToApp[];
extern uint8_t 						Buf_AppToCtrPos[];
extern uint8_t 						Buf_CtrRC500return[];			 //RC500返回数据缓存区
extern bool 						  App_to_CtrPosReq;
extern bool 						  App_to_CtrRC500Req;

extern uint8_t 						g_cardType[];					//返回卡类型
extern uint8_t 						respon[];
extern uint8_t 						g_cCid;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  pos;
	uint8_t  use;
	uint8_t  rev_num;
	uint8_t  rev_seq;
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  firstrev;
	uint32_t lost_package_num;
	uint32_t recv_package_num;
}Clicker_Typedef;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  pre_seq;
	uint32_t data_statistic_count;
}Revicer_Typedef;

typedef struct
{
	Clicker_Typedef uids[120];
	uint8_t    len;
	uint8_t    switch_status;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
}WhiteList_Typedef;

typedef struct
{
	uint8_t  status;
	uint32_t cnt;
}Timer_Typedef;

typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t s;
	uint16_t ms;
}timer_t;

#endif //_VARIABLE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

