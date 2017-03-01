/**
  ******************************************************************************
  * @file   	init.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions's declaritions
  ******************************************************************************
  */
#ifndef _INIT_H_
#define _INIT_H_

#include "define.h"

typedef struct 
{
	uint8_t 			tx_len;			  					//2.4G���ͳ���
	uint8_t				tx_buf[RF_NBUF];					//2.4G���ͻ���  
}RF_TypeDef;

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
	uint8_t  uart_pac_num;
	uint8_t  uart_seq_num;
	uint32_t data_statistic_count;
}Revicer_Typedef;

typedef struct
{
	Clicker_Typedef uids[120];
	uint8_t    len;
	uint8_t    switch_status;
	uint8_t    start;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
}WhiteList_Typedef;


extern bool 						  gbf_hse_setup_fail; //�ⲿ16M���������־
extern RF_TypeDef 				rf_var;							// 2.4G���ݰ�����
extern uint8_t 						respon[];

void Platform_Init(void);
void Usart1_Init(void);
void NVIC_Configuration_USART1(void);
void Usart2_Init(void);
void NVIC_Configuration_USART2(void);

void SPI_Init_NRF1(void);
void SPI_Init_NRF2(void);
void GPIOInit_SE2431L(void);
void SE2431L_LNA(void);
void SE2431L_Bypass(void);
void SE2431L_TX(void);
void GPIOInit_MFRC500(void);
void uart_send_char( uint8_t ch );

#endif //_INIT_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

