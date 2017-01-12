#include "define.h"

uint8_t	 DTQ_UID[4];
uint8_t JSQ_UID[4];
uint8_t  DTQ_NUM;
uint16_t STUDENT_ID;
uint8_t  UID[8] = {0};
uint8_t  PUBLIC_UID[4] ={0x00,0x00,0x00,0x00};		//广播UID 		
uint8_t  NDEF_DataRead[0xFF];					

RF_TypeDef					rf_var;										//接收发送数据缓冲区

packet_typedef_t			tx_data;									//发送数据包
packet_typedef_t			rx_data;									//接收数据包

answer_input_struct			ANSWER;									//输入数据结构体



/*----------------------------- 通用函数 ----------------------------------*/
uint8_t XOR_Cal(uint8_t * dat,uint16_t length)
{
	uint8_t temp_xor;
	uint16_t i;

	temp_xor = *dat;
	for(i = 1;i < length; i++)
	{
		temp_xor = temp_xor ^ *(dat+i);
	}
	return temp_xor;
}


/* 51822的gpio配置为默认(省功耗)状态 */
void GPIO_default(void)
{
    uint32_t i = 0;
    for(i = 0; i< 32 ; i++ ) 
	{
        NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                               | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                               | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                               | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                               | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
    }
}








uint8_t get_random_number(void)
{
    NRF_RNG->TASKS_START = 1; // start the RNG peripheral.

	// Wait until the value ready event is generated.
	while (NRF_RNG->EVENTS_VALRDY == 0)
	{
		// Do nothing.
	}
	NRF_RNG->EVENTS_VALRDY = 0;		 // Clear the VALRDY EVENT.
	
	return (uint8_t)NRF_RNG->VALUE;
}


void PARAMETER_init(void)
{
	
	ANSWER.current_num = 0;	
	ANSWER.score_or_answer = ANSWER_INPUT;				//默认开机即输入答案
	ANSWER.start_num = 0;						//默认题号为0
	ANSWER.total_num = 0;						//默认为单题作答
	
	
	DTQ_UID[0] = UID[3];	
	DTQ_UID[1] = UID[4];
	DTQ_UID[2] = UID[5];
	DTQ_UID[3] = UID[6];	
	DTQ_NUM = NDEF_DataRead[6];	
	
	//BCD码转10进制；取学号前三位在答题器上显示
	STUDENT_ID = NDEF_DataRead[7] + NDEF_DataRead[8]*10 + NDEF_DataRead[9]*100;	
	
	JSQ_UID[0] = NDEF_DataRead[2];
	JSQ_UID[1] = NDEF_DataRead[3];
	JSQ_UID[2] = NDEF_DataRead[4];
	JSQ_UID[3] = NDEF_DataRead[5];
	
	
	tx_data.header = 0x5A;
	tx_data.uid[0]= DTQ_UID[0];
	tx_data.uid[1]= DTQ_UID[1];
	tx_data.uid[2]= DTQ_UID[2];
	tx_data.uid[3]= DTQ_UID[3];
	tx_data.packnum = 0x00;		//根据新协议，这个不再是包号，而作为保留字段，默认0x00
	tx_data.end = 0xCA;
	
	TRANSPORT.data[0]		= NRF_DATA_HEAD;
	TRANSPORT.data[1]		= NDEF_DataRead[2];
	TRANSPORT.data[2]		= NDEF_DataRead[3];
	TRANSPORT.data[3]		= NDEF_DataRead[4];
	TRANSPORT.data[4]		= NDEF_DataRead[5];
	TRANSPORT.data[5]		= DTQ_UID[0];
	TRANSPORT.data[6]		= DTQ_UID[1];
	TRANSPORT.data[7]		= DTQ_UID[2];
	TRANSPORT.data[8]		= DTQ_UID[3];					
	TRANSPORT.data[9]		= 0x00;				//sequence
	TRANSPORT.data[10]	= 0x00;				// pack_num
	TRANSPORT.data[11]	= NRF_DATA_IS_USE;
	TRANSPORT.data[12]	= 0xFF;
	TRANSPORT.data[13]	= 0xFF;
	TRANSPORT.data[14]	= 0x00;				//length
//	TRANSPORT.data[233]				
//	TRANSPORT.xor			= 0x00;
//	TRANSPORT.end			= NRF_DATA_END;
	
	
	TRANSPORT.ack[0] = NRF_DATA_HEAD;
	TRANSPORT.ack[9] = 0x00;  //sequence
	TRANSPORT.ack[10] = 0x00;  // pack_num
	TRANSPORT.ack[11] = NRF_DATA_IS_ACK;
	TRANSPORT.ack[12] = NRF_DATA_RESERVE;
	TRANSPORT.ack[13] = NRF_DATA_RESERVE;
	TRANSPORT.ack[14] = 0x00;	//length
	TRANSPORT.ack[15] = 0x00;	//xor
	TRANSPORT.ack[16] = NRF_DATA_END;
}

//参数跟新
//13.56M配对和重新跟新参数，主要是与NDEF_DataRead[]数组相关的参数
void PARAMETER_update(void)
{

	DTQ_NUM = NDEF_DataRead[6];	
	
	//BCD码转10进制；取学号前三位在答题器上显示
	STUDENT_ID = NDEF_DataRead[7] + NDEF_DataRead[8]*10 + NDEF_DataRead[9]*100;	
	
	JSQ_UID[0] = NDEF_DataRead[2];
	JSQ_UID[1] = NDEF_DataRead[3];
	JSQ_UID[2] = NDEF_DataRead[4];
	JSQ_UID[3] = NDEF_DataRead[5];

	TRANSPORT.data[1]		= NDEF_DataRead[2];
	TRANSPORT.data[2]		= NDEF_DataRead[3];
	TRANSPORT.data[3]		= NDEF_DataRead[4];
	TRANSPORT.data[4]		= NDEF_DataRead[5];
}



bool stringcmp(uint8_t *str1, uint8_t *str2, uint8_t len)
{
	uint8_t i;
	for(i = 0; i < len ; i++)
	{
		if(str1[i] != str2[i])
			return false;
	}
	return true;
}










