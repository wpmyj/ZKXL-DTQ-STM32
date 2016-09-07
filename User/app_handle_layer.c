/**
  ******************************************************************************
  * @file   	app_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pos_handle_layer.h"
#include "rc500_handle_layer.h"

extern uint8_t uart_rf_cmd_sign[4],uart_card_cmd_sign[4];		

void App_rf_check_process(void);
void App_card_process(void);

/******************************************************************************
  Function:app_handle_layer
  Description:
		App 轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void app_handle_layer(void)
{
		/* nrf51822 Communication processing process */
		App_rf_check_process();
		
		/* serial cmd processing process */
		App_seirial_cmd_process();
		
		/* MI Card processing process */
		App_card_process();
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_rf_check_process(void)
{
	Uart_MessageTypeDef rf_message;
	uint8_t i = 0 ;
	
	if(rf_var.flag_rx_ok == true)
	{
		rf_message.HEADER = 0x5C;
		rf_message.TYPE = 0x10;
		
		memcpy(rf_message.SIGN,uart_rf_cmd_sign,4);
		
		rf_message.LEN = rf_var.rx_len;

		for (i=0;i<rf_var.rx_len+1;i++)
		{
			rf_message.DATA[i]=rf_var.rx_buf[i];
	#ifdef ENABLE_RF_DATA_SHOW
			printf("%2X ",rf_var.rx_buf[i]);
			if((i+1)%20 == 0 )
				printf("\r\n");			
	#endif
		}
		
		rf_message.XOR =  XOR_Cal((uint8_t *)(&(rf_message.TYPE)), i+6);		
		rf_message.END = 0xCA;
		
		/* 清空接收缓存 */
		memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
		rf_var.rx_len = 0x00;
		rf_var.flag_rx_ok = false;
		
		/* 执行完的指令存入发送缓存 */
		if(BUFFERFULL == buffer_get_buffer_status(SEND_RINGBUFFER))
		{
			DebugLog("Serial Send Buffer is full! \r\n");
		}
		else
		{
			serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_message);
		}	
	}
}

/******************************************************************************
  Function:App_card_process
  Description:
		App MI Card 轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_card_process(void)
{ 
	
}