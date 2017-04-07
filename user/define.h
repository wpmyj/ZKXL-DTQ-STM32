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
#include "lib_TagType4.h"


//����汾����
#define 	VERSION_PROTOCOL	(02)		//�汾֧�ֵ�Э��
#define 	VERSION_LEVEL_1		(01)		//һ���汾��
#define     VERSION_LEVEL_2		(07)		//�����汾��


//#define DEBUG		//���ڴ�ӡdebug
#ifdef DEBUG
#define debug_printf  printf   
#else  
#define debug_printf(...)                    
#endif 

//#define DEBUG_LOGIC
#ifdef DEBUG_LOGIC			//ʾ�����鿴�߼���ƽdebug
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

//�������radio.h���Ѿ����壬Ϊʲô��Ҫ�����ﶨ�壬����Ų������
#define		 NRF_TOTAL_DATA_LEN				(250)				
		

#define		QUESTION_MAX_NUMBER					(99)				
#define 	QUESTION_ANSWER_BIT_LEN				(6)				
#define 	QUESTION_ANS_TYPE_BIT_LEN			(2)	
#define 	QUESTION_ANS_RANGE_BIT_LEN			(QUESTION_ANSWER_BIT_LEN)
#define 	QUESTION_ANS_BIT_NUMBER				(8)	
#define		QUESTION_MAX_BYTE_LEN               (198)

#ifndef 	__ErrorStatus__
#define 	__ErrorStatus__
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;	
#endif


typedef enum {SYS_ON = 0x01, SYS_OFF = 0x02, SYS_SLEEP = 0x03}System_State;
typedef enum {ANSWER_INPUT = 0, SCORE_INPUT = 1}input_mode;
typedef enum {GRADE_RESULT = 0x01, SCORE_RESULT = 0x02}digit_input_result;
typedef enum {ANSWER_TRUE = 0x01, ANSWER_FALSE = 0x02}judgement_answer;
typedef enum {SINGLE_CHOICE = 0x01, MULTIPLE_CHOICE = 0x02, TRUE_OR_FALSE = 0x03} QuestionType;
typedef enum {A = 0x01, B = 0x02, C = 0x04, D = 0x08, E = 0x10, F = 0x20, G = 0x40, H = 0x80} ChoiceAnswer;

typedef union
{
	uint8_t 						true_or_false;				//�ж����Ӧ��
	uint8_t							choice_answer;				//ѡ�����Ӧ��
}answer_t;

typedef struct 
{
	bool							answering;									//���������־��falseʱ��ʾRTC			 
	answer_t						answer[QUESTION_MAX_NUMBER];				//�������Ϣ,���16����
	QuestionType					type[QUESTION_MAX_NUMBER];				//����𰸵�����
	uint8_t 						number[QUESTION_MAX_NUMBER];			//���		
	uint8_t							range[QUESTION_MAX_NUMBER];				//������Χ
	uint8_t							grade[QUESTION_MAX_NUMBER];					//���ֽ��
	uint8_t							score;						//ͶƱ���
	uint8_t							grade_or_score;				//��������ģʽʱ�������������ֻ���ͶƱ
	uint8_t							score_or_answer;			//�������뻹����������
	uint8_t 						total_num;			//��������ʱ����Ŀ��
	uint8_t 						start_num;			//��������ʱ����ʼ���
	uint8_t							current_num;			
}answer_input_struct;


typedef struct 
{
	uint8_t							header;						//�̶�0x5A
	uint8_t							uid[4];						//uid
	uint8_t							packnum;					//���ݰ��İ���
	uint8_t							type;						//���ݰ�������
	uint8_t							len;						//������ĳ���
	uint8_t							data[NRF_TOTAL_DATA_LEN];	//�����������
	uint8_t							xor;						//���У��
	uint8_t							end;						//�̶�Ϊ0xCA
}packet_typedef_t;

typedef struct 
{	
	bool 							flag_txing;		   				//2.4G����ing ��־��
	bool 							flag_rx_ok;		  				//2.4G������ɱ�־
	bool 							flag_tx_ok;		  				//2.4G������ɱ�־
	bool							rf_is_ok;						//2.4G�շ�����
	
	uint8_t 						tx_len;			  				//2.4G���ͳ���
	uint8_t	 						rx_len;			  				//2.4G���ճ���
	uint8_t							tx_buf[NRF_TOTAL_DATA_LEN];					//2.4G���ͻ���
	uint8_t							rx_buf[NRF_TOTAL_DATA_LEN];					//2.4G���ջ���
}RF_TypeDef;



extern uint8_t DTQ_UID[4];
extern uint8_t JSQ_UID[4];
extern uint8_t  PUBLIC_UID[4];
extern uint8_t  DTQ_NUM;
extern uint16_t STUDENT_ID;

extern answer_input_struct			ANSWER;					//�������ݽṹ��
extern packet_typedef_t			tx_data;									//�������ݰ�
extern packet_typedef_t			rx_data;									//�������ݰ�
extern RF_TypeDef					rf_var;										//���շ������ݻ�����
extern packet_typedef_t				tx_data;
extern packet_typedef_t				rx_data;




uint8_t XOR_Cal(uint8_t * dat,uint16_t length);
void GPIO_default(void);


uint8_t get_random_number(void);
void PARAMETER_init(void);
void PARAMETER_update(void);
bool stringcmp(uint8_t *str1, uint8_t *str2, uint8_t len);


#endif 

