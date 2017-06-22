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
#include "board.h"
#include "app_card_process.h"

extern void app_handle_layer(void);

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
	board_init();
	wl.match_status = ON;
	rf_set_card_status(1);
	printf("{\"fun\":\"system_init\",\"status\":\"0\"}\r\n");

	while(1)
	{	
		app_handle_layer();

	}	
}
