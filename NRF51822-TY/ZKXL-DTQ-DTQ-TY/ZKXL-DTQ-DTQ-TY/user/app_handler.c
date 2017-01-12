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


/* 内部函数定义---------------------------------------------------------*/

/******************************************************************************
  @函数:single_question_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void single_question_act_command_handler(void)
{
	;
}

/******************************************************************************
  @函数:multiple_question_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void multiple_question_act_command_handler(void)
{
	uint8_t i;
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];
	uint8_t temp_question_number;
	memset(temp_question_buf,0x00,QUESTION_MAX_BYTE_LEN);       //清零
	
	if(rx_data.data[0] > QUESTION_MAX_NUMBER || rx_data.data[0] < 1)			//题目数不合法
	{	return;	 }
	
	temp_question_number =  rf_var.rx_buf[8];
	memcpy(temp_question_buf, rf_var.rx_buf+9, rf_var.rx_buf[7] - 1);           //取出题目有效信息拷贝到temp_question_buf
	fetch_question_info(temp_question_buf, temp_question_number);				//解析题目信息到input_info结构体
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )									//清空输入答案
	{
		input_info.answer[i].true_or_false = 0;
		input_info.answer[i].choice_answer = 0;
		input_info.grade[i] = 0;
	}	
	
	input_info.score_or_answer = ANSWER_INPUT;									//设置为作答输入
	input_info.total_answer_num = rx_data.data[0];								//获取题数
	input_info.start_answer_num = input_info.ans_number[0];						//获取首题号

	LCD_clear_segment();														//清除数码管显示
	LCD_display_state.eng_num_flag 	  |= ENG_NUM_UPDATA_1;						//显示输入状态
	LCD_display_state.segment_flag 	  |= SEGMENT_UPDATA_2;						//显示题号
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;					//清除对错显示
	LCD_display_state.letter_flag  	  |= LETTER_UPDATA_0;                 		//清除字母显示
	current_answer_num = 0;														//当前题目索引为0，
	LCD_display_state.updata |= SEGMENT_UPDATA | ENG_NUM_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
	LCD_display_handler();														//立即更新LCD显示，否则上次题目会产生影响
}

/******************************************************************************
  @函数:mutual_vote_act_command_handler
  @描述:互动投票
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void mutual_vote_act_command_handler(void)
{
	input_info.score_or_answer = SCORE_INPUT;						//数字输入模式
	input_info.grade_or_score = SCORE_RESULT;						//输入结果为投票	
	input_info.score = 0;
	LCD_display_state.eng_num_flag 	  |= ENG_NUM_UPDATA_1;				 //显示输入状态
	LCD_display_state.segment_flag 	  |= SEGMENT_UPDATA_0;				 //清除所有数字和小数点	
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;			 //清除对错显示
	LCD_display_state.letter_flag  	  |= LETTER_UPDATA_0;                //清除字母显示
	LCD_display_state.updata |= SEGMENT_UPDATA | ENG_NUM_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;	
	LCD_display_handler();	
}

/******************************************************************************
  @函数:sys_off_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
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
  @函数:group_grade_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void group_grade_act_command_handler(void)
{
	uint8_t i;
	if(rx_data.data[0] < 1)		//组数不合法
	{	return;	}
	
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )				//清空输入答案
	{
		input_info.answer[i].true_or_false = 0;
		input_info.answer[i].choice_answer = 0;
		input_info.grade[i] = 0;
	}
	
	input_info.total_answer_num = rx_data.data[0];		//小组总数目
	for(i = 0;i < input_info.total_answer_num;i++)
	{
		input_info.ans_number[i] = i + 1;				//组号从1开始依次+1
	}
	
	input_info.score_or_answer = SCORE_INPUT;					//数字输入模式
	input_info.grade_or_score = GRADE_RESULT;					//输入结果为评分
	
	current_answer_num = 0;										//当前题目索引为0
	input_info.start_answer_num = input_info.ans_number[0];		//起始题号为第一题
	LCD_display_state.segment_flag   |= SEGMENT_UPDATA_2 | SEGMENT_UPDATA_1;		//显示题号
	LCD_display_state.eng_num_flag 	 |= ENG_NUM_UPDATA_1;							//显示输入状态
	LCD_display_state.letter_flag  	 |= LETTER_UPDATA_0;                 		//清除字母显示
	LCD_display_state.true_false_flag = TRUE_FALSE_UPDATA_0;						//清除对错显示			
	LCD_display_state.updata |= SEGMENT_UPDATA | ENG_NUM_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA ;	

	LCD_display_handler();										//立即更新LCD显示
}

/******************************************************************************
  @函数:activity_end_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void activity_end_act_command_handler(void)
{
	uint8_t i;
	printf("activity_end_act_command_handler \r\n");
	for( i = 0; i < QUESTION_MAX_NUMBER; i++ )		//清空所有输入信息
	{
		input_info.answer[i].true_or_false = 0;
		input_info.answer[i].choice_answer = 0;
	}
	
	input_info.total_answer_num = 1;						//切换为单题作答
	input_info.ans_number[0] = 0;								//单题作答题号为0
	input_info.score_or_answer = ANSWER_INPUT;				//设置为作答输入
	LCD_diplay_english();									//显示英
	
	LCD_clear_letter();
	LCD_clear_true_and_false();
	LCD_clear_segment();
}

/******************************************************************************
  @函数:receive_ok_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void receive_ok_act_command_handler(void)
{
	
}

/******************************************************************************
  @函数:heartbeat_packet_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void heartbeat_packet_act_command_handler(void)
{
	
}

/******************************************************************************
  @函数:student_id_match_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void student_id_match_act_command_handler(void)
{

}

/******************************************************************************
  @函数:empty_packet_act_command_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void empty_packet_act_command_handler(void)
{

}

/******************************************************************************
  @函数:key_fn_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void key_fn_handler(void)
{
	;
}

/******************************************************************************
  @函数:key_letter_input_handler
  @描述:字母输入相关的按键处理函数
  @输入:
  @输出:
  @调用:
******************************************************************************/
static void key_letter_input_handler(void)
{
	uint8_t temp_answer;
	if(input_info.total_answer_num == 1)               //默认开机为单题作答总题数为1
	{
		if(input_info.ans_type[current_answer_num] == TRUE_OR_FALSE)  //判断题
		{
			input_info.answer[current_answer_num].true_or_false = 0;
			LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;								//清除对错显示
			LCD_display_state.updata |= TRUE_FALSE_UPDATA;
		}
		//异或，再次按清除显示
		input_info.answer[current_answer_num].choice_answer ^= (0x01 << (g_variable.key_value - 1));
		if( input_info.answer[current_answer_num].choice_answer == A ||	input_info.answer[current_answer_num].choice_answer == B ||
			input_info.answer[current_answer_num].choice_answer == C ||	input_info.answer[current_answer_num].choice_answer == D ||
			input_info.answer[current_answer_num].choice_answer == E ||	input_info.answer[current_answer_num].choice_answer == F ||
			input_info.answer[current_answer_num].choice_answer == G ||	input_info.answer[current_answer_num].choice_answer == H ||
			input_info.answer[current_answer_num].choice_answer == 0)
		{
			input_info.ans_type[current_answer_num] = SINGLE_CHOICE;
		}
		else if(input_info.answer[current_answer_num].choice_answer )			//多选题
		{	
			input_info.ans_type[current_answer_num] = MULTIPLE_CHOICE;	
		}
		else{;}
	}
	else  //多题作答
	{
		switch(input_info.ans_type[current_answer_num])
		{
			case SINGLE_CHOICE:
				temp_answer = (0x01 << (g_variable.key_value - 1))&input_info.ans_range[current_answer_num];
				if(temp_answer)		//判断输入是否在作答范围内
				{
					if(input_info.answer[current_answer_num].choice_answer & temp_answer)
						input_info.answer[current_answer_num].choice_answer ^= temp_answer;		//二次输入删除
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
		LCD_display_state.letter_flag |= LETTER_UPDATA_1;								//显示选择题答案
		LCD_display_state.updata 	  |= LETTER_UPDATA;		
	}else{;}
}

/******************************************************************************
  @函数:key_digit_input_handler
  @描述:数字输入相关的按键处理函数
  @输入:
  @输出:
  @调用:
******************************************************************************/
static void key_digit_input_handler(void)
{
				
	if(input_info.grade_or_score == SCORE_RESULT)					//互动投票模式
	{
		input_info.score = g_variable.key_value;
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;			//显示评分/投票结果
		LCD_display_state.updata |= SEGMENT_UPDATA;
	}
	else if(input_info.grade_or_score == GRADE_RESULT) 				//小组评分模式
	{
		input_info.grade[current_answer_num] = g_variable.key_value;
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;		//显示评分/投票结果
		LCD_display_state.updata |= SEGMENT_UPDATA;
	}
	else {;}
}

/******************************************************************************
  @函数:key_judge_input_handler
  @描述:对错输入相关的按键处理函数
  @输入:
  @输出:
  @调用:
******************************************************************************/
static void key_judge_input_handler(void)
{
	if(input_info.total_answer_num == 1)
	{
		if(input_info.ans_type[current_answer_num] != TRUE_OR_FALSE)
		{
			input_info.answer[current_answer_num].choice_answer = 0x00;			//选择题的答案清空
			LCD_display_state.letter_flag |= LETTER_UPDATA_0;					//清除选择题显示
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
		LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_1;							//显示判断题答案
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
		LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_1;										//显示判断题答案
		LCD_display_state.updata |= TRUE_FALSE_UPDATA;
	}
}



/******************************************************************************
  @函数:key_clear_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void key_clear_handler(void)
{
	input_info.answer[current_answer_num].true_or_false = 0;
	input_info.answer[current_answer_num].choice_answer = 0;
	input_info.grade[current_answer_num] = 0; 
	input_info.score = 0;				
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;				//清除对错显示
	LCD_display_state.segment_flag |= SEGMENT_UPDATA_3;					//清除数码管中评分数值的显示
	LCD_display_state.letter_flag |= LETTER_UPDATA_0;					//清除字母显示
	LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
}

/******************************************************************************
  @函数:key_send_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void key_send_handler(void)
{ 
	uint8_t i;
	uint8_t temp_if_answer = 0;			//零时判断是否作答
	uint8_t temp_question_buf[QUESTION_MAX_BYTE_LEN];	
	uint8_t temp_question_buf_len;		
	for(i = 0 ;i < QUESTION_MAX_NUMBER;i++)
		temp_if_answer |= input_info.answer[i].true_or_false | input_info.answer[i].choice_answer | input_info.score | input_info.grade[i];
	if(temp_if_answer)		//输入内容非空才发送
	{
//		rf_send_blank_timers_stop();												//发送数据包时，停止发送空数据包
		if(input_info.score_or_answer == ANSWER_INPUT)								//如果输入模式为作答
		{
			if(input_info.total_answer_num == 1)
				tx_data.type = 0x10;				//单题作答
			else
				tx_data.type = 0x11;				//多题作答

			if(tx_data.type == 0x10)		
			{
				tx_data.len = 0x03;			//单题作答DATA固定为3
				tx_data.data[0] = 0x01;		//题目数量为1
				//tx_data.data[1] = 0x00; 	//题目序号为0
				//答案类型
				tx_data.data[1] = (input_info.ans_type[3]<<6)|(input_info.ans_type[2]<<4)|(input_info.ans_type[1]<<2)|(input_info.ans_type[0]);
				//答案值
				memcpy(&tx_data.data[2], &input_info.answer, input_info.total_answer_num);
			}
			else							//多题作答
			{
				pack_question_info(temp_question_buf,&temp_question_buf_len);	//封装input_info结构体的题目信息
				tx_data.len = temp_question_buf_len + 0x01;						
				tx_data.data[0] = input_info.total_answer_num;
				memcpy(&tx_data.data[1], temp_question_buf, temp_question_buf_len);
			}
		}
		else if(input_info.score_or_answer == SCORE_INPUT)
		{
			if(input_info.grade_or_score == SCORE_RESULT)		//投票模式
			{
				tx_data.type = 0x12;
				tx_data.len = 0x01;
				tx_data.data[0] = input_info.score;
			}
			else if(input_info.grade_or_score == GRADE_RESULT) //评分模式
			{
				tx_data.type = 0x13;  //by09.21
				tx_data.len = input_info.total_answer_num * 2 + 1;	 //每小组占用2字节
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
		my_nrf_tx_start();		//开始发送simulate_ack_layer结构体中的数据
		
		printf("key_send_handler:");
//		my_uart_trace_dump(simulate_ack.data,simulate_ack.length);	//串口打印
		
//		memset((tx_payload.data + NRF_ACK_PROTOCOL_LEN),0x00,NRF_USEFUL_DATA_LEN);		//防止上次发送数据对这次产生影响
//		memcpy((tx_payload.data + NRF_ACK_PROTOCOL_LEN), rf_var.tx_buf, rf_var.tx_len);
//		my_nrf_transmit_start();	//开始软件模拟ACK方式发送tx_payload里的内容
		rf_var.flag_txing = true;
	}
	else{;}
}

/******************************************************************************
  @函数:key_last_question_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void key_last_question_handler(void)
{
	if(input_info.total_answer_num == 1)		//单题作答不能切换题号
	{	return;	}
	
	if(input_info.score_or_answer == ANSWER_INPUT)								//如果输入模式为作答
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
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;										//显示题号
		LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
//		KeyScan_DebugLog("[KeyScan]: current answer number is %d\r\n",current_answer_num + input_info.start_answer_num);
	}
	else if(input_info.score_or_answer == SCORE_INPUT)			
	{
		if(input_info.grade_or_score == SCORE_RESULT)		//互动投票模式
		{;}
		else if(input_info.grade_or_score == GRADE_RESULT) //小组评分模式
		{
			if(current_answer_num > 0)
				current_answer_num--;
			else
				current_answer_num = input_info.total_answer_num - 1;
			
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;				//显示题号
			LCD_display_state.updata |= SEGMENT_UPDATA;
			LCD_display_handler();							//每次LCD处理函数只能显示一次
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;				//显示评分结果	
			LCD_display_state.updata |= SEGMENT_UPDATA;
		}
	}
}

/******************************************************************************
  @函数:key_next_question_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
static void key_next_question_handler(void)
{
	
	if(input_info.total_answer_num == 1)		//单题作答不能切换题号
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
		LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;										//显示题号
		LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
//		KeyScan_DebugLog("[KeyScan]: current answer number is %d\r\n",current_answer_num + input_info.start_answer_num);
	}
	else if(input_info.score_or_answer == SCORE_INPUT)			
	{
		if(input_info.grade_or_score == SCORE_RESULT)		//互动投票模式
		{;}
		else if(input_info.grade_or_score == GRADE_RESULT) //小组评分模式
		{
			current_answer_num ++ ;
			current_answer_num %= input_info.total_answer_num;
				
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_2;				//显示题号
			LCD_display_state.updata |= SEGMENT_UPDATA;
			LCD_display_handler();							//每次LCD处理函数只能显示一次
			LCD_display_state.segment_flag |= SEGMENT_UPDATA_1;				//显示评分结果	
			LCD_display_state.updata |= SEGMENT_UPDATA;
		}
		else {;}
	}

}


/* 外部函数定义---------------------------------------------------------*/

/******************************************************************************
  @函数:init_input_information
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
void init_input_information( void )
{
	tx_data.header = 0x5A;
	tx_data.uid[0]= UID[3];
	tx_data.uid[1]= UID[4];
	tx_data.uid[2]= UID[5];
	tx_data.uid[3]= UID[6];
	tx_data.packnum = 0x00;		//根据新协议，这个不再是包号，而作为保留字段，默认0x00
	tx_data.end = 0xCA;
		
	current_answer_num = 0;	
	input_info.score_or_answer = ANSWER_INPUT;												//默认开机即输入答案
	input_info.start_answer_num = 0;														//默认题号为0
	input_info.total_answer_num = 1;														//默认为单题作答

	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;													//清除对错显示
	LCD_display_state.letter_flag |= LETTER_UPDATA_0;														//第一题为选择题
	LCD_display_state.segment_flag |= SEGMENT_UPDATA_0;										//清除显示数字
	LCD_display_state.eng_num_flag |= ENG_NUM_UPDATA_1;														//显示输入状态
	LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA | ENG_NUM_UPDATA;
	LCD_display_handler();
}

/******************************************************************************
  @函数:clear_input_information
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
void clear_input_information( void )
{
	uint8_t temp_input_mode = input_info.score_or_answer;
	memset(&input_info, 0x00,sizeof(input_info_type_t));									//清除输入的答案
	input_info.score_or_answer = temp_input_mode;											//不改变当前的输入模式
	input_info.total_answer_num = 1;														//默认为单题作答
	current_answer_num = 0;																	//当前题号从0开始
	
	LCD_display_state.true_false_flag |= TRUE_FALSE_UPDATA_0;								//清除对错显示
	LCD_display_state.letter_flag |= LETTER_UPDATA_0;										//第一题为选择题
	LCD_display_state.segment_flag |= SEGMENT_UPDATA_0;										//清除显示数字
	LCD_display_state.updata |= SEGMENT_UPDATA | LETTER_UPDATA | TRUE_FALSE_UPDATA;
}

/******************************************************************************
  @函数:app_handler
  @描述:
  @输入:None
  @输出:
  @调用:
******************************************************************************/
void app_handler(void)
{
	if(rf_var.flag_rx_ok)		//如果有接收到数据包
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
  @函数:button_handler2
  @描述:button_handler 和 官方名字相同，所以button_handler2
  @输入:None
  @输出:
  @调用:
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
			if(input_info.score_or_answer == ANSWER_INPUT)		//如果输入模式为作答
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
			if(input_info.score_or_answer == ANSWER_INPUT)			//如果输入模式为作答
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


