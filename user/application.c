

/*-------------------------------- Include File -----------------------------------------*/
#include "application.h"
/*-------------------------------- Macro definition -------------------------------------*/
//#define APP_DEBUG
#ifdef APP_DEBUG
#define app_debug  debug_printf   
#else  
#define app_debug(...)                    
#endif 
/*-------------------------------- Local Variable ----------------------------------------*/

application_struct_t				application_struct;
										   
/*-------------------------------- Gocal Variable ----------------------------------------*/
application_struct_t				*APP = &application_struct;


/*-------------------------------- Function Declaration ----------------------------------*/


//2.4G��������
static void single_question_command_handler(void);
static void multiple_question_command_handler(void);
static void mutual_vote_command_handler(void);
static void sys_off_command_handler(void);
static void start_answer_command_handler(void);
static void group_grade_command_handler(void);
static void activity_end_command_handler(void);
static void receive_ok_command_handler(void);
static void heartbeat_command_handler(void);
static void student_id_match_command_handler(void);
static void empty_packet_command_handler(void);

//�����������
static void key_letter_input_handler(void);
static void key_digit_input_handler(void);
static void key_judge_input_handler(void);
static void key_fn_handler(void);
static void key_clear_handler(void);
static void key_send_handler(void);
static void key_last_question_handler(void);
static void key_next_question_handler(void);

//��Ŀ����ȡ�ͷ�װ
static void fetch_question_info(uint8_t *question_buf, uint8_t question_number);
static void pack_question_info(uint8_t *question_buf,uint8_t *question_buf_len);

/*----------------------------------------------------------------------------------------*/

void APP_init(void)
{	
	APP->answer_ing_flg  = false;
	APP->key_send_allow_flg = true;			//���������ͼ���Ч
	APP->last_cmd_type      = APP_CMD_TYPE_NULL;
	APP->DisplayVerFlg = false;
	
	

}

void test_command_handler(void)
{	
	RADIO->rx_window_on = (uint16_t)((rx_data.data[0]<<8) | rx_data.data[1]);
	RADIO->rx_window_off = (uint16_t)((rx_data.data[2]<<8) | rx_data.data[3]);
	RADIO->rx_window_add = (uint16_t)((rx_data.data[4]<<8) | rx_data.data[5]);
	RADIO->ack_delay = (uint16_t)((rx_data.data[6]<<8) | rx_data.data[7]);
}

void APP_CmdHandler(void)
{
	if(rf_var.flag_rx_ok)		//����н��յ����ݰ�
	{
		if(rf_var. rx_len < 3)
			return;
		memcpy(&rx_data, rf_var.rx_buf, rf_var. rx_len - 2);		
		
		switch(rx_data.type)
		{
			case 0x1B:		
				test_command_handler();
				break;    
			case SINGLE_QUESTION_COMMAND:		
				single_question_command_handler();
				break;
			case MULTIPLE_QUESTION_COMMAND:		
				multiple_question_command_handler();
				break;
			case MUTUAL_VOTE_COMMAND:		
				mutual_vote_command_handler();		
				break;
			case SYS_OFF_COMMAND:		
				sys_off_command_handler();
				break;
			case START_ANSWER_COMMAND:		
				start_answer_command_handler();
				break;
			case GROUP_GRADE_COMMAND:		
				group_grade_command_handler();				
				break;
			case ACTIVITY_END_COMMAND:		
				activity_end_command_handler();
				break;
			case RECEIVE_OK_COMMAND:		
				receive_ok_command_handler();
				break;
			case HEARTBEAT_COMMAND:		
				heartbeat_command_handler();
				break;
			case STUDENT_ID_MATCH_COMMAND:		
				student_id_match_command_handler();
				break;
			case EMPTY_PACKET_COMMAND:		
				empty_packet_command_handler();
				break;
			default	:		
				break;
		}
		rf_var.flag_rx_ok = false;
		memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
		rf_var.rx_len = 0x00;
	}
}


