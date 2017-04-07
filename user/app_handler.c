/** @file app_handler.c
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V4.73.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * $Date: 		2016.05.19
 * $Revision: 	V1000
 *
 */
#include "app_handler.h"


/* �ڲ���������---------------------------------------------------------*/

/******************************************************************************
  @����:single_question_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void single_question_act_command_handler(void)
{
	;
}

/******************************************************************************
  @����:multiple_question_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void multiple_question_act_command_handler(void)
{
	uint8_t i;
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];
	uint8_t temp_question_number;
	memset(temp_question_buf,0x00,QUESTION_MAX_BYTE_LEN);       //����
	
	if(rx_data.data[0] > QUESTION_MAX_NUMBER || rx_data.data[0] < 1)			//��Ŀ�����Ϸ�
	{	return;	 }
	
	temp_question_number =  rf_var.rx_buf[8];
	memcpy(temp_question_buf, rf_var.rx_buf+9, rf_var.rx_buf[7] - 1);           //ȡ����Ŀ��Ч��Ϣ������temp_question_buf
	fetch_question_info(temp_question_buf, temp_question_number);				//������Ŀ��Ϣ��input_info�ṹ��
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )									//��������
	{
		input_info.answer[i].true_or_false = 0;
		input_info.answer[i].choice_answer = 0;
		input_info.grade[i] = 0;
	}	
	
	input_info.score_or_answer = ANSWER_INPUT;									//����Ϊ��������
	input_info.total_answer_num = rx_data.data[0];								//��ȡ����
	input_info.start_answer_num = input_info.ans_number[0];						//��ȡ�����

	LCD_clear_segment();														//����������ʾ
	LCD_display_state.eng_num_flag 	  |= ENG_NUM_UPDATA_1;						//��ʾ����״̬
	LCD_display_state.segment_flag 	  |= SEGMENT_UPDATA_2;						//��ʾ���
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;					//����Դ���ʾ
	LCD_display_state.letter_flag  	  |= LETTER_UPDATA_0;                 		//�����ĸ��ʾ
	current_answer_num = 0;														//��ǰ��Ŀ����Ϊ0��
	LCD_display_state.updata |= SEGMENT_UPDATA | ENG_NUM_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
	LCD_display_handler();														//��������LCD��ʾ�������ϴ���Ŀ�����Ӱ��
}

/******************************************************************************
  @����:mutual_vote_act_command_handler
  @����:����ͶƱ
  @����:None
  @���:
  @����:
******************************************************************************/
static void mutual_vote_act_command_handler(void)
{
	input_info.score_or_answer = SCORE_INPUT;						//��������ģʽ
	input_info.grade_or_score = SCORE_RESULT;						//������ΪͶƱ	
	input_info.score = 0;
	LCD_display_state.eng_num_flag 	  |= ENG_NUM_UPDATA_1;				 //��ʾ����״̬
	LCD_display_state.segment_flag 	  |= SEGMENT_UPDATA_0;				 //����������ֺ�С����	
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;			 //����Դ���ʾ
	LCD_display_state.letter_flag  	  |= LETTER_UPDATA_0;                //�����ĸ��ʾ
	LCD_display_state.updata |= SEGMENT_UPDATA | ENG_NUM_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;	
	LCD_display_handler();	
}

