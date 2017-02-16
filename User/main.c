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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "stdlib.h"
#include "board.h"
#include "cJSON.h"

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
	Platform_Init();
	{
		char *out ;
		cJSON *root,*fmt;
		root=cJSON_CreateObject();
		cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
		cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());
		cJSON_AddStringToObject(fmt,"type", "rect");
		cJSON_AddNumberToObject(fmt,"width", 1920);
		cJSON_AddNumberToObject(fmt,"height", 1080);
		cJSON_AddNumberToObject(fmt,"frame rate", 24);
		out=cJSON_Print(fmt);
		printf("%s\n",out);
		free(out);
	}
	while(1)
	{	
		app_handle_layer();

	}	
}
