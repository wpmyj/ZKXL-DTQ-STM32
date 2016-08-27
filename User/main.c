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

extern void pos_handle_layer(void);
extern void app_handle_layer(void);
extern void rc500_handle_layer(void);
extern void rf_handle_layer(void);

int main(void)
{
	DISABLE_ALL_IRQ();
	Platform_Init();
	NRF_Module_Set();
	ENABLE_ALL_IRQ();
	while(1)
	{	

		pos_handle_layer();
		app_handle_layer();
//  rc500_handle_layer();
//  rf_handle_layer();	
		
	}	
}




/**
  * @}
  */
/**************************************END OF FILE****************************/





