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
#include "eeprom.h"

extern void pos_handle_layer(void);
extern void app_handle_layer(void);
extern void rc500_handle_layer(void);
extern void rf_handle_layer(void);

void Fee_Test(void);

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
	DISABLE_ALL_IRQ();
	Platform_Init();
	ENABLE_ALL_IRQ();
	
	/* System function test start-----------------------------------------------*/
	//Fee_Test();
	
	/* System function test end ------------------------------------------------*/
	
	while(1)
	{	
	  pos_handle_layer();
	  app_handle_layer();
//  rc500_handle_layer();
//  rf_handle_layer();		
	}	
}

/******************************************************************************
  Function:Fee_Test
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void Fee_Test(void)
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
	  /* --- Store successively many values of the three variables in the EEPROM ---*/
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
			Delay10Ms();
			//Delay10Ms();
			EE_ReadVariable(VarAddr, &ReadData);
			printf("FEE read data address  = %4x Write value = %4x Read Value = %4x\r\n",VarAddr,WriteNum,ReadData);			
			//Delay10Ms();
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
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",TestNum,TestOkNum,TestErrNum);
				
			if(TestNum == 3154)
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",TestNum,TestOkNum,TestErrNum);
				
			if(TestNum%20 == 0 )
			{
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",TestNum,TestOkNum,TestErrNum);
			}
		}
	}	
}

/**************************************END OF FILE****************************/