void APP_KeyHandler(void)
{
	uint8_t temp_answer;
	
	KEY_Scan();
//	app_debug("KEY->value %d \r\n",KEY->value);
	
	if(KEY->press_flg)				
	{
		KEY->press_flg = false;
		
		TIMER_SysSleepStop();		
		TIMER_SysSleepStart();		//�а�������ʱ���¿���ϵͳ������ʱ��
		
		switch(KEY->value)
		{
			case KEY_A_1:
			case KEY_B_2:
			case KEY_C_3:
			case KEY_D_4: 
			case KEY_E_5:	
			case KEY_F_6:
				if(ANSWER.score_or_answer == ANSWER_INPUT)			//�������ģʽΪ����
				{
					key_letter_input_handler();
				}
				else if(ANSWER.score_or_answer == SCORE_INPUT)
				{
					key_digit_input_handler();
				}
				break;
			case KEY_G_7:
				break;
			case KEY_H_8:
			case KEY_I_9:
				if(ANSWER.score_or_answer == ANSWER_INPUT)			//�������ģʽΪ����
				{
					key_judge_input_handler();
				}
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
}

void key_letter_input_handler(void)
{
	uint8_t temp_answer;
	
	if(ANSWER.total_num == 0)               //Ĭ�Ͽ���Ϊ��������������Ϊ1
	{
		//����ǰ����Ϊ�ж��⣬������������Լ�LCD��ʾ
		if(ANSWER.type[ANSWER.current_num] == TRUE_OR_FALSE)
		{
			ANSWER.answer[ANSWER.current_num].choice_answer = 0x00;
			ANSWER.answer[ANSWER.current_num].true_or_false = 0x00;
			LCD_display_true_false(0x00);			
		}
		
		//����ٴΰ������ʾ
		ANSWER.answer[ANSWER.current_num].choice_answer ^= (0x01 << (KEY->value - 1));
		if( ANSWER.answer[ANSWER.current_num].choice_answer == A ||	ANSWER.answer[ANSWER.current_num].choice_answer == B ||
			ANSWER.answer[ANSWER.current_num].choice_answer == C ||	ANSWER.answer[ANSWER.current_num].choice_answer == D ||
			ANSWER.answer[ANSWER.current_num].choice_answer == E ||	ANSWER.answer[ANSWER.current_num].choice_answer == F ||
			ANSWER.answer[ANSWER.current_num].choice_answer == G ||	ANSWER.answer[ANSWER.current_num].choice_answer == H ||
			ANSWER.answer[ANSWER.current_num].choice_answer == 0)
		{
			ANSWER.type[ANSWER.current_num] = SINGLE_CHOICE;
		}
		else if(ANSWER.answer[ANSWER.current_num].choice_answer )			//��ѡ��
		{	
			ANSWER.type[ANSWER.current_num] = MULTIPLE_CHOICE;	
		}
	}
	else  //��������
	{
		switch(ANSWER.type[ANSWER.current_num])
		{
			case SINGLE_CHOICE:
				temp_answer = (0x01 << (KEY->value - 1))&ANSWER.range[ANSWER.current_num];
				if(temp_answer)		//�ж������Ƿ�������Χ��
				{
					if(ANSWER.answer[ANSWER.current_num].choice_answer & temp_answer)
						ANSWER.answer[ANSWER.current_num].choice_answer ^= temp_answer;		//��������ɾ��
					else
						ANSWER.answer[ANSWER.current_num].choice_answer = temp_answer;
				}
				break;
			case MULTIPLE_CHOICE:
				ANSWER.answer[ANSWER.current_num].choice_answer ^= (0x01 << (KEY->value - 1));
				ANSWER.answer[ANSWER.current_num].choice_answer &= ANSWER.range[ANSWER.current_num];
				break;
			default :
				break;
		}
	}
	
	if(ANSWER.type[ANSWER.current_num] != TRUE_OR_FALSE)
	{
		LCD_diplay_letter(ANSWER.answer[ANSWER.current_num].choice_answer);
	}
}

void key_digit_input_handler(void)
{		
	if(ANSWER.grade_or_score == SCORE_RESULT)					//����ͶƱģʽ
	{
		 if(6 == KEY->value)							//���ַ�Χ1-5
			 return;
		ANSWER.score = KEY->value;
		LCD_display_grade_value(ANSWER.score);							//��ʾ����
	}
	else if(ANSWER.grade_or_score == GRADE_RESULT) 				//С������ģʽ
	{
		 if(6 == KEY->value)							//���ַ�Χ1-5
			 return;
		ANSWER.grade[ANSWER.current_num] = KEY->value;
		LCD_display_grade_value(ANSWER.grade[ANSWER.current_num]);			//ͶƱ���
	}
}

void key_judge_input_handler(void)
{
	if(ANSWER.total_num == 0)
	{
		//����ǰ���Ͳ����ж��⣬��תΪ�ж��⣬������������ݼ�LCD��ʾ
		if(ANSWER.type[ANSWER.current_num] != TRUE_OR_FALSE)
		{
			ANSWER.type[ANSWER.current_num] = TRUE_OR_FALSE;	
			ANSWER.answer[ANSWER.current_num].true_or_false = 0x00;
			ANSWER.answer[ANSWER.current_num].choice_answer = 0x00;
			LCD_diplay_letter(0x00);			
		}
		
		if(KEY->value == 8)
		{
			ANSWER.answer[ANSWER.current_num].true_or_false = ANSWER_TRUE;	
		}
		else
		{
			ANSWER.answer[ANSWER.current_num].true_or_false = ANSWER_FALSE;
		}
		
		LCD_display_true_false(ANSWER.answer[ANSWER.current_num].true_or_false);		//LCD��ʾ
	}
	else if(ANSWER.type[ANSWER.current_num] == TRUE_OR_FALSE)
	{
		if(KEY->value == 8)
		{
			ANSWER.answer[ANSWER.current_num].true_or_false = ANSWER_TRUE;
		}
		else
		{
			ANSWER.answer[ANSWER.current_num].true_or_false = ANSWER_FALSE;
		}
		
		LCD_display_true_false(ANSWER.answer[ANSWER.current_num].true_or_false);		//LCD��ʾ
	}
}

void key_fn_handler(void)
{
	
}


void key_clear_handler(void)
{
	ANSWER.answer[ANSWER.current_num].true_or_false = 0x00;
	ANSWER.answer[ANSWER.current_num].choice_answer = 0x00;
	ANSWER.grade[ANSWER.current_num] = 0x00; 
	ANSWER.score = 0x00;
		
	//����������״̬ʱ����ʾRTC���������ֲ��ֲ����
	if(APP->answer_ing_flg)
	{
		LCD_clear_segment();
	}
	LCD_diplay_letter(0x00);
	LCD_display_true_false(0x00);
	LCD_display_student_id(STUDENT_ID);
	
	if(ANSWER.score_or_answer == SCORE_INPUT)	//�����ǰΪ����/ͶƱģʽ����ʾ����ֵΪ0
	{
		LCD_display_grade_value(0);
	}
	
	/* û�·���Ŀ������ʾ��� */
	if(0 == ANSWER.total_num)
		return;
	/* ����ͶƱ����ţ����Բ���ʾ */
	if( SCORE_INPUT == ANSWER.score_or_answer && SCORE_RESULT == ANSWER.grade_or_score )
		return;
	
	//����ն������ʾ���
	LCD_display_question_num(ANSWER.number[ANSWER.current_num]);
}


void key_send_handler(void)
{
	uint8_t i;
	uint8_t temp_if_answer = 0;			//�ж��Ƿ�����
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];	
	uint8_t temp_question_buf_len;	

	if(false == APP->key_send_allow_flg)
		return;
	
	tx_data.header = 0x5A;					//��ֵֹ��������б��۸ģ�ÿ�η���ǰ���¸�ֵ
	tx_data.uid[0]= DTQ_UID[0];
	tx_data.uid[1]= DTQ_UID[1];
	tx_data.uid[2]= DTQ_UID[2];
	tx_data.uid[3]= DTQ_UID[3];
//	tx_data.packnum = 0x00;		
	tx_data.end = 0xCA;
	
	//�ж��Ƿ���������Ϣ���ǿղ���������
//	for(i = 0 ;i < QUESTION_MAX_NUMBER;i++)
//		temp_if_answer |= ANSWER.answer[i].true_or_false | ANSWER.answer[i].choice_answer 
//			| ANSWER.score | ANSWER.grade[i];
	temp_if_answer = 1;		//���������յ�����ָ�����������Ҳ�����ύ��Ŀ
	
	if(temp_if_answer && APP->answer_ing_flg)		//������Ϣ�ǿգ������������յ���Ŀ��Ϣ��
	{
		if(ANSWER.score_or_answer == ANSWER_INPUT)								//�������ģʽΪ����
		{
			if(ANSWER.total_num == 0)
				tx_data.type = 0x10;				//��������
			else
				tx_data.type = 0x11;				//��������

			if(tx_data.type == 0x10)		
			{
				tx_data.len = 0x03;			//��������DATA�̶�Ϊ3
				tx_data.data[0] = 0x01;		//��Ŀ����Ϊ1
				//tx_data.data[1] = 0x00; 	//��Ŀ���Ϊ0
				//������
				tx_data.data[1] = (ANSWER.type[3]<<6)|(ANSWER.type[2]<<4)|(ANSWER.type[1]<<2)|(ANSWER.type[0]);
				//��ֵ
				memcpy(&tx_data.data[2], &ANSWER.answer, ANSWER.total_num);
			}
			else							//��������
			{
				pack_question_info(temp_question_buf,&temp_question_buf_len);	//��װANSWER�ṹ�����Ŀ��Ϣ
				tx_data.len = temp_question_buf_len + 0x01;						
				tx_data.data[0] = ANSWER.total_num;
				memcpy(&tx_data.data[1], temp_question_buf, temp_question_buf_len);
			}
		}
		else if(ANSWER.score_or_answer == SCORE_INPUT)
		{
			if(ANSWER.grade_or_score == SCORE_RESULT)		//ͶƱģʽ
			{
				tx_data.type = 0x12;
				tx_data.len = 0x01;
				tx_data.data[0] = ANSWER.score;
			}
			else if(ANSWER.grade_or_score == GRADE_RESULT) //����ģʽ
			{
				tx_data.type = 0x13;  //by09.21
				tx_data.len = ANSWER.total_num * 2 + 1;	 //ÿС��ռ��2�ֽ�
				tx_data.data[0] = ANSWER.total_num;
				for(i = 0;i < ANSWER.total_num;i++)
				{
					tx_data.data[1 + 2 * i] = ANSWER.number[i];
					tx_data.data[2 + 2 * i] = ANSWER.grade[i];
				}
			}	
		}
		
		tx_data.xor = XOR_Cal(tx_data.uid, tx_data.len + 0x07);
		memcpy(rf_var.tx_buf, (uint8_t *)&tx_data, tx_data.len + 0x08);
		rf_var.tx_buf[tx_data.len + 0x08] = tx_data.xor;
		rf_var.tx_buf[tx_data.len + 0x09] = tx_data.end;
		rf_var.tx_len = tx_data.len + 0x0A;		
		
		/* ��Ҫ���͵���Ч���ݷ���TRANSPORT.data�� */
		TRANSPORT.data_len  =  NRF_ACK_PROTOCOL_LEN + rf_var.tx_len;
		
		TRANSPORT.data[9]++;
		TRANSPORT.data[10]++;
		TRANSPORT.data[14] = rf_var.tx_len;
		memcpy((TRANSPORT.data + 15), rf_var.tx_buf, rf_var.tx_len);
		TRANSPORT.data[NRF_ACK_PROTOCOL_LEN + rf_var.tx_len - 2] = XOR_Cal(TRANSPORT.data+1,TRANSPORT.data_len-3);
		TRANSPORT.data[NRF_ACK_PROTOCOL_LEN + rf_var.tx_len - 1] = 0x21;
		
//		tmp_debug("tx_len:%d \r\n",TRANSPORT.data_len);
//		for(i = 0;i < TRANSPORT.data_len;i++)
//			tmp_debug("%02X ",TRANSPORT.data[i]);
//		tmp_debug("\r\n");

		//��ʼ����TRANSPORT.data������
		TRANSPORT.tx_ok_flag = false;
		TRANSPORT.tx_ing_flag = false;

		RADIO_StartTx();		
	}
}


void key_last_question_handler(void)
{
	/* ��������ֱ���˳� */
	if(0 == ANSWER.total_num)
		return;
	
	if(ANSWER.score_or_answer == ANSWER_INPUT)								//�������ģʽΪ����
	{
		if(ANSWER.current_num == 0)
			ANSWER.current_num = ANSWER.total_num - 1;
		else
			ANSWER.current_num--;
			
		switch(ANSWER.type[ANSWER.current_num])
		{
			case SINGLE_CHOICE:
			case MULTIPLE_CHOICE:
				LCD_diplay_letter(ANSWER.answer[ANSWER.current_num].choice_answer);
				LCD_display_true_false(0x00);	
				LCD_display_question_num(ANSWER.number[ANSWER.current_num]);	
				break;
			case TRUE_OR_FALSE:
				LCD_diplay_letter(0x00);
				LCD_display_true_false(ANSWER.answer[ANSWER.current_num].choice_answer);	
				LCD_display_question_num(ANSWER.number[ANSWER.current_num]);	
			default :
				break;
		}
	}
	else if(ANSWER.score_or_answer == SCORE_INPUT)			
	{
		if(ANSWER.grade_or_score == SCORE_RESULT)		//����ͶƱģʽ
		{;}
		else if(ANSWER.grade_or_score == GRADE_RESULT) //С������ģʽ
		{
			if(ANSWER.current_num == 0)
				ANSWER.current_num = ANSWER.total_num - 1;
			else
				ANSWER.current_num--;
			
			LCD_display_question_num(ANSWER.number[ANSWER.current_num]);
			LCD_display_grade_value(ANSWER.grade[ANSWER.current_num]);
		}
	}
}

void key_next_question_handler(void)
{
	/* ��������ֱ���˳� */
	if(0 == ANSWER.total_num)
		return;
	
	if(ANSWER.score_or_answer == ANSWER_INPUT)								
	{
		ANSWER.current_num ++ ;
		ANSWER.current_num %= ANSWER.total_num;
		switch(ANSWER.type[ANSWER.current_num])
		{
			case SINGLE_CHOICE:
			case MULTIPLE_CHOICE:
				LCD_diplay_letter(ANSWER.answer[ANSWER.current_num].choice_answer);
				LCD_display_true_false(0x00);	
				LCD_display_question_num(ANSWER.number[ANSWER.current_num]);	
				break;
			case TRUE_OR_FALSE:
				LCD_diplay_letter(0x00);
				LCD_display_true_false(ANSWER.answer[ANSWER.current_num].choice_answer);	
				LCD_display_question_num(ANSWER.number[ANSWER.current_num]);
			default :
				break;
		}
	}
	else if(ANSWER.score_or_answer == SCORE_INPUT)			
	{
		if(ANSWER.grade_or_score == SCORE_RESULT)		//����ͶƱģʽ
		{;}
		else if(ANSWER.grade_or_score == GRADE_RESULT) //С������ģʽ
		{
			ANSWER.current_num ++ ;
			ANSWER.current_num %= ANSWER.total_num;
			
			LCD_display_question_num(ANSWER.number[ANSWER.current_num]);
			LCD_display_grade_value(ANSWER.grade[ANSWER.current_num]);
		}
	}
}






void single_question_command_handler(void)
{
	//�յ�����/��������ظ�����������ĿҪ�·�
	POWER->request_data_flg = true;
}

void multiple_question_command_handler(void)
{
	uint8_t i;
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];
	uint8_t temp_question_number;
	memset(temp_question_buf,0x00,QUESTION_MAX_BYTE_LEN);       //����
	
	//�յ�����/��������ظ�����������ĿҪ�·�
	POWER->request_data_flg = true;
	APP->answer_ing_flg  = true;
	APP->last_cmd_type = APP_CMD_TYPE_QUESTION;

	
	if(rx_data.data[0] > QUESTION_MAX_NUMBER || rx_data.data[0] < 1)			//��Ŀ�����Ϸ�
	{	return;	 }
	
	temp_question_number =  rx_data.data[0];
	memcpy(temp_question_buf, rx_data.data+1, rx_data.len - 1);           //ȡ����Ŀ��Ч��Ϣ������temp_question_buf
	fetch_question_info(temp_question_buf, temp_question_number);				//������Ŀ��Ϣ��ANSWER�ṹ��
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )									//��������
	{
		ANSWER.answer[i].true_or_false = 0;
		ANSWER.answer[i].choice_answer = 0;
		ANSWER.grade[i] = 0;
	}	
	
	ANSWER.score_or_answer = ANSWER_INPUT;									//����Ϊ��������
	ANSWER.total_num = rx_data.data[0];								//��ȡ����
	ANSWER.start_num = ANSWER.number[0];						//��ȡ�����
	ANSWER.current_num = 0;														//��ǰ��Ŀ����Ϊ0��

	LCD_clear_dot();														//���RTC��ʾ�е�ð��
	LCD_diplay_letter(0x00);														//�����ĸ��ʾ
	LCD_display_true_false(0x00);														//����Դ���ʾ
	LCD_clear_segment();													//����������ʾ
	LCD_display_student_id(STUDENT_ID);
	
	LCD_display_question_num(ANSWER.number[ANSWER.current_num]);			//��ʾ��ǰ�������
}