/******************************************************************************
  @����:sys_off_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void sys_off_act_command_handler(void)
{
	if(rx_data.len == 0x01)
	{
		if(rx_data.data[0] == 0x20)
		{
			send_system_state(SYS_OFF);
			system_off_delay = 0;
			power_manage();
		}
	}
}

/******************************************************************************
  @����:group_grade_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void group_grade_act_command_handler(void)
{
	uint8_t i;
	if(rx_data.data[0] < 1)		//�������Ϸ�
	{	return;	}
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )				//��������
	{
		input_info.answer[i].true_or_false = 0;
		input_info.answer[i].choice_answer = 0;
		input_info.grade[i] = 0;
	}
	
	input_info.total_answer_num = rx_data.data[0];		//С������Ŀ
	for(i = 0;i < input_info.total_answer_num;i++)
	{
		input_info.ans_number[i] = i + 1;				//��Ŵ�1��ʼ����+1
	}
	
	input_info.score_or_answer = SCORE_INPUT;					//��������ģʽ
	input_info.grade_or_score = GRADE_RESULT;					//������Ϊ����
	
	current_answer_num = 0;										//��ǰ��Ŀ����Ϊ0
	input_info.start_answer_num = input_info.ans_number[0];		//��ʼ���Ϊ��һ��
	LCD_display_state.segment_flag   |= SEGMENT_UPDATA_2 | SEGMENT_UPDATA_1;		//��ʾ���
	LCD_display_state.eng_num_flag 	 |= ENG_NUM_UPDATA_1;							//��ʾ����״̬
	LCD_display_state.letter_flag  	 |= LETTER_UPDATA_0;                 		//�����ĸ��ʾ
	LCD_display_state.true_false_flag = TRUE_FALSE_UPDATA_0;						//����Դ���ʾ			
	LCD_display_state.updata |= SEGMENT_UPDATA | ENG_NUM_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA ;	

	LCD_display_handler();										//��������LCD��ʾ
}

/******************************************************************************
  @����:activity_end_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void activity_end_act_command_handler(void)
{
	uint8_t i;
	printf("activity_end_act_command_handler \r\n");
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )		//�������������Ϣ
	{
		input_info.answer[i].true_or_false = 0;
		input_info.answer[i].choice_answer = 0;
	}
	
	input_info.total_answer_num = 1;						//�л�Ϊ��������
	input_info.ans_number[0] = 0;								//�����������Ϊ0
	input_info.score_or_answer = ANSWER_INPUT;				//����Ϊ��������
	LCD_diplay_english();									//��ʾӢ
	
	LCD_clear_letter();
	LCD_clear_true_and_false();
	LCD_clear_segment();
}

/******************************************************************************
  @����:receive_ok_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void receive_ok_act_command_handler(void)
{
	
}

/******************************************************************************
  @����:heartbeat_packet_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void heartbeat_packet_act_command_handler(void)
{
	
}

/******************************************************************************
  @����:student_id_match_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void student_id_match_act_command_handler(void)
{

}

/******************************************************************************
  @����:empty_packet_act_command_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void empty_packet_act_command_handler(void)
{

}

/******************************************************************************
  @����:key_fn_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void key_fn_handler(void)
{
	;
}

/******************************************************************************
  @����:key_letter_input_handler
  @����:��ĸ������صİ���������
  @����:
  @���:
  @����:
******************************************************************************/
static void key_letter_input_handler(void)
{
	uint8_t temp_answer;
	if(input_info.total_answer_num == 1)               //Ĭ�Ͽ���Ϊ��������������Ϊ1
	{
		if(input_info.ans_type[current_answer_num] == TRUE_OR_FALSE)  //�ж���
		{
			input_info.answer[current_answer_num].true_or_false = 0;
			LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;								//����Դ���ʾ
			LCD_display_state.updata |= TRUE_FALSE_UPDATA;
		}
		//����ٴΰ������ʾ
		input_info.answer[current_answer_num].choice_answer ^= (0x01 << (g_variable.key_value - 1));
		if( input_info.answer[current_answer_num].choice_answer == A ||	input_info.answer[current_answer_num].choice_answer == B ||
			input_info.answer[current_answer_num].choice_answer == C ||	input_info.answer[current_answer_num].choice_answer == D ||
			input_info.answer[current_answer_num].choice_answer == E ||	input_info.answer[current_answer_num].choice_answer == F ||
			input_info.answer[current_answer_num].choice_answer == G ||	input_info.answer[current_answer_num].choice_answer == H ||
			input_info.answer[current_answer_num].choice_answer == 0)
		{
			input_info.ans_type[current_answer_num] = SINGLE_CHOICE;
		}
		else if(input_info.answer[current_answer_num].choice_answer )			//��ѡ��
		{	
			input_info.ans_type[current_answer_num] = MULTIPLE_CHOICE;	
		}
		else{;}
	}
	else  //��������
	{
		switch(input_info.ans_type[current_answer_num])
		{
			case SINGLE_CHOICE:
				temp_answer = (0x01 << (g_variable.key_value - 1))&input_info.ans_range[current_answer_num];
				if(temp_answer)		//�ж������Ƿ�������Χ��
				{
					if(input_info.answer[current_answer_num].choice_answer & temp_answer)
						input_info.answer[current_answer_num].choice_answer ^= temp_answer;		//��������ɾ��
					else
						input_info.answer[current_answer_num].choice_answer = temp_answer;
				}
				break;
			case MULTIPLE_CHOICE:
				input_info.answer[current_answer_num].choice_answer ^= (0x01 << (g_variable.key_value - 1));
				input_info.answer[current_answer_num].choice_answer &= input_info.ans_range[current_answer_num];
				break;
			default :
				break;
		}
	}
	
	if(input_info.ans_type[current_answer_num] != TRUE_OR_FALSE)
	{
		LCD_display_state.letter_flag |= LETTER_UPDATA_1;								//��ʾѡ�����
		LCD_display_state.updata 	  |= LETTER_UPDATA;		
	}else{;}
}

