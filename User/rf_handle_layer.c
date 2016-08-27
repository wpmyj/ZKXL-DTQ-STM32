/**
  ******************************************************************************
  * @file   	rf_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions
  ******************************************************************************
  */
  
#include "main.h"

void rf_handle_layer(void)
{
	uint16_t temp_len = 0;
	datiqi_type_t recv_package;
	answer_packet_t ans_package;
	if(1 == rf_var.flag_rx_ok)								//数据接收完成
	{	
		recv_package.payload_len = rf_var.rx_buf[7];
		memcpy(&recv_package, rf_var.rx_buf, recv_package.payload_len + 8);
		ledToggle(LED1);
	
		DebugLog("\r\n===========================================================================\r\n");

		DebugLog("recv_raw_data:");
		for(temp_len = 0; temp_len < rf_var.rx_len; temp_len++)
		{
			DebugLog(" %02X ", rf_var.rx_buf[temp_len]);
		}	
	
		DebugLog("\r\nrecv_data_len: %d  \r\n", rf_var.rx_len);

		if(recv_package.header == 0x5A)
		{

			DebugLog("UID:		");
			for(temp_len = 0; temp_len < 4; temp_len++)
			{
				DebugLog("%02X", recv_package.uid[temp_len]);
			}
			DebugLog("\r\n");	
			
			if(recv_package.pack_type == 0x10||recv_package.pack_type == 0x11)
			{
				memcpy(&ans_package, recv_package.payload, recv_package.payload_len);
				for(temp_len = 0; temp_len < ans_package.question_amount; temp_len ++)
				{
					DebugLog("-------------------------------------------------------------------------\r\n");
					DebugLog("question ID:	%d\r\n", ans_package.question_number + temp_len );
					DebugLog("question type:	");
					switch((ans_package.question_type[(15-temp_len)/4]&(0x03 << (temp_len%4))) >> (temp_len%4) )
					{
						case SINGLE_CHOICE:
							DebugLog("single choice\r\n");
							DebugLog("choice answer:	");
							switch(ans_package.answer[temp_len].choice_answer)
							{
								case A:
									DebugLog("A");
									break;
								case B:
									DebugLog("B");
									break;
								case C:
									DebugLog("C");
									break;
								case D:
									DebugLog("D");
									break;
								case E:
									DebugLog("E");
									break;
								case F:
									DebugLog("F");
									break;
								case G:
									DebugLog("G");
									break;
								case H:
									DebugLog("H");
									break;
								default:
									break;
							}
							DebugLog("\r\n");
							break;
						case MULTIPLE_CHOICE:
							DebugLog("multiple choice\r\n");
							DebugLog("choice answer:	");
							if(ans_package.answer[temp_len].choice_answer & A)
							{
								DebugLog("A	");
							}
							if(ans_package.answer[temp_len].choice_answer & B)
							{
								DebugLog("B	");
							}
							if(ans_package.answer[temp_len].choice_answer & C)
							{
								DebugLog("C	");
							}
							if(ans_package.answer[temp_len].choice_answer & D)
							{
								DebugLog("D	");
							}
							if(ans_package.answer[temp_len].choice_answer & E)
							{
								DebugLog("E	");
							}
							if(ans_package.answer[temp_len].choice_answer & F)
							{
								DebugLog("F	");
							}
							if(ans_package.answer[temp_len].choice_answer & G)
							{
								DebugLog("G	");
							}
							if(ans_package.answer[temp_len].choice_answer & H)
							{
								DebugLog("H	");
							}
							DebugLog("\r\n");
							break;
						case TRUE_OR_FALSE:
							DebugLog("true or false\r\n");
							if(ans_package.answer[temp_len].true_or_false == 0x01)
							{
								DebugLog("answer:		%c\r\n", 86);
							}
							else
							{
								DebugLog("answer:		%c\r\n", 88);
							}
							break;
						default:
							break;
					}				
				}				
				
			}
			else if(recv_package.pack_type == 0x12)
			{
				DebugLog("-------------------------------------------------------------------------\r\n");
				DebugLog("score:		%d\r\n", recv_package.payload[0]);
			}
		}
		DebugLog("===========================================================================\r\n");
		
		hal_uart_clr_rx();
		rf_var.flag_rx_ok = 0;
		memset(rf_var.rx_buf, 0, rf_var.rx_len);
		rf_var.rx_len = 0;
	}
}





/**
  * @}
  */
/**************************************END OF FILE****************************/



