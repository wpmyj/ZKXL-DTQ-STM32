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
#include "stdlib.h"

extern void app_handle_layer(void);
extern void rc500_handle_layer(void);
extern clicker_t clickers[120];
extern uint32_t clicker_test_printf_flg;
extern timer_t clicker_time;

/* Private functions ---------------------------------------------------------*/
static void Fee_Test(void);
static void nrf51822_spi_send_test(void);
static void nrf51822_spi_revice_test(void);
static void Whitelist_test( void );
static void Ringbuffer_test(void);
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
	// Whitelist_test();
	// Ringbuffer_test();
	
	/* System function test end ------------------------------------------------*/
	
	while(1)
	{	
		int i;
		
		app_handle_layer();
		
//		if(clicker_test_printf_flg == 1)
//		{
//			for(i=0;i<120;i++)
//			{
//				if(clickers[i].use ==1 )
//				{
//					printf("[%2d:%2d:%2d] clickers : %02x%02x%02x%02x, revice = %8d, lost = %8d, lost_rate = %5.3f\r\n",
//						clicker_time.hour,clicker_time.min,clicker_time.s,	
//						clickers[i].uid[0],
//						clickers[i].uid[1],
//						clickers[i].uid[2],
//						clickers[i].uid[3],
//						(clickers[i].revice_package_num),
//						(clickers[i].lost_package_num),
//						(clickers[i].lost_package_num)*100.0/(clickers[i].revice_package_num + clickers[i].lost_package_num)
//						);
//				}
//			}
//			clicker_test_printf_flg = 0;
//			printf("\r\n");
//		}
		
//		if(serial_ringbuffer_get_usage_rate(0) !=0 )
//		{
//			printf("Uart revice Buffer status = %d uasge rate = %d \r\n",
//				buffer_get_buffer_status(0),serial_ringbuffer_get_usage_rate(0));
//		}
//		if(serial_ringbuffer_get_usage_rate(1) !=0 )
//		{
//			printf("Uart send   Buffer status = %d uasge rate = %d \r\n",
//				buffer_get_buffer_status(1),serial_ringbuffer_get_usage_rate(1));	
//		}
	}	
}

/******************************************************************************
  Function:Ringbuffer_test
  Description:
		缓冲区测试函数
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void Ringbuffer_test(void)
{
	Uart_MessageTypeDef uart_test_message;
	uint16_t i = 0;
	
	uart_test_message.HEADER = 0x5C;
	uart_test_message.TYPE = 0X25;
	memset(uart_test_message.SIGN,0,4);
	uart_test_message.LEN = 0x00;
	uart_test_message.XOR = 0x5C;
	uart_test_message.END = 0xCA;
	
	while(1)
	{
		for(i=0;i<300;i++)
		{
			if(BUFFERFULL != buffer_get_buffer_status(REVICE_RINGBUFFER))
			{
				serial_ringbuffer_write_data(REVICE_RINGBUFFER,&uart_test_message);
			}	
			printf("Uart revice Buffer status = %d uasge rate = %d \r\n",buffer_get_buffer_status(0),serial_ringbuffer_get_usage_rate(0));
		}
		
		for(i=0;i<300;i++)
		{
			if(BUFFEREMPTY != buffer_get_buffer_status(REVICE_RINGBUFFER))
			{
				serial_ringbuffer_read_data(REVICE_RINGBUFFER,&uart_test_message);
			}	
			printf("Uart revice Buffer status = %d uasge rate = %d \r\n",buffer_get_buffer_status(0),serial_ringbuffer_get_usage_rate(0));
		}
	}
}

/******************************************************************************
  Function:Whitelist_test
  Description:
		发送数据到答题器测试函数
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void Whitelist_test( void )
{
	uint8_t i;
	uint8_t writeuid[4];
	uint8_t readuid[4];
	
	uint8_t uidpos = 0;
	uint8_t status =0;
	static uint32_t storetestcntOk = 0, storetestcntErr = 0,storetestcnt = 0;
	static uint32_t deletetestcntOk = 0,deletetestcntErr = 0,deletetestcnt =0 ;
	
	initialize_white_list();
	
	while(1)
	{
		uint8_t status = 0;
		uint8_t uidvalue = 0;
		
		for(i=0;i<120;i++)
		{ 
			uidvalue = rand()/100;
			writeuid[0] = uidvalue + 0;
			writeuid[1] = uidvalue + 1;
			writeuid[2] = uidvalue + 2;
			writeuid[3] = uidvalue + 3;
			
			if(i == 64)
				printf(" uidpos = 64");
			
			status = add_uid_to_white_list(writeuid,&uidpos);
			DelayMs(20);
			if(status == OPERATION_SUCCESS)
			{
				printf(" uidpos = %d Write UID = %2X%2X%2X%2X \r\n",uidpos,writeuid[0],writeuid[1],writeuid[2],writeuid[3]);
				status = get_index_of_uid(uidpos,readuid);
				if(status == OPERATION_SUCCESS)
				{
					printf(" uidpos = %d Read  UID = %2X%2X%2X%2X \r\n",uidpos,readuid[0],readuid[1],readuid[2],readuid[3]);
		
					status = uidcmp(writeuid,readuid);
					if(status == OPERATION_SUCCESS)
					{
						storetestcntOk++;
					}
					else
					{
						storetestcntErr++;
					}
				}
				
				delete_uid_from_white_list(writeuid);
				printf("the len of white list is %d \r\n",get_len_of_white_list());
			}
			storetestcnt++;
			if(storetestcnt%20 == 0)
					printf(" Store OK cnt = %d  Err cnt = %d\r\n",storetestcntOk,storetestcntErr);
		}
		
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
				my_nrf_transmit_start(TestBuffer,10,NRF_DATA_IS_USEFUL,1);
				
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
	Fee_Init(FEE_INIT_POWERUP);	
	
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
			
			TestNum++;
			if(ReadData == WriteNum)
			{
				TestOkNum++;
			}
			else
			{
				TestErrNum++;
			}

			if(TestNum%100 == 0 )
			{
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",
				        TestNum,TestOkNum,TestErrNum);
			}
		}
	}	
}

/**************************************END OF FILE****************************/