/******************************************************************************
  @����:key_digit_input_handler
  @����:����������صİ���������
  @����:
  @���:
  @����:
******************************************************************************/
static void key_digit_input_handler(void)
{
				
	if(input_info.grade_or_score == SCORE_RESULT)					//����ͶƱģʽ
	{
		input_info.score = g_variable.key_value;
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;			//��ʾ����/ͶƱ���
		LCD_display_state.updata |= SEGMENT_UPDATA;
	}
	else if(input_info.grade_or_score == GRADE_RESULT) 				//С������ģʽ
	{
		input_info.grade[current_answer_num] = g_variable.key_value;
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;		//��ʾ����/ͶƱ���
		LCD_display_state.updata |= SEGMENT_UPDATA;
	}
	else {;}
}

/******************************************************************************
  @����:key_judge_input_handler
  @����:�Դ�������صİ���������
  @����:
  @���:
  @����:
******************************************************************************/
static void key_judge_input_handler(void)
{
	if(input_info.total_answer_num == 1)
	{
		if(input_info.ans_type[current_answer_num] != TRUE_OR_FALSE)
		{
			input_info.answer[current_answer_num].choice_answer = 0x00;			//ѡ����Ĵ����
			LCD_display_state.letter_flag |= LETTER_UPDATA_0;					//���ѡ������ʾ
			LCD_display_state.updata |= LETTER_UPDATA;	
		}
		
		input_info.ans_type[current_answer_num] = TRUE_OR_FALSE;
		if(g_variable.key_value == 8)
		{
			input_info.answer[current_answer_num].true_or_false = ANSWER_TRUE;
//			KeyScan_DebugLog("[KeyScan]: judgement answer is true\r\n");	
		}
		else
		{
			input_info.answer[current_answer_num].true_or_false = ANSWER_FALSE;
//			KeyScan_DebugLog("[KeyScan]: judgement answer is false\r\n");	
		}
		LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_1;							//��ʾ�ж����
		LCD_display_state.updata |= TRUE_FALSE_UPDATA;
	}
	else if(input_info.ans_type[current_answer_num] == TRUE_OR_FALSE)
	{
		if(g_variable.key_value == 8)
		{
			input_info.answer[current_answer_num].true_or_false = ANSWER_TRUE;
//			KeyScan_DebugLog("[KeyScan]: judgement answer is true\r\n");	
		}
		else
		{
			input_info.answer[current_answer_num].true_or_false = ANSWER_FALSE;
//			KeyScan_DebugLog("[KeyScan]: judgement answer is false\r\n");	
		}
		LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_1;										//��ʾ�ж����
		LCD_display_state.updata |= TRUE_FALSE_UPDATA;
	}
}



