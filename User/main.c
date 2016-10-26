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

	while(1)
	{	
		app_handle_layer();
		
	}	
}