void mutual_vote_command_handler(void)
{
	
	//�յ�����/��������ظ�����������ĿҪ�·�
	POWER->request_data_flg = true;
	APP->answer_ing_flg  = true;
	APP->last_cmd_type = APP_CMD_TYPE_QUESTION;

	
	ANSWER.score_or_answer = SCORE_INPUT;						//��������ģʽ
	ANSWER.grade_or_score = SCORE_RESULT;						//������ΪͶƱ	
	ANSWER.score = 0;											//ͶƱ�����ʼֵΪ0
	
	LCD_clear_dot();														//���RTC��ʾ�е�ð��
	LCD_diplay_letter(0x00);														//�����ĸ��ʾ
	LCD_display_true_false(0x00);														//����Դ���ʾ
	LCD_clear_segment();													//����������ʾ	
	
	LCD_display_student_id(STUDENT_ID);
	LCD_display_grade_value(ANSWER.score);	
}

void sys_off_command_handler(void)
{	
	POWER_Off();		//���汾�����߼����ң���һ���ػ���������
}

void start_answer_command_handler(void)
{
	
}


void group_grade_command_handler(void)
{
	uint8_t i;
	
	//�յ�����/��������ظ�����������ĿҪ�·�
	POWER->request_data_flg = true;
	APP->answer_ing_flg  = true;

	
	if(rx_data.data[0] < 1)		//�������Ϸ�
	{	return;	}
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )				//��������
	{
		ANSWER.answer[i].true_or_false = 0;
		ANSWER.answer[i].choice_answer = 0;
		ANSWER.grade[i] = 0;
	}
	
	ANSWER.total_num = rx_data.data[0];		//С������Ŀ
	for(i = 0;i < ANSWER.total_num;i++)
	{
		ANSWER.number[i] = i + 1;				//��Ŵ�1��ʼ����+1
	}
	
	ANSWER.score_or_answer = SCORE_INPUT;					//��������ģʽ
	ANSWER.grade_or_score = GRADE_RESULT;					//������Ϊ����
	
	ANSWER.current_num = 0;										//��ǰ��Ŀ����Ϊ0
	ANSWER.start_num = ANSWER.number[0];		//��ʼ���Ϊ��һ��
	
	LCD_clear_dot();														//���RTC��ʾ�е�ð��
	LCD_diplay_letter(0x00);														//�����ĸ��ʾ
	LCD_display_true_false(0x00);														//����Դ���ʾ
	LCD_clear_segment();													//����������ʾ
	LCD_display_student_id(STUDENT_ID);
	
	LCD_display_grade_value(ANSWER.grade[ANSWER.current_num]);					//��ʾ��ǰ���ֽ��
	LCD_display_question_num(ANSWER.number[ANSWER.current_num]);			//��ʾ��ǰ�������
}

