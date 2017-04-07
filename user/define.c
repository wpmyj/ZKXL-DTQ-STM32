#include "define.h"

uint8_t	 DTQ_UID[4];
uint8_t JSQ_UID[4];
uint8_t  DTQ_NUM;
uint16_t STUDENT_ID;
uint8_t  PUBLIC_UID[4] ={0x00,0x00,0x00,0x00};		//�㲥UID 						

RF_TypeDef					rf_var;										//���շ������ݻ�����

packet_typedef_t			tx_data;									//�������ݰ�
packet_typedef_t			rx_data;									//�������ݰ�

answer_input_struct			ANSWER;									//�������ݽṹ��



/*----------------------------- ͨ�ú��� ----------------------------------*/
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


/* 51822��gpio����ΪĬ��(ʡ����)״̬ */
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
	
	NRF_RNG->TASKS_STOP = 1;		//�����������ҪSTOP������һֱ���У���������
	
	return (uint8_t)NRF_RNG->VALUE;
}


void PARAMETER_init(void)
{
	ANSWER.current_num = 0;	
	ANSWER.score_or_answer = ANSWER_INPUT;				//Ĭ�Ͽ����������
	ANSWER.start_num = 0;						//Ĭ�����Ϊ0
	ANSWER.total_num = 0;						//Ĭ��Ϊ��������
	
	
	DTQ_UID[0] = NFC.UID[3];	
	DTQ_UID[1] = NFC.UID[4];
	DTQ_UID[2] = NFC.UID[5];
	DTQ_UID[3] = NFC.UID[6];	
	DTQ_NUM = NFC.DataRead[6];	
	
	//BCD��ת10���ƣ�ȡѧ��ǰ��λ�ڴ���������ʾ
	STUDENT_ID = NFC.DataRead[7] + NFC.DataRead[8]*10 + NFC.DataRead[9]*100;	
//	STUDENT_ID = DTQ_NUM;
	
	JSQ_UID[0] = NFC.DataRead[2];
	JSQ_UID[1] = NFC.DataRead[3];
	JSQ_UID[2] = NFC.DataRead[4];
	JSQ_UID[3] = NFC.DataRead[5];
	
	
	tx_data.header = 0x5A;
	tx_data.uid[0]= DTQ_UID[0];
	tx_data.uid[1]= DTQ_UID[1];
	tx_data.uid[2]= DTQ_UID[2];
	tx_data.uid[3]= DTQ_UID[3];
	tx_data.packnum = 0x00;		//������Э�飬��������ǰ��ţ�����Ϊ�����ֶΣ�Ĭ��0x00
	tx_data.end = 0xCA;
	
	TRANSPORT.data[0]		= NRF_DATA_HEAD;
	TRANSPORT.data[1]		= NFC.DataRead[2];
	TRANSPORT.data[2]		= NFC.DataRead[3];
	TRANSPORT.data[3]		= NFC.DataRead[4];
	TRANSPORT.data[4]		= NFC.DataRead[5];
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

//��������
//13.56M��Ժ����¸��²�������Ҫ����NDEF_DataRead[]������صĲ���
void PARAMETER_update(void)
{

	DTQ_NUM = NFC.DataRead[6];	
	
	//BCD��ת10���ƣ�ȡѧ��ǰ��λ�ڴ���������ʾ
	STUDENT_ID = NFC.DataRead[7] + NFC.DataRead[8]*10 + NFC.DataRead[9]*100;	
	
	JSQ_UID[0] = NFC.DataRead[2];
	JSQ_UID[1] = NFC.DataRead[3];
	JSQ_UID[2] = NFC.DataRead[4];
	JSQ_UID[3] = NFC.DataRead[5];

	TRANSPORT.data[1]		= NFC.DataRead[2];
	TRANSPORT.data[2]		= NFC.DataRead[3];
	TRANSPORT.data[3]		= NFC.DataRead[4];
	TRANSPORT.data[4]		= NFC.DataRead[5];
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