/******************************************************************************
  @����:key_clear_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void key_clear_handler(void)
{
	input_info.answer[current_answer_num].true_or_false = 0;
	input_info.answer[current_answer_num].choice_answer = 0;
	input_info.grade[current_answer_num] = 0; 
	input_info.score = 0;				
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;				//����Դ���ʾ
	LCD_display_state.segment_flag |= SEGMENT_UPDATA_3;					//����������������ֵ����ʾ
	LCD_display_state.letter_flag |= LETTER_UPDATA_0;					//�����ĸ��ʾ
	LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
}

/******************************************************************************
  @����:key_send_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void key_send_handler(void)
{ 
	uint8_t i;
	uint8_t temp_if_answer = 0;			//��ʱ�ж��Ƿ�����
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];	
	uint8_t temp_question_buf_len;		
	for(i = 0 ;i < QUESTION_MAX_NUMBER;i++)
		temp_if_answer |= input_info.answer[i].true_or_false | input_info.answer[i].choice_answer | input_info.score | input_info.grade[i];
	if(temp_if_answer)		//�������ݷǿղŷ���
	{
//		rf_send_blank_timers_stop();												//�������ݰ�ʱ��ֹͣ���Ϳ����ݰ�
		if(input_info.score_or_answer == ANSWER_INPUT)								//�������ģʽΪ����
		{
			if(input_info.total_answer_num == 1)
				tx_data.type = 0x10;				//��������
			else
				tx_data.type = 0x11;				//��������

			if(tx_data.type == 0x10)		
			{
				tx_data.len = 0x03;			//��������DATA�̶�Ϊ3
				tx_data.data[0] = 0x01;		//��Ŀ����Ϊ1
				//tx_data.data[1] = 0x00; 	//��Ŀ���Ϊ0
				//������
				tx_data.data[1] = (input_info.ans_type[3]<<6)|(input_info.ans_type[2]<<4)|(input_info.ans_type[1]<<2)|(input_info.ans_type[0]);
				//��ֵ
				memcpy(&tx_data.data[2], &input_info.answer, input_info.total_answer_num);
			}
			else							//��������
			{
				pack_question_info(temp_question_buf,&temp_question_buf_len);	//��װinput_info�ṹ�����Ŀ��Ϣ
				tx_data.len = temp_question_buf_len + 0x01;						
				tx_data.data[0] = input_info.total_answer_num;
				memcpy(&tx_data.data[1], temp_question_buf, temp_question_buf_len);
			}
		}
		else if(input_info.score_or_answer == SCORE_INPUT)
		{
			if(input_info.grade_or_score == SCORE_RESULT)		//ͶƱģʽ
			{
				tx_data.type = 0x12;
				tx_data.len = 0x01;
				tx_data.data[0] = input_info.score;
			}
			else if(input_info.grade_or_score == GRADE_RESULT) //����ģʽ
			{
				tx_data.type = 0x13;  //by09.21
				tx_data.len = input_info.total_answer_num * 2 + 1;	 //ÿС��ռ��2�ֽ�
				tx_data.data[0] = input_info.total_answer_num;
				for(i = 0;i < input_info.total_answer_num;i++)
				{
					tx_data.data[1 + 2 * i] = input_info.ans_number[i];
					tx_data.data[2 + 2 * i] = input_info.grade[i];
				}
			}			
		}
		tx_data.xor = XOR_Cal(tx_data.uid, tx_data.len + 0x07);
		memcpy(rf_var.tx_buf, (uint8_t *)&tx_data, tx_data.len + 0x08);
		rf_var.tx_buf[tx_data.len + 0x08] = tx_data.xor;
		rf_var.tx_buf[tx_data.len + 0x09] = tx_data.end;
		rf_var.tx_len = tx_data.len + 0x0A;
		
		simulate_ack.length = rf_var.tx_len;
		memcpy(simulate_ack.data, rf_var.tx_buf, simulate_ack.length);
		my_nrf_tx_start();		//��ʼ����simulate_ack_layer�ṹ���е�����
		
		printf("key_send_handler:");
//		my_uart_trace_dump(simulate_ack.data,simulate_ack.length);	//���ڴ�ӡ
		
//		memset((tx_payload.data + NRF_ACK_PROTOCOL_LEN),0x00,NRF_USEFUL_DATA_LEN);		//��ֹ�ϴη������ݶ���β���Ӱ��
//		memcpy((tx_payload.data + NRF_ACK_PROTOCOL_LEN), rf_var.tx_buf, rf_var.tx_len);
//		my_nrf_transmit_start();	//��ʼ���ģ��ACK��ʽ����tx_payload�������
		rf_var.flag_txing = true;
	}
	else{;}
}

/******************************************************************************
  @����:key_last_question_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void key_last_question_handler(void)
{
	if(input_info.total_answer_num == 1)		//�����������л����
	{	return;	}
	
	if(input_info.score_or_answer == ANSWER_INPUT)								//�������ģʽΪ����
	{
		if(current_answer_num > 0)
			current_answer_num--;
		else
			current_answer_num = input_info.total_answer_num - 1;
		
		switch(input_info.ans_type[current_answer_num])
		{
			case SINGLE_CHOICE:
			case MULTIPLE_CHOICE:
				LCD_display_state.letter_flag |= LETTER_UPDATA_1;
				LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;
				break;
			case TRUE_OR_FALSE:
				LCD_display_state.letter_flag |= LETTER_UPDATA_0;
				LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_1;
			default :
				break;
		}
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;										//��ʾ���
		LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
//		KeyScan_DebugLog("[KeyScan]: current answer number is %d\r\n",current_answer_num + input_info.start_answer_num);
	}
	else if(input_info.score_or_answer == SCORE_INPUT)			
	{
		if(input_info.grade_or_score == SCORE_RESULT)		//����ͶƱģʽ
		{;}
		else if(input_info.grade_or_score == GRADE_RESULT) //С������ģʽ
		{
			if(current_answer_num > 0)
				current_answer_num--;
			else
				current_answer_num = input_info.total_answer_num - 1;
			
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;				//��ʾ���
			LCD_display_state.updata |= SEGMENT_UPDATA;
			LCD_display_handler();							//ÿ��LCD������ֻ����ʾһ��
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;				//��ʾ���ֽ��	
			LCD_display_state.updata |= SEGMENT_UPDATA;
		}
	}
}

/******************************************************************************
  @����:key_next_question_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
static void key_next_question_handler(void)
{
	
	if(input_info.total_answer_num == 1)		//�����������л����
	{	return;	}
	
	if(input_info.score_or_answer == ANSWER_INPUT)								
	{
		current_answer_num ++ ;
		current_answer_num %= input_info.total_answer_num;
		switch(input_info.ans_type[current_answer_num])
		{
			case SINGLE_CHOICE:
			case MULTIPLE_CHOICE:
				LCD_display_state.letter_flag |= LETTER_UPDATA_1;
				LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;
				break;
			case TRUE_OR_FALSE:
				LCD_display_state.letter_flag |= LETTER_UPDATA_0;
				LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_1;
			default :
				break;
		}
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;										//��ʾ���
		LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
//		KeyScan_DebugLog("[KeyScan]: current answer number is %d\r\n",current_answer_num + input_info.start_answer_num);
	}
	else if(input_info.score_or_answer == SCORE_INPUT)			
	{
		if(input_info.grade_or_score == SCORE_RESULT)		//����ͶƱģʽ
		{;}
		else if(input_info.grade_or_score == GRADE_RESULT) //С������ģʽ
		{
			current_answer_num ++ ;
			current_answer_num %= input_info.total_answer_num;
				
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;				//��ʾ���
			LCD_display_state.updata |= SEGMENT_UPDATA;
			LCD_display_handler();							//ÿ��LCD������ֻ����ʾһ��
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;				//��ʾ���ֽ��	
			LCD_display_state.updata |= SEGMENT_UPDATA;
		}
		else {;}
	}

}


/* �ⲿ��������---------------------------------------------------------*/