void activity_end_command_handler(void)
{
	
	uint8_t i;
	
	//�ظ��յ�����ָ���������
	if(APP_CMD_TYPE_CLEAR == APP->last_cmd_type)
		return;
	
	APP->last_cmd_type = APP_CMD_TYPE_CLEAR;
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )		//�������������Ϣ
	{
		ANSWER.answer[i].true_or_false = 0;
		ANSWER.answer[i].choice_answer = 0;
	}
	
	APP->answer_ing_flg  = false;				//�������������������־����ʼRTC��ʾ	
	
	ANSWER.total_num = 0;						//�л�Ϊ��������
	ANSWER.number[0] = 0;							//�����������Ϊ0
	ANSWER.score_or_answer = ANSWER_INPUT;				//����Ϊ��������
	
	LCD_clear_dot();										//���RTC��ʾ�е�ð��
	LCD_diplay_letter(0x00);										//�����ĸ��ʾ
	LCD_display_true_false(0x00);										//����Դ���ʾ
	LCD_clear_segment();									//����������ʾ
	LCD_display_student_id(STUDENT_ID);								//ѧ�Ų����
}

void receive_ok_command_handler(void)
{
	
}

void heartbeat_command_handler(void)
{
	uint16_t tmp_year;
	uint8_t  rtc_year,rtc_month,rtc_day,rtc_hour,rtc_min,rtc_sec;
	uint8_t  rtc_data[7];
	bool     rtc_calibrate_flg;
	
	//���ʱ���֡���ȫ��0��˵�����������͵�ʱ���Ǵ���ġ�������У׼
	if((0 == rx_data.data[5]) && (0 == rx_data.data[6]) && (0 == rx_data.data[7]))
	{
		return;
	}
	
	//��ȡ��������ʱ����Ϣ, ע����תΪBCD��
	tmp_year  = rx_data.data[0] | rx_data.data[1]<<8;			
	rtc_year  = (tmp_year%100/10 << 4)	  |(tmp_year%10);			//���ֻ����λ��Ч������2016��ֻ16��Ч��		
	rtc_month = (rx_data.data[2]/10 << 4)|(rx_data.data[2]%10);
	rtc_day   = (rx_data.data[3]/10 << 4)|(rx_data.data[3]%10);
	rtc_hour  = (rx_data.data[4]/10 << 4)|(rx_data.data[4]%10);
	rtc_min   = (rx_data.data[5]/10 << 4)|(rx_data.data[5]%10);
	rtc_sec   = (rx_data.data[6]/10 << 4)|(rx_data.data[6]%10);
	
	//��ȡRTCоƬ��ʱ����Ϣ
	RTC->read_data();	
	
	//��������RTCʱ��Աȣ�������XX�룬�����У׼
	if(rtc_year == rtc_time.year)
	{
		if(rtc_month == rtc_time.month)
		{
			if(rtc_day == rtc_time.day)
			{
				if(rtc_hour == rtc_time.hour)
				{
					if(rtc_min == rtc_time.minute)
					{
						if((rtc_sec&0xF0) == (rtc_time.minute&0xF0))		//���ӵ�BCD���ʮλ����ȣ������У��
						{
							rtc_calibrate_flg = false;
						}
						else
						{
							rtc_calibrate_flg = true;
						}
					}
					else
					{
						rtc_calibrate_flg = true;
					}
				}
				else
				{
					rtc_calibrate_flg = true;
				}
			}
			else
			{
				rtc_calibrate_flg = true;
			}
		}
		else
		{
			rtc_calibrate_flg = true;
		}
	}
	else
	{
		rtc_calibrate_flg = true;
	}
	
	if(rtc_calibrate_flg)
	{
		rtc_calibrate_flg = false;
		
		rtc_data[0] = rtc_sec;			//��BCD����ʽд��RTCоƬ 
		rtc_data[1] = rtc_min;
		rtc_data[2] = rtc_hour;
		rtc_data[3] = rtc_day;
		rtc_data[4] = rtc_time.week;
		rtc_data[5] = rtc_month;
		rtc_data[6] = rtc_year;
		RTC_WriteNReg(0x02,rtc_data, 7);
	}
}

