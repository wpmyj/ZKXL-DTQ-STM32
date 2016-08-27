/**
  ******************************************************************************
  * @file   	clock.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	clock config functions and delay functions
  ******************************************************************************
  */
  
#include "main.h"
white_list_t				white_list[MAX_WHITE_LEN];		// 白名单列表
uint8_t						white_len;						// 白名单长度
Switch_State				white_on_off;					// 白名单开关 
Switch_State				attendance_on_off;				// 考勤开关 
Switch_State				match_on_off;					// 配对开关 
uint16_t					match_number = 1;					// 配对序号

uint8_t 					NDEF_DataWrite[30] = {0x00, 0x00, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
uint8_t 					NDEF_DataRead[0xFF] = {0x00};

bool 						gbf_hse_setup_fail = FALSE;		// 外部16M晶振起振标志
uint8_t 					time_for_buzzer_on = 0;			// 多长时间之后开始响
uint16_t 					time_for_buzzer_off = 0;		// 响了多长时间之后关

nrf_parameter_t				nrf_parameter;					// 2.4G初始化参数

uint8_t                     sign_buffer[4];
uint8_t						uart_tx_i = 0;					// 串口发送缓冲索引

RF_TypeDef 					rf_var;							// 2.4G数据包缓冲

uint16_t					delay_nms = 0;					// 中断延时变量
uint32_t 					timer_1ms = 0;					// 喂狗定时器
time_t						time;							// RTC结构体

uint8_t						uid_len = 0;					// M1卡序列号长度
uint8_t 					g_cSNR[10];						// M1卡序列号

bool						flag_upload_uid_once = false;	// 是否单次上传卡号标志
uint8_t						ReadNDEF_Step = 0;				// 读取NDEF文件的步骤
uint8_t						FindCard_Step = 0;				// 寻卡的步骤

	
uint16_t					Length_CtrRC500ToApp;			// RC500返回数据长度
uint16_t 					Length_AppToCtrRC500;			// 应用层下发RC500指令长度

uint16_t					Length_CtrPosToApp;				// pos下发指令长度
uint16_t					Length_AppToCtrPos;				// 应用层上报指令长度
	
uint8_t						Buf_CtrRC500ToApp[UART_NBUF];	// RC500返回数据缓冲区
uint8_t						Buf_AppToCtrRC500[UART_NBUF];	// 应用层下发RC500指令缓冲区
	
uint8_t						Buf_CtrPosToApp[UART_NBUF];		// pos下发指令缓冲
uint8_t						Buf_AppToCtrPos[UART_NBUF];		// 应用层上报指令缓冲区

uint8_t						Buf_CtrRC500return[UART_NBUF];	// RC500返回数据缓存区？？？

bool 						  App_to_CtrPosReq = false;		// pos指令请求
bool 						  App_to_CtrRC500Req = false;		// RC500指令请求

uint8_t 					g_cardType[40] = {0x00};		// 返回卡类型
uint8_t 					respon[BUF_LEN + 20] = {0x00};
uint8_t 					g_cCid;							


/**************************************END OF FILE****************************/

