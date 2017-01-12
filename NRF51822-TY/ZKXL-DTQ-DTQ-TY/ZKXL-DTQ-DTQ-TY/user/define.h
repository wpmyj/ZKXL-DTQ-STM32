#ifndef __DEFINE_H
#define	__DEFINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "radio.h"
#include "key.h"
#include "lcd.h"
#include "rtc.h"
#include "radio.h"
#include "adc.h"
#include "timer.h"
#include "uart.h"
#include "application.h"
#include "power.h"
#include "clock.h"
#include "nfc.h"

//#define DEBUG		//串口打印debug
#ifdef DEBUG
#define debug_printf  printf   
#else  
#define debug_printf(...)                    
#endif 

//#define DEBUG_LOGIC
#ifdef DEBUG_LOGIC			//示波器查看逻辑电平debug
#define debug_logic_init()	nrf_gpio_cfg_output(TX_PIN_NUMBER_2)
#define logic_high()		nrf_gpio_pin_set(TX_PIN_NUMBER_2)
#define logic_low()			nrf_gpio_pin_clear(TX_PIN_NUMBER_2)
#define logic_toggle()		nrf_gpio_pin_toggle(TX_PIN_NUMBER_2)
#else 
#define debug_logic_init(...)	
#define logic_high(...)		
#define logic_low(...)	
#define logic_toggle(...)
#endif 

#define TMP_DEBUG
#ifdef TMP_DEBUG
#define tmp_debug  debug_printf   
#else  
#define tmp_debug(...)                    
#endif 

//这个宏在radio.h中已经定义，为什么还要在这里定义，编译才不会出错？
#define		 NRF_TOTAL_DATA_LEN				(250)				
		

#define		QUESTION_MAX_NUMBER					(99)				
#define 	QUESTION_ANSWER_BIT_LEN				(6)				
#define 	QUESTION_ANS_TYPE_BIT_LEN			(2)	
#define 	QUESTION_ANS_RANGE_BIT_LEN			(QUESTION_ANSWER_BIT_LEN)
#define 	QUESTION_ANS_BIT_NUMBER				(8)	
#define		QUESTION_MAX_BYTE_LEN               (198)

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;	
typedef enum {SYS_ON = 0x01, SYS_OFF = 0x02, SYS_SLEEP = 0x03}System_State;
typedef enum {ANSWER_INPUT = 0, SCORE_INPUT = 1}input_mode;
typedef enum {GRADE_RESULT = 0x01, SCORE_RESULT = 0x02}digit_input_result;
typedef enum {ANSWER_TRUE = 0x01, ANSWER_FALSE = 0x02}judgement_answer;
typedef enum {SINGLE_CHOICE = 0x01, MULTIPLE_CHOICE = 0x02, TRUE_OR_FALSE = 0x03} QuestionType;
typedef enum {A = 0x01, B = 0x02, C = 0x04, D = 0x08, E = 0x10, F = 0x20, G = 0x40, H = 0x80} ChoiceAnswer;

typedef union
{
	uint8_t 						true_or_false;				//判断题对应答案
	uint8_t							choice_answer;				//选择题对应答案
}answer_t;

typedef struct 
{
	bool							answering;									//正在作答标志，false时显示RTC			 
	answer_t						answer[QUESTION_MAX_NUMBER];				//输入答案信息,最多16个答案
	QuestionType					type[QUESTION_MAX_NUMBER];				//输入答案的类型
	uint8_t 						number[QUESTION_MAX_NUMBER];			//题号		
	uint8_t							range[QUESTION_MAX_NUMBER];				//可作答范围
	uint8_t							grade[QUESTION_MAX_NUMBER];					//评分结果
	uint8_t							score;						//投票结果
	uint8_t							grade_or_score;				//数字输入模式时，输入结果是评分还是投票
	uint8_t							score_or_answer;			//评分输入还是作答输入
	uint8_t 						total_num;			//多题作答时的题目数
	uint8_t 						start_num;			//多题作答时的起始题号
	uint8_t							current_num;			
}answer_input_struct;


typedef struct 
{
	uint8_t							header;						//固定0x5A
	uint8_t							uid[4];						//uid
	uint8_t							packnum;					//数据包的包号
	uint8_t							type;						//数据包的类型
	uint8_t							len;						//数据域的长度
	uint8_t							data[NRF_TOTAL_DATA_LEN];	//数据域的内容
	uint8_t							xor;						//异或校验
	uint8_t							end;						//固定为0xCA
}packet_typedef_t;

typedef struct 
{	
	bool 							flag_txing;		   				//2.4G发送ing 标志，
	bool 							flag_rx_ok;		  				//2.4G接收完成标志
	bool 							flag_tx_ok;		  				//2.4G发送完成标志
	bool							rf_is_ok;						//2.4G收发正常
	
	uint8_t 						tx_len;			  				//2.4G发送长度
	uint8_t	 						rx_len;			  				//2.4G接收长度
	uint8_t							tx_buf[NRF_TOTAL_DATA_LEN];					//2.4G发送缓存
	uint8_t							rx_buf[NRF_TOTAL_DATA_LEN];					//2.4G接收缓存
}RF_TypeDef;



extern uint8_t DTQ_UID[4];
extern uint8_t JSQ_UID[4];
extern uint8_t  PUBLIC_UID[4];
extern uint8_t  DTQ_NUM;
extern uint16_t STUDENT_ID;
extern uint8_t  UID[8];
extern uint8_t 					NDEF_DataRead[0xFF];

extern answer_input_struct			ANSWER;					//输入数据结构体
extern packet_typedef_t			tx_data;									//发送数据包
extern packet_typedef_t			rx_data;									//接收数据包
extern RF_TypeDef					rf_var;										//接收发送数据缓冲区
extern packet_typedef_t				tx_data;
extern packet_typedef_t				rx_data;




uint8_t XOR_Cal(uint8_t * dat,uint16_t length);
void GPIO_default(void);


uint8_t get_random_number(void);
void PARAMETER_init(void);
void PARAMETER_update(void);
bool stringcmp(uint8_t *str1, uint8_t *str2, uint8_t len);


#endif 

