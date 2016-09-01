/**
  ******************************************************************************
  * @file   	Main.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	main function for STM32F103RB
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern void app_handle_layer(void);
extern void rc500_handle_layer(void);

/* Private functions ---------------------------------------------------------*/
static void Fee_Test(void);
static void nrf51822_spi_send_test(void);
static void nrf51822_spi_revice_test(void);

/******************************************************************************
  Function:main
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
int main(void)
{
	/* System initialize -------------------------------------------------------*/
	Platform_Init();

	/* System function test start-----------------------------------------------*/
	// Fee_Test();
	// nrf51822_spi_send_test();
	// nrf51822_spi_revice_test();
	
	/* System function test end ------------------------------------------------*/
	
	while(1)
	{	
	  pos_handle_layer();
	  app_handle_layer();
		
//  rc500_handle_layer();		
	}	
}


/******************************************************************************
  Function:nrf51822_spi_test
  Description:
		发送数据到答题器测试函数
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void nrf51822_spi_send_test(void)
{
    static uint32_t i = 0;
    uint8_t j = 0;
	  uint8_t TestBuffer[10] = {0xAA,0x55,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
	
    while(1)
		{
				my_nrf_transmit_start(TestBuffer,10,NRF_DATA_IS_USEFUL);
				
				for(j=0;j<10;j++)
				{
					printf(" %2x ",TestBuffer[j]);
				}
				printf("\r\n");

				printf("Sent Data test num = %d! \r\n",i++);
				
				DelayMs(500);
		}
}
/******************************************************************************
  Function:Fee_Test
  Description:
		Flash 模拟 EEPROM 测试函数
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void Fee_Test(void)
{
/* Define --------------------------------------------------------------------*/
#define  TEST_END_NUM   480
#define  TEST_START_NUM 0
	
	uint16_t ReadData;
	uint32_t TestNum = 0;
	uint32_t TestOkNum = 0;
	uint32_t TestErrNum = 0;
	uint32_t WriteNum = 0;
	uint32_t WriteFlag = 0;
	uint16_t VarAddr = 0;

	/* EEPROM Init */
	Fee_Init();	
	
	while(1)
	{
	  /* --- Store many values in the EEPROM ---*/
		for (VarAddr = TEST_START_NUM; VarAddr <= TEST_END_NUM; VarAddr++)
		{
			if(VarAddr == TEST_END_NUM)
			{
				if(WriteFlag == 0)
					WriteFlag = 1;
				else
					WriteFlag = 0;
			}
			
			if(WriteFlag == 0)
				WriteNum = VarAddr;
			else
				WriteNum = 480-VarAddr;
			
			EE_WriteVariable(VarAddr, WriteNum);
			DelayMs(100);

			EE_ReadVariable(VarAddr, &ReadData);
			printf("FEE read data address  = %4x Write value = %4x Read Value = %4x\r\n",
			        VarAddr,WriteNum,ReadData);
			TestNum++;
			if(ReadData == WriteNum)
			{
				TestOkNum++;
			}
			else
			{
				TestErrNum++;
			}
			if(TestNum == 3154-255)
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",
			          TestNum,TestOkNum,TestErrNum);
				
			if(TestNum == 3154)
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",
			          TestNum,TestOkNum,TestErrNum);
				
			if(TestNum%20 == 0 )
			{
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",
				        TestNum,TestOkNum,TestErrNum);
			}
		}
	}	
}

/******************************************************************************
  Function:nrf51822_spi_revice_test
  Description:
		接收答题器数据测试函数
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void nrf51822_spi_revice_test(void)
{
	uint16_t temp_len = 0;
	datiqi_type_t recv_package;
	answer_packet_t ans_package;
	
	while(1)
	{
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
			
			//hal_uart_clr_rx();
			rf_var.flag_rx_ok = 0;
			memset(rf_var.rx_buf, 0, rf_var.rx_len);
			rf_var.rx_len = 0;
		}
	}
}


/**************************************END OF FILE****************************/