/******************************************************************************
  @����:init_input_information
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
void init_input_information( void )
{
	tx_data.header = 0x5A;
	tx_data.uid[0]= UID[3];
	tx_data.uid[1]= UID[4];
	tx_data.uid[2]= UID[5];
	tx_data.uid[3]= UID[6];
	tx_data.packnum = 0x00;		//������Э�飬��������ǰ��ţ�����Ϊ�����ֶΣ�Ĭ��0x00
	tx_data.end = 0xCA;
		
	current_answer_num = 0;	
	input_info.score_or_answer = ANSWER_INPUT;												//Ĭ�Ͽ����������
	input_info.start_answer_num = 0;														//Ĭ�����Ϊ0
	input_info.total_answer_num = 1;														//Ĭ��Ϊ��������

	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;													//����Դ���ʾ
	LCD_display_state.letter_flag |= LETTER_UPDATA_0;														//��һ��Ϊѡ����
	LCD_display_state.segment_flag |= SEGMENT_UPDATA_0;										//�����ʾ����
	LCD_display_state.eng_num_flag |= ENG_NUM_UPDATA_1;														//��ʾ����״̬
	LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA | ENG_NUM_UPDATA;
	LCD_display_handler();
}

/******************************************************************************
  @����:clear_input_information
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
void clear_input_information( void )
{
	uint8_t temp_input_mode = input_info.score_or_answer;
	memset(&input_info, 0x00,sizeof(input_info_type_t));									//�������Ĵ�
	input_info.score_or_answer = temp_input_mode;											//���ı䵱ǰ������ģʽ
	input_info.total_answer_num = 1;														//Ĭ��Ϊ��������
	current_answer_num = 0;																	//��ǰ��Ŵ�0��ʼ
	
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;								//����Դ���ʾ
	LCD_display_state.letter_flag |= LETTER_UPDATA_0;										//��һ��Ϊѡ����
	LCD_display_state.segment_flag |= SEGMENT_UPDATA_0;										//�����ʾ����
	LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
}

/******************************************************************************
  @����:app_handler
  @����:
  @����:None
  @���:
  @����:
******************************************************************************/
void app_handler(void)
{
	if(rf_var.flag_rx_ok)		//����н��յ����ݰ�
	{
		memcpy(&rx_data, rf_var.rx_buf, rf_var. rx_len - 2);
		
		switch(rx_data.type)
		{
			case SINGLE_QUESTION_ACT_COMMAND:		
				single_question_act_command_handler();
				break;
			case MULTIPLE_QUESTION_ACT_COMMAND:		
				multiple_question_act_command_handler();
				break;
			case MUTUAL_VOTE_ACT_COMMAND:		
				mutual_vote_act_command_handler();		
				break;
			case SYS_OFF_ACT_COMMAND:		
				sys_off_act_command_handler();
				break;
			case GROUP_GRADE_ACT_COMMAND:		
				group_grade_act_command_handler();				
				break;
			case ACTIVITY_END_ACT_COMMAND:		
				activity_end_act_command_handler();
				break;
			case RECEIVE_OK_ACT_COMMAND:		
				receive_ok_act_command_handler();
				break;
			case HEARTBEAT_PACKET_ACT_COMMAND:		
				heartbeat_packet_act_command_handler();
				break;
			case STUDENT_ID_MATCH_ACT_COMMAND:		
				student_id_match_act_command_handler();
				break;
			case EMPTY_PACKET_ACT_COMMAND:		
				empty_packet_act_command_handler();
				break;
			default	:		
				break;
		}
		rf_var.flag_rx_ok = false;
		memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
		rf_var.rx_len = 0x00;
	}
}