void student_id_match_command_handler(void)
{
	
}

void empty_packet_command_handler(void)
{
	
}


/******************************************************************************
  @����:fetch_question_info
  @����:�ӽ��յ���2.4G�������������Ŀ��Ϣ����ANSWER�ṹ����
  @����:
  @���:
  @����:
******************************************************************************/
void fetch_question_info(uint8_t *question_buf, uint8_t question_number)
{
	uint8_t i;	
   for(i=0;i<question_number;i++)
   {
     ANSWER.number[i]=question_buf[2*i];
	 ANSWER.type[i]=(question_buf[2*i+1])>>6;
	 ANSWER.range[i]=(question_buf[2*i+1])&0x3F;
   }
}

/******************************************************************************
  @����:pack_question_info
  @����:��Ŀ��Ϣ����Э����ANSWER�ṹ�����ݣ�����2.4G���ͳ�ȥ
  @����:
  @���:
  @����:
******************************************************************************/
void pack_question_info(uint8_t *question_buf,uint8_t *question_buf_len)
{
	uint8_t i,j,k;
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];			
	uint8_t temp_question_buf_len;

	temp_question_buf_len=ANSWER.total_num;
	for(i=0;i<temp_question_buf_len;i++)
	{
		temp_question_buf[2*i]=ANSWER.number[i];
		temp_question_buf[2*i+1]=(ANSWER.type[i]<<6)&0xC0|((ANSWER.answer[i].choice_answer & 0x3F)|(ANSWER.answer[i].true_or_false & 0x3F))  ;
	}

	*question_buf_len = 2*temp_question_buf_len;
	memcpy(question_buf,temp_question_buf,*question_buf_len);

}