/******************************************************************************
  @����:button_handler2
  @����:button_handler �� �ٷ�������ͬ������button_handler2
  @����:None
  @���:
  @����:
******************************************************************************/
void button_handler2(uint8_t key_value)
{
	
	g_variable.key_value = key_value;

	switch(key_value)
	{
		case KEY_A_1:
		case KEY_B_2:
		case KEY_C_3:
		case KEY_D_4: 
		case KEY_E_5:
		case KEY_F_6:
			if(input_info.score_or_answer == ANSWER_INPUT)		//�������ģʽΪ����
			{
				key_letter_input_handler();
			}
			else if(input_info.score_or_answer == SCORE_INPUT)
			{
				key_digit_input_handler();
			}
			else {;}
			break;
		case KEY_G_7:
			break;
		case KEY_H_8:
		case KEY_I_9:
			if(input_info.score_or_answer == ANSWER_INPUT)			//�������ģʽΪ����
			{
				key_judge_input_handler();
			}
			else {;}
			break;
		case KEY_J_0:
			break;
		case KEY_SYS_POWER:	
			break;
		case KEY_FN:	
			key_fn_handler();
			break;
		case KEY_CLEAR:	
			key_clear_handler();
			break;
		case KEY_SEND:   
			key_send_handler();
			break;
		case KEY_LAST_QUESTION:	
			key_last_question_handler();
			break;			
		case KEY_NEXT_QUESTION:	
			key_next_question_handler();
			break;
		default:
			break;
	}
}

/** @} */
/** @} */


